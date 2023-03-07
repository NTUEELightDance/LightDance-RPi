#include <iostream>
#include <cstdint>
#include <vector>
using namespace std;

#include "OFController.h"

int main()
{
    OFController OF;
    OF.init();

    int OFnum = 7;

    vector<int> status;
    status.resize(5 * OFnum);

    for (int i = 0; i < 5*OFnum; i++)
    {
        status[i] = 0x00aa00aa;
    }

    OF.sendAll(status);
    
    return 0;
}  


