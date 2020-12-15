#pragma once
#include<vector>
#include<algorithm>
#include<iostream>
#include<stack>

#define task_num 7
#define core_num 3

using namespace std;
//tools
template<typename T>
T SumVector(vector<T>& vec)
{
	T res = 0;
	for (size_t i = 0; i < vec.size(); i++)
	{
		res += vec[i];
	}
	return res;
}
template<typename M>
vector<M*> Deepcopy1(vector<M*> vec) {
	vector<M*> res;
	for (auto i : vec) {
		M* temp = new M(*i);
		res.push_back(temp);
	}
	return res;
}

class TASK
{
public:
	TASK(int id) :id(id) {}
	TASK(const TASK& obj) {
		
		this->id = obj.id;

		this->core_speed = obj.core_speed;
		this->cloud_speed = obj.cloud_speed;

		this->finish_time_local = obj.finish_time_local;
		this->finish_time_wireless_sending = obj.finish_time_wireless_sending;
		this->finish_time_cloud = obj.finish_time_cloud;
		this->finish_time_wireless_receiving = obj.finish_time_wireless_receiving;

		this->ready_time_local = obj.ready_time_local;
		this->ready_time_wireless_sending = obj.ready_time_wireless_sending;
		this->ready_time_cloud = obj.ready_time_cloud;
		this->ready_time_wireless_receiving = obj.ready_time_wireless_receiving;

		this->is_local = obj.is_local;
		this->computation_cost = obj.computation_cost;
		this->priority_score = obj.priority_score;
		this->assignment = obj.assignment;
		this->start_time = obj.start_time;
		this->is_scheduled = obj.is_scheduled;
	}

	int id;
	vector<TASK*> predecessors;
	vector<TASK*> successors;
	vector<double> core_speed;
	vector<double> cloud_speed;

	double finish_time_local=0;
	double finish_time_wireless_sending = 0;
	double finish_time_cloud = 0;
	double finish_time_wireless_receiving = 0;

	double ready_time_local = -1;
	double ready_time_wireless_sending = -1;
	double ready_time_cloud = -1;
	double ready_time_wireless_receiving = -1;
	
	bool is_local=false;

	double computation_cost = 0;//w_i
	double priority_score = 0;
	int assignment = -2;// 0 1 2 3; 3 is cloud
	vector<double> start_time = { -1,-1,-1,-1 };//start time for core1 core2 core3 cloud
	bool is_scheduled = false;

	void assign_local_cloud() {
		int t_l_min = this->core_speed[2];
		int t_c_min = 5;
		if (t_l_min <= t_c_min) {
			this->is_local = true;
			this->finish_time_wireless_sending = 0;
			this->finish_time_cloud = 0;
			this->finish_time_wireless_receiving = 0;
		}
		else {
			this->is_local = false;
			this->finish_time_local = 0;
		}
	}
	void compute_computation_cost() {
		double computation_cost = 0;
		if (this->is_local==true) {//calculate average computational time of task v in local cores (14)
			computation_cost = (double) SumVector(this->core_speed)/core_num;
		}
		else {
			computation_cost = 5;//(13)
		}
		this->computation_cost = computation_cost;
	}
	void init(int id, vector<double> core_speed, vector<double> cloud_speed) {
		this->id = id;
		this->core_speed = core_speed;
		this->cloud_speed = cloud_speed;
		assign_local_cloud();
		compute_computation_cost();
		
	}
	
};

bool comp_by_prio(const TASK* a, const TASK* b) {
	return a->priority_score > b->priority_score;
}
vector<TASK*> Deepcopy(vector<TASK*> vec) {
	vector<TASK*> res;
	for (auto i : vec) {
		TASK* temp = new TASK(*i);
		res.push_back(temp);
	}
	return res;
}
class ALGORITHM {
public:

	double Total_Time(vector<TASK*> nodes) {
		double total_t = 0;
		for (auto node : nodes) {
			if (node->successors.empty()) {
				total_t = max(node->finish_time_local, node->finish_time_wireless_receiving);
			}
		}
		return total_t;
	}
	double Total_Energy(vector<TASK*> nodes) {
		vector<double> core_cloud_power = { 1,2,4,0.5 };
		double total_e = 0;
		double current_node_e = 0;
		for (auto node : nodes) {
			if (node->is_local == true) {
				current_node_e = node->core_speed[node->assignment] * core_cloud_power[node->assignment];
				total_e += current_node_e;
			}
			else {
				current_node_e = node->cloud_speed[0] * core_cloud_power[3];
				total_e += current_node_e;
			}
		}
		return total_e;
	}

