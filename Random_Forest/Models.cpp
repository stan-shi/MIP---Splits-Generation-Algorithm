#include "Models.h"


GRBEnv env;
void Model::Misic_base_model(Data_Reader* inst, ofstream& myfile) {
	cout << endl << "================================Misic MIP Model - Direct======================" << endl << endl;
	//GRBEnv env;
	try {
		GRBModel model(env);
		GRBVar** x;
		GRBVar** y;
		//GRBVar* z_LB;
		//GRBVar* z_UB;
		//cout << "p1" << endl; // function of 'p1'?

		x = new GRBVar*[inst->data_variables.size()];
		int x_number = 0;
		for (int v = 0; v < inst->data_variables.size(); ++v) {
			x[v] = new GRBVar[inst->data_variables[v]->unique_splits.size()];
			for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
				x[v][u] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
				x_number += 1;
			}
		}

		cout << x_number << endl;
		myfile << "N_levels\t" << x_number << endl;


		//cout << "p2" << endl;
		y = new GRBVar*[inst->trees.size()];
		for (int t = 0; t < inst->trees.size(); ++t) {
			y[t] = new GRBVar[inst->trees[t]->tree_leafs.size()];
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				y[t][l] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS);
			}
		}
		
		//cout << "p3" << endl;
		model.update();

		////Adding constraints

		//// adding solution of our model as a constraint
		//int Y[5][16] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0} };
		//for (int t = 0; t < inst->trees.size(); ++t) {
		//	for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
		//		model.addConstr(y[t][l] == Y[t][l]);
		//	}
		//}

		//cout << "correct solution" << endl;

		for (int t = 0; t < inst->trees.size(); ++t) {
			GRBLinExpr equ = 0.0;
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				equ += y[t][l];
			}
			model.addConstr(equ == 1);
		}

		//cout << "p4" << endl;

		for (int t = 0; t < inst->trees.size(); ++t) {
			for (int s = 0; s < inst->trees[t]->n_split_nodes; ++s) {
				// left branch of split nodes
				GRBLinExpr equLS = 0.0;
				for (int l = 0; l < inst->splitnodes[t][s]->left_children.size(); ++l) {
					int n = inst->splitnodes[t][s]->left_children[l];
					equLS += y[t][n];
				}
				int v = inst->splitnodes[t][s]->variable;
				double c = inst->splitnodes[t][s]->criterion;
				std::vector<double> us = inst->data_variables[v]->unique_splits;
				std::vector<double>::iterator it = std::find(us.begin(), us.end(), c);
				auto k = std::distance(us.begin(), it);
				model.addConstr(equLS <= x[inst->splitnodes[t][s]->variable][k]);
			}
		}

		//cout << "p5" << endl;

		for (int t = 0; t < inst->trees.size(); ++t) {
			for (int s = 0; s < inst->trees[t]->n_split_nodes; ++s) {
				// right branch of split nodes
				GRBLinExpr equRS = 0.0;
				for (int l = 0; l < inst->splitnodes[t][s]->right_children.size(); ++l) {
					int n = inst->splitnodes[t][s]->right_children[l];
					equRS += y[t][n];
				}
				int v = inst->splitnodes[t][s]->variable;
				double c = inst->splitnodes[t][s]->criterion;
				std::vector<double> us = inst->data_variables[v]->unique_splits;
				std::vector<double>::iterator it = std::find(us.begin(), us.end(), c);
				auto k = std::distance(us.begin(), it);
				model.addConstr(equRS <= 1 - x[inst->splitnodes[t][s]->variable][k]);
			}
		}



		//cout << "p6" << endl;

		for (int f = 0; f < inst->data_variables.size(); ++f) {
			if (inst->data_variables[f]->unique_splits.size() > 1) {
				for (int s = 1; s < inst->data_variables[f]->unique_splits.size(); ++s) {
					model.addConstr(x[f][s - 1] <= x[f][s]);
				}
			}
		}

		cout << "p7" << endl;

		GRBLinExpr obj = 0.0;
		for (int t = 0; t < inst->trees.size(); ++t) {
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				obj += inst->leafs[t][l]->value * y[t][l];
			}
		}
		//obj = obj / inst->trees.size()


		model.update();
		cout << "obj added" << endl;
		model.setObjective(obj, GRB_MAXIMIZE);
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 7200);
		//model.getEnv().set(GRB_IntParam_SolutionLimit, 1);
		//model.getEnv().set(GRB_DoubleParam_NodeLimit, 0);
		model.getEnv().set(GRB_IntParam_OutputFlag, 1);
		model.getEnv().set(GRB_IntParam_PreCrush, 1);
		model.getEnv().set(GRB_IntParam_DualReductions, 0);
		model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
		// master.getEnv().set(GRB_IntParam_VarBranch,  0);
		model.update();
		//cout << "solve gurobi" << endl;
		//model.write("BADMIP.lp");

		model.optimize();

		//// print decision variable value
		//for (int t = 0; t < inst->trees.size(); ++t) {
		//	cout << "tree" << t << "\t";
		//	for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
		//		if (y[t][l].get(GRB_DoubleAttr_X) > 0.9) {
		//			cout << "leaf" << l << "\t";
		//		}
		//		//cout << y[t][l].get(GRB_DoubleAttr_X) << "\t";
		//	}
		//	cout << endl;
		//}



		//for (int v = 0; v < inst->data_variables.size(); ++v) {
		//	cout << "x" << v << "\t";
		//	for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
		//		cout << x[v][u].get(GRB_DoubleAttr_X) << "\t";
		//	}
		//	cout << endl;
		//}

		//// map x to leaf based on the solution "GetLeaf"

		//for (int t = 0; t < inst->trees.size(); ++t) {
		//	cout << "Tree" << t << "\t";
		//	int node_id = 0;
		//	int leaf_id;
		//	while (true) {
		//		int var = inst->frames[t][node_id]->variable;
		//		double cri = inst->frames[t][node_id]->criterion;
		//		vector <double> v = inst->data_variables[var]->unique_splits;
		//		std::vector<double>::iterator itr = std::find(v.begin(), v.end(), cri);
		//		auto j = std::distance(v.begin(), itr);

		//		if (x[var][j].get(GRB_DoubleAttr_X) == 1) {
		//			if (inst->frames[t][node_id]->left_child_is_leaf == 1) {
		//				leaf_id = inst->frames[t][node_id]->left_child;
		//				break;
		//			}
		//			else {
		//				node_id = inst->frames[t][node_id]->left_child;
		//			}
		//		}
		//		else {
		//			if (inst->frames[t][node_id]->right_child_is_leaf == 1) {
		//				leaf_id = inst->frames[t][node_id]->right_child;
		//				break;
		//			}
		//			else {
		//				node_id = inst->frames[t][node_id]->right_child;
		//			}
		//		}

		//	}
		//	cout << leaf_id << endl;

		//}

		//// printing stuff to file

		if (model.get(GRB_IntAttr_Status) != 3) {
			myfile << model.get(GRB_DoubleAttr_ObjVal) << "\t" << model.get(GRB_DoubleAttr_ObjBound) << "\t" << model.get(GRB_DoubleAttr_Runtime) << endl;
		}
		else {
			myfile << "model is infeasible" << endl;
		}
	}
	catch (GRBException e) {
		cout << "Error number: " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
		exit(1);
	}
	catch (...) {
		cout << "Other error ... " << endl;
		exit(1);
	}

}


