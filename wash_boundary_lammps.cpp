#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <fftw3.h>
int main(int argc,char* argv[]){
    double px[2]={0.0,0.0};
    double py[2]={0.0,0.0};
    double pz[2]={0.0,0.0};
    double period[3]={0.0,0.0,0.0};
    std::string filein=argv[2];
    std::string fileout=filein+".after.PBC";
    std::fstream fin,fout;
    fin.open(filein.c_str(),std::fstream::in);
    fout.open(fileout.c_str(),std::fstream::out);
    std::string line;
    std::istringstream fs_temp;
    int cell=std::stoi(argv[1]);
    int tick=0;
    double posit_before[cell*cell*cell*5][3];
    double posit_now[cell*cell*cell*5][3];
    while(getline(fin,line)){
        if(line.find("ITEM: BOX BOUNDS pp pp pp")!=std::string::npos){
            getline(fin,line);
            fs_temp.clear();
            fs_temp.str(line);
            fs_temp>>px[0];
            fs_temp>>px[1];
            getline(fin,line);
            fs_temp.clear();
            fs_temp.str(line);
            fs_temp>>py[0];
            fs_temp>>py[1];          
            getline(fin,line);
            fs_temp.clear();
            fs_temp.str(line);
            fs_temp>>pz[0];
            fs_temp>>pz[1];
            period[0]=px[1]-px[0];
            period[1]=py[1]-py[0];
            period[2]=pz[1]-pz[0];
        }
        if(line.find("ITEM: ATOMS x y z")!=std::string::npos){
            if(tick==0){
            for(int i=0;i<5*cell*cell*cell;i++){
              getline(fin,line);
              fs_temp.clear();
              fs_temp.str(line);
              for(size_t j=0;j<3;j++){
                fs_temp>>posit_before[i][j];
                fout<<posit_before[i][j]<<" ";
              }
              fout<<std::endl;
            }
            tick++;
            }
            else{
                std::cout<<"I am here"<<std::endl;
                for(int i=0;i<5*cell*cell*cell;i++){
                    getline(fin,line);
                    fs_temp.clear();
                    fs_temp.str(line);
                    for(size_t j=0;j<3;j++){
                        fs_temp>>posit_now[i][j];
                        if(posit_now[i][j]-posit_before[i][j]>period[j]/2.0){
                            posit_now[i][j]=posit_now[i][j]-period[j];
                        }
                        else if(posit_now[i][j]-posit_before[i][j]<-period[j]/2.0){
                            posit_now[i][j]=posit_now[i][j]+period[j];
                        }
                        else{
                        };
                        fout<<posit_now[i][j]<<" ";
                    }
                    fout<<std::endl;
                }
                tick++;
            }
        }
    }
    fin.close();
    fout.close();
    /*start to FFT the trajectory*/
    std::fstream fs_traject;
    fs_traject.open("dump.xyz.after.PBC",std::fstream::in);
    int variable=5*cell*cell*cell*3;
    tick=0;
    while(getline(fs_traject,line)){
        tick=tick+1;
    }
    fs_traject.close();
    int timeframe=tick/5/cell/cell/cell;
    double* traject[timeframe];
    for(size_t i=0;i<timeframe;i++){
        traject[i]=new double[variable];
    }
    fs_traject.open("dump.xyz.after.PBC",std::fstream::in);
    for(size_t i=0;i<timeframe;i++){
        for(size_t j=0;j<variable;j++){
            fs_traject>>traject[i][j];
        }
    }
    fs_traject.close();
    int fftw_out_count=timeframe;
    double in[timeframe];
    fftw_complex* out;
    fftw_plan p;
    out=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(timeframe/2+1));
    std::fstream fs_fft_out;
    fs_fft_out.open("mode_filter_result.txt",std::fstream::out);
    for(size_t i=0;i<variable;i++){
        for(size_t j=0;j<timeframe;j++){
            in[j]=traject[j][i];
        }
        p=fftw_plan_dft_r2c_1d(timeframe,in,out,FFTW_ESTIMATE);
        fftw_execute(p);
        for(size_t j=0;j<timeframe/2+1;j++){
             fs_fft_out<<out[j][0]<<" "<<out[j][1]<<std::endl;
         }
        fftw_destroy_plan(p);
    }
    fs_fft_out.close();
    /*****************************************/
}
