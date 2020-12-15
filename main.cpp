#include<iostream>
#include<vector>
#include <time.h>
#include<algorithm>
#include "Task.h"

#define T_max_constraint 27
using namespace std;


int main() {
	ALGORITHM algo;
	//test 1
	/*TASK node1(1), node2(2), node3(3), node4(4), node5(5), node6(6), node7(7), node8(8), node9(9), node10(10);
	node10.predecessors.push_back(&node7);
	node10.predecessors.push_back(&node8);
	node10.predecessors.push_back(&node9);

	node9.predecessors.push_back(&node2);
	node9.predecessors.push_back(&node4);
	node9.predecessors.push_back(&node5);
	node9.successors.push_back(&node10);

	node8.predecessors.push_back(&node2);
	node8.predecessors.push_back(&node4);
	node8.predecessors.push_back(&node6);
	node8.successors.push_back(&node10);

	node7.predecessors.push_back(&node3);
	node7.successors.push_back(&node10);

	node6.predecessors.push_back(&node1);
	node6.successors.push_back(&node8);

	node5.predecessors.push_back(&node1);
	node5.successors.push_back(&node9);

	node4.predecessors.push_back(&node1);
	node4.successors.push_back(&node8);
	node4.successors.push_back(&node9);

	node3.predecessors.push_back(&node1);
	node3.successors.push_back(&node7);

	node2.predecessors.push_back(&node1);
	node2.successors.push_back(&node8);
	node2.successors.push_back(&node9);

	node1.successors.push_back(&node2);
	node1.successors.push_back(&node3);
	node1.successors.push_back(&node4);
	node1.successors.push_back(&node5);
	node1.successors.push_back(&node6);

	node10.init(10, { 7,4,2 }, { 3,1,1 });
	node9.init(9, { 5,3,2 }, { 3,1,1 });
	node8.init(8, { 6,4,2 }, { 3,1,1 });
	node7.init(7, { 8,5,3 }, { 3,1,1 });
	node6.init(6, { 7,6,4 }, { 3,1,1 });
	node5.init(5, { 5,4,2 }, { 3,1,1 });
	node4.init(4, { 7,5,3 }, { 3,1,1 });
	node3.init(3, { 6,5,4 }, { 3,1,1 });
	node2.init(2, { 8,6,5 }, { 3,1,1 });
	node1.init(1, { 9,7,5 }, { 3,1,1 });

	node2.is_local = false;
	node2.compute_computation_cost();*/
	//test2
	TASK node1(1), node2(2), node3(3), node4(4), node5(5), node6(6), node7(7);
	node7.predecessors.push_back(&node6);
	node7.predecessors.push_back(&node5);

	node6.predecessors.push_back(&node3);
	node6.predecessors.push_back(&node4);
	node6.successors.push_back(&node7);

	node5.predecessors.push_back(&node2);
	node5.predecessors.push_back(&node3);
	node5.successors.push_back(&node7);

	node4.predecessors.push_back(&node1);
	node4.successors.push_back(&node6);

	node3.predecessors.push_back(&node1);
	node3.successors.push_back(&node5);
	node3.successors.push_back(&node6);

	node2.predecessors.push_back(&node1);
	node2.successors.push_back(&node5);

	node1.successors.push_back(&node2);
	node1.successors.push_back(&node3);
	node1.successors.push_back(&node4);

	node7.init(7, { 8,5,3 }, { 3,1,1 });
	node6.init(6, { 7,6,4 }, { 3,1,1 });
	node5.init(5, { 5,4,2 }, { 3,1,1 });
	node4.init(4, { 7,5,3 }, { 3,1,1 });
	node3.init(3, { 6,5,4 }, { 3,1,1 });
	node2.init(2, { 8,6,5 }, { 3,1,1 });
	node1.init(1, { 9,7,5 }, { 3,1,1 });

	vector<TASK*> node_list = { &node7,&node6,&node5,&node4,&node3,&node2,&node1 };
	clock_t start, end;
	start = clock();
	//calculate Priority score
	for (auto node : node_list) {
		double priority_score = node->computation_cost;
		if (node->successors.empty()) {
			node->priority_score = priority_score;
			continue;
		}
		double max_children_score=0;
		for (auto p : node->successors) {
			if (p->priority_score > max_children_score) {
				max_children_score = p->priority_score;
			}
		}
		node->priority_score = priority_score + max_children_score;
	}
	sort(node_list.begin(), node_list.end(), comp_by_prio);
	cout << "compute priority score" << endl;
	for (auto node : node_list) {
		cout << "node id: " << node->id << " prio score: " << node->priority_score << endl;
	}

	auto sequence = algo.Primary_Assignment(node_list);
	double T_init = algo.Total_Time(node_list);
	double E_init = algo.Total_Energy(node_list);
	cout << "initial time and energy: " << T_init << ", " << E_init << endl;

	//start outer loop
	int iter_num = 0;
	while (iter_num < 100) {
		cout << "----------" << endl << "iter: " << iter_num << endl << "----------" << endl;
		T_init = algo.Total_Time(node_list);
		E_init = algo.Total_Energy(node_list);
		cout << "initial time and energy: " << T_init << ", " << E_init << endl;
		vector<int> temp;
		vector<vector<int>> migration_choice(node_list.size(),temp);
		int current_row_id = 0;
		vector<int> current_row_value;
		for (int i = 0; i < node_list.size(); i++) {
			if (node_list[i]->assignment == 3) {//cloud task
				current_row_id = node_list[i]->id - 1;
				current_row_value = { 1,1,1,1 };
				migration_choice[current_row_id] = current_row_value;
			}
			else {
				current_row_id = node_list[i]->id - 1;
				current_row_value = { 0,0,0,0 };
				current_row_value[node_list[i]->assignment] = 1;
				migration_choice[current_row_id] = current_row_value;
			}
		}
		cout << "migration" << endl;
		vector<double> temp1(2, -1);
		vector<vector<double>> temp2(4, temp1);
		vector<vector<vector<double>>> result_table(node_list.size(), temp2);

		for (int i = 0; i < migration_choice.size(); i++) {
			auto ith_row = migration_choice[i];
			for (int k = 0; k < ith_row.size(); k++) {
				if (ith_row[k] == 1) {
					continue;
				}
				auto seq_copy = sequence;
				auto nodes_copy = Deepcopy(node_list);
				for (int i = 0; i < nodes_copy.size();i++) {
					for (auto p : node_list[i]->predecessors) {
						TASK* temp = new TASK(*p);
						nodes_copy[i]->predecessors.push_back(temp);
					}
					for (auto p : node_list[i]->successors) {
						TASK* temp = new TASK(*p);
						nodes_copy[i]->successors.push_back(temp);
					}
				}
				seq_copy = algo.New_Sequence(nodes_copy, i + 1, k, seq_copy);
				algo.Kernel_Algorithm(nodes_copy, seq_copy);

				double current_t = algo.Total_Time(nodes_copy);
				double current_e = algo.Total_Energy(nodes_copy);

				result_table[i][k] = { current_t,current_e };
			}
		}
		int i_best = -1;
		int k_best = -1;
		double t_best = T_init;
		double e_best = E_init;
		double ration_best = -1;

		for (int i = 0; i < result_table.size(); i++) {
			for (int j = 0; j < result_table[i].size(); j++) {
				auto value = result_table[i][j];
				if (value[0] == -1 && value[1] == -1) {
					continue;
				}
				if (value[0] > T_max_constraint) {
					continue;
				}
				double ration = (double) (e_best - value[1]) / abs(value[0] - t_best + 0.00005);
				if (ration > ration_best) {
					ration_best = ration;
					i_best = i;
					k_best = j;
				}
			}
		}
		if (i_best==-1 && k_best==-1) {
			break;
		}
		i_best += 1;
		k_best += 1;
		t_best = result_table[i_best - 1][k_best - 1][0];
		e_best = result_table[i_best - 1][k_best - 1][1];
		cout << "current migration: task: " << i_best << " k: " << k_best << " total time: " << t_best << " total energy: " << e_best << endl;
		cout << "update after current outer loop" << endl;
		sequence = algo.New_Sequence(node_list, i_best, k_best - 1, sequence);
		node_list=algo.Kernel_Algorithm(node_list, sequence);
		for (auto s : sequence) {
			for (auto i : s) {
				cout << i<<" ";
			}
			cout << endl;
		}
		double T_current = algo.Total_Time(node_list);
		double E_current = algo.Total_Energy(node_list);
		double E_diff = E_init - E_current;
		double T_diff = abs(T_current - T_init);
		iter_num++;
		
		cout << "time and energy: " << T_current << " " << E_current << endl;
		if (E_diff <= 1) {
			break;
		}
	}

	cout << "reschedule finished" << endl;
	for (auto node : node_list) {
		if (node->is_local == true) {
			cout << "node id:" << node->id << " assignment:" << node->assignment << " ready time:" << node->ready_time_local << " local start time:" << node->start_time[node->assignment] << endl;
			cout << "-----------" << endl;
		}
		else {
			cout << "node id:" << node->id << " assignment:" << node->assignment << " ws ready time:" << node->ready_time_wireless_sending << " cloud ready time: " << node->ready_time_cloud << " wr ready time: " << node->ready_time_wireless_receiving << " cloud start time:" << node->start_time[3] << endl;
			cout << "-----------" << endl;
		}
	}
	end = clock();
	cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
	cout << "final sequence" << endl;
	for (auto s : sequence) {
		for (auto i : s) {
			cout << i<<" ";
		}
		cout << endl;
	}
	cout << "final time: " << algo.Total_Time(node_list)<<endl;
	cout << "final energy: " << algo.Total_Energy(node_list);
	return 0;
}