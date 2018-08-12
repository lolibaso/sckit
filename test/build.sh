#/bin/bash

g++ -o aes128_test aes128_test.cpp -lsckit -I../inc -L../lib
g++ -o aes256_test aes256_test.cpp -lsckit -I../inc -L../lib
g++ -o wheel_timer_test wheel_timer_test.cpp -std=c++11 -lsckit -lsc_timer -I../inc -L../lib 
g++ -o htshm htshm_test.cpp  -std=c++11 -lsckit -I../inc -L../lib
g++ -o bloomfilter_test bloomfilter_test.cpp  -std=c++11 -lsckit -I../inc -L../lib
g++ -o log_test log_test.cpp  -std=c++11 -lsckit -I../inc -L../lib
g++ -o macro_test macro_test.cpp  -std=c++11 -lsckit -I../inc -L../lib

