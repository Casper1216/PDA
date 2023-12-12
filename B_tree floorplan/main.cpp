#include<iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include<list>
#include<queue>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<time.h>
#include<limits.h>
#include<cmath>
#include<algorithm>
using namespace std;
//Fast SA

class block{
    public:
    int id;
    int x;
    int y;
    int width;
    int height;
    int rotate;
    block(): id(-1), x(-1), y(-1), width(-1), height(-1),rotate(0){};
    block(const block & new_block): id(new_block.id), x(new_block.x), y(new_block.y), width(new_block.width), height(new_block.height),rotate(new_block.rotate){};
    void set_origin();
    void display();
};
void block::set_origin(){
    id    =   -1;
    x     =   -1;
    y     =   -1;
    width =   -1;
    height=   -1;
    rotate=   0;
}
void block::display(){
    cout<<"id "<<id<<endl;
    cout<<"x: "<<x<<" ,y: "<<y<<" ,width: "<<width<<" ,height: "<<height<<endl;
    cout<<"rotate: "<<rotate<<endl;
}
class B_Tree_Node{
    public:
    int parent;
    int left;
    int right;
    B_Tree_Node():parent(-1),left(-1),right(-1){};
    void reset();
};
void B_Tree_Node::reset(){
    parent = -1;
    left = -1;
    right = -1;
}
int BuildInitBtree(vector<B_Tree_Node>& Btree,vector<block>& block_list,vector<int>& temp_vec){  //build tree, return idx in  block_list
    
    vector<int> vec(temp_vec);
    int root_idx = rand()%vec.size();
    int root = vec[root_idx];
    vec.erase(vec.begin()+root_idx,vec.begin()+root_idx+1);
    queue<int> q;
    q.push(root);
    while(!q.empty()){
        int q_size = q.size();
        for(int j=0;j<q_size;j++){
            int parent = q.front();
            q.pop();

            
            int place_case = rand()%3;
            if(place_case==0){
                //two child
                
                if(vec.size()>0){
                    int left_idx = rand()%vec.size();
                    int left = vec[left_idx];
                    vec.erase(vec.begin()+left_idx,vec.begin()+left_idx+1);
                    Btree[parent].left  = left;
                    q.push(left);
                    Btree[left].parent = parent;
                }
                if(vec.size()>0){
                    int right_idx = rand()%vec.size();
                    int right = vec[right_idx];
                    vec.erase(vec.begin()+right_idx,vec.begin()+right_idx+1);
                    Btree[parent].right = right;
                    q.push(right);
                    Btree[right].parent = parent;
                }

            }
            else if(place_case==1){
                if(vec.size()>0){
                    int left_idx = rand()%vec.size();
                    int left = vec[left_idx];
                    vec.erase(vec.begin()+left_idx,vec.begin()+left_idx+1);
                    Btree[parent].left  = left;
                    q.push(left);
                    Btree[left].parent = parent;
                }
                
            }
            else{
                if(vec.size()>0){
                    int right_idx = rand()%vec.size();
                    int right = vec[right_idx];
                    vec.erase(vec.begin()+right_idx,vec.begin()+right_idx+1);
                    Btree[parent].right = right;
                    q.push(right);
                    Btree[right].parent = parent;
                }

            }
            
        }
            
    }
    return root;
}

