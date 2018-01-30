#include "../include/potentials.h"

// single particle potentials
double V1nopotential(double*, double){
  return 0.;
}

double V1harmonic(double* par, double x){
  return par[0]*(x-par[1])*(x-par[1]);
}

double V1quartic(double* par, double x){
  return par[0]*x*x*x*x;
}

double V1cosine(double* par, double x){
  return par[0]*(1-cos((2*3.1415/par[1])*x+par[2]));
}

double V1harmonicplus(double* par, double x){
  return par[0]*x*x + par[1]*(1-cos((2*3.1415/par[2])*x+par[3]));
}

// pair-interaction potentials
double V2nointeraction(double*, double, double){
  return 0.;
}

double V2coulomb(double* par, double x1, double x2){
  double epsilon=0.000001; // avoids the pole
  if(fabs((x1-x2)/(x1+x2))<epsilon)
    return 0.5*par[0]*epsilon*fabs(x1+x2);
  else
    return 0.5*par[0]/fabs(x1-x2);
}


// potential function wrappers
std::function<double(double)> V1wrapper(std::string V1form, double* V1par){

  using namespace std::placeholders; //to use _1,_2,...
  std::function<double(double)> V1;
  if(V1form=="nopotential") V1 = std::bind(V1nopotential,V1par,_1);
  else if(V1form=="harmonic") V1 = std::bind(V1harmonic,V1par,_1);
  else if(V1form=="quartic") V1 = std::bind(V1quartic,V1par,_1);
  else if(V1form=="cosine") V1 = std::bind(V1cosine,V1par,_1);
  else if(V1form=="harmonicplus") V1 = std::bind(V1harmonicplus,V1par,_1);
  else return NULL;

  return V1;

}

std::function<double(double,double)> V2wrapper(std::string V2form, double* V2par){

  using namespace std::placeholders; //to use _1,_2,...
  std::function<double(double,double)> V2;
  if(V2form=="nointeraction") V2 = std::bind(V2nointeraction,V2par,_1,_2);
  else if(V2form=="coulomb") V2 = std::bind(V2coulomb,V2par,_1,_2);
  else return NULL;

  return V2;

}
