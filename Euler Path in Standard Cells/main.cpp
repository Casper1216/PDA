#include<iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include<string>
#include<queue>
#include<unordered_map>
#include<unordered_set>
#include<time.h>
#include<limits.h>
#include<cmath>
#include<algorithm>

using namespace std;

//Best one version
//⊿Τσ納 gate source or drain

class mos{
    public:
    string name;
    string drain;
    string gate;
    string source;
    string body;
    double w;
    double l;
};

class graph_node{
    public:
    string name;    //graph node name
    unordered_set<string> adjacent;
};

double Cal_HPWL_dummy_exist(vector<string> p_path,vector<string>& n_path,unordered_set<string>& p_node_set,unordered_set<string>& n_node_set,double p_mos_w,double n_mos_w){
    double hwpl = 0;
    unordered_set<string> temp_n_node_set(n_node_set);
    for(auto e:p_node_set){
        int p_l = 0,p_r = p_path.size()-1,n_l = INT_MAX ,n_r = 0;
        while(p_path[p_l]!=e || p_path[p_r]!=e ||(p_l%2)==1||(p_r%2)==1){
            if(p_path[p_l]!=e||(p_l%2)==1)
                p_l++;
            if(p_path[p_r]!=e||(p_r%2)==1)
                p_r--;
        }
       
        if(temp_n_node_set.count(e)){
            n_l = 0 ,n_r = n_path.size()-1;
            while(n_path[n_l]!=e || n_path[n_r]!=e ||(n_l%2)==1||(n_r%2)==1){
                if(n_path[n_l]!=e||(n_l%2)==1)
                    n_l++;
                if(n_path[n_r]!=e||(n_r%2)==1)
                    n_r--;
            }
            
            double height = 0;
            height += 27 + (p_mos_w+n_mos_w)*0.5;
            hwpl += height;
            
            temp_n_node_set.erase(e);
        }
        int st_point = min(p_l,n_l),end_point = max(p_r,n_r);
        int dum_cnt =0;
        for(int i=st_point;i<=end_point;i++){
            if(p_path[i]=="Dummy")
                dum_cnt++;
        }
        double width = 0;
        double length = end_point - st_point + dum_cnt*2;
        if(length > 0){

            if(st_point==0)
                width += 25*0.5;
            else
                width += 34*0.5;
            if(end_point==n_path.size()-1)
                width += 25*0.5;
            else
                width += 34*0.5;
            width += ((length)*0.5-1)*34 + (length)*0.5*20;
        }
        hwpl += width;
        
    }
    for(auto e:temp_n_node_set){
        int n_l = 0 ,n_r = n_path.size()-1;
        while(n_path[n_l]!=e || n_path[n_r]!=e||(n_l%2)==1||(n_r%2)==1){
            if(n_path[n_l]!=e||(n_l%2)==1)
                n_l++;
            if(n_path[n_r]!=e||(n_r%2)==1)
                n_r--;
        }
        double width = 0;
        if(n_r - n_l > 0){
            if(n_l == 0)
                width += 25*0.5;
            else
                width += 34*0.5;
            if(n_r == n_path.size()-1)
                width += 25*0.5;
            else
                width += 34*0.5;
            int dum_cnt = 0;
            for(int i=n_l;i<=n_r;i++){
                if(n_path[i]=="Dummy")
                    dum_cnt++;
            }
            double length = n_r - n_l + 2*dum_cnt;
            width += ((length)*0.5-1)*34 + (length)*0.5*20;
        }
        hwpl += width;
    }
    return hwpl;
}

