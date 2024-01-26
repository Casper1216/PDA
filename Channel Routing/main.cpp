#include<iostream>
#include <iomanip>
#include<sstream>
#include<fstream>
#include<vector>
#include<string>
#include<unordered_map>
#include<map>
#include<unordered_set>
#include<time.h>
#include<limits.h>
#include<algorithm>

using namespace std;

class Net{
    public:
    int net_name;
    int ID;     //ID in VCG
    pair<int,int> horiztal_interval;
    unordered_set<int> parent;     //VCG    save ID
    unordered_set<int> children;   //VCG    save ID
    Net(const int& net_name,const int& ID):net_name(net_name),ID(ID),horiztal_interval(make_pair(-1,-1)){};
};
class Net_output_inform{
    public:
    int net_name;
    int track_idx;
    pair<int,int> horiztal_interval;
    Net_output_inform(int& net_name,int& track_idx):net_name(net_name),track_idx(track_idx),horiztal_interval(make_pair(-1,-1)){};
};

bool Left_edge_compare(Net& a,Net& b){
    if(a.horiztal_interval.first!=b.horiztal_interval.first)
        return a.horiztal_interval.first<b.horiztal_interval.first;
    else
        return a.horiztal_interval.second<b.horiztal_interval.second;
}
bool net_compare(Net_output_inform& a,Net_output_inform& b){
    if(a.net_name==b.net_name)
        return a.horiztal_interval.first<b.horiztal_interval.first;
    else
        return a.net_name<b.net_name;
}
class Left_edge_Alg{
    public:
    vector<vector<int>> Track;
    vector<Net> Net_vec;
    unordered_map<int,int> VCG_ID_to_Net_vec_idx;
    unordered_set<int> VCG_id_set;
    vector<unordered_set<int>> net_ID_T_B_set;
    void Build_Net_inform(vector<vector<vector<int>>>& ID_T_B,vector<unordered_set<int>>& VCG_ID_T_B_set,vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
    void initial_track(vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
    void initial_VCG(unordered_set<int>& unscheduled,vector<vector<vector<int>>>& ID_T_B);
    bool check_is_root(int ID,unordered_set<int>& unscheduled);
    bool check_is_vaild_interval(int track_idx,int ID);
    bool check_is_zero_track(int track_idx,pair<int,int>& seg);
    void pin_connect(int track_idx,int& top_track_num,unordered_set<int>& unscheduled,vector<int>& unscheduled_vec,vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
    void down_compact(bool& is_change,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
    void LEA(vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
    void output_data(ofstream & filename,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary);
};

void Left_edge_Alg::Build_Net_inform(vector<vector<vector<int>>>& ID_T_B,vector<unordered_set<int>>& VCG_ID_T_B_set,vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    unordered_set<int> net_id_set;
    unordered_map<int,int> net_ID_to_pre_seg_idx;
    unordered_map<int,int> net_ID_to_pre_seg_top_or_down;   //pre_seg is on top ==0 , otherwise, ==1

    net_ID_T_B_set = vector<unordered_set<int>>(2);
    for(int i=0;i<net_ID_T_B[0].size();i++){
        if(!net_id_set.count(net_ID_T_B[0][i])&& net_ID_T_B[0][i]!=0){
            int ID = Net_vec.size()+1;
            VCG_ID_to_Net_vec_idx[net_ID_T_B[0][i]] = ID;
            net_ID_to_pre_seg_idx[net_ID_T_B[0][i]] = Net_vec.size();   //record net pre seg idx
            
            Net temp(net_ID_T_B[0][i],ID);        
            temp.horiztal_interval.first = i;
            VCG_id_set.insert(ID);      //update VCG ID
            ID_T_B[0][i].push_back(ID); //update seg pin ,represented by VCG ID
            VCG_ID_T_B_set[0].insert(ID);

            Net_vec.push_back(temp);
            net_id_set.insert(net_ID_T_B[0][i]);
        }
        else if(net_id_set.count(net_ID_T_B[0][i])){
            if(Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]].horiztal_interval.second==-1){
                Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]].horiztal_interval.second = i;
                ID_T_B[0][i].push_back(net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]+1);
                VCG_ID_T_B_set[0].insert(net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]+1);
                net_ID_to_pre_seg_top_or_down[net_ID_T_B[0][i]] = 0;
            }
            else{
                int ID = Net_vec.size()+1;
                Net temp(net_ID_T_B[0][i],ID); 
                temp.horiztal_interval.first = Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]].horiztal_interval.second;
                temp.horiztal_interval.second= i;
                
