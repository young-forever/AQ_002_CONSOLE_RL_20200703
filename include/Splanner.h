//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_SPLANNER_H
#define AQ_002_CONSOLE_SPLANNER_H
#include "planner_interface.h"

class Splanner : public planner_interface
{
    const float a_max=1.5;
    const float v_max=1.0;

    float Xd;
    float Xc;
    float Td;

    float t_acc;
    float t_con;
    float t_dacc;

    float a_mv;
    float v_mv;

public:
    Splanner();
    ~Splanner();

    void TrjPlan();


    //interface reslize:
    void PlannerParaSet(float xd, float xc, float td);
    float UpdatePlanner(float tc);
};


#endif //AQ_002_CONSOLE_SPLANNER_H