void Build_graph(vector<mos>& p_mos_vec,vector<mos>& n_mos_vec,vector<graph_node>& n_mos_graph,unordered_map<string,int>& n_mos_map,vector<graph_node>& p_mos_graph,unordered_map<string,int>& p_mos_map
                ,unordered_map<string,int>& n_name_to_idx,unordered_map<string,int>& p_name_to_idx,unordered_map<string,vector<string>>& n_gate_to_name){
    
    for(int i=0;i<n_mos_vec.size();i++){
        n_name_to_idx[n_mos_vec[i].name] = i ;
        n_gate_to_name[n_mos_vec[i].gate].push_back(n_mos_vec[i].name);
        if(!n_mos_map.count(n_mos_vec[i].gate)){
            n_mos_map[n_mos_vec[i].gate] = n_mos_graph.size();

            graph_node temp_node;
            temp_node.name = n_mos_vec[i].gate;
            temp_node.adjacent.insert(n_mos_vec[i].source);
            temp_node.adjacent.insert(n_mos_vec[i].drain);
            n_mos_graph.push_back(temp_node);
        }
        else{
            n_mos_graph[n_mos_map[n_mos_vec[i].gate]].adjacent.insert(n_mos_vec[i].source);
            n_mos_graph[n_mos_map[n_mos_vec[i].gate]].adjacent.insert(n_mos_vec[i].drain);
        }
        if(!n_mos_map.count(n_mos_vec[i].source)){
            n_mos_map[n_mos_vec[i].source] = n_mos_graph.size();

            graph_node temp_node;
            temp_node.name = n_mos_vec[i].source;
            temp_node.adjacent.insert(n_mos_vec[i].gate);
            n_mos_graph.push_back(temp_node);
        }
        else{
            n_mos_graph[n_mos_map[n_mos_vec[i].source]].adjacent.insert(n_mos_vec[i].gate);
        }
        if(!n_mos_map.count(n_mos_vec[i].drain)){
            n_mos_map[n_mos_vec[i].drain] = n_mos_graph.size();

            graph_node temp_node;
            temp_node.name = n_mos_vec[i].drain;
            temp_node.adjacent.insert(n_mos_vec[i].gate);
            n_mos_graph.push_back(temp_node);
        }
        else{
            n_mos_graph[n_mos_map[n_mos_vec[i].drain]].adjacent.insert(n_mos_vec[i].gate);
        }
    }

    for(int i=0;i<p_mos_vec.size();i++){
        p_name_to_idx[p_mos_vec[i].name] = i ;
        if(!p_mos_map.count(p_mos_vec[i].gate)){
            p_mos_map[p_mos_vec[i].gate] = p_mos_graph.size();

            graph_node temp_node;
            temp_node.name = p_mos_vec[i].gate;
            temp_node.adjacent.insert(p_mos_vec[i].source);
            temp_node.adjacent.insert(p_mos_vec[i].drain);
            p_mos_graph.push_back(temp_node);
        }
        else{
            p_mos_graph[p_mos_map[p_mos_vec[i].gate]].adjacent.insert(p_mos_vec[i].source);
            p_mos_graph[p_mos_map[p_mos_vec[i].gate]].adjacent.insert(p_mos_vec[i].drain);
        }
        if(!p_mos_map.count(p_mos_vec[i].source)){
            p_mos_map[p_mos_vec[i].source] = p_mos_graph.size();

            graph_node temp_node;
            temp_node.name = p_mos_vec[i].source;
            temp_node.adjacent.insert(p_mos_vec[i].gate);
            p_mos_graph.push_back(temp_node);
        }
        else{
            p_mos_graph[p_mos_map[p_mos_vec[i].source]].adjacent.insert(p_mos_vec[i].gate);
        }
        if(!p_mos_map.count(p_mos_vec[i].drain)){
            p_mos_map[p_mos_vec[i].drain] = p_mos_graph.size();

            graph_node temp_node;
            temp_node.name = p_mos_vec[i].drain;
            temp_node.adjacent.insert(p_mos_vec[i].gate);
            p_mos_graph.push_back(temp_node);
        }
        else{
            p_mos_graph[p_mos_map[p_mos_vec[i].drain]].adjacent.insert(p_mos_vec[i].gate);
        }
    }
}