                ID_T_B[net_ID_to_pre_seg_top_or_down[net_ID_T_B[0][i]]][Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[0][i]]].horiztal_interval.second].push_back(ID);
                ID_T_B[0][i].push_back(ID);

                VCG_id_set.insert(ID);
                VCG_ID_T_B_set[0].insert(ID);
                net_ID_to_pre_seg_top_or_down[net_ID_T_B[0][i]] = 0;
                net_ID_to_pre_seg_idx[net_ID_T_B[0][i]] = Net_vec.size();
                Net_vec.push_back(temp);
            }
        }
        if(!net_id_set.count(net_ID_T_B[1][i]) && net_ID_T_B[1][i]!=0){
            int ID = Net_vec.size()+1;
            VCG_ID_to_Net_vec_idx[net_ID_T_B[1][i]] = ID;
            net_ID_to_pre_seg_idx[net_ID_T_B[1][i]] = Net_vec.size();   

            Net temp(net_ID_T_B[1][i],ID);        
            temp.horiztal_interval.first = i;
            VCG_id_set.insert(ID);
            ID_T_B[1][i].push_back(ID); //update seg pin ,represented by VCG ID
            VCG_ID_T_B_set[1].insert(ID);

            Net_vec.push_back(temp);
            net_id_set.insert(net_ID_T_B[1][i]);
        }
        else if(net_id_set.count(net_ID_T_B[1][i])){
            if(Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]].horiztal_interval.second==-1){
                Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]].horiztal_interval.second = i;
                ID_T_B[1][i].push_back(net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]+1);
                VCG_ID_T_B_set[1].insert(net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]+1);
                net_ID_to_pre_seg_top_or_down[net_ID_T_B[1][i]] = 1;
            }
            else{
                int ID = Net_vec.size()+1;
                Net temp(net_ID_T_B[1][i],ID); 
                temp.horiztal_interval.first = Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]].horiztal_interval.second;
                temp.horiztal_interval.second= i;
                VCG_id_set.insert(ID);
                ID_T_B[net_ID_to_pre_seg_top_or_down[net_ID_T_B[1][i]]][Net_vec[net_ID_to_pre_seg_idx[net_ID_T_B[1][i]]].horiztal_interval.second].push_back(ID);
                ID_T_B[1][i].push_back(ID);
                VCG_ID_T_B_set[1].insert(ID);

                net_ID_to_pre_seg_idx[net_ID_T_B[1][i]] = Net_vec.size();
                net_ID_to_pre_seg_top_or_down[net_ID_T_B[1][i]] = 1;
                Net_vec.push_back(temp);
            }
        }
        net_ID_T_B_set[0].insert(net_ID_T_B[0][i]);
        net_ID_T_B_set[1].insert(net_ID_T_B[1][i]);
    }
}

void Left_edge_Alg::initial_track(vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    Track = vector<vector<int>>(Top_boundary.size()-1,vector<int>(net_ID_T_B[0].size(),0));
    for(auto e:Top_boundary){
        if(e.first==Top_boundary.size()-1)
            break;
        for(auto r:e.second){
            int t_idx = abs((long)Top_boundary.size()-2-e.first);
            for(int j=r.first; j<=r.second;j++){
                Track[t_idx][j] = -1;
            }
            if(e.first>=1){
                for(int i=0;i<net_ID_T_B[0].size();i++)
                    Track[t_idx][i] += Track[t_idx+1][i]; 
            }
        }
    }
    for(auto e:Bottom_boundary){
        if(e.first==Bottom_boundary.size()-1)
            break;
        
        vector<int> temp(net_ID_T_B[0].size(),0);
        for(auto r:e.second){
            for(int j = r.first; j<= r.second;j++){
                temp[j] = -1;
            }
        }
        Track.push_back(temp);
        if(e.first>=1){
            for(int i=0;i<net_ID_T_B[0].size();i++)
                Track[Track.size()-1][i] += Track[Track.size()-2][i]; 
        }
    }
}

