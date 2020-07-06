//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_CONTROLLER_INTERFACE_H
#define AQ_002_CONSOLE_CONTROLLER_INTERFACE_H

class controller_interface
{
public:
    ~controller_interface(){};

public:
    virtual float controller_cal(float error)=0;
    virtual void controller_reset(void)=0;

};

#endif //AQ_002_CONSOLE_CONTROLLER_INTERFACE_H
