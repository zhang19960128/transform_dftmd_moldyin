data=importdata("allfftw.result");
kspace=length(data)/20/20/20/5/3;
step=20*20*20*5*3;
simulationtime=40000;
freq=1/(simulationtime)*10^(15)/(10^(12));%units THz.
powerf=zeros(kspace,1);
for i=1:kspace
    for j=1:step
        powerf(i)=powerf(i)+data((j-1)*kspace+i,1)*data((j-1)*kspace+i,1)+data((j-1)*kspace+i,2)*data((j-1)*kspace+i,2);
    end
    powerf(i)=powerf(i)/step;
end
plot((2:kspace)*freq,powerf(2:kspace),'-v')
xlim([0,2.0])
xlabel("Frequency(THz)");
ylabel("mode amplitude");
delta=0.025;
freq1=0.54;
