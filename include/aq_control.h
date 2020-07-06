//
// Created by zyq on 2020/6/17.
//

#ifndef AQ_002_CONSOLE_AQ_CONTROL_H
#define AQ_002_CONSOLE_AQ_CONTROL_H
#include "planner_interface.h"
#include "m_thread.h"
#include "controller_interface.h"
#include "control2ilogic_interface.h"
#include "iLogic2control_interface.h"
#include "zv_planner.h"

class aq_control : public m_thread, public ctr2ilogic_interface
{
    const int timeout=20000;
    float Td;
    float L_inital;
    int zv_mode=0;

    planner_interface* m_xplanner;
    planner_interface* m_lplanner;
    zv_planner* m_zvplanner;

    controller_interface* m_xcontroller;
    iLogic2ctr_interface* m_ilogic;

public:
    aq_control();
    ~aq_control();

    void set_ilogic_interface(iLogic2ctr_interface* ilogic){m_ilogic=ilogic;};
    void set_xplanner(planner_interface* planner){m_xplanner=planner;};
    void set_lplanner(planner_interface* planner){m_lplanner=planner;};
    void set_zvplanner(zv_planner* planner){m_zvplanner=planner;};
    void set_xcontroller(controller_interface* controller){m_xcontroller=controller;};

    void PlannerParaSet(float xd, float xc, float ld,float lc, float td);
    void ZVModeSet(unsigned char mode);
    void StartMoving(void);
    void StopMoving(void);

    void run();


};


#endif //AQ_002_CONSOLE_AQ_CONTROL_H