class callback_forest_for_split_constraint : public GRBCallback {

public:
	// put objects required

	GRBVar** x;
	GRBVar** y;
	Data_Reader* inst_callback;
	Model* Model_callback;
	callback_forest_for_split_constraint(GRBVar** _x, GRBVar** _y, Data_Reader* _inst) {

		//cout << "We are at a callback .. " << endl;
		x = _x;
		y = _y;
		inst_callback = _inst;
		//Model_callback = _Model;

	};

protected:

	void callback() {
		try {

			if (where == GRB_CB_MIPSOL) {

				//// ADD CUTS BELOW THIS
				double obj = getDoubleInfo(GRB_CB_MIPSOL_OBJBND);
				//cout << "upper bound:   " << obj << endl;
				vector < vector< int>> fixed_x(inst_callback->data_variables.size());
				for (int v = 0; v < inst_callback->data_variables.size(); ++v) {
					fixed_x[v].resize(inst_callback->data_variables[v]->unique_splits.size());
					for (int u = 0; u < inst_callback->data_variables[v]->unique_splits.size(); ++u) {
						fixed_x[v][u] = int(round(getSolution(x[v][u])));
						//cout << int(getSolution(x[v][u])) << endl;
					}
				}

				vector < vector< double>> fixed_y(inst_callback->trees.size());
				for (int v = 0; v < inst_callback->trees.size(); ++v) {
					fixed_y[v].resize(inst_callback->trees[v]->n_leaf_nodes);
					for (int u = 0; u < inst_callback->trees[v]->n_leaf_nodes; ++u) {
						fixed_y[v][u] = double(round(getSolution(y[v][u])));
						//cout << int(getSolution(y[v][u])) << endl;
					}
				}




				// Checking if constraint 2c 2d is satisfied given the fixed_x and fixed_y, if violated, add lazy
				for (int t = 0; t < inst_callback->trees.size(); ++t) {
					int node_id = 0;
					while (true) {
						int var = inst_callback->frames[t][node_id]->variable;
						double cri = inst_callback->frames[t][node_id]->criterion;
						vector <double> vv = inst_callback->data_variables[var]->unique_splits;
						std::vector<double>::iterator itr = std::find(vv.begin(), vv.end(), cri);
						auto j = std::distance(vv.begin(), itr);
						if (fixed_x[var][j] >= 0.5) {
							double left_hand_side = 0.0;
							GRBLinExpr equ = 0.0;
							for (int i = 0; i < inst_callback->splitnodes[t][node_id]->right_children.size(); ++i) {
								left_hand_side += fixed_y[t][inst_callback->splitnodes[t][node_id]->right_children[i]];
								equ += y[t][inst_callback->splitnodes[t][node_id]->right_children[i]];
							}
							if (left_hand_side >= 1 - fixed_x[var][j] + 0.0001) {
								equ -= (1 - x[var][j]);
								addLazy(equ, GRB_LESS_EQUAL, 0);
								break;
							}
							else {
								if (inst_callback->frames[t][node_id]->left_child_is_leaf == 1) {
									break;
								}
								else {
									node_id = inst_callback->frames[t][node_id]->left_child;
								}
							}
						}
						else {
							double left_hand_side = 0.0;
							GRBLinExpr equ = 0.0;
							for (int i = 0; i < inst_callback->splitnodes[t][node_id]->left_children.size(); ++i) {
								left_hand_side += fixed_y[t][inst_callback->splitnodes[t][node_id]->left_children[i]];
								equ += y[t][inst_callback->splitnodes[t][node_id]->left_children[i]];
							}
							if (left_hand_side >= fixed_x[var][j] + 0.0001) {
								equ -= x[var][j];
								addLazy(equ, GRB_LESS_EQUAL, 0);
								break;
							}
							else {
								if (inst_callback->frames[t][node_id]->right_child_is_leaf == 1) {
									break;
								}
								else {
									node_id = inst_callback->frames[t][node_id]->right_child;
								}
							}
						}
					}

					//cout << "p3" << endl;
					///// ADD CUTS ABOVE THIS
				}

			}
		}
		catch (GRBException e) {
			cout << "Error number: " << e.getErrorCode() << endl;
			cout << e.getMessage() << endl;
			exit(1);
		}
		catch (...) {
			cout << "Error during callback" << endl;
		}

		}
};


