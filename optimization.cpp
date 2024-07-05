#include <bits/stdc++.h>

using namespace std;

const double a0 = 0.88, a1 = 0.89, b0 = 0.5, b1 = 0.44;

int MANT = 54;

double A,B;

double f0(double x) {
  return a0*pow(x,B)+b0*pow(1-x,B)-exp2(A);
}

double f1(double x) {
  return exp2(A)*a1*pow(x,B)+b1*pow(1-x,B)-exp2(A);
}

bool q0(){
  double lx = 0.0, rx = 1.0;
  for(int i = 0; i < 100; i++){
    double m1 = lx + (rx-lx)/3;
    double m2 = rx - (rx-lx)/3;
    double v1 = f0(m1);
    double v2 = f0(m2);
    if(v1 > 0.0 or v2 > 0.0)return true;
    if(v1 < v2)lx = m1;
    else rx = m2;
  }
  return false;
}

bool q1(){
  double lx = 0.0, rx = 1.0;
  for(int i = 0; i < 100; i++){
    double m1 = lx + (rx-lx)/3;
    double m2 = rx - (rx-lx)/3;
    double v1 = f1(m1);
    double v2 = f1(m2);
    if(v1 > 0.0 or v2 > 0.0)return true;
    if(v1 < v2)lx = m1;
    else rx = m2;
  }
  return false;
}

bool querry(double C){
  double mA = 0.0, MA = 1.0; 
  for(int i = 0; i < 100; i++){
    double mid = (mA + MA) / 2.0;
    A = mid;
    B = C - mid;
    bool a0 = q0();
    bool a1 = q1();
    if(a0 and a1){
      return false;
    }
    else if(a0){
      mA = mid;
    }
    else if(a1){
      MA = mid;
    }
    else{
      return true;
    } 
  }
  cout << "!!!" << C << " " << A << " " << B << endl;
  exit(0);
}

int main() {
  double BA, BB, C;
  double mC = 0.0, MC = 1.0; 
  for(int i = 0; i < 100; i++){
    double mid = (mC + MC) / 2.0;
    if(querry(mid)){
      MC = mid;
      BA = A;
      BB = B;
      C = mid;
    }else{
      mC = mid;
    }
  }
  cout << setprecision(30);
  cout << C << " " << A << " " << B << endl;
}
