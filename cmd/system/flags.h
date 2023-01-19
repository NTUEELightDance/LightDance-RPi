#include <stdio.h>
#include <algorithm>
#include <iostream>
//funcs
char* getCmdOption(char ** itr,char** end){
    if (itr != end && ++itr != end){
        std::cout<<*itr;
        return *itr;
    }
    return 0;
};
void cmdOptionExists(){
    printf("hi im i");
};
enum flag{ //captial of the flag
   H,
   F,
};

class Flags{
    public:
        Flags(char ** begin, char ** end){
             
            char* a;
            int option=-1;
            for (int i=0;i<sizeof(ffs);i++){
                itr = std::find(begin, end, ffs[i]);
                if(itr!=end){
                    option=i;
                    std::cout<<option;
                    break;
                }
            }
            switch (option){
                case H:
                    printf("hi im -h\n");
                    cmdOptionExists();
                    break;
                
                case F:
                    printf("hi im -f\n");
                    getCmdOption(itr,end);
                    break;

                default:
                    break;
            }
        };
    private:
        char** itr;
        std::string ffs[2]={"-h","-f"};//the flag
};

