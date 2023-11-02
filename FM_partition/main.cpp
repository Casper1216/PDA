#include<iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include<list>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<time.h>
#include<limits.h>
#include<cmath>
#include<algorithm>

using namespace std;

bool check_balance(int G0_size,int G1_size,float balance_upper_bound,float balance_lower_bound){
    if((G0_size<=balance_upper_bound&&balance_lower_bound<=G0_size)&&(G1_size<=balance_upper_bound&&balance_lower_bound<=G1_size))
        return 1;
    else    
        return 0;
}

void compute_initial_gain(vector<vector<int>>& FT_n,vector<int>& gain,int Pmax,vector<vector<int>>& cell_array,vector<int>& cell_Group){
    
    for(int i=0;i<cell_array.size();i++){
        gain[i]=0;
        int G = cell_Group[i];   //cell[i] belong to Group G
        
        for(int j=0;j<cell_array[i].size();j++){
            if(FT_n[cell_array[i][j]][G]==1){           //when cell i F(n)==1
                gain[i]++;
            }
            if(FT_n[cell_array[i][j]][abs(G-1)]==0){    //when cell i T(n)==0
                gain[i]--;
            }
        }
    }
}

int compute_cutsize(vector<vector<int>>& net_array,vector<int>& cell_Group,unordered_map<int,int>& cell_to_idx){
    int cutsize=0;
    for(int i=0;i<net_array.size();i++){
        bool G0=0,G1=0;
        for(int j=0;j<net_array[i].size();j++){
            if(cell_Group[cell_to_idx[net_array[i][j]]]==0)
                G0=1;
            else
                G1=1;
        }
        if(G0 && G1)
            cutsize++;
    }
    return cutsize;
}

int get_move_cell(int max_gain,vector<vector<list<int>>>& gain_list,vector<vector<int>>& Group,int Pmax,float balance_upper_bound,float balance_lower_bound){
    //get gain max of cell k and check if move cell k is balance after move

    int move_cell;
    for(int i=(max_gain+Pmax);i>=0;i--){
        for(int g=0;g<gain_list.size();g++){
           
            for(list<int>::iterator itr=gain_list[g][i].begin();itr!=gain_list[g][i].end();itr++){
                int G0,G1;
                
                if(g==0){
                    G0 = Group[0].size()-1;
                    G1 = Group[1].size()+1;
                }
                else{
                    G0 = Group[0].size()+1;
                    G1 = Group[1].size()-1;
                }
                if(check_balance(G0,G1,balance_upper_bound,balance_lower_bound)){
                    move_cell = *itr;
                    gain_list[g][i].erase(itr);
                    return move_cell;
                
                }
            }
        }
    }
    return move_cell;
}

void get_update_gain(vector<int>& update_gain,vector<vector<int>>& FT_n,vector<vector<int>>& Group, vector<int>& cell_Group,
vector<vector<int>>& cell_array,vector<vector<int>>& net_array,unordered_map<int,int>& cell_to_idx,int move_cell,int& local_cutsize){
    
    int G = cell_Group[cell_to_idx[move_cell]];     //cell[i] belong to Group 0

    int F_B = G,T_B = abs(G-1);     //F_B: Front_Block of the move cell, T_B: To_Block of the move cell
    for(auto e:cell_array[cell_to_idx[move_cell]]){

        //Before move cell k
        if(FT_n[e][T_B]==0){
            for(int j=0;j<net_array[e].size();j++){
                update_gain[cell_to_idx[net_array[e][j]]]++;
            }
            local_cutsize++;
        }
        else if(FT_n[e][T_B]==1){
            for(int j=0;j<net_array[e].size();j++){
                if(cell_Group[cell_to_idx[net_array[e][j]]]==T_B)
                    update_gain[cell_to_idx[net_array[e][j]]]--;
            }
        }
        
        FT_n[e][F_B]--;
        FT_n[e][T_B]++;

        //After move cell k
        if(FT_n[e][F_B]==0){
            for(int j=0;j<net_array[e].size();j++){
                update_gain[cell_to_idx[net_array[e][j]]]--;
            }
            local_cutsize--;
        }
        else if(FT_n[e][F_B]==1){
            for(int j=0;j<net_array[e].size();j++){
                if(cell_Group[cell_to_idx[net_array[e][j]]]==F_B)
                    update_gain[cell_to_idx[net_array[e][j]]]++;
            }
        }
    }
    vector<int>::iterator erase_itr = find(Group[F_B].begin(),Group[F_B].end(),move_cell);
    Group[F_B].erase(erase_itr);
    Group[T_B].push_back(move_cell);
    cell_Group[cell_to_idx[move_cell]] = T_B;
}

