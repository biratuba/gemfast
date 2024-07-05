#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

#include <iomanip>

#include <iostream>

#include <unistd.h>

using namespace std;

const double a0 = 0.88, a1 = 0.89, b0 = 0.5, b1 = 0.44;

//how many diferennt values we will keep for each rank
const int NUMBER_OF_SAMPLES = 1e4;

//How much bigger the values will get than an generic upgrade
const double VALUE_RATIO = 1e15;

double get_size(double n,int g){
    return pow(VALUE_RATIO,(n+1.0)/(double)NUMBER_OF_SAMPLES) * exp2(g);
}

// v = VALUE_RATIO^(n)/NUMBER_OF_SAMPLES
// v^(NUMBER_OF_SAMPLE) = VALUE_RATIO^n
// NUMBER_OF_SAMPLES*log(v) = n*log(VALUE_RATIO)
// n = NUMBER_OF_SAMPLES*log(v)/log(VALUE_RATIO)

int get_index(double v){
    int ans =  (NUMBER_OF_SAMPLES*log(v)/log(VALUE_RATIO));
    ans = max(ans,0);
    ans = min(ans,NUMBER_OF_SAMPLES-1);
    return ans;
}

double combine1limit(int g){
    return exp(0.85*g)/10.0;
}

const int MAX_RANK = 100;

struct gem{
    double value = -1;
    double component = -1;
    double growth = -1;
    //all the folowing variables are for finding good parameters
    double max_combine_0 = 0;// a/b
    double min_combine_0 = 100;// a/b
    double max_combine_1 = 0;// a/b
    double min_combine_1 = 100;// a/b
    double over_rank = 1;
    double recipe = 0;
    int grade = 1;
    int rgrade = -1;
    int rvalue = -1;
    int lgrade = -1;
    int lvalue = -1;
    
};

gem combine0(gem &a, gem &b){
    gem c;
    c.value = a.value + b.value;
    double component1 = a.component, component2 = b.component;
    if(component1 < component2) swap(component1,component2);
    c.component = component1 * a0 + component2 * b0;
    c.growth = log(c.component) / log(c.value);
    c.max_combine_0 = max(a.max_combine_0, b.max_combine_0);
    c.max_combine_1 = max(a.max_combine_1, b.max_combine_1);
    c.min_combine_1 = min(a.min_combine_1, b.min_combine_1);
    double v1 = a.value, v2 = b.value;
    if(v1 < v2) swap(v1,v2);
    c.max_combine_0 = max(c.max_combine_0, v1/v2);
    c.min_combine_0 = min(c.min_combine_0, v1/v2);
    c.over_rank = max(a.over_rank, b.over_rank);
    c.recipe = v1/v2;

    c.grade = a.grade +1;
    c.rgrade = a.grade;
    c.rvalue = a.value;
    c.lgrade = b.grade;
    c.lvalue = b.value;
    
    return c;
}

gem combine1(gem &a, gem &b){
    gem c;
    c.value = a.value + b.value;
    double component1 = a.component, component2 = b.component;
    if(component1 < component2) swap(component1,component2);
    c.component = component1 * a1 + component2 * b1;
    c.growth = log(c.component) / log(c.value);
    c.max_combine_0 = max(a.max_combine_0, b.max_combine_0);
    c.max_combine_1 = max(a.max_combine_1, b.max_combine_1);
    c.min_combine_1 = min(a.min_combine_1, b.min_combine_1);
    c.max_combine_1 = max({c.max_combine_1, a.value/b.value});
    c.min_combine_1 = min({c.min_combine_1, a.value/b.value});
    c.over_rank = max(a.over_rank, b.over_rank);
    c.recipe = -a.value/b.value;

    c.grade = max(a.grade,b.grade);
    c.rgrade = a.grade;
    c.rvalue = a.value;
    c.lgrade = b.grade;
    c.lvalue = b.value;

    
    return c;
}

gem dp[2][NUMBER_OF_SAMPLES];


