#include <iostream>
#include <fftw3.h>
#include <fstream>
#include <string>
int main(){
    std::fstream fs_traject;
    fs_traject.open("dump.xyz.after.PBC",std::fstream::in);
    int cell=20;
    int variable=5*cell*cell*cell*3;
    int tick=0;
    std::string line;
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
    for(size_t i=0;i<variable;i++){
        for(size_t j=0;j<timeframe;j++){
            in[j]=traject[j][i];
        }
        p=fftw_plan_dft_r2c_1d(timeframe,in,out,FFTW_ESTIMATE);
        fftw_execute(p);
        for(size_t j=0;j<timeframe/2+1;j++){
             std::cout<<out[j][0]<<" "<<out[j][1]<<std::endl;
         }
        fftw_destroy_plan(p);
    }
}