	vector<vector<int>> Primary_Assignment(vector<TASK*> nodes) {
		vector<vector<int>> res;
		vector<int> cloud_speed = { 3,1,1 };
		vector<double> local_source = { 0,0,0 };
		vector<double> cloud_source = { 0,0,0 };

		vector<int> core1_seq;
		vector<int> core2_seq;
		vector<int> core3_seq;
		vector<int> cloud_seq;

		for(auto node:nodes){
			if (node->is_local == true) {
				if (node->predecessors.empty()) {
					node->ready_time_local = 0;
				}
				else {//(3)
					for (auto p : node->predecessors) {
						double p_ft = max(p->finish_time_local, p->finish_time_wireless_receiving);
						if (p_ft > node->ready_time_local) {
							node->ready_time_local = p_ft;
						}
					}
				}
				double core1_ft = max(local_source[0], node->ready_time_local) + node->core_speed[0];
				double core2_ft = max(local_source[1], node->ready_time_local) + node->core_speed[1];
				double core3_ft = max(local_source[2], node->ready_time_local) + node->core_speed[2];
				//choose the fastest one
				int core_assign_id = 0;
				double core_assign_ft = core1_ft;
				if (core_assign_ft > core2_ft) {
					core_assign_ft = core2_ft;
					core_assign_id = 1;
				}
				if (core_assign_ft > core3_ft) {
					core_assign_ft = core3_ft;
					core_assign_id = 2;
				}
				node->assignment = core_assign_id;
				node->finish_time_local = core_assign_ft;
				node->start_time[core_assign_id] = max(local_source[core_assign_id], node->ready_time_local);

				local_source[core_assign_id] = node->finish_time_local;
				//add the node to core seq
				if (node->assignment == 0) {
					core1_seq.push_back(node->id);
				}
				if (node->assignment == 1) {
					core2_seq.push_back(node->id);
				}
				if (node->assignment == 2) {
					core3_seq.push_back(node->id);
				}
				cout << "node id:" << node->id << " assignment:" << node->assignment << " ready time:" << node->ready_time_local << " local start time:" << node->start_time[node->assignment] << endl;
				cout << "-----------" << endl;
			}
			if (node->is_local == false){
				//(4)
				for (auto p : node->predecessors) {
					double p_ws = max(p->finish_time_local, p->finish_time_wireless_sending);
					if (p_ws > node->ready_time_wireless_sending) {
						node->ready_time_wireless_sending = p_ws;
					}
				}
				double cloud_ws_ft = max(cloud_source[0], node->ready_time_wireless_sending) + node->cloud_speed[0];
				node->finish_time_wireless_sending = cloud_ws_ft;
				//(5)
				double p_max_ft_c = 0;
				for (auto p : node->predecessors) {
					if (p->finish_time_cloud > p_max_ft_c) {
						p_max_ft_c = p->finish_time_cloud;
					}
				}
				node->ready_time_cloud = max(node->finish_time_wireless_sending, p_max_ft_c);
				double cloud_c_ft = max(cloud_source[1], node->ready_time_cloud) + node->cloud_speed[1];
				node->finish_time_cloud = cloud_c_ft;
				//(6)
				node->ready_time_wireless_receiving = node->finish_time_cloud;
				double cloud_wr_ft = max(cloud_source[2], node->ready_time_wireless_receiving) + node->cloud_speed[2];
				node->finish_time_wireless_receiving = cloud_wr_ft;
				node->assignment = 3;
				node->start_time[3] = max(cloud_source[0], node->ready_time_wireless_sending);//cloud task start time is sending start time
				
				cloud_source[0] = cloud_ws_ft;
				cloud_source[1] = cloud_c_ft;
				cloud_source[2] = cloud_wr_ft;

				cloud_seq.push_back(node->id);
				cout << "node id:" << node->id << " assignment:" << node->assignment << " ws ready time:" << node->ready_time_wireless_sending << " cloud ready time: "<<node->ready_time_cloud<<" wr ready time: "<<node->ready_time_wireless_receiving<<" cloud start time:" << node->start_time[3] << endl;
				cout << "-----------" << endl;
			}
		}
		double total_t = Total_Time(nodes);
		double total_e = Total_Energy(nodes);
		cout << "total time: " << total_t << endl;
		cout << "total energy: " << total_e << endl;
		
		res.push_back(core1_seq);
		res.push_back(core2_seq);
		res.push_back(core3_seq);
		res.push_back(cloud_seq);
		return res;
	}

