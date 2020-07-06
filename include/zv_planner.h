//
// Created by zyq on 2020/6/23.
//

#ifndef AQ_002_CONSOLE_ZV_PLANNER_H
#define AQ_002_CONSOLE_ZV_PLANNER_H
#include "planner_interface.h"


// ZV MODE
#define NO_ZV           0x00
#define ZV_MODE         0x01
#define ZVD_MODE        0x02
#define ZVDD_MODE       0x03

class zv_planner : public planner_interface
{
    planner_interface* m_planner;
    unsigned char ZVMode;
    const unsigned char ZV_DeltaT = 1.0;

public:
    zv_planner();
    ~zv_planner();

    void ZV_ModeSet(unsigned char mode);

    void setPlanner(planner_interface* planner){m_planner = planner;};
    float UpdatePlanner(float tc);

};


#endif //AQ_002_CONSOLE_ZV_PLANNER_H
