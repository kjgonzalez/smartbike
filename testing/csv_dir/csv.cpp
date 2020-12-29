/* simple csv test */

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

struct CsvSimple {
    /* simple, per-item / per-line csv writer */
    std::ofstream fout;
    CsvSimple(std::string filename){
        fout.open(filename);
    }
    ~CsvSimple(){ close(); }
    void addval(int val){ fout << val<< ','; }
    void addval(float val){ fout << val<< ','; }
    void addval(double val){ fout << val<< ','; }
    void addval(std::string val){ fout << val.c_str() << ','; }
    void endl(){ fout << std::endl; }
    void close(){ endl(); fout.close();}
};

int main(){
    std::printf("hello world\n");
    std::cout << time << std::endl;
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf,sizeof(buf),"%Y-%m-%d.%X", &tstruct);
    printf("current time is %s\n", buf);
    CsvSimple f("test.csv");
    f.fout << "Date=" << buf << ',';
    f.addval("thing");f.endl();
    for(int i=0;i<5;i++){
        f.addval(i);
        f.addval(i*i);
        f.endl();
    }
    f.close();
    printf("done\n");

}