void Left_edge_Alg::initial_VCG(unordered_set<int>& unscheduled,vector<vector<vector<int>>>& ID_T_B){

    for(int i=0;i<ID_T_B[0].size();i++){
        for(auto e1:ID_T_B[0][i]){
            if(unscheduled.count(e1)){
                for(auto e2:ID_T_B[1][i]){
                    if(unscheduled.count(e2)){
                        Net_vec[VCG_ID_to_Net_vec_idx[e1]].children.insert(e2);
                        Net_vec[VCG_ID_to_Net_vec_idx[e2]].parent.insert(e1);
                    }

                }
            }
        }
    }
}

bool Left_edge_Alg::check_is_root(int ID,unordered_set<int>& unscheduled){
    for(auto e:Net_vec[VCG_ID_to_Net_vec_idx[ID]].parent){
        if(unscheduled.count(e))
            return false;
        else{
            if(!check_is_root(e, unscheduled))
                return false;
        } 
    }
    return true;
}

bool Left_edge_Alg::check_is_vaild_interval(int track_idx,int ID){
    pair<int,int> seg = Net_vec[VCG_ID_to_Net_vec_idx[ID]].horiztal_interval;
    int name = Net_vec[VCG_ID_to_Net_vec_idx[ID]].net_name;
    bool is_true = true;

    for(int i=seg.first;i<=seg.second;i++){
        if(i==seg.first||i==seg.second){
            if(Track[track_idx][i]<0)
                return false;
            else if(Track[track_idx][i]>0 && name!=Net_vec[VCG_ID_to_Net_vec_idx[Track[track_idx][i]]].net_name)
                return false;
        }
        else if(Track[track_idx][i]!=0)
            return false;
    }
    return true;
}

bool Left_edge_Alg::check_is_zero_track(int track_idx,pair<int,int>& seg){
    bool zero_track = true;
    for(int j=seg.first;j<=seg.second;j++){
        if(Track[track_idx][j]!=0)
            zero_track = false;
    }
    return zero_track;
}

void Left_edge_Alg::pin_connect(int track_idx,int& top_track_num,unordered_set<int>& unscheduled,vector<int>& unscheduled_vec,vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    //top_track_num為 initial後的數量
    int Top_track_end_idx = top_track_num-1;    
    
    if(track_idx<=Top_track_end_idx){
        for(int i=0;i<unscheduled_vec.size();i++){
            pair<int,int> seg = Net_vec[VCG_ID_to_Net_vec_idx[unscheduled_vec[i]]].horiztal_interval;
            int name = Net_vec[VCG_ID_to_Net_vec_idx[unscheduled_vec[i]]].net_name;
            if(check_is_root(unscheduled_vec[i],unscheduled)&&check_is_vaild_interval(track_idx,unscheduled_vec[i])){
                for(int t=seg.first;t<=seg.second;t++)
                    Track[track_idx][t] = unscheduled_vec[i];
                unscheduled.erase(unscheduled_vec[i]);
                unscheduled_vec.erase(unscheduled_vec.begin()+i,unscheduled_vec.begin()+i+1);
                i--;
            }   
        
        }
    }
    else{
        //做一般的 LEA , 直接加新的track
        vector<int> new_track(net_ID_T_B[0].size(),0);
        int watermark = -1;

        for(int i=0;i<unscheduled_vec.size();i++){
            pair<int,int> seg = Net_vec[VCG_ID_to_Net_vec_idx[unscheduled_vec[i]]].horiztal_interval;
            int name = Net_vec[VCG_ID_to_Net_vec_idx[unscheduled_vec[i]]].net_name;
            if(check_is_root(unscheduled_vec[i],unscheduled)&& (seg.first>watermark||(seg.first==watermark&&Net_vec[VCG_ID_to_Net_vec_idx[new_track[watermark]]].net_name==name))){
                for(int j=seg.first;j<=seg.second;j++){
                    new_track[j] =  unscheduled_vec[i];
                }

                watermark = seg.second;
                unscheduled.erase(unscheduled_vec[i]);
                unscheduled_vec.erase(unscheduled_vec.begin()+i,unscheduled_vec.begin()+i+1);
                i--;
            
            }
        }
        Track.insert(Track.begin()+ track_idx,new_track);
    }
}

