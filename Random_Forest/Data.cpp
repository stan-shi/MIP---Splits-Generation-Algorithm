#include "Data.h"
using namespace std;


void Data_Reader::read(const char*	_trees, const char*	_splits, const char*	_variables, const char*	_frames) {
	int reader_int;
	double reader_double;

	ifstream input;
	input.open(_trees);
	int n_trees;
	input >> reader_int;
	n_trees = reader_int;

	for (int t = 0; t < n_trees; ++t) {
		Tree* new_tree = new Tree();
		for (int i = 0; i < 2; ++i) {
			if (i == 0) {
				input >> reader_int;
				new_tree->index = reader_int;
			}
			if (i == 1) {
				input >> reader_int;
				new_tree->n_leaf_nodes = reader_int;
			}
		}
		trees.push_back(new_tree); // Tree is the object we built earlier. trees is the vector we build after put all the info we read.
	}

	for (int t = 0; t < n_trees; ++t) {
		trees[t]->n_split_nodes = trees[t]->n_leaf_nodes - 1; // Tree is the object we built earlier. trees is the vector we build after put all the info we read.
	}


	ifstream input1;
	input1.open(_splits);
	int n_splits;
	input1 >> reader_int;
	n_splits = reader_int;

	for (int s = 0; s < n_splits; ++s) {
		Split* new_split = new Split();
		for (int i = 0; i < 8; ++i) {
			if (i == 0) {
				input1 >> reader_int;
				new_split->index = reader_int;
			}
			if (i == 1) {
				input1 >> reader_int;
				new_split->tree_id = reader_int;
			}
			if (i == 2) {
				input1 >> reader_int;
				new_split->leaf_id = reader_int;
			}
			if (i == 3) {
				input1 >> reader_double;
				new_split->prob1 = reader_double;
			}
			if (i == 4) {
				input1 >> reader_int;
				new_split->variable = reader_int;
			}
			if (i == 5) {
				input1 >> reader_int;
				new_split->alpha = reader_int;
			}
			if (i == 6) {
				input1 >> reader_double;
				new_split->criterion = reader_double;
			}
			if (i == 7) {
				input1 >> reader_int;
				new_split->split_node = reader_int;
			}
		}
		splits.push_back(new_split);
	}

	ifstream input2;
	input2.open(_variables);
	int n_vars;
	input2 >> reader_int;
	n_vars = reader_int;
	for (int v = 0; v < n_vars; ++v) {
		Variable* new_var = new Variable(); //every line of variable info (new_var) is read into Varibale that is defined/framed in Data.h. new_var should include three columns: index, LB, UB as are defined
		for (int i = 0; i < 3; ++i) {
			if (i == 0) {
				input2 >> reader_int;
				new_var->index = reader_int;
			}
			if (i == 1) {
				input2 >> reader_double;
				new_var->LB = reader_double;
			}
			if (i == 2) {
				input2 >> reader_double;
				new_var->UB = reader_double;
			}
		}
		data_variables.push_back(new_var);
	}

	ifstream input3;
	input3.open(_frames);

	frames.resize(n_trees);
	for (int t = 0; t < n_trees; ++t) {
		for (int s = 0; s < trees[t]->n_split_nodes; ++s) {
			Frame* new_frame = new Frame();
			for (int i = 0; i < 8; ++i) {
				if (i == 0) {
					input3 >> reader_int;
					new_frame->tree_id = reader_int;
				}
				if (i == 1) {
					input3 >> reader_int;
					new_frame->splitnode_id = reader_int;
				}
				if (i == 2) {
					input3 >> reader_int;
					new_frame->variable = reader_int;
				}
				if (i == 3) {
					input3 >> reader_double;
					new_frame->criterion = reader_double;
				}
				if (i == 4) {
					input3 >> reader_int;
					new_frame->left_child = reader_int;
				}
				if (i == 5) {
					input3 >> reader_int;
					new_frame->left_child_is_leaf = reader_int;
				}
				if (i == 6) {
					input3 >> reader_int;
					new_frame->right_child = reader_int;
				}
				if (i == 7) {
					input3 >> reader_int;
					new_frame->right_child_is_leaf = reader_int;
				}
			}
			frames[t].push_back(new_frame);
		}
	}


	// read unique splits to variable
	for (int v = 0; v < n_vars; ++v) {
		for (int s = 0; s < n_splits; ++s) {
			if (splits[s]->variable == v) {
				data_variables[v]->unique_splits.push_back(splits[s]->criterion);
				sort(data_variables[v]->unique_splits.begin(), data_variables[v]->unique_splits.end());
				data_variables[v]->unique_splits.erase(unique(data_variables[v]->unique_splits.begin(), data_variables[v]->unique_splits.end()), data_variables[v]->unique_splits.end());
			}
		}
	}


	leafs.resize(n_trees);
	for (int t = 0; t < n_trees; ++t) {
		for (int l = 0; l < trees[t]->n_leaf_nodes; ++l) {
			Leaf* new_leaf = new Leaf(); // Leaf is defined previously. Now we insert info for each leaf in it.
			new_leaf->index = l; // 
			new_leaf->tree_id = t;
			trees[t]->tree_leafs.push_back(new_leaf);
			leafs[t].push_back(new_leaf);
		}
	}

	for (int s = 0; s < n_splits; ++s) {
		leafs[splits[s]->tree_id][splits[s]->leaf_id]->leaf_splits.push_back(splits[s]);
		leafs[splits[s]->tree_id][splits[s]->leaf_id]->leaf_splits_variable.push_back(splits[s]->variable);
		leafs[splits[s]->tree_id][splits[s]->leaf_id]->value = splits[s]->prob1;
		if (splits[s]->alpha == 0) {
			leafs[splits[s]->tree_id][splits[s]->leaf_id]->left_splitnode.push_back(splits[s]->split_node);
		}
		else {
			leafs[splits[s]->tree_id][splits[s]->leaf_id]->right_splitnode.push_back(splits[s]->split_node);
		}
	}


	// split nodes 

	splitnodes.resize(n_trees);
	for (int t = 0; t < n_trees; ++t) {
		//splitnodes[t].resize(trees[t]->n_split_nodes);
		for (int s = 0; s < trees[t]->n_split_nodes; ++s) {
			SplitNode* new_splitnode = new SplitNode(); // SplitNode is defined previously. Now we insert info for each split node in it.
			new_splitnode->index = s; // 
			new_splitnode->tree_id = t;
			splitnodes[t].push_back(new_splitnode);
			//splitnodes[t][s] = new_splitnode;
		}
	}

	for (int s = 0; s < n_splits; ++s) {
		splitnodes[splits[s]->tree_id][splits[s]->split_node]->variable = splits[s]->variable;
		splitnodes[splits[s]->tree_id][splits[s]->split_node]->criterion = splits[s]->criterion;
		if (splits[s]->alpha == 0) {
			splitnodes[splits[s]->tree_id][splits[s]->split_node]->left_children.push_back(splits[s]->leaf_id);
		}
		else {
			splitnodes[splits[s]->tree_id][splits[s]->split_node]->right_children.push_back(splits[s]->leaf_id);
		}
	}

}


