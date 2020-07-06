//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_PID_CONTROLLER_H
#define AQ_002_CONSOLE_PID_CONTROLLER_H
#include "controller_interface.h"

class pid_controller : public controller_interface
{
    float d_q_last;
    float d_q;
    float dd_q;
    float i_item;

    const float Kp=3;
    const float Ki=0;
    const float Kd=20;

public:
    pid_controller();
    ~pid_controller();

    float controller_cal(float xd);
    void controller_reset(void);



};


#endif //AQ_002_CONSOLE_PID_CONTROLLER_H
