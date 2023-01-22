#include <stdlib.h>

class Reboot{
    public :
        Reboot(){
            system("sudo reboot -h 0");
        };
};