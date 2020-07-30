#ifndef Data_
#define Data_

#include <vector>
#include <list>    
#include <string>
#include <iostream>
#include "math.h"
#include <cstdlib>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
//#include "gurobi_c++.h"

using namespace std;

struct Variable {
	int index;
	double LB;
	double UB;
	vector <double> unique_splits;
	Variable() {

	}
};

struct Frame {
	int tree_id;
	int splitnode_id;
	int variable;
	double criterion;
	int left_child;
	int left_child_is_leaf;
	int right_child;
	int right_child_is_leaf;

	Frame() {

	}
};

struct Split {
	int index;
	int tree_id;
	int leaf_id;
	double prob1;
	int variable;
	int alpha;
	double criterion;
	int split_node;

	Split() {

	}
};

struct Leaf {
	int index;
	int tree_id;
	double value;
	vector <Split*> leaf_splits; //define a name that put all split ids from Splits into list for each leaf 
	vector <int> left_splitnode;
	vector <int> right_splitnode;
	vector <double> leaf_splits_variable;
	Leaf() {

	}
};

struct Tree {
	int index;
	int n_leaf_nodes;
	int n_split_nodes;
	vector <Leaf*> tree_leafs;
	Tree() {

	}

};

struct LeafRange {
	int index;
	int tree_id;
	int feature_id;
	double lb;
	double ub;

	LeafRange() {

	}
};

struct SplitNode {
	int index;
	int tree_id;
	int variable;
	double criterion;
	vector <int> left_children; //define a name that put all leaf nodes derived from left branch of the split node, alpha = 0 
	vector <int> right_children;  // ........... right branch.........., alpha = 1
	SplitNode() {

	}
};



struct Data_Reader {
	const char*	Trees_file;
	const char*	Splits_file;
	const char*	variables_file;
	const char*	Frames_file;
	vector <Split*> splits;
	vector <Tree*> trees;
	vector <vector <Frame*>> frames;
	vector <vector <Leaf*>> leafs;
	vector <Variable*> data_variables;
	vector <vector <vector <LeafRange*>>> leafbounds;
	vector <vector <SplitNode*>> splitnodes;

	void read(const char*	_trees, const char*	_splits, const char*	_variables, const char*	_frames);
	void print();

	Data_Reader(const char*	_trees, const char*	_splits, const char*	_variables, const char*	_frames) {
		Trees_file = _trees;
		Splits_file = _splits;
		variables_file = _variables;
		Frames_file = _frames;
	}
};



#endif#pragma once
