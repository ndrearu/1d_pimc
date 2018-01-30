#include "../include/pathclass.h"
#include <iostream>

Path::Path(int Nt, int Np){
  Nt_ = Nt;
  Np_ = Np;
  // Total path obtained appending single-particle paths
  // [(path particle 0),(path particle 1),...,(path particle Np-1)]
  // = [(x0,x1,...,x_Nt-1),(x0,x1,...,x_Nt-1),...,(x0,x1,...,x_Nt-1)]
  path_ = new double [Nt*Np];
}

Path::~Path(){
  delete[] path_;
}

int Path::get_index(int nt, int np){
  // Defines the geometry above; fake index
  // if out of path
  if((nt<Nt_)&&(np<Np_)) return nt+np*Nt_;
  else return -1;
}

void Path::set_value(int n, double val){
  path_[n] = val;
}

double Path::get_value(int n){
  return path_[n];
}

int Path::get_particle(int n){
  return n/Nt_;
}

int Path::get_timeslice(int n){
  return n%Nt_;
}

int Path::get_next_timeslice(int n){
  int nt = get_timeslice(n);
  // Periodic time boundary conditions
  if(nt==Nt_-1) return n-(Nt_-1);
  else return n+1;
}

int Path::get_prev_timeslice(int n){
  int nt = get_timeslice(n);
  // Periodic time boundary conditions
  if(nt==0) return n+(Nt_-1);
  else return n-1;
}

int Path::get_right_neightbour(int n){
  int np = get_particle(n);
  // Fake index if no neigthbour exists
  if(np==Np_-1) return -1;
  else return n+Nt_;
}

int Path::get_left_neightbour(int n){
  int np = get_particle(n);
  // Fake index if no neigthbour exists
  if(np==0) return -1;
  else return n-Nt_;
}

double Path::get_right_distance(int n){
  int nr = get_right_neightbour(n);
  // fake distance if no right neightbour
  if(nr==-1) return -1;
  else return path_[nr]-path_[n];
}

double Path::get_left_distance(int n){
  int nl = get_left_neightbour(n);
  // fake distance if no left neightbour
  if(nl==-1) return -1;
  else return path_[n]-path_[nl];
}
