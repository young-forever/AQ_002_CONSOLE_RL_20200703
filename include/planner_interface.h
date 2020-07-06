//
// Created by zyq on 2020/6/18.
//

#ifndef AQ_002_CONSOLE_PLANNER_INTERFACE_H
#define AQ_002_CONSOLE_PLANNER_INTERFACE_H

class planner_interface
{
public:
    planner_interface(){};
    ~planner_interface(){};

public:
    virtual void PlannerParaSet(float xd, float xc,float td){};
    virtual float UpdatePlanner(float tc)=0;

};


#endif //AQ_002_CONSOLE_PLANNER_INTERFACE_H