void SA_DFS_travel(int start_point,int goal_idx,vector<string>& goal_mos_order,vector<string>& one_gate,vector<string>& find_mos_order,vector<string>& find_path,vector<string>& temp_mos_order,vector<string>& temp_path,vector<mos>& mos_vec,vector<graph_node>& mos_graph,unordered_map<string,int>& mos_map,unordered_map<string,int>& name_to_idx){
    //р┮Τ隔畖癬ㄓゑ癸 琌 gate_order 
    if(find_mos_order.size()>0){
        temp_path.push_back("find return");
        return ;
    }
    
    one_gate.push_back(mos_graph[start_point].name);
    temp_path.push_back(mos_graph[start_point].name);
    
    if(one_gate.size()==1){
        SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx]]].gate],goal_idx,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);
        temp_path.pop_back();
    }
    else if(one_gate.size()==2){    //arrive gate
        
        if(mos_vec[name_to_idx[goal_mos_order[goal_idx]]].source==one_gate[0]){
            SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx]]].drain],goal_idx,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);
            temp_path.pop_back();
        }
        else if(mos_vec[name_to_idx[goal_mos_order[goal_idx]]].drain==one_gate[0]){
            SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx]]].source],goal_idx,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);
            temp_path.pop_back();
        }
    }
    else if(one_gate.size()==3){
        
        temp_mos_order.push_back(goal_mos_order[goal_idx]); //finish whole gate
        if(goal_idx== goal_mos_order.size()-1){
            
            find_mos_order = temp_mos_order;
            find_path = temp_path;
            return ;
        }

        one_gate.clear();
        //パ狠тgoal_mos_order[goal_idx+1]
        string next_gate = mos_vec[name_to_idx[goal_mos_order[goal_idx+1]]].gate;
        for(auto e:mos_graph[mos_map[next_gate]].adjacent){
            if(e==mos_graph[start_point].name){
                one_gate.push_back(mos_graph[start_point].name);
                SA_DFS_travel(mos_map[next_gate],goal_idx+1,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);        
                temp_path.pop_back();
                break;
            }
        }     
        
        if(find_mos_order.size()==0){
            if(rand()%2==1){

                temp_path.push_back("Dummy");
                temp_mos_order.push_back("Dummy");
                one_gate.clear();
                SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx+1]]].source],goal_idx+1,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);        
                temp_path.pop_back();       //remove next node

                if(find_mos_order.size()==0){
                    one_gate.clear();
                    SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx+1]]].drain],goal_idx+1,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);        
                }
                temp_mos_order.pop_back(); //remove dummy
                temp_path.pop_back();       //remove dummy
            }
            else{
                temp_path.push_back("Dummy");
                temp_mos_order.push_back("Dummy");
                one_gate.clear();
                SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx+1]]].drain],goal_idx+1,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);        
                temp_path.pop_back();       //remove next node

                if(find_mos_order.size()==0){
                    one_gate.clear();
                    SA_DFS_travel(mos_map[mos_vec[name_to_idx[goal_mos_order[goal_idx+1]]].source],goal_idx+1,goal_mos_order,one_gate,find_mos_order,find_path,temp_mos_order,temp_path,mos_vec,mos_graph,mos_map,name_to_idx);        
                }
                temp_mos_order.pop_back(); //remove dummy
                temp_path.pop_back();       //remove dummy
            }
        }   
    }
}