//Move
void rotate(int node,vector<block>& block_list){
    int temp =  block_list[node].width;
    block_list[node].width = block_list[node].height;
    block_list[node].height = temp;
    block_list[node].rotate = 1 - block_list[node].rotate;
}
void swap_node(int node_1,int node_2,int &root,vector<B_Tree_Node>& Btree,vector<block>& block_list){
    
    int node_1_parent = Btree[node_1].parent, node_2_parent = Btree[node_2].parent;
    //swap parent
    if(node_1_parent!=-1){

        if(Btree[node_1_parent].left == node_1)
            Btree[node_1_parent].left = node_2;
        else if(Btree[node_1_parent].right == node_1)
            Btree[node_1_parent].right = node_2;
        else {
            cout << "[Error] node not parent's child\n";
            exit(1);
        }
    }
    if(node_2_parent!=-1){
       
        if(Btree[node_2_parent].left == node_2)
            Btree[node_2_parent].left = node_1;
        else if(Btree[node_2_parent].right == node_2)
            Btree[node_2_parent].right = node_1;
        else {
            cout << "[Error] node not parent's child\n";
            exit(1);
        }

    }
    Btree[node_1].parent= node_2_parent;
    Btree[node_2].parent= node_1_parent;

    int node_1_left = Btree[node_1].left, node_1_right = Btree[node_1].right;
    int node_2_left = Btree[node_2].left, node_2_right = Btree[node_2].right;
    //swap node_1 node_2 child
    Btree[node_1].left  = node_2_left;
    Btree[node_1].right = node_2_right;
    Btree[node_2].left  = node_1_left;
    Btree[node_2].right = node_1_right;

    //swap child's parent
    if(node_1_left!=-1){
        Btree[node_1_left].parent = node_2;
    }
    if(node_1_right!=-1){
        Btree[node_1_right].parent = node_2;
    }
    if(node_2_left!=-1){
        Btree[node_2_left].parent = node_1;
    }
    if(node_2_right!=-1){
        Btree[node_2_right].parent = node_1;
    }

    if(node_1==root){
        root = node_2;
    }
    else if(node_2==root){
        root = node_1;
    }
}

void del(int node,int &root,vector<B_Tree_Node>& Btree,vector<block>& block_list){
    if(Btree[node].left==-1&& Btree[node].right==-1){
        //leaf
        int parent = Btree[node].parent;
        if(Btree[parent].left==node)
            Btree[parent].left = -1;
        else if(Btree[parent].right==node)
            Btree[parent].right = -1;
        else{
            cout << "[Error] node not parent's child\n";
            exit(1);
        }
    }
    else if(Btree[node].left !=-1 && Btree[node].right !=-1){
        //two child
        do{
            int is_left_replace ;
            if(Btree[node].left!=-1&&Btree[node].right!=-1){
                is_left_replace = rand()%2;
            }
            else if(Btree[node].left!=-1)
                is_left_replace = 1;
            else 
                is_left_replace = 0;

            if(is_left_replace){
                swap_node(node,Btree[node].left,root,Btree,block_list);
            }
            else{
                swap_node(node,Btree[node].right,root,Btree,block_list);
            }

        }while(Btree[node].left!=-1 || Btree[node].right!=-1);  //swap to leaf

        int parent = Btree[node].parent;
        if(Btree[parent].left==node)
            Btree[parent].left = -1;
        else if(Btree[parent].right==node)
            Btree[parent].right = -1;
        else{
            cout << "[Error] node not parent's child\n";
            exit(1);
        }
    }
    else{
        //one child
        int child;
        if(Btree[node].left!=-1)
            child = Btree[node].left;
        else
            child = Btree[node].right;
        int parent = Btree[node].parent;
        if(parent!=-1){
            if(Btree[parent].left==node)
                Btree[parent].left = child;
            else if(Btree[parent].right==node)
                Btree[parent].right = child;
            else{
                cout << "[Error] node not parent's child\n";
                exit(1);
            }
        }
        Btree[child].parent = parent;
        if(root==node)
            root = child;
    }
    
    Btree[node].parent = -1;
    Btree[node].left = -1;
    Btree[node].right =-1;
}

void delete_and_insert(int node,int to_node,int &root,vector<B_Tree_Node>& Btree,vector<block>& block_list){
    //delete
    del(node,root,Btree,block_list);
    //insert
    int four_case = rand()%4;
    int child;
    if(four_case==0){
        child = Btree[to_node].left;
        Btree[to_node].left = node;
        Btree[node].right = -1;
        Btree[node].left = child;
    }
    else if(four_case==1){
        child = Btree[to_node].left;
        Btree[to_node].left = node;
        Btree[node].left = -1;
        Btree[node].right = child;

    }
    else if(four_case==2){
        child = Btree[to_node].right;
        Btree[to_node].right = node;
        Btree[node].right = -1;
        Btree[node].left = child;
    }
    else{
        child = Btree[to_node].right;
        Btree[to_node].right = node;
        Btree[node].left = -1;
        Btree[node].right = child;

    }
    Btree[node].parent = to_node;
    if (child != -1)
        Btree[child].parent = node;
}