int main()
{
    std::ofstream file("data.csv");
    std::ofstream out("out");
    /*std::ofstream rec("recipe");*/
    

    
    dp[0][0] = {1,1,1};
    
    out << setprecision(15);

    for(int i = 1; i < MAX_RANK; i++){
        /*j = first gem id*/
        for(int j = 0; j < NUMBER_OF_SAMPLES; j++){
            if(dp[(i-1)&1][j].value < 1) continue;
            /*l = output gem id*/
            int l = j;
            /*k = second gem id*/
  
            for(int k = j; k < NUMBER_OF_SAMPLES; k++){
                if(dp[(i-1)&1][k].value < 1)continue;
                if(dp[(i-1)&1][k].value/dp[(i-1)&1][j].value > 5.0)break;
                gem output = combine0(dp[(i-1)&1][j],dp[(i-1)&1][k]);
                output.over_rank = max(output.over_rank, output.value/exp2(i));
                while(output.value > get_size(l,i)) l++;
                if(l >= NUMBER_OF_SAMPLES)break;
                if(output.growth > dp[i&1][l].growth)dp[i&1][l] = output;
            }
        }


        /*j = first gem id*/
        int start = get_index(combine1limit(i));
        start = min(start,9*NUMBER_OF_SAMPLES/10);
        for(int j = start; j < NUMBER_OF_SAMPLES; j++){
            if(dp[(i-1)&1][j].value < 1) continue;
            
            int l = j;
            
            for(int k = j; k <= NUMBER_OF_SAMPLES; k++){
                if(dp[(i)&1][k].value < 1)continue;
                if(dp[(i)&1][k].value/dp[(i-1)&1][j].value > 8.0)break;
                if(dp[(i)&1][k].value/dp[(i-1)&1][j].value > 6.0 and i >= 4)break;
                if(dp[(i)&1][k].value/dp[(i-1)&1][j].value > 4.2 and i >= 10)break;
                gem output = combine1(dp[i&1][k],dp[(i-1)&1][j]);
                output.over_rank = max(output.over_rank, output.value/exp2(i));
                while(output.value > get_size(l,i)) l++;
                if(l >= NUMBER_OF_SAMPLES)break;
                if(output.growth > dp[i&1][l].growth)dp[i&1][l] = output;
            }
        }

        if(i == 10){
            for(int j = 0; j < NUMBER_OF_SAMPLES; j++){
                dp[i&1][j].max_combine_0 = 0;
                dp[i&1][j].min_combine_0 = 100;
                dp[i&1][j].max_combine_1 = 0;
                dp[i&1][j].min_combine_1 = 100;
                dp[i&1][j].over_rank = 1;
            }
        }
        gem best;

        /*
        for(int j = 0; j < NUMBER_OF_SAMPLES; j++){
            if(get_size(j,0) > 16.0)break;
            if(dp[i&1][j].value < 1) continue;
            gem g = dp[i&1][j];
            
             rec << "(g" << g.grade << " " << g.value << ") <= (g" << g.lgrade << " " << g.lvalue << ") + (g" << g.rgrade << " " << g.rvalue << ")" << endl;
        }*/

        
        for(int j = 0; j < NUMBER_OF_SAMPLES; j++){
            if(dp[i&1][j].growth > best.growth)best = dp[i&1][j];
            dp[(i-1)&1][j] = gem();
        }
        
        out << "Value:  " << best.value << '\n';
        out << "Growth: " << best.growth << '\n';
        out << "Grade:  +" << i << '\n';
        out << "Leech: " << best.component << "\n";
        out << endl;
        
        cout << "Max combine 0: " << best.max_combine_0 << '\n';
        cout << "Min combine 0: " << best.min_combine_0 << '\n';
        cout << "Max combine 1: " << best.max_combine_1 << '\n';
        cout << "Min combine 1: " << best.min_combine_1 << '\n';
        cout << "Over rank: " << best.over_rank << "\n\n";

        
        double v = 0;
        for(int j = 0; j < NUMBER_OF_SAMPLES; j++){
            if(dp[i&1][j].recipe > 0)v = dp[i&1][j].lvalue/ exp2(i);
        }
        file << v << endl;
            
    }
}
