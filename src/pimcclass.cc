#include "../include/pimcclass.h"

PIMCClass::PIMCClass(PIMCParams param){

  param_ = &param;
  path_ = new Path(param_->Nt,param_->Np); // define path

  #ifdef _OPENMP // set number of threads used
  omp_set_num_threads(omp_num_threads);
  #endif

  PIMCdelta = 2*sqrt(param_->tau); // define PIMC delta
  boxlength = param_->boxlength;   // define boxlength

  // initialize path
  if(param_->MCstart){  // init path from saved conf

		std::ifstream inconfs(param_->conffile);
		std::string line, last_line;
		while(std::getline(inconfs, line)) last_line = line;  // keep last line
		inconfs.close();

		double pos;
		std::stringstream last_line_stream(last_line); // create stream
		for(int n=0; last_line_stream >> pos; n++) path_->set_value(n,pos);

  } else {

    double sep = PIMCdelta;
    if(sep*(param_->Np-1)>boxlength){
      sep = boxlength/(param_->Np+1);
    }

    #pragma omp parallel for
    for(int np=0; np<param_->Np; np++){
      double pos = sep*(np-0.5*(param_->Np-1));
      for(int nt=0; nt<param_->Nt; nt++){
        int n = path_->get_index(nt,np);
        path_->set_value(n,pos);
      }
    }

  }

  // set potentials
  V1 = V1wrapper(param_->V1form, param_->V1pars);
  V2 = V2wrapper(param_->V2form, param_->V2pars);

  // define random number generator(s)
  randgen_ = new std::mt19937 [omp_get_max_threads()];
  for(int i=0; i<omp_get_max_threads(); i++){
    std::random_device rd;
    randgen_[i].seed(rd());
  }

  // print informations about PIMC
	print_info();

}


PIMCClass::~PIMCClass(){

  delete path_;      // free path
  delete[] randgen_; // free rand generators

}

double PIMCClass::get_path_value(int n){
  return path_->get_value(n);
}

double PIMCClass::get_Ekin_local(int n){

  // Kinetik energy with right/next derivative
  int n_next = path_->get_next_timeslice(n);
  double x = path_->get_value(n);
  double x_next = path_->get_value(n_next);

  return 0.5*(param_->mass)*(x_next-x)*(x_next-x)/(param_->tau*param_->tau);

}

double PIMCClass::get_V1_local(int n){

  double x = path_->get_value(n);
  return V1(x);

}

double PIMCClass::get_V2_local(int n){

  double x = path_->get_value(n);
  double V = 0.;

  int nl = path_->get_left_neightbour(n);
  for(; nl>=0; nl=path_->get_left_neightbour(nl)){
    // add contributes from the left
    double xl = path_->get_value(nl);
    V += V2(x, xl);
  }
  int nr = path_->get_right_neightbour(n);
  for(; nr>=0; nr=path_->get_right_neightbour(nr)){
    // add contributes from the right
    double xr = path_->get_value(nr);
    V += V2(x, xr);
  }

  return V;

}

double PIMCClass::get_Action_local(int n){

  double S = 0.;
  int nb = path_->get_prev_timeslice(n);
  S += get_Ekin_local(n);
  S += get_Ekin_local(nb);
  S += get_V1_local(n);
  S += get_V2_local(n);
  return (param_->tau)*S;

}

double PIMCClass::get_Action(){

  double S = 0.;
  #pragma omp parallel for reduction(+:S)
  for(int n=0; n<(param_->Np*param_->Nt); n++){
    S += get_Ekin_local(n);
    S += get_V1_local(n);
    S += 0.5*get_V2_local(n);
  }
  return (param_->tau)*S;

}

double PIMCClass::update_path(){

  double acc = 0;
  for(int eo=0; eo<2; eo++){

    #pragma omp parallel for reduction(+:acc)
    for(int nt=eo; nt<param_->Nt; nt+=2){

      std::uniform_real_distribution<double> randu(0,1);

      for(int np=0; np<param_->Np; np++){

        int n = path_->get_index(nt,np);

        // propose random update
        double xinit = path_->get_value(n);
        double xrand = xinit - PIMCdelta;
        xrand += 2*PIMCdelta*randu(randgen_[omp_get_thread_num()]);

	      // check if random update is possible (no crossings)
	      double drand = xrand - xinit;
	      double dr = path_->get_right_distance(n);
        if (dr<0) dr = 0.5*boxlength - xinit;
	      double dl = path_->get_left_distance(n);
        if (dl<0) dl = 0.5*boxlength + xinit;

	      bool okrand = false;
	      if(drand>0 && drand<+dr) okrand = true;
	      else if(drand<0 && drand>-dl) okrand = true;

	      if(okrand){ //try update if xrand is allowed

	         // flip and compute action
	         double dS = -get_Action_local(n);
	         path_->set_value(n,xrand);
	         dS += get_Action_local(n);

	         // Metropolis accept or reject
	         if(randu(randgen_[omp_get_thread_num()])<exp(-dS)) acc++;
	         else path_->set_value(n,xinit);
        }

      } // closes particle loop
    } // closes time steps loop
  } // closes odd-even loop

  return acc/(param_->Np*param_->Nt);

}

void PIMCClass::print_info(){

  std::cout << " Working directory:\t" << param_->workdir;
	std::cout << "\n Configuration file:\t" << param_->conffile;
	std::cout << "\n Measures file:\t\t" << param_->measfile;
	std::cout << "\n Acceptance file:\t" << param_->accpfile;
  double soc_in_kb = sizeof(double)*param_->Np*param_->Nt*1./1024;
  std::cout << "\n Path config size is " << soc_in_kb << "Kb.";
	std::cout << "\n Number of saved confs is ";
	std::cout << param_->MCsteps/param_->MCeach;
  std::cout << "\n Estimated configs file size ";
  std::cout << soc_in_kb*param_->MCsteps/(param_->MCeach*1024) << "Mb.";
  std::cout << "\n System temperature is " << 1./(param_->Nt*param_->tau);
  std::cout << "\n PIMC Delta value set to " << PIMCdelta;

	if(param_->MCstart){

		std::cout << "\n Path initialized with hot start from saved confs";
		std::ifstream inconfs(param_->workdir+"confs");
		std::string line;
		int number_of_confs = 0;
		while(std::getline(inconfs, line)) number_of_confs++;
		std::cout << "\n Number of confs read " << number_of_confs;
		inconfs.close();

	} else{

		std::cout << "\n Path initialized with cold start";
		std::cout << "\n Straigth paths in range [" << path_->get_value(0);
		std::cout << "," << path_->get_value(param_->Np*param_->Nt-1) << "]";

}

  #ifdef _OPENMP
  std::cout << "\n Defined _OPENMP: using ";
	std::cout << omp_get_max_threads() << " threads";
  #else
  std::cout << "\n No _OPENMP defined, run wih 1 thread";
  #endif

	std::cout << "\n";

}
