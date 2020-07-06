//
// Created by zyq on 2020/6/18.
//

#include "pid_controller.h"

pid_controller::pid_controller()
{


}


pid_controller::~pid_controller()
{

}

float pid_controller::controller_cal(float error)
{
    d_q_last = d_q;
    d_q = error;
    dd_q = d_q - d_q_last;
    i_item = i_item + d_q;

    return Kp*d_q+Ki*i_item+Kd*dd_q;
}

void pid_controller::controller_reset()
{
    d_q_last=0;
    d_q=0;
    dd_q=0;
    i_item=0;
}


