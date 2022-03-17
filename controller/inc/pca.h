/****************************************************************************
  FileName     [ pca.h ]
  PackageName  [ clientApp ]
  Synopsis     [  ]
  Author       [ Ken Chung ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/

//----------------------------------------------------------------------
//    Global
//----------------------------------------------------------------------
#ifndef PCA_H
#define PCA_H

#include <vector>

#include "pca995X.h"

// Initialize function : PCA() , which is the default constructor
// There're two different function to write data to a PCA, WriteAll(vector<vector<unsigned char>>) and WriteChannel(vector<vector<unsigned char>>)
// WriteAll can be used to update all OFs' datas
// WriteChannel is used to updata only one OF datas, which can be used to Debug
class PCA {
   public:
    // constructor : initialize
    PCA();

    // Write pwm/iref to all slaves' registers
    int WriteAll(std::vector<std::vector<char> > &data);
    // Write one pwm/iref to a specific channel
    int WriteChannel(std::vector<char> &data, int channel);
    // Read pwm/iref registers of all slaves
    void Read();

   private:
    std::vector<PCA995X> PCAs;
};

#endif /* PCA */