	vector<vector<int>> New_Sequence(vector<TASK*> nodes,int target_id,int k,vector<vector<int>> seq) {
		//k: migration location 0 1 2 3
		int temp_id = 0;
		int node_index[task_num+1];
		TASK* target_node=nullptr;
		double node_tar_rt = 0;

		//find target node
		for (auto node : nodes) {
			node_index[node->id] = temp_id;
			temp_id++;
			if (node->id == target_id) {
				target_node = node;
			}
		}

		if (target_node->is_local==true) {//calculate tar ready time in (19)
			node_tar_rt = target_node->ready_time_local;
		}
		else {
			node_tar_rt = target_node->ready_time_wireless_sending;
		}
		
		for (int i = 0; i < seq[target_node->assignment].size(); i++) {
			if (seq[target_node->assignment][i] == target_node->id) {
				seq[target_node->assignment].erase(seq[target_node->assignment].begin() + i);
				break;
			}
		}
		vector<int> s_new = seq[k];
		vector<int> s_new_prim;
		bool flag = false;
		for (auto node_id : s_new) {
			TASK* node = nodes[node_index[node_id]];
			if (node->start_time[k] < node_tar_rt) {
				s_new_prim.push_back(node_id);
			}
			if (node->start_time[k] >= node_tar_rt && flag == false) {
				s_new_prim.push_back(target_node->id);
				flag = true;
			}
			if (node->start_time[k] >= node_tar_rt && flag == true) {
				s_new_prim.push_back(node->id);
			}
		}
		if (flag == false) {
			s_new_prim.push_back(target_node->id);
		}
		seq[k] = s_new_prim;

		target_node->assignment = k;
		if (k == 3) {
			target_node->is_local = false;
		}
		else {
			target_node->is_local = true;
		}
		return seq;
	}

