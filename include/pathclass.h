#ifndef PATHCLASS_H_
#define PATHCLASS_H_

#include <string>
#include <iostream>
#include <fstream>

class Path {

  int Nt_;
  int Np_;
  double* path_;

  public:

    Path(int Nt, int Np); // define the path
    ~Path();

    void set_value(int n, double val); // set and get values functions
    double get_value(int n);
    int get_index(int nt, int np);
    int get_particle(int n);
    int get_timeslice(int n);
    int get_next_timeslice(int n);
    int get_prev_timeslice(int n);
    int get_left_neightbour(int n);
    int get_right_neightbour(int n);
    double get_left_distance(int n);
    double get_right_distance(int n);

};

#endif
