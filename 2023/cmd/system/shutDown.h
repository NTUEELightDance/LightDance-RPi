#include <stdlib.h>

class ShutDown{
    public :
        ShutDown(){
            system("sudo shutdown -h 0");
        };
};