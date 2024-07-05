#include <bits/stdc++.h>

using namespace std;

const double a0 = 0.88, a1 = 0.89, b0 = 0.5, b1 = 0.44;

const int MAX_RANK = 200;

//We need 2 of those and we compute the other

//How many gems we keep of each grade
const int SAMPLES = 1e4;

//How bigger than U we keep our gems
const double COST_OVER_U = 1e60;

//How diferent gems have to be for us to diferenciate them
//const double STEP = 1.01;

//STEP^SAMPLES = COST_OVER_U

//const int SAMPLES = (int)(log(COST_OVER_U) / log(STEP));

//const double COST_OVER_U = pow(STEP, SAMPLES);

const double STEP = pow(COST_OVER_U,1.0/SAMPLES);


//STEP^i * 2^g = v;
//at a given index and grade, what size should be there:
double get_value(int g, int i){
  return pow(STEP,i)*exp2(g);
}

//i*log(STEP) + g*log(2) = log(v)
//i = (log(v)-g*log(2))/log(STEP)
//at a given value and grade, what index should it be in:
int get_index(int g, double v){
  int ans = ((log(v)-g*log(2))/log(STEP))-5;
  if(get_value(g, ans+1) >= v){
    cout << g << ' ' << v << ' ' << ans << ' ' << get_value(g, ans+1) << endl;
    exit(0);
  }
  assert(get_value(g, ans+1) < v);
  while(get_value(ans+1, g) < v)ans++;
  return max(ans,0);
}

struct gem{
    double value = 1;
    double component = 1;
    double growth = -1;
    int d;
    int g1;
    int g2;
};

double A = 0.0374847551932528944007572135888;
double B = 0.475185616921946385460273631907;
double f(int g, double v){
  return exp2(A*g)*pow(v,B);
}

void stats(gem &c, gem a, gem b){
  c.value = a.value + b.value;
  c.growth = log(c.component)/log(c.value);
}

gem combine0(gem a, gem b){
  if(a.component < b.component)swap(a,b);
  gem c;
  c.component = a0 * a.component + b0 * b.component;
  stats(c,a,b);
  return c;
}

gem combine1(gem a, gem b){
  if(a.component < b.component)swap(a,b);
  gem c;
  c.component = a1 * a.component + b1 * b.component;
  stats(c,a,b);
  return c;
}

gem dp[MAX_RANK][SAMPLES];

//const double MIN_COMBINE_0 = 1.0;
const double MAX_COMBINE_0 = 5.0;

//const double MIN_COMBINE_1 = 2.0;
const double MAX_COMBINE_1_START = 6.0;
const double MAX_COMBINE_1 = 4.2;


int main() {
  std::ofstream out("out");
  out << setprecision(20) << endl;
  
  dp[0][0].growth = 0;

  cout << "?" << endl;
  //g is the current grade we are computing
  for(int g = 1; g < MAX_RANK; g++){

    //i is the first gem of rank g-1
    for(int i = 0; i < SAMPLES; i++){
      if(dp[g-1][i].growth < 0)continue;
      //k is the index of the output gem
      int k = i;
      //j is the second gem of rank g-1
      for(int j = i; j < SAMPLES; j++){
        if(get_value(g-1, j)/get_value(g-1, i) > MAX_COMBINE_0) break;
        if(dp[g-1][j].growth < 0)continue;
        gem out = combine0(dp[g-1][j], dp[g-1][i]);//biger gem on the first position

        while(out.value > get_value(g,k+1)) k++;
        if(k >= SAMPLES)break;
        if(out.growth > dp[g][k].growth)dp[g][k] = out;
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
      if(dp[g-1][i].growth < 0)continue;
      
      //k is the index of the output gem
      int k = i;
      
      for(int j = i; j < SAMPLES; j++){
        if(dp[g][j].growth < 0)continue;
        if(get_value(g, j)/get_value(g-1, i) > MAX_COMBINE_1_START)break;
        if(get_value(g, j)/get_value(g-1, i) > MAX_COMBINE_1 and g >= 4)break;
        gem out = combine1(dp[g][j], dp[g-1][i]);
        while(out.value > get_value(g,k+1)) k++;
        if(k >= SAMPLES)break;
        if(out.growth > dp[g][k].growth)dp[g][k] = out;
      }
    }
    gem best;
    for(int i = 0; i < SAMPLES; i++){
      if(dp[g][i].growth > best.growth)best = dp[g][i];
    }

    out << "Value : " << best.value << '\n';
    out << "Leech : " << best.component << '\n';
    out << "Growth: " << best.growth << '\n';
    out << "Grade : +" << g << '\n';
    out << endl;
    
  }
  
  return 0;
}