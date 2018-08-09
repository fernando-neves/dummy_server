# Dummy Server
Server echo for test latency.







# How to compile
  - linux
    - install boost lib, g++
    - command line (g++ -std=c++11 -pthread -I source/ source/main.cpp -o dummy_server -lboost_system -lboost_filesystem -lboost_thread)
  - windows
    - use Visual Studio 2013 (v120)
    - link boost includes and lib $(BOOST_ROOT)

# Libs
  - Boost 1.58.0