void update_gain_list(vector<int>& gain_change_cell,vector<vector<list<int>>>& gain_list,vector<int>& cell_Group,unordered_map<int,int>& cell_to_idx,
vector<int>& gain,vector<int>& update_gain,unordered_set<int>& locked_set,vector<int>& cell,int Pmax,int num_cell){

    //only need to go through the cell of the net connected to move_cell
    for(auto e:gain_change_cell){
        if(locked_set.count(e))
            continue;
        int G = cell_Group[cell_to_idx[e]];     //cell[i] belong to Group 0
        
    
        list<int>::iterator erase_itr = find(gain_list[G][gain[cell_to_idx[e]]+Pmax].begin(),gain_list[G][gain[cell_to_idx[e]]+Pmax].end(),e);
        gain_list[G][gain[cell_to_idx[e]]+Pmax].erase(erase_itr);
        gain_list[G][update_gain[cell_to_idx[e]]+Pmax].emplace_back(e);
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

    string line;
    vector<unordered_set<int>> net_set;
    vector<vector<int>> net_array;      //net_array[i] save the cells connected the ith net
    unordered_set<int> cell_set;
    float balance_factor = 0;

    while(getline( ifs, line,' ')){  
        
        if(line[0]=='n'){
            unordered_set<int> cell;
            net_set.push_back(cell);
        }
        else if(line[0]=='c'){
            string num = line.substr(1);
            int n = atoi(num.c_str());
            net_set[net_set.size()-1].insert(n);
            cell_set.insert(n);
        }
        else if(line[0]!='N' && line[0]!=';'&&line!=""){
            balance_factor = atof(line.c_str());
        }
    }
    ifs.close();
    int num_cell = cell_set.size();
    int num_net = net_set.size();
    cout<<"balance_factor: "<<balance_factor<<endl;
    cout<<"num_cell: "<<num_cell<<" ,num_net: "<<num_net<<endl;
    
    for(int i=0;i<num_net;i++){
        vector<int> temp(net_set[i].begin(),net_set[i].end());
        net_array.push_back(temp);
    }
    
    vector<int> cell(cell_set.begin(),cell_set.end());
    unordered_map<int,int> cell_to_idx;     //cell number : idx in cell
    for(int i=0;i<num_cell;i++){
        cell_to_idx[cell[i]]= i;
    }
    vector<unordered_set<int>>().swap(net_set);
    unordered_set<int>().swap(cell_set);

    vector<vector<int>> cell_array(num_cell);     //cell_array[i] save the nets connect the ith cell
    for(int i=0;i<num_net;i++){
        for(int j=0;j<net_array[i].size();j++){
            cell_array[cell_to_idx[net_array[i][j]]].push_back(i);
        }
    }
    
    float balance_lower_bound = ceil((1-balance_factor)*0.5*num_cell);
    float balance_upper_bound = floor((1+balance_factor)*0.5*num_cell);
    cout<<"balance_lower_bound: "<<balance_lower_bound<<endl;
    cout<<"balance_upper_bound: "<<balance_upper_bound<<endl;  

    int Pmax=0;
    //Random divde to two partitions
    vector<vector<int>> Group_initial(2);
    vector<int> cell_vec(cell);
    for(int i=0;i<num_cell;i++){
        if(cell_array[i].size()>Pmax)
            Pmax = cell_array.size();

        int temp_idx = rand()%cell_vec.size();
        if(i%2)
            Group_initial[0].push_back(cell_vec[temp_idx]);
        else
            Group_initial[1].push_back(cell_vec[temp_idx]);
            
        cell_vec.erase(cell_vec.begin()+temp_idx,cell_vec.begin()+temp_idx+1);
        
    }
    vector<int>().swap(cell_vec);

    vector<int> cell_Group_initial(num_cell,0);
    for(int i=0;i<Group_initial.size();i++){
        for(auto e:Group_initial[i]){
            cell_Group_initial[cell_to_idx[e]] = i;
        }
    }

    vector<vector<int>> Group_result(Group_initial);
    int cutsize_result = INT_MAX;
    int G_k;
    
    do{
        time_t  start = clock();
        
        G_k = INT_MIN;
        vector<vector<int>> Group(Group_initial);
        vector<int> cell_Group(cell_Group_initial);
        
        vector<vector<int>> FT_n(net_array.size(),vector<int>(2,0));    //FT_n[i][0] represent F(i), FT_n[i][1] represent T(i)
        for(int i=0;i<net_array.size();i++){
            for(int j=0;j<net_array[i].size();j++){
                
                if(cell_Group[cell_to_idx[net_array[i][j]]]==0){
                    FT_n[i][0]++;
                }
                else{
                    FT_n[i][1]++;
                }
            }
        }
        
        vector<int> gain(num_cell,0);               //gain[k] represent cell[cell_to_idx[k]] have gain[k]
        compute_initial_gain(FT_n,gain,Pmax,cell_array,cell_Group);
        
        vector<vector<list<int>>> gain_list(2,vector<list<int>>(2*Pmax+1));  //gain -Pmax~Pmax, gain[i][0] = gain==-Pmax at Group i
        for(int i=0;i<num_cell;i++){
            if(cell_Group[i]==0){
                gain_list[0][gain[i]+Pmax].emplace_back(cell[i]);
            }
            else{
                gain_list[1][gain[i]+Pmax].emplace_back(cell[i]);
            }
        }

        int sum_gain=0;    
        unordered_set<int> locked_set;
        int local_cutsize = compute_cutsize(net_array,cell_Group,cell_to_idx);
        
        for(int c=0;c<num_cell;c++){
            

            int max_gain = INT_MIN;
            for(int i=0;i<num_cell;i++){
                if(gain[i]>max_gain)
                    max_gain = gain[i];
            }
            int move_cell = get_move_cell(max_gain,gain_list,Group,Pmax,balance_upper_bound,balance_lower_bound);
            
            locked_set.insert(move_cell);
            sum_gain += gain[cell_to_idx[move_cell]];        //add max_gain
            
            vector<int> update_gain(gain);  //new gain
            get_update_gain(update_gain,FT_n,Group,cell_Group,cell_array,net_array,cell_to_idx,move_cell,local_cutsize);
           
            vector<int> gain_change_cell;
            for(int i=0;i<num_cell;i++){
                if(gain[i]!=update_gain[i]){
                    gain_change_cell.push_back(cell[i]);
                }
            }
            
            
            update_gain_list(gain_change_cell,gain_list, cell_Group, cell_to_idx,gain, update_gain,locked_set, cell,Pmax, num_cell);
            
            
            //update gain  
            for(auto e:gain_change_cell)
                gain[cell_to_idx[e]] = update_gain[cell_to_idx[e]]; 

            //update result, update G_k
            if(G_k<sum_gain){
                
                G_k = sum_gain;
                Group_initial = Group;
                cell_Group_initial = cell_Group;

                if(local_cutsize<cutsize_result){
                    cutsize_result = local_cutsize;
                    Group_result = Group;
                }
                else if(local_cutsize==cutsize_result){
                    int local_diff = Group[0].size() - Group[1].size(); //Group size diff
                    int diff = Group_result[0].size() - Group_result[1].size();

                    if(abs(local_diff)<abs(diff)){   
                        Group_result = Group;
                    }   
                } 
                
            }
            else if(G_k==sum_gain){
                int local_diff = Group[0].size() - Group[1].size(); //Group size diff
                int diff = Group_initial[0].size() - Group_initial[1].size();
                
                if(abs(local_diff)<abs(diff)){   
                    Group_initial = Group;
                    cell_Group_initial = cell_Group;
                    
                }
                if(local_cutsize<cutsize_result){
                    cutsize_result = local_cutsize;
                    Group_result = Group;
                }
                else if(local_cutsize==cutsize_result){
                    int local_diff = Group[0].size() - Group[1].size(); //Group size diff
                    int diff = Group_result[0].size() - Group_result[1].size();

                    if(abs(local_diff)<abs(diff)){   
                        Group_result = Group;
                    } 
                }    
            }
        }
        cout<<"G_K: "<<G_k<<endl;
        time_t end = clock();
        float time = end - start; // ms
        cout<<"total: "<<(time / CLOCKS_PER_SEC)<< "second"<<endl;

        ofstream ofs;
        ofs.open(argv[2]);
        if(!ofs.is_open()) {
            cout << "Failed to open file.\n";
            return 1; // EXIT_FAILURE
        }
        ofs<<"Cutsize = "<<cutsize_result<<'\n';
        ofs<<"G1 "<<Group_result[0].size()<<'\n';
        for(auto e:Group_result[0])
            ofs<<'c'<<e<<' ';
        ofs<<';'<<'\n';
        ofs<<"G2 "<<Group_result[1].size()<<'\n';
        for(auto e:Group_result[1])
            ofs<<'c'<<e<<' ';
        ofs<<';'<<'\n';
        ofs.close();
        
    }while(G_k>0);    

    // cout<<"Answer Group"<<endl;
    // for(auto e:Group_result){
    //     for(auto i:e)
    //         cout<<i<<' ';
    //     cout<<endl;
    // }
    //cout<<"Cutsize_result: "<<cutsize_result<<endl;

    return 0;
}