void Data_Reader::print() {
	cout << "Variables:" << endl << endl;
	int count = 0;
	for (int v = 0; v < data_variables.size(); ++v) {
		count += static_cast<int>(data_variables[v]->unique_splits.size());

		cout << data_variables[v]->index << "\t" << data_variables[v]->LB << "\t" << data_variables[v]->UB << "\t";
		for (int u = 0; u < data_variables[v]->unique_splits.size(); ++u) {
			cout << data_variables[v]->unique_splits[u] << "\t";
		}
		cout << endl;
	}


	cout << endl << endl << "Frames: " << endl << endl;

	for (int t = 0; t < trees.size(); ++t) {
		for (int s = 0; s < trees[t]->n_split_nodes; ++s) {
			cout << t << "\t" << s << "\t" << frames[t][s]->variable << "\t" << frames[t][s]->criterion << "\t" << frames[t][s]->left_child << "\t" << frames[t][s]->left_child_is_leaf << "\t" << frames[t][s]->right_child << "\t" << frames[t][s]->right_child_is_leaf << endl;
		}
	}


	//leafs
	cout << endl << endl << "leafs" << endl << endl;
	for (int t = 0; t < trees.size(); ++t) {
		for (int l = 0; l < leafs[t].size(); ++l) {
			cout << leafs[t][l]->tree_id << "\t" << leafs[t][l]->index << "\t" << leafs[t][l]->value << "\t";
			cout << "left" << "\t";
			for (int s = 0; s < leafs[t][l]->left_splitnode.size(); ++s) {
				cout << leafs[t][l]->left_splitnode[s] << "\t";
			}
			cout << "right" << "\t";
			for (int r = 0; r < leafs[t][l]->right_splitnode.size(); ++r) {
				cout << leafs[t][l]->right_splitnode[r] << "\t";
			}
			cout << endl;
		}
	}
	
}