void Pre_SA_Cal(double& delta_avg,double& C_norm,unordered_set<string>& n_node_set,unordered_set<string>& p_node_set,vector<mos>& p_mos_vec,vector<mos>& n_mos_vec,vector<graph_node>& n_mos_graph,unordered_map<string,int>& n_mos_map,vector<graph_node>& p_mos_graph,unordered_map<string,int>& p_mos_map
                ,vector<string>& n_name_vec,vector<string>& p_name_vec,unordered_map<string,int>& n_name_to_idx,unordered_map<string,int>& p_name_to_idx,unordered_map<string,vector<string>>& n_gate_to_name){
    int uphill_time = 0;
    int sum_time = 0;
    double pre_hpwl=0;
    double sum_uphill_avg = 0;
    double sum_cost = 0;
    while(uphill_time <= 50*p_name_vec.size()){
        
        vector<string> temp_p_name_vec(p_name_vec);  
        vector<string> goal_gate_order(p_name_vec.size());
        vector<string> p_goal_mos_order(p_name_vec.size());;
        
        for(int i=0;i<p_name_vec.size();i++){
            int temp_idx = rand()%temp_p_name_vec.size();  
            p_goal_mos_order[i] = temp_p_name_vec[temp_idx];
            temp_p_name_vec.erase( temp_p_name_vec.begin()+temp_idx, temp_p_name_vec.begin()+temp_idx+1);

            goal_gate_order[i] = p_mos_vec[p_name_to_idx[p_goal_mos_order[i] ]].gate;
        }

        vector<string> find_p_gate_order;
        vector<string> find_p_path;
        if(rand()%2==1){
            string start1 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].source;
            vector<string> temp_gate_order;
            vector<string> temp_path;
            vector<string> one_gate;    //纗ヘ玡竒筁–mos秈, 程3element
            SA_DFS_travel(p_mos_map[start1],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
            if(find_p_gate_order.empty()){
                string start2 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].drain;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(p_mos_map[start2],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
            }
        }
        else{
            string start2 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].drain;
            vector<string> temp_gate_order;
            vector<string> temp_path;
            vector<string> one_gate;
            SA_DFS_travel(p_mos_map[start2],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
            if(find_p_gate_order.empty()){
                string start1 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].source;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(p_mos_map[start1],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
            }
        }      

        unordered_map<string,vector<string>> temp_n_gate_to_name(n_gate_to_name);
        vector<string> n_goal_mos_order(n_name_vec.size());
        
        for(int i=0;i<n_name_vec.size();i++){
            if(temp_n_gate_to_name[goal_gate_order[i]].size()==1){
                n_goal_mos_order[i] = temp_n_gate_to_name[goal_gate_order[i]][0];
            }
            else{
                int temp_idx2 = rand()%temp_n_gate_to_name[goal_gate_order[i]].size();
                n_goal_mos_order[i] = temp_n_gate_to_name[goal_gate_order[i]][temp_idx2];
                temp_n_gate_to_name[goal_gate_order[i]].erase( temp_n_gate_to_name[goal_gate_order[i]].begin()+temp_idx2, temp_n_gate_to_name[goal_gate_order[i]].begin()+temp_idx2+1);
            }    
        }

        vector<string> find_n_gate_order;
        vector<string> find_n_path;
        if(rand()%2==1){
            string start1 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].source;
            vector<string> temp_gate_order;
            vector<string> temp_path;
            vector<string> one_gate;    //纗ヘ玡竒筁–mos秈, 程3element
            SA_DFS_travel(n_mos_map[start1],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
            if(find_n_gate_order.empty()){
                string start2 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].drain;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(n_mos_map[start2],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
            }
        }
        else{
            string start2 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].drain;
            vector<string> temp_gate_order;
            vector<string> temp_path;
            vector<string> one_gate;
            SA_DFS_travel(n_mos_map[start2],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
            if(find_n_gate_order.empty()){
                string start1 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].source;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(n_mos_map[start1],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
            }
        }

        if(find_p_gate_order.size()>0 && find_n_gate_order.size()>0){

            //琩琌癸非gate ぃ癸非ぃdummy
            int p_idx = 0, n_idx=0;    
            while(p_idx<find_p_gate_order.size() && n_idx<find_n_gate_order.size()){
                if(find_p_gate_order[p_idx]!="Dummy"&&find_n_gate_order[n_idx]=="Dummy"){
                    find_p_gate_order.insert(find_p_gate_order.begin()+p_idx,"Dummy");

                    string pre_net = find_p_path[2*p_idx];
                    find_p_path.insert(find_p_path.begin()+(2*p_idx+1),pre_net);
                    find_p_path.insert(find_p_path.begin()+(2*p_idx+1),"Dummy");

                }
                    
                else if(find_n_gate_order[n_idx]!="Dummy"&&find_p_gate_order[p_idx]=="Dummy"){
                    find_n_gate_order.insert(find_n_gate_order.begin()+n_idx,"Dummy");

                    string pre_net = find_n_path[2*n_idx];
                    find_n_path.insert(find_n_path.begin()+(2*n_idx+1),pre_net);
                    find_n_path.insert(find_n_path.begin()+(2*n_idx+1),"Dummy");
                }
                    
                p_idx++;
                n_idx++;
            }

            double temp_hpwl = INT_MAX;   
            temp_hpwl = Cal_HPWL_dummy_exist(find_p_path,find_n_path,p_node_set,n_node_set,p_mos_vec[0].w, n_mos_vec[0].w);

            if(pre_hpwl==0&&temp_hpwl!=INT_MAX){
                pre_hpwl = temp_hpwl;
            }
            if(pre_hpwl!=0 && temp_hpwl!=INT_MAX){
                sum_cost += temp_hpwl;
                sum_time ++;
                double cost_change = temp_hpwl - pre_hpwl;
                if(cost_change>0){
                    sum_uphill_avg += cost_change;
                    uphill_time++;

                }
                pre_hpwl = temp_hpwl;
            }
            
        }
    }
    delta_avg = sum_uphill_avg/uphill_time;
    C_norm = sum_cost/sum_time;
}

