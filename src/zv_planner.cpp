//
// Created by zyq on 2020/6/23.
//

#include "zv_planner.h"

zv_planner::zv_planner()
{

}

zv_planner::~zv_planner()
{


}

void zv_planner::ZV_ModeSet(unsigned char mode)
{
    ZVMode = mode;
}

float zv_planner::UpdatePlanner(float tc)
{
    float target;
    switch(ZVMode){
        case NO_ZV:
            target = m_planner->UpdatePlanner(tc)*1000.0;
            break;
        case ZV_MODE:
            target = (0.5*m_planner->UpdatePlanner(tc)+0.5*m_planner->UpdatePlanner(tc-ZV_DeltaT))*1000.0;
            break;
        case ZVD_MODE:
            target = (0.25*m_planner->UpdatePlanner(tc)+0.5*m_planner->UpdatePlanner(tc-1*ZV_DeltaT)+\
                        0.25*m_planner->UpdatePlanner(tc-2*ZV_DeltaT))*1000.0;
            break;
        case ZVDD_MODE:
            target = (0.125*m_planner->UpdatePlanner(tc)+0.375*m_planner->UpdatePlanner(tc-ZV_DeltaT)+\
                        0.375*m_planner->UpdatePlanner(tc-2*ZV_DeltaT)+0.125*m_planner->UpdatePlanner(tc-3*ZV_DeltaT))*1000.0;
            break;
    }
    return target;
}