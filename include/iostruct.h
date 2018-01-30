#ifndef IOSTRUCT_H_
#define IOSTRUCT_H_

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

struct PIMCParams {

  int nofparams = 5+6+3+1; // number of parameters

  int Np;           // number of particles
  int Nt;           // time slices
  double tau;       // imaginary tau spacing
  double mass;      // particle mass
  double boxlength; // system box length

  std::string V1form; // 1-body or trap potential type
  double V1pars[9];   // parameters of the potential
  double V1nofpars;   // number of parameters
  std::string V2form; // 2-body interaction potential
  double V2pars[9];
  double V2nofpars;

  int MCsteps;  // number of MC updates
  int MCeach;   // measure every
  bool MCstart; // start from cold or hot configuration

  std::string workdir;   // working directory by command line
  std::string conffile;
  std::string measfile;
  std::string accpfile;

};

PIMCParams readPIMCParams(std::ifstream& infile);
bool parsePIMCParams(PIMCParams params);
void printPIMCParams(PIMCParams params);
std::string headerPIMCParams(PIMCParams params);

#endif
