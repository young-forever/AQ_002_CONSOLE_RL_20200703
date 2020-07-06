//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_ILOGIC2CONTROL_INTERFACE_H
#define AQ_002_CONSOLE_ILOGIC2CONTROL_INTERFACE_H

class iLogic2ctr_interface
{
public:
    ~iLogic2ctr_interface(){};

public:
    virtual float GetXc()=0;

    virtual int SendControlValue(int motnum,float target_vx)=0;

    virtual void ControlFinished(void)=0;

    virtual void SetTargetValue(float vx, float vl)=0;
};


#endif //AQ_002_CONSOLE_ILOGIC2CONTROL_INTERFACE_H
