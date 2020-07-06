//
// Created by zyq on 2020/6/9.
//

#ifndef AQ_002_CONSOLE_CONSOLE_INTERFACE_H
#define AQ_002_CONSOLE_CONSOLE_INTERFACE_H

class console_interface
{
public:
    virtual void RecShakeHandOk()=0;
    virtual void ResetOk()=0;
    virtual void RecieveBack()=0;
    virtual void RecWorkStateBack(unsigned char workstate)=0;
    virtual void RecMotmodBack(unsigned char motmod)=0;
    virtual void RecSensorData(std::vector<float> sdata)=0;
    virtual void RecCableL(float L)=0;
    virtual void RecExppos(float pos)=0;
    virtual void RecExpvel(float vel)=0;
    virtual void RecExpcur(float cur)=0;

    virtual void RecErrorBack(unsigned char error)=0;

    virtual void ControlFinished(void)=0;
};


#endif //AQ_002_CONSOLE_CONSOLE_INTERFACE_H