	vector<TASK*> Kernel_Algorithm(vector<TASK*> nodes_new,vector<vector<int>> seq_new) {
		vector<double> local_source = { 0,0,0 };
		vector<double> cloud_source = { 0,0,0 };

		vector<int> ready1(nodes_new.size(), -1);
		vector<int> ready2(nodes_new.size(), -1);
		ready1[nodes_new[0]->id - 1] = 0; // id start from 1
		for (int i = 0; i < seq_new.size(); i++) {
			if (seq_new[i].size() > 0) {
				ready2[seq_new[i][0] - 1] = 0;
			}
		}
		int node_index[task_num+1];
		int temp_id = 0;
		for (auto node : nodes_new) {
			node_index[node->id] = temp_id;
			node->ready_time_local = -1;
			node->ready_time_wireless_sending = -1;
			node->ready_time_cloud = -1;
			node->ready_time_wireless_receiving = -1;
			temp_id++;
		}
		//start the rescheduling task
		stack<TASK*> s;
		s.push(nodes_new[0]);

		while (!s.empty()) {
			auto v_i = s.top();
			s.pop();
			v_i->is_scheduled = true;
			//first, calculate v_i local ready time

			if (v_i->is_local == true) {//local task
				if (v_i->predecessors.empty()) {
					v_i->ready_time_local = 0;
				}
				else {//equation (3)
					for (auto p : v_i->predecessors) {
						auto p_ft = max(p->finish_time_local, p->finish_time_wireless_receiving);
						if (p_ft > v_i->ready_time_local) {
							v_i->ready_time_local = p_ft;
						}
					}
				}
			}
			// part 2: schedule on the corresponding core
			if (v_i->assignment == 0) {//local core1
				v_i->start_time[0] = -1;
				v_i->start_time[1] = -1;
				v_i->start_time[2] = -1;
				v_i->start_time[3] = -1;
				v_i->start_time[0] = max(local_source[0], v_i->ready_time_local);
				v_i->finish_time_local = v_i->start_time[0] + v_i->core_speed[0];
				v_i->finish_time_wireless_sending = -1;
				v_i->finish_time_cloud = -1;
				v_i->finish_time_wireless_receiving = -1;
				local_source[0] = v_i->finish_time_local;
			}
			if (v_i->assignment == 1) {//local core2
				v_i->start_time[0] = -1;
				v_i->start_time[1] = -1;
				v_i->start_time[2] = -1;
				v_i->start_time[3] = -1;
				v_i->start_time[1] = max(local_source[1], v_i->ready_time_local);
				v_i->finish_time_local = v_i->start_time[1] + v_i->core_speed[1];
				v_i->finish_time_wireless_sending = -1;
				v_i->finish_time_cloud = -1;
				v_i->finish_time_wireless_receiving = -1;
				local_source[1] = v_i->finish_time_local;
			}
			if (v_i->assignment == 2) {//local core3
				v_i->start_time[0] = -1;
				v_i->start_time[1] = -1;
				v_i->start_time[2] = -1;
				v_i->start_time[3] = -1;
				v_i->start_time[2] = max(local_source[2], v_i->ready_time_local);
				v_i->finish_time_local = v_i->start_time[2] + v_i->core_speed[2];
				v_i->finish_time_wireless_sending = -1;
				v_i->finish_time_cloud = -1;
				v_i->finish_time_wireless_receiving = -1;
				local_source[2] = v_i->finish_time_local;
			}
			if (v_i->assignment == 3) {//cloud
				if (v_i->predecessors.empty()) {
					v_i->ready_time_wireless_sending = 0;
				}
				else {
					for (auto p : v_i->predecessors) {
						auto p_ws = max(p->finish_time_local, p->finish_time_wireless_sending);
						if (p_ws > v_i->ready_time_wireless_sending) {
							v_i->ready_time_wireless_sending = p_ws;
						}
					}
				}
				v_i->finish_time_wireless_sending = max(cloud_source[0], v_i->ready_time_wireless_sending) + v_i->cloud_speed[0];
				v_i->start_time[0] = -1;
				v_i->start_time[1] = -1;
				v_i->start_time[2] = -1;
				v_i->start_time[3] = -1;
				v_i->start_time[3] = max(cloud_source[0], v_i->ready_time_wireless_sending);
				cloud_source[0] = v_i->finish_time_wireless_sending;
				//2 cloud part
				double p_max_ft_c = 0;
				for (auto p : v_i->predecessors) {
					if (p->finish_time_cloud > p_max_ft_c) {
						p_max_ft_c = p->finish_time_cloud;
					}
				}
				v_i->ready_time_cloud = max(v_i->finish_time_wireless_sending, p_max_ft_c);
				v_i->finish_time_cloud = max(cloud_source[1], v_i->ready_time_cloud) + v_i->cloud_speed[1];
				cloud_source[1] = v_i->finish_time_cloud;
				//3 receiving part
				v_i->ready_time_wireless_receiving = v_i->finish_time_cloud;
				v_i->finish_time_wireless_receiving = max(cloud_source[2], v_i->ready_time_wireless_receiving) + v_i->cloud_speed[2];
				v_i->finish_time_local = -1;
				cloud_source[2] = v_i->finish_time_wireless_receiving;
			}

			//update ready1 and ready2
			vector<int> corresponding_seq = seq_new[v_i->assignment];//the sequence that current v_i is assigned
			
			//position of v_i in seq list
			int v_i_index = 0;
			for (int i = 0; i < corresponding_seq.size(); i++) {
				if (v_i->id == corresponding_seq[i]) {
					v_i_index = i;
					break;
				}
			}

			int next_node_id=-1;
			if (v_i_index != corresponding_seq.size() - 1) {
				next_node_id = corresponding_seq[v_i_index + 1];
			}
			else {
				next_node_id = -1;
			}

			for (auto node : nodes_new) {
				int flag = 0;
				for (auto p : node->predecessors) {
					for (auto n : nodes_new) {
						if (n->id == p->id) {
							p->is_scheduled = n->is_scheduled;
							break;
						}
					}
					if (p->is_scheduled != true) {
						flag += 1;
					}
					ready1[node->id - 1] = flag;
				}
				if (node->id == next_node_id) {
					ready2[node->id - 1] = 0;
				}
			}

			for (auto node : nodes_new) {
				//add node into stack if satisfied
				if (ready1[node->id - 1] == 0 && ready2[node->id - 1] == 0 && node->is_scheduled != true){
					stack<TASK*> temp;//check if node is in stack s
					bool is_in=false;
					while (!s.empty()) {
						if (s.top()->id != node->id) {
							temp.push(s.top());
							s.pop();
						}
						else {
							while (!temp.empty()) {
								s.push(temp.top());
								temp.pop();
							}
							is_in = true;
							break;
						}
					}
					if (is_in == false) {
						while (!temp.empty()) {
							s.push(temp.top());
							temp.pop();
						}
						s.push(node);
					}
				}
			}
		}

		for (auto node : nodes_new) {
			node->is_scheduled = false;
		}
		return nodes_new;
	}
}; 
