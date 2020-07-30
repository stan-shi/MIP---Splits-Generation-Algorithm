#ifndef IP_MODELS_
#define IP_MODELS_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include<tuple>
#include <random>
#include <sstream>
#include <string>
#include <list>
#include <cmath>
#include "math.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include "gurobi_c++.h"
#include "Data.h"

using namespace std;

struct Model {

	Data_Reader* inst;

	void Misic_base_model(Data_Reader* _inst, ofstream& myfile);

	void Splits_algorithm(Data_Reader* _inst, ofstream& myfile);

	Model(Data_Reader* _inst) {
		inst = _inst;
	
	}
};


#endif