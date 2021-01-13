/*
work on filesystem manipulation for proper file output
compile: g++ -std=c++17 path.cpp -l stdc++fs
*/

#include <iostream>
#include <string>
// using namespace std;

int main(int argc, char **argv){
    // std::cout << argc << '\n';
    std::string s = argc>1 ? argv[1] : "/home/pi/smartbike/output/";
    std::printf("dir: %s\n",s.c_str());
    // std::printf("run dir: %s\n",argv[0]);
    // std::cout << fs::current_path() << '\n';
    
    return 0;
}


// eof
