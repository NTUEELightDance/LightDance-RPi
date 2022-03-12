/****************************************************************************
  FileName     [ pcaDefinition.h ]
  PackageName  [ clientApp ]
  Synopsis     [ definitions ]
  Author       [ Ken Chung ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/

//----------------------------------------------------------------------
//    Global
//----------------------------------------------------------------------
#ifndef _DEF_H_
#define _DEF_H_

// PCA(OF) I2C address
#define PCA_ADDR_1 0x3f
#define PCA_ADDR_2 0x2b
#define PCA_ADDR_3 0x40
#define PCA_ADDR_4 0x60

// PCA(OF) types
#define PCA_TYPE_1 9956
#define PCA_TYPE_2 9956
#define PCA_TYPE_3 9955
#define PCA_TYPE_4 9955

// PCA type
#define _PCA9956 9956
#define _PCA9955B 9955

// numbers of pca data needed
#define NUM_PCA 4  // number of PCA used
#define NUM_OF 26
#define NUM_OF_PARAMS 6                                                                    // number of total OFs
#define NUM_AN_OF_NEED_IREF 3                                                              // number of IREF data need sent to an of
#define NUM_AN_OF_NEED_PWM 3                                                               // number of PWM data need sent to an of
#define NUM_AN_OF_NEED_DATA (NUM_AN_OF_NEED_IREF + NUM_AN_OF_NEED_PWM)                     // number of data need sent to an of
#define NUM_CHANNEL_FROM_PCA9955B 5                                                        // number of OFs provided from a pca9955B
#define NUM_CHANNEL_FROM_PCA9956 8                                                         // number of OFs provided from a pca9956
#define NUM_IREF_DATA_TO_PCA9955B (NUM_CHANNEL_FROM_PCA9955B * NUM_AN_OF_NEED_IREF)        // number of iref data need sent to a pca9955B
#define NUM_PWM_DATA_TO_PCA9955B (NUM_CHANNEL_FROM_PCA9955B * NUM_AN_OF_NEED_PWM)          // number of pwm need sent to a pca9955B
#define NUM_TOTAL_DATA_TO_PCA9955B (NUM_IREF_DATA_TO_PCA9955B + NUM_PWM_DATA_TO_PCA9955B)  // number of total datas(iref & pwm) need sent to a pca9955B
#define NUM_IREF_DATA_TO_PCA9956 (NUM_CHANNEL_FROM_PCA9956 * NUM_AN_OF_NEED_IREF)          // number of iref need sent to a pca9956
#define NUM_PWM_DATA_TO_PCA9956 (NUM_CHANNEL_FROM_PCA9956 * NUM_AN_OF_NEED_PWM)            // number of pwm need sent to a pca9956
#define NUM_TOTAL_DATA_TO_PCA9956 (NUM_IREF_DATA_TO_PCA9956 + NUM_PWM_DATA_TO_PCA9956)     // number of total datas(iref & pwm) need sent to a pca9956

// pca type and addr for pca initialize in pca.cpp
// an (NUM_PCA * 2) 2D array of int type
// each row of the array refer to one pca
// On each row, the first element means the type of pca,
// where value equals to 9955 refering to PCA9955B and that of 9956 refering to PCA9956
// the second element means the I2C address of pca, which will be value between 0 ~ 127
const int pcaTypeAddr[NUM_PCA][2] = {
    {PCA_TYPE_1, PCA_ADDR_1},
    {PCA_TYPE_2, PCA_ADDR_2},
    {PCA_TYPE_3, PCA_ADDR_3},
    {PCA_TYPE_4, PCA_ADDR_4}};

#endif /* _DEF_H_ */
