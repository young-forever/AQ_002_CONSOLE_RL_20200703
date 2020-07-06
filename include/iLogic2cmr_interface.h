//
// Created by zyq on 2020/6/15.
//

#ifndef AQ_002_CONSOLE_ILOGIC2CMR_H
#define AQ_002_CONSOLE_ILOGIC2CMR_H

#include <iostream>
using namespace std;

class iLogic2cmr_interface
{
public:
    virtual void sample_add_image(unsigned char* p,unsigned int len,string format)=0;

};

#endif //AQ_002_CONSOLE_ILOGIC2CMR_H
