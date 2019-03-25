export LD_LIBRARY_PATH=/export/apps/gcc/4.8.2/lib:/export/apps/gcc/4.8.2/lib64:/export/apps/ocelot/lib/:/export/apps/boost/lib/:$LD_LIBRARY_PATH
/export/apps/cuda/5.5/bin/nvcc -cuda hello.cu -I /export/apps/ocelot/include/ocelot/api/interface/ -arch=sm_20
/export/apps/gcc/4.8.2/bin/g++ -o hello hello.cu.cpp.ii -I /export/apps/ocelot/include/ocelot/api/interface/ -L /export/apps/ocelot/lib/ -locelot 
