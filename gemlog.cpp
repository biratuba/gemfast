#include <bits/stdc++.h>

using namespace std;

const double a0 = 0.88, a1 = 0.89, b0 = 0.5, b1 = 0.44;

const int MAX_RANK = 1000;

//We need 2 of those and we compute the other

//How many gems we keep of each grade
const int SAMPLES = 4e4;

//How bigger than U we keep our gems
const double COST_OVER_U = 500;

//How diferent gems have to be for us to diferenciate them
//const double STEP = 1.01;

//STEP^SAMPLES = COST_OVER_U

//const int SAMPLES = (int)(log(COST_OVER_U) / log(STEP));

//const double COST_OVER_U = pow(STEP, SAMPLES);

//const double STEP = pow(COST_OVER_U,1.0/SAMPLES);


//COST_OVER_U*(i/SAMPLES) * 2^g = v;
//at a given index and grade, what size should be there:
double get_value(int g, int i){
  return COST_OVER_U * ((double)i/(double)SAMPLES)+g;
}
    
struct gem{
    double value = -1;
    double component = -1;
    double growth = -1;
    int grade = 0;
    double over_grade = 0;
    double recipe = 0;
};

//return log(2^a+2^b) = log(2^a*(1+2^(b-a))) = log(2^a) + log(1+2^(b-a))
double add(double a,double b){
  if(a < b)swap(a,b);
  return a + log2(1+exp2(b-a));
}

void stats(gem &c,gem a, gem b){
  c.value = add(a.value,b.value);
  c.over_grade = max({a.over_grade,b.over_grade,c.value-c.grade});
  c.growth = c.component/c.value;
  c.recipe = a.value-b.value;
}

gem combine0(gem a, gem b){
  assert(a.grade == b.grade);
  if(a.component < b.component)swap(a,b);
  gem c;
  c.grade = a.grade+1;
  c.component = add(a.component + log2(a0) ,b.component+ log2(b0));
  stats(c,a,b);
  return c;
}

gem combine1(gem a, gem b){
  assert(abs(a.grade - b.grade) == 1);
  if(a.component < b.component)swap(a,b);
  gem c;
  c.grade = max(a.grade,b.grade);
  c.component = add(a.component + log2(a1) ,b.component+ log2(b1));
  stats(c,a,b);
  return c;
}

gem dp[2][SAMPLES];

//const double MIN_COMBINE_0 = 1.0;
//const double MAX_COMBINE_0 = 5.0;

//const double MIN_COMBINE_1 = 2.0;
//const double MAX_COMBINE_1_START = 6.0;
//const double MAX_COMBINE_1 = 4.2;


int main() {
  std::ofstream out("out");
  std::ofstream file("data.csv");
  out << setprecision(20);
  file << setprecision(20);
  
  dp[0][0].value = 0;
  dp[0][0].component = 0;
  dp[0][0].growth = 1;


  cout << "?" << endl;


  file << "grade" << ',' << "value" << ',' << "component" << ',' << "growth" << ',' << "over_grade" << ',' << "value we want" << endl; 
  
  //g is the current grade we are computing
  for(int g = 1; g < MAX_RANK; g++){
    gem delta0[SAMPLES];
    gem delta1[SAMPLES];
    for(int i = 0; i < SAMPLES; i++){
      delta0[i] = gem();
      delta1[i] = gem();
    }
    //i is the first gem of rank g-1
    for(int i = 0; i < SAMPLES; i++){
      if(dp[(g-1)&1][i].growth < 0)continue;
      //k is the index of the output gem
      int k = i;
      //j is the second gem of rank g-1
      for(int j = i; j < SAMPLES; j++){
        if(get_value(g-1, j)-get_value(g-1, i) > log2(5.0)) break;
        if(dp[(g-1)&1][j].growth < 0)continue;
        gem out = combine0(dp[(g-1)&1][j], dp[(g-1)&1][i]);//biger gem on the first position

        while(out.value > get_value(g,k+1)) k++;
        if(k >= SAMPLES)break;
        if(out.growth > dp[g&1][k].growth)dp[g&1][k] = out;
      }
    }
  /* 
    bool combine1better[SAMPLES];
    for(int i = 0; i < SAMPLES; i++)combine1better[i] = false;

    for(int i = 0; i < SAMPLES; i++){
      if(dp[g-1][i].P_growth < 0)continue;
      for(int j = i; j < SAMPLES; j++){
        if(dp[g][j].P_growth < 0)continue;
        if(get_value(g, j)/get_value(g-1, i) > MAX_COMBINE_1_START)break;
        if(get_value(g, j)/get_value(g-1, i) > MAX_COMBINE_1 and i >= 4)break;
        gem out = combine1(dp[g][j], dp[g-1][i]);
        int idx = get_index(g, out.P_value / unit_val);
        assert(idx >= 0);
        if(idx >= SAMPLES)break;
        if(out.P_growth > dp[g][idx].P_growth)combine1better[idx]=true;
      }
    }

    int invmex = 0;

    for(int i = 0; i < SAMPLES; i++)if(!combine1better[i]) invmex = i;
 */
    for(int i = 0; i < SAMPLES; i++){
      if(dp[(g-1)&1][i].growth < 0)continue;
      
      //k is the index of the output gem
      int k = i;
      
      for(int j = i; j < SAMPLES; j++){
        if(dp[g&1][j].growth < 0)continue;
        if(get_value(g, j)-get_value(g-1, i) > log2(6.0))break;
        if(get_value(g, j)-get_value(g-1, i) > log2(4.3) and g >= 4)break;
        gem out = combine1(dp[g&1][j], dp[(g-1)&1][i]);
        while(out.value > get_value(g,k+1)) k++;
        if(k >= SAMPLES)break;
        if(out.growth > dp[g&1][k].growth)dp[g&1][k] = out;
      }
    }
    /*for(int i = 0; i < SAMPLES; i++){
      if(delta0[i].growth > delta1[i].growth)dp[g&1][i] = delta0[i];
      else dp[g&1][i] = delta1[i];
    }*/
    gem best;
    double prev = 0;
    double worst = -1e100;
    //file << 'g' << g;


    for(int i = 0; i < SAMPLES; i++){
      if(dp[g&1][i].growth > best.growth)best = dp[g&1][i];

      if(dp[g&1][i].growth > 0){
        prev = dp[g&1][i].component;
        const double A = 0.0373;
        const double B = 0.474;
        worst = max(worst,dp[g&1][i].component - exp2(A*g)*pow(dp[g&1][i].value,B));
      }
      
      //if(i%10 == 0) file << ',' << prev;
      
      dp[(g-1)&1][i] = gem();
    }

    cout << g << ':' << worst <<endl;
    //file << endl;
    //best = dp[g&1][1];
    out << "log2(Value) : " << best.value << '\n';
    out << "log2(Leech) : " << best.component << '\n';
    out << "Growth      : " << best.growth << '\n';
    out << "Grade       : +" << g << '\n';
    out << "Over grade  : " << best.over_grade << '\n';
    out << endl;

    file << g << ',' << best.value << ',' << best.component << ',' << best.growth << ',' << best.over_grade << ',' << best.value - g <<  endl; 


    
  }
  
  return 0;
}