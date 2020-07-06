//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_CONTROL2ILOGIC_INTERFACE_H
#define AQ_002_CONSOLE_CONTROL2ILOGIC_INTERFACE_H

class ctr2ilogic_interface
{
public:
    ~ctr2ilogic_interface(){};

    virtual void PlannerParaSet(float xd, float xc, float ld,float lc, float td)=0;
    virtual void ZVModeSet(unsigned char mode)=0;
    virtual void StartMoving(void)=0;
    virtual void StopMoving(void)=0;
};

#endif //AQ_002_CONSOLE_CONTROL2ILOGIC_INTERFACE_H
