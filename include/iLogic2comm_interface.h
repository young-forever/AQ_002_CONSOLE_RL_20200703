//
// Created by zyq on 2020/6/10.
//

#ifndef AQ_002_CONSOLE_ILOGIC2COMM_INTERFACE_H
#define AQ_002_CONSOLE_ILOGIC2COMM_INTERFACE_H

#include <vector>

class ilogic2comm_interface
{
public:
    virtual void ResetOk()=0;
    virtual void RecieveBack()=0;
    virtual void RecShakeHandOk()=0;

    virtual void RecWorkStateBack(unsigned char workstate)=0;
    virtual void RecMotmodBack(unsigned char motmod)=0;
    virtual void RecSensorData(std::vector<float> sdata)=0;
    virtual void RecCableL(float L)=0;
    virtual void RecExppos(float pos)=0;
    virtual void RecExpvel(float vel)=0;
    virtual void RecExpcur(float cur)=0;
    virtual void RecErrorBack(unsigned char error)=0;
};
#endif //AQ_002_CONSOLE_ILOGIC2COMM_INTERFACE_H
