/*
work on filesystem manipulation for proper file output
*/

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv){
    std::printf("run dir: %s\n",argv[0]);
    std::cout << fs::current_path() << '\n';
    
    
    
    
    return 0;
}


// eof