void Model::Splits_algorithm(Data_Reader* _inst, ofstream& myfile) {
	cout << endl << "================================Split Generation Model======================" << endl << endl;

	try {
		GRBModel model(env);
		GRBVar** x;
		GRBVar** y;
	



		x = new GRBVar*[inst->data_variables.size()];
		for (int v = 0; v < inst->data_variables.size(); ++v) {
			x[v] = new GRBVar[inst->data_variables[v]->unique_splits.size()];
			for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
				x[v][u] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
			}
		}

		cout << "p2" << endl;
		y = new GRBVar*[inst->trees.size()];
		for (int t = 0; t < inst->trees.size(); ++t) {
			y[t] = new GRBVar[inst->trees[t]->tree_leafs.size()];
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				y[t][l] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS);
			}
		}

		model.update();


		for (int t = 0; t < inst->trees.size(); ++t) {
			GRBLinExpr equ = 0.0;
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				equ += y[t][l];
			}
			model.addConstr(equ == 1);
		}

		cout << "p4" << endl;


		for (int f = 0; f < inst->data_variables.size(); ++f) {
			if (inst->data_variables[f]->unique_splits.size() > 1) {
				for (int s = 1; s < inst->data_variables[f]->unique_splits.size(); ++s) {
					model.addConstr(x[f][s - 1] <= x[f][s]);
				}
			}
		}

		cout << "p7" << endl;

		GRBLinExpr obj = 0.0;
		for (int t = 0; t < inst->trees.size(); ++t) {
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				obj += inst->leafs[t][l]->value * y[t][l];
			}
		}


		//Model* _Model;
		callback_forest_for_split_constraint cb_SplitConstraint = callback_forest_for_split_constraint(x, y, inst);
		model.setCallback(&cb_SplitConstraint);

		model.update();
		cout << "obj added" << endl;
		model.setObjective(obj, GRB_MINIMIZE);
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 1800);
		//model.getEnv().set(GRB_IntParam_SolutionLimit, 1);
		//model.getEnv().set(GRB_DoubleParam_NodeLimit, 0);
		model.getEnv().set(GRB_IntParam_OutputFlag, 1);
		model.getEnv().set(GRB_IntParam_PreCrush, 1);
		model.getEnv().set(GRB_IntParam_DualReductions, 0);
		model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
		// master.getEnv().set(GRB_IntParam_VarBranch,  0);
		model.update();
		//cout << "solve gurobi" << endl;
		//model.write("BADMIP.lp");

		model.optimize();
		if (model.get(GRB_IntAttr_Status) != 3) {
			myfile << model.get(GRB_DoubleAttr_ObjVal) << "\t" << model.get(GRB_DoubleAttr_ObjBound) << "\t" << model.get(GRB_DoubleAttr_Runtime) << endl;
		}
		else {
			myfile << "model is infeasible" << endl;
		}
		cout << "optimal solution" << endl;
		//cout << "x" << endl;
		//vector < vector< int>> fixed_x(inst->data_variables.size());
		//for (int v = 0; v < inst->data_variables.size(); ++v) {
		//	fixed_x[v].resize(inst->data_variables[v]->unique_splits.size());
		//	for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
		//		fixed_x[v][u] = int(round(x[v][u].get(GRB_DoubleAttr_X)));
		//		cout << v << "\t" << u << "\t" << fixed_x[v][u] << endl;
		//	}
		//}

		//print decision variable value
		for (int t = 0; t < inst->trees.size(); ++t) {
			cout << "TREE: " << t << "\t";
			for (int l = 0; l < inst->trees[t]->tree_leafs.size(); ++l) {
				if (y[t][l].get(GRB_DoubleAttr_X) > 0.5) {
					cout << l << "\t"; //y[t][l].get(GRB_DoubleAttr_X)
				}
			}
			cout << endl;
		}