void Left_edge_Alg::down_compact(bool& is_change,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    
    int top_num = Top_boundary.size()-1;
    for(int i=Track.size()-2; i>=top_num ;i--){
        
        for(int j=0;j<Track[i].size();j++){
            if(Track[i][j]>0){
                int netname = Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name;
                pair<int,int> seg;
                seg.first = j;
                int pre = netname;
                while(VCG_ID_to_Net_vec_idx.count(Track[i][j]) &&j<Track[i].size()&&Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name==pre){
                    pre = Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name;
                    j++;
                }
                seg.second = j-1;

                if(check_is_zero_track(i+1,seg)){
                    is_change = true;
                    for(int q=seg.first;q<=seg.second;q++){
                        swap(Track[i][q],Track[i+1][q]);
                    }
                }
                j--;
            }
        }

        pair<int,int> temp = make_pair(0,Track.size()-1);
        if(check_is_zero_track(i,temp)){
            Track.erase(Track.begin()+i,Track.begin()+i+1);
        }
    }
}

void Left_edge_Alg::LEA(vector<vector<int>>& net_ID_T_B,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    
    vector<vector<vector<int>>> ID_T_B(2,vector<vector<int>>(net_ID_T_B[0].size()));
    vector<unordered_set<int>> VCG_ID_T_B_set(2);
    Build_Net_inform(ID_T_B,VCG_ID_T_B_set,net_ID_T_B, Top_boundary, Bottom_boundary);
    
    //Left edge ordered
    sort(Net_vec.begin(),Net_vec.end(),Left_edge_compare);
    vector<int> unscheduled_vec(Net_vec.size());
    //update Net_vec idx
    for(int i=0;i<Net_vec.size();i++){
        VCG_ID_to_Net_vec_idx[Net_vec[i].ID] = i;
        unscheduled_vec[i] = Net_vec[i].ID;
    }
    
    initial_track(net_ID_T_B,Top_boundary, Bottom_boundary);
    unordered_set<int> unscheduled = VCG_id_set;
    int top_track_num = Top_boundary.size()-1;
    int bottom_track_num = Bottom_boundary.size()-1;

    initial_VCG(unscheduled,ID_T_B);
    
    int track_idx = 0;  
    while(unscheduled.size()>0){
        pin_connect(track_idx, top_track_num, unscheduled, unscheduled_vec,net_ID_T_B, Top_boundary, Bottom_boundary);
        track_idx++;
    }
    
    //做簡單的向下壓縮
    //由倒數第二層開始壓縮, 壓縮後檢查該曾是否為全0 , 全0則直接刪除此Track
    bool is_change;
    do{
        is_change = false;
        down_compact(is_change,Top_boundary,Bottom_boundary);
    }while(is_change);
    
}

