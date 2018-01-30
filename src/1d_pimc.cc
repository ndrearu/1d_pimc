#include <iostream>
#include <fstream>
#include <cstdlib>

#include "../include/iostruct.h"
#include "../include/pathclass.h"
#include "../include/pimcclass.h"

int main(int argc, char* argv[]) {

  // init input
  std::string workdir;
  if(argc!=2){
    std::cout << "Usage: " << argv[0] << " WORKDIR" << std::endl;
    return 0;
  } else {
    workdir = argv[1];
    if(workdir.back()!='/') workdir+="/";
  }

  std::cout << "\n /////1DPIMC/////" << std::endl;

  // read parameters from file and print
  std::cout << "\n [Reading input file]" << std::endl;
  std::ifstream init(workdir+"init");
  if(!init){
    std::cerr << " Error: no init file found." << std::endl;
    init.close();
    return 1;
  }
  PIMCParams params = readPIMCParams(init);
  params.workdir  = workdir;
  params.conffile = workdir + "confs";
  params.measfile = workdir + "meas";
  params.accpfile = workdir + "accpt";
  init.close();

  // sanity check of all inputs, exit if errors
  bool check = parsePIMCParams(params);
  if(!check){
    std::cerr << " Aborted." << std::endl;
    return 1;
  }
  printPIMCParams(params);

  // define files and open streams
  std::cout << "\n [PIMC setup]" << std::endl;
  std::ofstream outconf;
  std::ofstream outmeas;
  std::ofstream outaccp;
  if(params.MCstart){
    outconf.open(params.conffile, std::ios_base::app);
    outmeas.open(params.measfile, std::ios_base::app);
    outaccp.open(params.accpfile, std::ios_base::app);
  } else {
    outconf.open(params.conffile);
    outmeas.open(params.measfile);
    outaccp.open(params.accpfile);
  }
  std::string header = headerPIMCParams(params);
  outconf << header;
  outmeas << header;
  outaccp << header;

  // define PIMCClass of the system
  PIMCClass pimcclass(params);

  /*** Monte-Carlo run ***/
  std::cout << "\n [PIMC run]";
  std::cout << "\n Running: ";
  double accpt=0;
  for(int nstep=0; nstep<params.MCsteps; nstep++){

    // print each...; first conf printed only if cold start
    if(nstep%params.MCeach==0 && (!params.MCstart || nstep>0)){

      for(int n=0; n<params.Np*params.Nt; n++){       // save conf
        outconf << pimcclass.get_path_value(n) << " ";
      }
      outconf << std::endl;
      outmeas << pimcclass.get_Action() << std::endl; // print meas
      if(nstep>0) outaccp << accpt << std::endl; // print acceptance

    }

    if(nstep%(params.MCsteps/25)==0) std::cout << "=" << std::flush;
    accpt = pimcclass.update_path();

  }

  std::cout << " Done! \n Data printed on outfiles.";
  std::cout << "\n\n ////////////////\n"  << std::endl;
  /***********************/

  //close outfiles
  outconf.close();
  outmeas.close();
  outaccp.close();

}
