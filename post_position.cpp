#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
int main(int argc,char* argv[]){
    std::string moldyoutfile=argv[1];
    int atomid=std::atoi(argv[2]);
    std::fstream traject;
    traject.open(moldyoutfile,std::fstream::in);
    std::string temp;
    std::istringstream fs_temp;
    int count=0;
    std::vector<double> start(3,0.0);
    std::vector<double> instant(3,0.0);
    while(getline(traject,temp)){
        if(temp.find("Reduced ionic position")!=std::string::npos){
           for(int i=0;i<atomid;i++){
                getline(traject,temp);
           }
            fs_temp.str(temp);
            if(count==0){
                for(int i=0;i<3;i++){
                    fs_temp>>start[i];
                    instant[i]=start[i];
                }
            }
            else{
                for(int i=0;i<3;i++){
                    fs_temp>>instant[i];
                }
            }
            fs_temp.clear();
            for(int i=0;i<3;i++){
            std::cout<<instant[i]-start[i]-round(instant[i]-start[i])<<"\t";
            }
            std::cout<<std::endl;
            count++;
        }
    }
}
