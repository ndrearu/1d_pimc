#ifndef PIMCCLASS_H_
#define PIMCCLASS_H_

#include "../include/pathclass.h"
#include "../include/potentials.h"
#include "../include/iostruct.h"

#include <functional>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <random>

#ifdef _OPENMP
  // compiled with -fopenmp _OPENMP is defined
  #include <omp.h>
  #define omp_num_threads 2 // max used number_of_threads
#else
  #define omp_get_thread_num() 0
  #define omp_get_max_threads() 1
#endif

class PIMCClass {

  void init_path(); // init path

  // get energy contribs from the configuration
  double get_Ekin_local(int n);
  double get_V1_local(int n);
  double get_V2_local(int n);
  double get_Action_local(int n);

  // potentials
  std::function<double(double)> V1;
  std::function<double(double,double)> V2;

  // random number generators - one per thread
  std::mt19937 *randgen_;

  public:

    PIMCClass(PIMCParams param); // defines the class
    ~PIMCClass();

    PIMCParams *param_; // parameters used by the class
    Path *path_;        // the path

    double PIMCdelta;     // pimc delta
    double boxlength;     // lenght of the system box
    double update_path(); // update path with Metropolis
    double get_Action();  // compute action
    double get_path_value(int n);

    void print_info();    // print

};

#endif
