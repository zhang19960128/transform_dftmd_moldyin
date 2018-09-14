#/bin/bash
tempfile="temp_file.txt";
tac control.opt | grep -B20 -m1 "Potential parameters" | tac | grep -v "Potential parameters" >$tempfile;
line=`tac control.opt | grep -B20 -m1 "Potential parameters" | tac | grep -v "Potential parameters" | wc -l`;
pair=`bc <<< "($line-6)/2"`;
echo $pair
fileout="test.lammpsinput"
if [ -f $fileout ]
then
	rm $fileout
	touch $fileout
else
	touch $fileout
fi
echo "# temperarory input for lammps" >>$fileout
echo " ">>$fileout
echo "units     metal" >>$fileout
echo "atom_style full" >>$fileout
echo "boundary p p p"  >>$fileout
echo "kspace_style pppm 1.0e-4" >>$fileout
echo "pair_style hybrid/overlay  12lj/cut/coul/long 8.0 8.0 bv 2.0 8.0 bvv 2.0 8.0" >>$fileout
echo "angle_style harmonic" >>$fileout
echo "read_data mixdata.BTO" >>$fileout
echo "#read_restart BTO.restart" >>$fileout
for i in `seq 1 3`
do
	echo "" >>$fileout
done
#####################here we print the output for lammps lj force################
echo "#              alpha rcut     epsilon Bij" >>$fileout
for i in `seq 1 $pair`
do
	linenum=`expr $i + 1`;
	para=`sed -n ${linenum}p $tempfile`;
	para=($para);
	echo "pair_coeff ${para[0]} ${para[1]} 12lj/cut/coul/long 2.0 ${para[9]}">>$fileout
done
#####################finished print the output for lammps lj force ##############
echo "" >>$fileout
echo "" >>$fileout
#####################here we print the output for bond valence term##############
echo "#                   r0   Nij    S     v0 rcut">>$fileout
for i in `seq 1 $pair`
do
	linenum=`expr $i + 1`;
	para=`sed -n ${linenum}p $tempfile`;
	para=($para);
	echo "pair_coeff ${para[0]} ${para[1]} bv ${para[2]} ${para[3]} ${para[4]} ${para[6]} ${para[7]}" >>$fileout;
done
#################### finish generating the output for bond valence term###########
echo "" >>$fileout
echo "" >>$fileout
###################here we print the output for bond valence vector term##########
echo "#                    r0  Nij     S     Bvv0  rcut">>$fileout
for i in `seq 1 $pair`
do
	linenum=`expr $i + 1`;
	para=`sed -n ${linenum}p $tempfile`;
	para=($para);
	echo "pair_coeff ${para[0]} ${para[1]} bvv ${para[2]} ${para[3]} ${para[12]} ${para[13]} ${para[7]}" >>$fileout;
done
################## finishe printing out the bond valence vector term#############
echo "">>$fileout;
echo "">>$fileout;
echo "neighbor        2.0 bin">>$fileout
echo "neigh_modify    every 1">>$fileout
echo "# time unit ps">>$fileout
echo "timestep         0.001">>$fileout
echo "">>$fileout
echo "#group Pb id 1:512">>$fileout
echo "#group Ti id 513:1024">>$fileout
echo "#group O1 id 1025:1536">>$fileout
echo "#group O2 id 1537:2048">>$fileout
echo "#group O3 id 2049:2560">>$fileout
echo "thermo          100">>$fileout
echo "thermo_style custom step temp eangle etotal press vol lx ly lz">>$fileout
echo "thermo_modify line one format float %12.5f">>$fileout
echo "">>$fileout
echo "">>$fileout
echo "fix 1 all nvt temp 5.0 5.0 1.0">>$fileout
echo "run 50000">>$fileout
echo "unfix 1">>$fileout
echo "">>$fileout
echo "fix             4 all npt temp 5.0 5.0 1.0 aniso 1.01325 1.01325 5.0">>$fileout
echo "dump            4 all custom 200 dump.xyz x y z">>$fileout
echo "dump_modify     4 sort id">>$fileout
echo "run 50000">>$fileout
echo "unfix 4">>$fileout
echo "write_restart BTO.restart2">>$fileout