void Preorder_traversal(int node,vector<B_Tree_Node>& Btree,vector<block>& block_list,list<pair<int,int>>& h_list,bool is_left,int& x_floorplan,int& y_floorplan){
    if(node==-1)
        return ;
    int parent = Btree[node].parent;
    if(is_left)
        block_list[node].x = block_list[parent].x + block_list[parent].width;
    else    
        block_list[node].x = block_list[parent].x;
    if(x_floorplan<block_list[node].x+block_list[node].width)
        x_floorplan = block_list[node].x+block_list[node].width;
    int x_start = block_list[node].x, x_end = block_list[node].x+block_list[node].width;
    int y_max = 0;
    list<pair<int,int>>::iterator start_point=h_list.begin(),end_point=h_list.end();
    for(list<pair<int,int>>::iterator itr=h_list.begin();itr!=h_list.end();itr++){
        if((*itr).first==x_start){
            start_point = itr;
        }
        if((*itr).first>x_start)
            break;
    }

    for(list<pair<int,int>>::iterator itr=start_point;itr!=h_list.end();itr++){
        if((*itr).first>=x_start && (*itr).first<x_end && (*itr).second>y_max){
            y_max = (*itr).second;
        }  
        if((*itr).first==x_end){
            end_point = itr;
        }
        else if((*itr).first>x_end){
            if(end_point==h_list.end())
                end_point = itr;
            break;
        }
    }
    list<pair<int,int>> C_new;
    C_new.push_back(make_pair(x_start,y_max+block_list[node].height));
    C_new.push_back(make_pair(x_end,y_max+block_list[node].height));
    C_new.push_back(make_pair(x_end,y_max));    
    
    list<pair<int,int>>::iterator pre_start_point = start_point;
    pre_start_point--;
    h_list.erase(start_point,end_point);

    pre_start_point++;
    h_list.insert(pre_start_point,C_new.begin(),C_new.end()); 

    block_list[node].y = y_max;
    if(y_floorplan<y_max+block_list[node].height)
        y_floorplan = y_max+block_list[node].height;
    Preorder_traversal(Btree[node].left, Btree, block_list, h_list,1,x_floorplan,y_floorplan);
    Preorder_traversal(Btree[node].right, Btree, block_list, h_list,0,x_floorplan,y_floorplan);

}
void B_tree_to_Floorplan(int& x_floorplan,int& y_floorplan,int& root,vector<B_Tree_Node>& Btree,vector<block>& block_list){
    list<pair<int,int>> h_list;
    h_list.push_back(make_pair(0,0));
    h_list.push_back(make_pair(INT_MAX,0));
    
    block_list[root].x =0;
    block_list[root].y =0;
    list<pair<int,int>> C_new;

    C_new.push_back(make_pair(0,block_list[root].height));
    C_new.push_back(make_pair(block_list[root].width,block_list[root].height));
    C_new.push_back(make_pair(block_list[root].width,0));
    h_list.insert(++h_list.begin(),C_new.begin(),C_new.end());
    
    x_floorplan = block_list[root].width;
    y_floorplan = block_list[root].height;
    Preorder_traversal(Btree[root].left, Btree, block_list, h_list,1,x_floorplan,y_floorplan);
    Preorder_traversal(Btree[root].right, Btree, block_list, h_list,0,x_floorplan,y_floorplan);
}

