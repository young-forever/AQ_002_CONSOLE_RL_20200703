//
// Created by zyq on 2020/6/17.
//
#include <iostream>
#include "aq_control.h"
#include "Splanner.h"
#include "pid_controller.h"
#include "unistd.h"
#include "main.h"

aq_control::aq_control()
{
    //x & l plannner set:
    Splanner* xplanner = new Splanner();
    Splanner* lplanner = new Splanner();
    set_xplanner(xplanner);
    set_lplanner(lplanner);

    // zv planner set:
    zv_planner* zvplanner_ = new zv_planner();
    zvplanner_->setPlanner(m_xplanner);
    set_zvplanner(zvplanner_);
    zvplanner_->ZV_ModeSet(NO_ZV);

    //controller set:
    pid_controller* controller = new pid_controller();
    set_xcontroller(controller);
}

aq_control::~aq_control()
{
    delete m_xplanner;
    delete m_lplanner;
    delete m_zvplanner;
    delete m_xcontroller;
}

void aq_control::PlannerParaSet(float xd, float xc, float ld,float lc, float td)
{
    Td = td;
    L_inital = lc;
    m_xplanner->PlannerParaSet(xd,xc,td);
    m_lplanner->PlannerParaSet(ld,lc,td);
}

void aq_control::ZVModeSet(unsigned char mode)
{
    m_zvplanner->ZV_ModeSet(mode);
    zv_mode = mode;
}

void aq_control::StartMoving(void)
{
    start();
}

void aq_control::StopMoving(void)
{
    stop();
}

void aq_control::run()
{
    m_xcontroller->controller_reset();

    int comm_delay=1500;
    int cnt=0;

    float l_last = L_inital*1000.0;

    do {
        usleep(timeout-comm_delay);
        /***** update time. ******/
        float tc = cnt*timeout/1000000.0;

        /***** update planner.******/
        float target_x = m_zvplanner->UpdatePlanner(tc);
//        float target_x = m_xplanner->UpdatePlanner(tc)*1000.0;
        float target_l = m_lplanner->UpdatePlanner(tc)*1000.0;

        /********* update controller.********/
        float xc = m_ilogic->GetXc()*1000.0;
        //std::cout<<"xc:"<<xc<<std::endl;
        float target_vx = m_xcontroller->controller_cal(target_x-xc);//input control target and output control value.
        float target_vl = (target_l-l_last)/((float)timeout/1000000.0);
        l_last = target_l;

        m_ilogic->SetTargetValue(target_x,target_l);
        /******* send command. **********/
        m_ilogic->SendControlValue(1,target_vx*60/125);
        usleep(comm_delay);
        m_ilogic->SendControlValue(2,target_vl*60.0/78.5);

#if CONTROL_DEBUG_ENABLE
        std::cout<<"t: "<<tc<<" "<<"target_x: "<<target_x<<"    "<<"target_vx: "<<target_vx<<"  "<<"target_l: "<<target_l<<"  "<<"target_vl: "<<target_vl<<"  "<<std::endl;
#endif
        if(tc-(float)zv_mode-2.0>Td)
        {
            stop();
            m_ilogic->ControlFinished();
        }

        m_mutex->lock();

        if(ThreadStopFlag)
        {
            m_mutex->unlock();
            return;
        }
        m_mutex->unlock();
        cnt++;
    }while(1);

}