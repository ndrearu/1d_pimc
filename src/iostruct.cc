#include "../include/iostruct.h"
#include "../include/potentials.h"

PIMCParams readPIMCParams(std::ifstream& infile){

	PIMCParams param;

	std::string in;
	infile >> in >> param.Np;
	infile >> in >> param.Nt;
	infile >> in >> param.tau;
	infile >> in >> param.mass;
	infile >> in >> param.boxlength;
	infile >> in >> param.V1form;
	infile >> in >> param.V1nofpars;
	for(int n=0; n<param.V1nofpars; n++) infile >> param.V1pars[n];
	infile >> in >> param.V2form;
	infile >> in >> param.V2nofpars;
	for(int n=0; n<param.V2nofpars; n++) infile >> param.V2pars[n];
	infile >> in >> param.MCsteps;
	infile >> in >> param.MCeach;
	infile >> in >> param.MCstart;

	return param;

}


bool parsePIMCParams(PIMCParams param){

	bool check = true;

	// check parameters
	if(param.Np < 0 || (int)param.Np!=param.Np){
		std::cerr << " Error: invalid Np value in init file\n";
		check = false;
	}
	if(param.Nt < 2 || (int)param.Nt!=param.Nt){
		std::cerr << " Error: invalid Nt value in init file\n";
		check = false;
	}
	if(param.tau <= 0.){
		std::cerr << " Error: invalid tau value in init file\n";
		check = false;
	}
	if(param.mass <= 0.){
		std::cerr << " Error: invalid mass value in init file\n";
		check = false;
	}
	if(param.boxlength <= 0){
		std::cerr << " Error: invalid boxlength value in init file\n";
		check = false;
	}
	std::function<double(double)> V1;
	V1 = V1wrapper(param.V1form, param.V1pars);
	if(!V1){
		std::cerr << " Error: not recognized V1 form\n";
		check = false;
	}
	if(param.V1nofpars<1 || (int)param.V1nofpars!=param.V1nofpars){
		std::cerr << " Error: invalid V1 parameters in init file\n";
		check = false;
	}
	std::function<double(double,double)> V2;
	V2 = V2wrapper(param.V2form, param.V2pars);
	if(!V2){
		std::cerr << " Error: not recognized V2 form\n";
		check = false;
	}

	if(param.V2nofpars<1 || (int)param.V2nofpars!=param.V2nofpars){
		std::cerr << " Error: invalid V2 parameters in init file\n";
		check = false;
	}

	if(param.MCsteps < 1 || (int)param.MCsteps!=param.MCsteps){
		std::cerr << " Error: invalid MCsteps value in init file\n";
		check = false;
	}

	if(param.MCeach < 1 || (int)param.MCeach!=param.MCeach ||
		 param.MCeach > param.MCsteps){
		std::cerr << " Error: invalid MCeach value in init file\n";
		check = false;
	}

	if(param.MCstart!=0 && param.MCstart!=1){
		std::cerr << " Error: invalid MCstart value in init file\n";
		check = false;
	} else if (param.MCstart==1) {
		// check conf file to be read for hot start
		std::ifstream inconfs(param.conffile);
		std::string line;
		int number_of_columns;
		int number_of_lines = 0;
		while(std::getline(inconfs, line)){
			number_of_columns = 0;
			number_of_lines++;
			double pos;
			std::stringstream line_stream(line);
			while(line_stream >> pos) number_of_columns++;
			if(number_of_columns!=param.Np*param.Nt){
				check = false;
				std::cerr << " Error: saved confs do not match at line "
				          << number_of_lines << " found " << number_of_columns
				          << " columns instead of " << param.Np*param.Nt << std::endl;
				break;
			}
		}
		inconfs.close();
	}

	return check;

}

void printPIMCParams(PIMCParams params){

	std::cout << " Np:\t\t" << params.Np;
	std::cout << "\n Nt:\t\t" << params.Nt;
	std::cout << "\n tau:\t\t" << params.tau;
	std::cout << "\n mass:\t\t" << params.mass;
	std::cout << "\n boxlength:\t" << params.boxlength;
	std::cout << "\n V1 form:\t" << params.V1form;
	std::cout << "\n V1 nofpars:\t" << params.V1nofpars;
	std::cout << "\n V1 pars:\t";
	for(int i=0; i<params.V1nofpars; i++) std::cout << params.V1pars[i] << " ";
	std::cout << "\n V2 form:\t" << params.V2form;
	std::cout << "\n V2 nofpars:\t" << params.V2nofpars;
	std::cout << "\n V2 pars:\t";
	for(int i=0; i<params.V2nofpars; i++) std::cout << params.V2pars[i] << " ";
	std::cout << "\n MCsteps:\t" << params.MCsteps;
	std::cout << "\n MCeach:\t" << params.MCeach;
	std::cout << "\n MCstart:\t" << params.MCstart;
	std::cout << std::endl;

}

std::string headerPIMCParams(PIMCParams params){

	std::ostringstream hs;
	hs << "# ";
	hs << params.Np << " " << params.Nt << " ";
	hs << params.tau << " " << params.mass << " " << params.boxlength << " ";
	hs << params.V1form << " " << params.V1nofpars << " ";
	for(int i=0; i<params.V1nofpars; i++) hs << params.V1pars[i] << " ";
	hs << params.V2form << " " << params.V2nofpars << " ";
	for(int i=0; i<params.V2nofpars; i++) hs << params.V2pars[i] << " ";
	hs << params.MCsteps << " " << params.MCeach << " " << params.MCstart;
	hs << "\n";

	return hs.str();

}