void Left_edge_Alg::output_data(ofstream &ofs,map<int,vector<pair<int,int>>>& Top_boundary, map<int,vector<pair<int,int>>>& Bottom_boundary){
    int channel_density = Track.size()-(Top_boundary.size()-1)-(Bottom_boundary.size()-1);
    ofs<<"Channel density: "<<channel_density<<'\n';
    
    vector<Net_output_inform> Net_output_inform_vec;
    for(int i=0;i<Track.size();i++){
        for(int j=0;j<Track[i].size();j++){

            if(Track[i][j]>0){
                
                int netname = Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name;
                Net_output_inform new_ele(netname,i);
                new_ele.horiztal_interval.first = j;
                int pre = netname;
                while(VCG_ID_to_Net_vec_idx.count(Track[i][j])&& j<Track[i].size()&&Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name==pre){
                    pre = Net_vec[VCG_ID_to_Net_vec_idx[Track[i][j]]].net_name;
                    
                    j++;
                }
                
                new_ele.horiztal_interval.second = j-1;
                
                Net_output_inform_vec.push_back(new_ele);
                j--;
                

            }
        }
    }

    sort(Net_output_inform_vec.begin(),Net_output_inform_vec.end(),net_compare);

    int top_track_num       = Top_boundary.size()-1;
    int top_track_last_idx  = Top_boundary.size() - 2;
    int bottom_track_start_idx  =   Track.size() -( Bottom_boundary.size()-1);
    int Add_track_num = channel_density;
    int pre_net = -1;
    int pre_track_idx = -1;

    for(auto e:Net_output_inform_vec){
        if(pre_net!=e.net_name){
            ofs<<"Net "<<e.net_name<<'\n';
            if(e.track_idx<=top_track_last_idx){
                ofs<<"T"<<(abs((long)e.track_idx- top_track_last_idx))<<' ';
            }
            else if(bottom_track_start_idx<=e.track_idx){
                ofs<<"B"<<(abs((long)(e.track_idx- bottom_track_start_idx)))<<' ';
            }
            else{
                int temp = e.track_idx - top_track_num ;
                ofs<<"C"<<(Add_track_num - temp)<<' ';
            }
            ofs<<e.horiztal_interval.first<<' '<<e.horiztal_interval.second<<'\n';
        }
        else{
            if(e.track_idx<=top_track_last_idx){
                ofs<<"T"<<(abs((long)e.track_idx- top_track_last_idx))<<' ';
            }
            else if(bottom_track_start_idx<=e.track_idx){
                ofs<<"B"<<(abs((long)(e.track_idx- bottom_track_start_idx)))<<' ';
            }
            else{
                int temp = e.track_idx - (Top_boundary.size()-1);
                ofs<<"C"<<(Add_track_num-temp)<<' ';
            }
            ofs<<e.horiztal_interval.first<<' '<<e.horiztal_interval.second<<'\n';
            if(e.track_idx!=pre_track_idx){
                ofs<<"Dogleg "<<e.horiztal_interval.first<<'\n';
            }
        }

        pre_net = e.net_name;
        pre_track_idx = e.track_idx;
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
    vector<vector<int>> net_ID_T_B;
    map<int,vector<pair<int,int>>> Top_boundary;
    map<int,vector<pair<int,int>>> Bottom_boundary;
    while(getline( ifs, line)){  
        stringstream linestream;
        linestream.str(line);
        string str;
        if(line[0]=='T'){
            int cnt = 0;
            int T_idx;
            while(linestream>>str){
                if(cnt==0){
                    string sub = str.substr(1);
                    T_idx = stoi(sub);
                    
                    Top_boundary[T_idx].push_back(make_pair(0,0));
                }
                else if(cnt==1)
                    Top_boundary[T_idx][Top_boundary[T_idx].size()-1].first = stoi(str);
                else if(cnt==2)
                    Top_boundary[T_idx][Top_boundary[T_idx].size()-1].second = stoi(str);
                
                cnt++;
            }
        }
        else if(line[0]=='B'){
            int cnt = 0;
            int B_idx;
            while(linestream>>str){
                if(cnt==0){
                    string sub = str.substr(1);
                    B_idx = stoi(sub);
                    Bottom_boundary[B_idx].push_back(make_pair(0,0));
                }
                else if(cnt==1)
                    Bottom_boundary[B_idx][Bottom_boundary[B_idx].size()-1].first = stoi(str);
                else if(cnt==2)
                    Bottom_boundary[B_idx][Bottom_boundary[B_idx].size()-1].second = stoi(str);
                
                cnt++;
            }

        }
        else{
            vector<int> net_ID;
            while(linestream>>str){
                net_ID.push_back(stoi(str));
            }
            net_ID_T_B.push_back(net_ID);
        }
    }
    //time_t start = clock();
    
    Left_edge_Alg LEA_0;
    LEA_0.LEA(net_ID_T_B, Top_boundary, Bottom_boundary);

    //time_t end = clock();
    //double runtime = ((double)(end-start))/CLOCKS_PER_SEC;
    //cout<<"time: "<<runtime<<endl;

    ofstream ofs;
    ofs.open(argv[2]);
    if(!ofs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }
    LEA_0.output_data(ofs,Top_boundary, Bottom_boundary);

    return 0;
}