//
//  main.cpp
//  PrimalDualFacilityLocation
//
//  Created by qrafzv on 2017-05-26.
//  Copyright © 2017 qrafzv. All rights reserved.
//

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <random>
#include <map>
using namespace std;
struct Instance
{
    std::vector<std::vector<double> > points;
    vector<int> connectivity;
    vector<bool> openity;
    Eigen::VectorXd budget;
    struct facility
    {
        
        vector<double> poistion;
        
        facility(vector<double> poistion): poistion{poistion}{};
        size_t cost;
        double distance(vector<double> point){
            return sqrt(pow(poistion[0]-point[0],2)+pow(poistion[1]-point[1],2));
        }
    };
    vector<facility> sets;
    Eigen::MatrixXd dmx;
    //vector<int> cost;
    Instance(size_t points_count, size_t facility_count){
        std::uniform_real_distribution<double> unif(0,1000);
        std::uniform_int_distribution<int> uniif(0,100);
        std::default_random_engine re;
        for(int i =0;i<points_count;i++){
             connectivity.push_back(-1);
            std::vector<double> tmp_point = {unif(re),unif(re)};
            points.push_back(tmp_point);
        }
        for(int i =0;i<facility_count;i++){
            openity.push_back(false);
            facility fa({unif(re),unif(re)});
            fa.cost = 10000;
            sets.push_back(fa);
        }
        
    }
    Eigen::MatrixXd distanceMatrix(){
        Eigen::MatrixXd dmx(points.size(),sets.size());
        for(int i=0;i<points.size();i++){
            for(int j=0;j<sets.size();j++){
                dmx(i,j)=round((sets[j].distance(points[i])));
            }
        }
        this->dmx = dmx;
        return dmx;
    };
    void show_instance(){
        int i=0;
        for(auto p: points ){
            cout<<"The point"<<(++i)<<" "<<p.front()<<","<<p.back()<<endl;
        }
        i=0;
        for(auto f:sets){
            cout<<"The facility"<<(++i)<<" "<<f.cost<<" position"<<f.poistion.front()<<" "<<f.poistion.back()<<endl;
        }
    }
    void show_distance(){
        for(int i=0;i<points.size();i++){
            for(int j=0;j<sets.size();j++){
                cout<<" "<<this->dmx(i,j);
            }
            cout<<endl;
        }
    }
    void solve(){
        Eigen::VectorXd budget{dmx.rows()};
        
        while(isStillUnconnectedCity()){
            static int iteration = 0;
            cout<<"Iteration-"<<++iteration<<endl;
            Eigen::MatrixXd offer{dmx.rows(),dmx.cols()};
            for(int i=0;i<dmx.rows();i++){
                if(connectivity[i]!=-1) continue;
                budget[i]+=0.9;
                
            }
            this->budget=budget;
            //Update offer
            for(int m=0;m<dmx.rows();m++){
                for(int n = 0;n<dmx.cols();n++){
                    if(connectivity[m]==-1){
                        offer(m,n)=std::max(budget(m)-dmx(m,n),0.0);
                    }else{
                        offer(m,n)=std::max(dmx(m,connectivity[m])-dmx(m,n),0.0);
                    }
                }
            }
            for(int n = 0;n<dmx.cols();n++){
                if(!openity[n]&&sets[n].cost<=offer.col(n).sum()){
                    openity[n]=true;
                    for(int t=0;t<offer.col(n).size();t++){
                        if(offer.col(n)[t]>0){
                            connectivity[t]=n;
                            
                        }
                    }
                }
            }
            
            for(int m = 0;m<dmx.rows();m++){
                for(int n=0;n<dmx.cols();n++){
                    if(connectivity[m]==-1&&openity[n]&&budget(m)>=dmx(m,n)){
                        connectivity[m]=n;
                    }
            }
        }
        
            
            
            
        }
        //printFlPResult();
    }
    void printFlPResult(){
        int totalBudget=0;
        double totalPrimalCost =0;
        for(int i=0;i<connectivity.size();i++){
            cout<<"city-"<<i<< "to facility-"<< connectivity[i]<<"   ";
            totalBudget+=budget[i];
            totalPrimalCost+=dmx(i,connectivity[i]);
        }
        cout<<"total dual cost is: "<< totalBudget<<endl;
        int opensize = 0;
        for(int j =0;j<openity.size();j++){
            if(openity[j]) {totalPrimalCost += sets[j].cost;
                opensize++;
            }
        }
        cout<<"total primal cost is: " << totalPrimalCost<< "with open facility:"<<opensize<<endl;
    }
   
    bool isStillUnconnectedCity(){
        for(auto p:connectivity){
            if(-1!=(int)p) continue;
            return true;
        }
        return false;
    };
};


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    Instance inst(400,20);
    inst.distanceMatrix();
    //inst.show_instance();
    inst.solve();
    inst.printFlPResult();
    
    return 0;
}
