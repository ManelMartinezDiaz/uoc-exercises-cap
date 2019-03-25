export LD_LIBRARY_PATH=/share/apps/gcc/4.8.2/lib:/share/apps/gcc/4.8.2/lib64:/share/apps/ocelot/lib/:/share/apps/boost/lib/:$LD_LIBRARY_PATH
/export/apps/cuda/5.5/bin/nvcc -cuda pac4_ex3.cu -I /export/apps/ocelot/include/ocelot/api/interface/ -arch=sm_20
/export/apps/gcc/4.8.2/bin/g++ -o pac4_ex3 pac4_ex3.cu.cpp.ii -I /export/apps/ocelot/include/ocelot/api/interface/ -L /export/apps/ocelot/lib/ -locelot
./pac4_ex3 >> pac4_ex3_result.csv