//
		for (int v = 0; v < inst->data_variables.size(); ++v) {
			cout << "FEATURE: " << v << "\t";
			//for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
			//	cout << x[v][u].get(GRB_DoubleAttr_X) << "\t";
			//}
			//cout << endl;
			double lowerbound_feature = inst->data_variables[v]->LB;
			double upperbound_feature = inst->data_variables[v]->UB;
			int upperbound_count = 0;
			for (int u = 0; u < inst->data_variables[v]->unique_splits.size(); ++u) {
				//cout << inst->data_variables[v]->unique_splits[u] << "\t";
				if (x[v][u].get(GRB_DoubleAttr_X) > 0.5) {
					//cout << "nth split\t" << u << "\t";
					//if (upperbound_count < 1) {
					//	upperbound_feature = min(upperbound_feature, inst->data_variables[v]->unique_splits[u]);
					//	upperbound_count += 1;
					//}
					//else {
					//	upperbound_feature = min(upperbound_feature, inst->data_variables[v]->unique_splits[u]);
					//}

					upperbound_feature = upperbound_feature;
				}
				else {
					lowerbound_feature = max(lowerbound_feature, inst->data_variables[v]->unique_splits[u]);
				}

			}
			cout << "lb: " << lowerbound_feature << "\t" << "ub: " << upperbound_feature << endl;
			myfile << lowerbound_feature << "\t" << upperbound_feature << endl;
			//cout << endl;
		}

	}
	catch (GRBException e) {
		cout << "Error number: " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
		exit(1);
	}
	catch (...) {
		cout << "Other error ... " << endl;
		exit(1);
	}
}







