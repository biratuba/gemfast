#include <bits/stdc++.h>

#define f first
#define s second

using namespace std;

const double a0 = 0.88, a1 = 0.89, b0 = 0.5, b1 = 0.44;

const int MAX_RANK = 20;

//We need 2 of those and we compute the other

//How many gems we keep of each grade
const int SAMPLES = 1e3;

//How bigger than U we keep our gems
const double COST_OVER_U = 1e6;

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
  assert(get_value(g, ans+1) < v);
  while(get_value(ans+1, g) < v)ans++;
  return max(ans,0);
}

struct gem{
    double value = 1;
    double component = 1;
    double growth = -1;
  bool operator==(const gem& other) {
      return value == other.value && component == other.component && growth == other.growth;
  }
};

double A = 0.0374847551932528944007572135888;
double B = 0.475185616921946385460273631907;
double f(int g, double v){
  return exp2(A*g)*pow(v,B);
}

void stats(gem &c, const gem &a, const gem &b){
  c.value = a.value + b.value;
  c.growth = log(c.component)/log(c.value);
}

gem combine0(const gem &a, const gem &b){
  double v1 = a.component;
  double v2 = b.component;
  if(v1 < v2)swap(v1,v2);
  gem c;
  c.component = a0 * v1 + b0 * v2;
  stats(c,a,b);
  return c;
}

gem combine1(const gem &a, const gem &b){
  double v1 = a.component;
  double v2 = b.component;
  if(v1 < v2)swap(v1,v2);
  gem c;
  c.component = a1 * v1 + b1 * v2;
  stats(c,a,b);
  return c;
}

gem dp[MAX_RANK][SAMPLES];

int main() {

  std::ofstream out("out");
  std::ofstream data("data");
  out << setprecision(20);
  for(int i = 0; i < SAMPLES; i++)dp[0][i].growth = 0;
  cout << "?" << endl;
  for(int g = 1; g < MAX_RANK; g++){   

    for(int i = 1; i < SAMPLES; i++)dp[g&1][i] = gem();
    
    dp[g&1][0] = combine0(dp[(g-1)&1][0], dp[(g-1)&1][0]);


    int i = 0;
    gem val = combine1(dp[(g)&1][0], dp[(g-1)&1][0]);
    while(val.value > get_value(g, i))i++;
    if(dp[(g)&1][i].growth < val.growth)dp[(g)&1][i] = val;

    pair<int,int> prev1 = {0,0};
    
    pair<int,int> prev0 = {0,0};
    for(int i = 1; i < SAMPLES; i++){
      //DELTA0
      
      const pair<int,int> null = {-1,-1};
      pair<int,int> next0 = {-1,-1};
      
      while(prev0.f + 1 < SAMPLES && dp[(g-1)&1][prev0.f].value == dp[(g-1)&1][prev0.f+1].value)prev0.f++;
      while(prev0.s + 1 < SAMPLES && dp[(g-1)&1][prev0.s].value == dp[(g-1)&1][prev0.s+1].value)prev0.s++;
      {
        int k = max(prev0.s - 200, 0);
        for(int j = min(prev0.f + 100,SAMPLES - 1); j >= max(0,prev0.f-100) ; j--){
          while(k+1 < SAMPLES && get_value(g, i) > dp[(g-1)&1][j].value + dp[(g-1)&1][k].value)k++;
          if(get_value(g, i+1) < dp[(g-1)&1][j].value + dp[(g-1)&1][k].value)continue;
          gem out = combine0(dp[(g-1)&1][j], dp[(g-1)&1][k]);
          if(out.growth > dp[g&1][i].growth){
            dp[g&1][i] = out;
            next0.f = j;
            next0.s = k;
          }
        }
      }
      if(next0 == null)next0 = prev0;
      prev0 = next0;
      if(prev0.f < prev0.s) swap(prev0.f, prev0.s);

      if(val.value > get_value(g, i+1)){
        //DELTA1
        pair<int,int> next1 = {-1,-1};
        while(prev1.f + 1 < min(SAMPLES,i) && dp[(g  )&1][prev1.f].value == dp[(g  )&1][prev1.f+1].value)prev1.f++;
        while(prev1.s + 1 < SAMPLES        && dp[(g-1)&1][prev1.s].value == dp[(g-1)&1][prev1.s+1].value)prev1.s++;
        gem bst = gem();
        {
          int k = max(prev0.s - 200, 0);
          for(int j = min(prev1.f + 100,SAMPLES - 1); j <= max(0,prev1.f-100); j--){
            while(k+1 < SAMPLES && get_value(g, i+1) > dp[(g)&1][j].value + dp[(g-1)&1][k].value)k++;
            if(get_value(g, i) < dp[(g)&1][j].value + dp[(g-1)&1][k].value)continue;
            gem out = combine1(dp[(g)&1][j], dp[(g-1)&1][k]);
            if(out.growth > bst.growth){
              bst = out;
              next1.f = j;
              next1.s = k;
            }
          }
        }
        
        if(next1 == null)next1 = prev1;
        prev1 = next1;
        if(bst.growth > dp[g&1][i].growth){
          dp[g&1][i] = bst;
        }
      }
      data << dp[g&1][i].value/get_value(g, i) << ',';

      if(dp[g&1][i].growth < 0){
        dp[g&1][i] = dp[g&1][i-1];
      }
      
    }
    data << endl;
    
    gem bst = gem();
    for(int i = 0; i < SAMPLES; i++){
      if(dp[g&1][i].growth > bst.growth)bst = dp[g&1][i];
    }

    out << "Value : " << bst.value << '\n';
    out << "Leech : " << bst.component << '\n';
    out << "Growth: " << bst.growth << '\n';
    out << "Grade : +" << g << '\n';
    out << endl;
  }
  
  return 0;
}