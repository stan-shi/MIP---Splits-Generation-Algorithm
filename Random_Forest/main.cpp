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
#include "Data.h"
#include "Models.h"
#include <unordered_map>

#include "gurobi_c++.h"

using namespace std;
int main() {
	cout << "mark1" << endl;
	for (int i =7; i < 8; ++i) {
	
		ofstream myfile("test.txt", ios_base::app | ios_base::out);
		myfile << "#\t" << i << endl;
		double time_limit = 7200;
		const char*     tree_file;
		const char*     split_file;
		const char*     var_file;
		const char*     frame_file;
		cout << "mark2" << endl;
		string file;
		stringstream ss;
		ss << "toy_example\\T" << i << ".txt"; 
		string file1;
		stringstream sss;
		sss << "toy_example\\S" << i << ".txt";
		string file2;
		stringstream ssss;
		ssss << "toy_example\\F" << i << ".txt";
		string file3;
		stringstream sssss;
		sssss << "toy_example\\C" << i << ".txt";
		file = ss.str();
		file1 = sss.str();
		file2 = ssss.str();
		file3 = sssss.str();

		tree_file = file.c_str();
		split_file = file1.c_str();
		var_file = file2.c_str();
		frame_file = file3.c_str();

		Data_Reader* data = new Data_Reader(tree_file, split_file, var_file, frame_file);
		cout << "mark3" << endl;
		data->read(tree_file, split_file, var_file, frame_file);
		cout << "mark4" << endl;
		//data->print();

		
		Model* mod = new Model(data);
		mod->Splits_algorithm(data, myfile);
		



	}
}