void SA_find(double& best_HPWL,vector<string>& best_p_gate_order,vector<string>& best_n_gate_order,vector<string>& best_p_path,vector<string>& best_n_path,vector<mos>& p_mos_vec,vector<mos>& n_mos_vec,vector<graph_node>& n_mos_graph,unordered_map<string,int>& n_mos_map,vector<graph_node>& p_mos_graph,unordered_map<string,int>& p_mos_map,
            unordered_set<string>& n_node_set,unordered_set<string>& p_node_set,vector<string>& n_name_vec,vector<string>& p_name_vec,unordered_map<string,int>& n_name_to_idx,unordered_map<string,int>& p_name_to_idx,unordered_map<string,vector<string>>& n_gate_to_name){

    //cal uphill_avg, C_norm
    int c = 100;
    int k = 1;
    double delta_avg = 0,C_norm=0;
    Pre_SA_Cal(delta_avg,C_norm,n_node_set,p_node_set, p_mos_vec, n_mos_vec, n_mos_graph, n_mos_map, p_mos_graph, p_mos_map,n_name_vec,p_name_vec,n_name_to_idx, p_name_to_idx, n_gate_to_name);
    
    double P = 0.5;
    double T0;
    if(delta_avg==0){
        T0 = 1000;
    }
    else 
        T0 = -delta_avg/log(P);    
    double T = T0;
    
    int Iter = 0;
    double frozen = 0.1;
    double pre_cost = 0;
    double sum_cost_change = 0;
    int MT = 0;
    double TIME_LIMIT = 1800;
    double runtime=0;
    
    while(frozen<T && runtime<TIME_LIMIT){
        for(int k_idx=0;k_idx<k;k_idx++){

            vector<string> temp_p_name_vec(p_name_vec);
            vector<string> goal_gate_order(p_name_vec.size());
            vector<string> p_goal_mos_order(p_name_vec.size());;
            
            for(int i=0;i<p_name_vec.size();i++){
                int temp_idx = rand()%temp_p_name_vec.size();  
                p_goal_mos_order[i] = temp_p_name_vec[temp_idx];
                temp_p_name_vec.erase( temp_p_name_vec.begin()+temp_idx, temp_p_name_vec.begin()+temp_idx+1);

                goal_gate_order[i] = p_mos_vec[p_name_to_idx[p_goal_mos_order[i] ]].gate;
            }

            vector<string> find_p_gate_order;
            vector<string> find_p_path;
            if(rand()%2==1){
                string start1 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].source;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;    //纗ヘ玡竒筁–mos秈, 程3element
                SA_DFS_travel(p_mos_map[start1],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
                if(find_p_gate_order.empty()){
                    string start2 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].drain;
                    vector<string> temp_gate_order;
                    vector<string> temp_path;
                    vector<string> one_gate;
                    SA_DFS_travel(p_mos_map[start2],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
                }
            }
            else{
                string start2 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].drain;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(p_mos_map[start2],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
                if(find_p_gate_order.empty()){
                    string start1 = p_mos_vec[p_name_to_idx[p_goal_mos_order[0]]].source;
                    vector<string> temp_gate_order;
                    vector<string> temp_path;
                    vector<string> one_gate;
                    SA_DFS_travel(p_mos_map[start1],0,p_goal_mos_order,one_gate,find_p_gate_order,find_p_path,temp_gate_order,temp_path,p_mos_vec,p_mos_graph,p_mos_map,p_name_to_idx);
                }
            }      

            unordered_map<string,vector<string>> temp_n_gate_to_name(n_gate_to_name);
            vector<string> n_goal_mos_order(n_name_vec.size());
           
            for(int i=0;i<n_name_vec.size();i++){
                if(temp_n_gate_to_name[goal_gate_order[i]].size()==1){
                    n_goal_mos_order[i] = temp_n_gate_to_name[goal_gate_order[i]][0];
                }
                else{
                    int temp_idx2 = rand()%temp_n_gate_to_name[goal_gate_order[i]].size();
                    n_goal_mos_order[i] = temp_n_gate_to_name[goal_gate_order[i]][temp_idx2];
                    temp_n_gate_to_name[goal_gate_order[i]].erase( temp_n_gate_to_name[goal_gate_order[i]].begin()+temp_idx2, temp_n_gate_to_name[goal_gate_order[i]].begin()+temp_idx2+1);
                }    
            }

            vector<string> find_n_gate_order;
            vector<string> find_n_path;
            if(rand()%2==1){
                string start1 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].source;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;    //纗ヘ玡竒筁–mos秈, 程3element
                SA_DFS_travel(n_mos_map[start1],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
                if(find_n_gate_order.empty()){
                    string start2 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].drain;
                    vector<string> temp_gate_order;
                    vector<string> temp_path;
                    vector<string> one_gate;
                    SA_DFS_travel(n_mos_map[start2],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
                }
            }
            else{
                string start2 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].drain;
                vector<string> temp_gate_order;
                vector<string> temp_path;
                vector<string> one_gate;
                SA_DFS_travel(n_mos_map[start2],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
                if(find_n_gate_order.empty()){
                    string start1 = n_mos_vec[n_name_to_idx[n_goal_mos_order[0]]].source;
                    vector<string> temp_gate_order;
                    vector<string> temp_path;
                    vector<string> one_gate;
                    SA_DFS_travel(n_mos_map[start1],0,n_goal_mos_order,one_gate,find_n_gate_order,find_n_path,temp_gate_order,temp_path,n_mos_vec,n_mos_graph,n_mos_map,n_name_to_idx);
                }
            }
            
            if(find_p_gate_order.size()>0 && find_n_gate_order.size()>0){  

                //琩琌癸非gate ぃ癸非ぃdummy
                int p_idx = 0, n_idx=0;    
                while(p_idx<find_p_gate_order.size() && n_idx<find_n_gate_order.size()){
                    if(find_p_gate_order[p_idx]!="Dummy"&&find_n_gate_order[n_idx]=="Dummy"){
                        find_p_gate_order.insert(find_p_gate_order.begin()+p_idx,"Dummy");

                        string pre_net = find_p_path[2*p_idx];
                        find_p_path.insert(find_p_path.begin()+(2*p_idx+1),pre_net);
                        find_p_path.insert(find_p_path.begin()+(2*p_idx+1),"Dummy");
                    } 
                    else if(find_n_gate_order[n_idx]!="Dummy"&&find_p_gate_order[p_idx]=="Dummy"){
                        find_n_gate_order.insert(find_n_gate_order.begin()+n_idx,"Dummy");

                        string pre_net = find_n_path[2*n_idx];
                        find_n_path.insert(find_n_path.begin()+(2*n_idx+1),pre_net);
                        find_n_path.insert(find_n_path.begin()+(2*n_idx+1),"Dummy");
                    }
                    p_idx++;
                    n_idx++;
                }

                double temp_hpwl = INT_MAX;
                temp_hpwl = Cal_HPWL_dummy_exist(find_p_path,find_n_path,p_node_set,n_node_set,p_mos_vec[0].w, n_mos_vec[0].w);
                
                if(temp_hpwl<best_HPWL){
                    best_HPWL = temp_hpwl;
                    best_n_gate_order = find_n_gate_order;
                    best_p_gate_order = find_p_gate_order;
                    best_n_path = find_n_path;
                    best_p_path = find_p_path;
                }
                if(pre_cost!=0 && temp_hpwl!=INT_MAX &&C_norm!=0){
                    double cost_change = (temp_hpwl/C_norm) - pre_cost;
                    sum_cost_change += cost_change;
                    
                    MT++;
                    pre_cost = temp_hpwl/C_norm;
                }
                else if(pre_cost==0 && temp_hpwl!=INT_MAX)
                    pre_cost = temp_hpwl;
            }
        }
        
        //璝Τтpath 穝放
        if(sum_cost_change!=0){
            double avg_cost_change = sum_cost_change/MT;
            Iter++;
            T = T0*(abs(avg_cost_change)/(c*Iter));
        }
        if(delta_avg==0){
            Iter++;
            T = T0*pow(0.99,Iter);
        }
        time_t end = clock();
        runtime = ((double)(end))/CLOCKS_PER_SEC;
    }
}

