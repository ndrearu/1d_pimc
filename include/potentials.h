#ifndef POTENTIALS_H_
#define POTENTIALS_H_

#include <cmath>
#include <iostream>
#include <string>
#include <functional>

// Add here the declaration of new V1 single particle potentials that will be
// defined in lib/potentials.cc and must be added to the wrapper/selector
double V1nopotential(double*,double);
double V1harmonic(double*,double);
double V1quartic(double*,double);
double V1cosine(double*,double);
double V1harmonicplus(double*,double);

// Add here the V2 pair potentials that will be defined in lib/potentials.cc and
// must be added to the wrapper/selector
double V2nointeraction(double*,double,double);
double V2coulomb(double*,double,double);

// Potential listed before are selected and wrapped by the following funtion
// that is called in the initialization of a PIMCClass class (see
// lib/pimclass.cc)
std::function<double(double)> V1wrapper(std::string, double*);
std::function<double(double,double)> V2wrapper(std::string, double*);

#endif
