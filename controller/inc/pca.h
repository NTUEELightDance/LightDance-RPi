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

#include "./pca9955.h"
#include "./pca9956.h"
#include "./pcaDefinition.h"

class LinkedList;
class PCAnode;

// handle PCAs, each type of whom may be pca9956 or pca9955B, and they're address will be different
// each PCAnode refer to a pca of a dancer
class LinkedList {
   public:
    LinkedList() : first(nullptr){};

    void Add(int PCA_ADDR, bool IsPCA9956);

    PCAnode *first;
};
// each PCAnode refer to a pca of a dancer
// call different type of class according to the type of each PCAnode
class PCAnode {
   public:
    PCAnode();
    PCAnode(int PCA_ADDR, bool IsPCA9956);

    PCA9956 *pca9956;
    PCA9955 *pca9955;
    PCAnode *nxt;

    friend class LinkedList;
};
// Initialize function : PCA() , which is the default constructor
// There're two different function to write data to a PCA, WriteAll(vector<vector<unsigned char>>) and WriteChannel(vector<vector<unsigned char>>)
// WriteAll can be used to update all OFs' datas
// WriteChannel is used to updata only one OF datas, which can be used to Debug
class PCA {
   public:
    PCA();

    int WriteAll(std::vector<std::vector<char> > &data);
    int WriteChannel(std::vector<char> &data, int channel);
    void Read();

   private:
    LinkedList PCAs;
};

#endif /* PCA */