int main(int argc, char* argv[]){
    srand(time(0));
    ifstream ifs;
    ifs.open(argv[1], ios::in);
    if(!ifs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }

    vector<mos> n_mos_vec;
    vector<mos> p_mos_vec;
    string line;
    vector<string> n_name_vec;
    vector<string> p_name_vec;
    unordered_set<string> p_node_set;
    unordered_set<string> n_node_set;

    while(getline( ifs, line)){  
        stringstream linestream;
        linestream.str(line);
        string str;
        if(line[0]=='M'){
            mos temp_mos;
            int cnt = 0;
            string mos_type;
            while(linestream>>str){
                if(cnt==0){
                    string name = str.substr(1);
                    temp_mos.name = name;
                }
                else if(cnt==1)
                    temp_mos.drain = str;
                else if(cnt==2)
                    temp_mos.gate = str;
                else if(cnt==3)
                    temp_mos.source = str;
                else if(cnt==4)
                    temp_mos.body = str;
                else if(cnt==5){
                    string type = str.substr(0,4);
                    mos_type = str[0];
                    if(mos_type=="p"){
                        p_node_set.insert(temp_mos.source);
                        p_node_set.insert(temp_mos.drain);
                    }   
                    else if(mos_type=="n"){
                        n_node_set.insert(temp_mos.source);
                        n_node_set.insert(temp_mos.drain);
                    }

                }
                else if(cnt==6||cnt==7){
                    string t = str.substr(0,2);
                    if(t=="w="){
                        string w_str = str.substr(2,str.length()-1);
                        temp_mos.w = atof(w_str.c_str());
                    }
                    else if(t=="l="){
                        string l_str = str.substr(2,str.length()-1);
                        temp_mos.l = atof(l_str.c_str());
                    }
                }   
                cnt++;
            }
            if(mos_type=="p"){
                p_mos_vec.push_back(temp_mos);
                p_name_vec.push_back(temp_mos.name);
            }  
            else{
                n_mos_vec.push_back(temp_mos);
                n_name_vec.push_back(temp_mos.name);      
            }
                
        }

    }
    
    vector<graph_node> n_mos_graph;
    unordered_map<string,int> n_mos_map;        //map node name to n_mos_graph position
    vector<graph_node> p_mos_graph;
    unordered_map<string,int> p_mos_map;        //map node name to p_mos_graph position

    unordered_map<string,int> n_name_to_idx;    //map n_mos_name to idx of n_mos_vec
    unordered_map<string,int> p_name_to_idx;    //map p_mos_name to idx of p_mos_vec
    unordered_map<string,vector<string>> n_gate_to_name;
    Build_graph(p_mos_vec,n_mos_vec,n_mos_graph, n_mos_map, p_mos_graph, p_mos_map,n_name_to_idx,p_name_to_idx,n_gate_to_name);
    
    vector<string> best_p_gate_order;
    vector<string> best_n_gate_order;
    vector<string> best_p_path;
    vector<string> best_n_path;
    double best_HPWL = INT_MAX;

    SA_find(best_HPWL,best_p_gate_order,best_n_gate_order,best_p_path,best_n_path,p_mos_vec,n_mos_vec,n_mos_graph, n_mos_map, p_mos_graph, p_mos_map,n_node_set,p_node_set,n_name_vec,p_name_vec,n_name_to_idx,p_name_to_idx,n_gate_to_name);
    
    ofstream ofs;
    ofs.open(argv[2]);
    if(!ofs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }
    ofs<<best_HPWL<<endl;
    
    for(auto e:best_p_gate_order)
        ofs<<e<<' ';
    ofs<<'\n';
    for(auto e:best_p_path){
        ofs<<e<<' ';
    }
    ofs<<'\n';
    for(auto e:best_n_gate_order)
        ofs<<e<<' ';
    ofs<<'\n';
    for(auto e:best_n_path){
        ofs<<e<<' ';
    }
    return 0;
}