void calculate_delta_avg(double& delta_avg,double& A_norm,int& Best_root,vector<B_Tree_Node>& Best_Btree,vector<block>& Best_block_list,int& x_floorplan,int& y_floorplan,double R_upper_bound,double R_lower_bound,int z){

    int num_uphill = 0;
    int move_time = 0;
    vector<B_Tree_Node> Btree(Best_Btree);
    vector<block> block_list(Best_block_list);
    int root = Best_root;
    int num_block = Best_block_list.size();
    int pre_area = x_floorplan*y_floorplan;
    int x = x_floorplan, y = y_floorplan;
    for(int i=0;i<(z*num_block);i++){ 
        vector<B_Tree_Node> temp_Btree(Btree);
        vector<block> temp_block_list(block_list);
        int temp_root = root; 
        int Move_case = rand()%3;
        if(Move_case==0){
            //rotate
            int node = rand()%num_block;
            rotate(node,temp_block_list);
        } 
        else if(Move_case==1){
            //swap
            int node_1 = rand()%num_block;
            int node_2 = rand()%num_block;
            while(node_1==node_2)
                node_2 = rand()%num_block;
            swap_node(node_1,node_2,temp_root,temp_Btree,temp_block_list);
        }
        else{
            //del_and_insert
            int node = rand()%num_block;
            int to_node = rand()%num_block;
            while(node==to_node||temp_Btree[node].parent==to_node)
                to_node = rand()%num_block;
            delete_and_insert(node,to_node,temp_root,temp_Btree,temp_block_list);
        }
        B_tree_to_Floorplan(x,y,temp_root,temp_Btree,temp_block_list);
        double Ratio = (double)x/(double)y;
        
        int cur_area = x*y;
        int cost_change = cur_area - pre_area;
        
        if(cost_change > 0){
            delta_avg += cost_change;
            num_uphill++;
        }
        pre_area = cur_area;
        Btree = temp_Btree;
        block_list = temp_block_list;
        root = temp_root;
        A_norm += cur_area;
        move_time ++;
            
    }
    delta_avg /= num_uphill;
    A_norm /= move_time;
}
void SA(int& Best_root,vector<B_Tree_Node>& Best_Btree,vector<block>& Best_block_list,int& x_floorplan,int& y_floorplan,double R_upper_bound,double R_lower_bound){
    
    vector<B_Tree_Node> Btree(Best_Btree);
    vector<block> block_list(Best_block_list);
    int root = Best_root;
    B_tree_to_Floorplan(x_floorplan, y_floorplan,Best_root,Best_Btree,Best_block_list);

    int num_block = Best_block_list.size();
    const double P = 0.85;
    const double fronzed = 0.001;
    const int c = 100;
    const int k = 7;
    const int z = 10;
    const int N = z*num_block;    //each temperature move time
    //perturb num_block time calculate delta_avg
    double delta_avg =0 ,A_norm =0;
    calculate_delta_avg(delta_avg,A_norm, Best_root,Best_Btree,Best_block_list,x_floorplan,y_floorplan,R_upper_bound,R_lower_bound,z);

    double min_cost = (x_floorplan*y_floorplan)/A_norm;
    const double T0 = -delta_avg / log(P);
    double T = T0;
    int MT = 0;
    int uphill = 0;
    int reject = 0;

    const int TIME_LIMIT = 1800 ; // 30 minutes
    double runtime = 0;
    double pre_cost = min_cost;
    double sum_cost_change = 0;
    double avg_cost_change = 0;
    int move_time = 0;
    int iter = 1;
    do{
        MT = 0;
        uphill = 0;
        reject = 0;
        do{
            vector<block> temp_block_list(block_list);
            vector<B_Tree_Node> temp_Btree(Btree);
            int temp_x = x_floorplan, temp_y = y_floorplan;
            int temp_root = root;
            int Move_case = rand()%3;
            if(Move_case==0){
                //rotate
                int node = rand()%num_block;
                rotate(node,temp_block_list);
            } 
            else if(Move_case==1){
                //swap
                int node_1 = rand()%num_block;
                int node_2 = rand()%num_block;
                while(node_1==node_2)
                    node_2 = rand()%num_block;
                swap_node(node_1,node_2,temp_root,temp_Btree,temp_block_list);
            }
            else{
                //del_and_insert
                int node = rand()%num_block;
                int to_node = rand()%num_block;
                while(node==to_node||temp_Btree[node].parent==to_node)
                    to_node = rand()%num_block;
                delete_and_insert(node,to_node,temp_root,temp_Btree,temp_block_list);
            }
            MT++;
            
            B_tree_to_Floorplan(temp_x, temp_y,temp_root,temp_Btree,temp_block_list); 
            double Ratio = (double)temp_x/(double)temp_y;
            double new_cost = ((double)(temp_x*temp_y))/A_norm;
            double cost_change = new_cost - pre_cost;
            double prob_uphill = ((double)rand()) / RAND_MAX;    //0~1
            
            if(cost_change<=0||prob_uphill<exp(-abs(cost_change)/T)){
                if(cost_change>0)
                    uphill++;
                if(new_cost<min_cost&&(Ratio<=R_upper_bound && Ratio>=R_lower_bound)){
                    
                    x_floorplan = temp_x;
                    y_floorplan = temp_y;
                    min_cost = new_cost;
                    Best_Btree = temp_Btree;
                    Best_block_list = temp_block_list;
                    Best_root = temp_root;
                }

                root = temp_root;
                Btree = temp_Btree;
                block_list = temp_block_list;
            }
            else
                reject++;   //if bad answer and random > exp() then Btree do not update
            sum_cost_change += cost_change;
            pre_cost = new_cost;
            move_time++;
            
        }while(uphill <= N && MT <= 2 * N);
        avg_cost_change = sum_cost_change/move_time;
        iter++;
        if(iter>=2&&iter<=k){
            T = (T0*abs(avg_cost_change))/(iter*c);
        }
        else{
            T = (T0*abs(avg_cost_change))/(iter);
        }
        
        time_t end = clock();
        runtime = ((double)(end))/CLOCKS_PER_SEC;
        cout<<"T: "<<T<<" ,runtime: "<<runtime<<" ,R: "<<(double)x_floorplan/(double)y_floorplan<<endl;
    }while(T>fronzed && (runtime)<TIME_LIMIT &&((double)reject/MT)<0.95);
}
int main(int argc, char* argv[]){
    srand(time(0));
    
    ifstream ifs;
    ifs.open(argv[1], ios::in);
    if(!ifs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }

    vector<block> Best_block_list;
    double R_lower_bound=-1,R_upper_bound=-1;
    block temp_b;

    for(string line; ifs >> line;){  
        if(line[0]=='b'){
            string num = line.substr(1);
            int n = atoi(num.c_str());
            temp_b.id = n;
        }
        else{
            if(temp_b.id==-1){
                if(R_lower_bound==-1){
                    R_lower_bound = atof(line.c_str());
                }
                else{
                    R_upper_bound = atof(line.c_str());;
                }
            }
            else if(temp_b.id!=-1&&temp_b.width==-1){
                int n = atoi(line.c_str());
                temp_b.width = n;
            }
            else{
                int n = atoi(line.c_str());
                temp_b.height = n;
                block new_b(temp_b);
                temp_b.set_origin();
                Best_block_list.push_back(new_b);

            }
        }
    }
    ifs.close();

    int num_block = Best_block_list.size();
    unordered_map<int,int> idx_to_id;
    vector<int> temp_vec(num_block);

    for(int i=0;i<num_block;i++){
        idx_to_id[i] = Best_block_list[i].id;
        temp_vec[i] = i;
    }

    vector<B_Tree_Node> Best_Btree(num_block);
    int x,y;
    int Best_root;
    
    double runtime=0;
    do{
        for(int i=0;i<num_block;i++)
            Best_Btree[i].reset();
        Best_root = BuildInitBtree(Best_Btree, Best_block_list, temp_vec); //root is idx in Best_block_list
        B_tree_to_Floorplan(x,y,Best_root, Best_Btree,Best_block_list);
        time_t end = clock();
        runtime = ((double)(end))/CLOCKS_PER_SEC;
    }while((R_lower_bound>((double)x/(double)y)||((double)x/(double)y)>R_upper_bound)&&runtime<5);
        
    int x_floorplan,y_floorplan;    
    //Run SA
    SA(Best_root,Best_Btree,Best_block_list,x_floorplan,y_floorplan,R_upper_bound,R_lower_bound);
    
    ofstream ofs;
    ofs.open(argv[2]);
    if(!ofs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }
    int area = x_floorplan*y_floorplan;
    double Ratio = (double)x_floorplan/(double)y_floorplan;
    ofs<<"A = "<<(int)area<<'\n';
    ofs<<"R = "<<Ratio<<'\n';
    for(int i=0;i<num_block;i++){
        ofs<<'b'<<Best_block_list[i].id<<' '<<Best_block_list[i].x<<' '<<Best_block_list[i].y;
        if(Best_block_list[i].rotate==1)
            ofs<<" R"<<'\n';
        else
            ofs<<'\n';
    }
    return 0;
}