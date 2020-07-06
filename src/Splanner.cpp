//
// Created by zyq on 2020/6/18.
//

#include "Splanner.h"
#include "math.h"

Splanner::Splanner()
{

}

Splanner::~Splanner()
{

}

void Splanner::TrjPlan()
{
    // planner:
    float ld = fabs(Xd-Xc);
    float l_max = v_max*v_max/a_max;

    float delta = (a_max*Td)*(a_max*Td)-4*a_max*ld;

    if(ld<0.0001)
    {
        t_acc = 0;
        t_con = 0;

        t_dacc = Td;
        v_mv = 0;
        a_mv = 0;
        return;
    }

    if(delta>=0) {
        t_acc = (a_max * Td - sqrt(delta)) / (2 * a_max);

        if (a_max * t_acc > v_max)
            t_acc = v_max / a_max;
    }
    else if (l_max>ld)
    {
        t_acc = sqrt(ld/a_max);
    }
    else
    {
        t_acc = v_max / a_max;
    }

    v_mv =t_acc*a_max;
    t_dacc = (ld - a_max*t_acc*t_acc)/v_mv+2*t_acc;
    t_con = t_dacc - t_acc;
    a_mv = a_max;

    if( Xd < Xc)
    {
        v_mv = -v_mv;
        a_mv = -a_max;
    }
}


//interface reslize:
void Splanner::PlannerParaSet(float xd, float xc,float td)
{
    Xd = xd;
    Xc = xc;
    Td = td;
    TrjPlan();
}


float Splanner::UpdatePlanner(float tc)
{
    float dc;
    // update planner:
    if(tc<0)
    {
        dc=0;
    }
    else if(tc<=t_acc)
    {
        dc = 0.5*a_mv*tc*tc;
    }
    else if (tc<=t_con)
    {
        dc = 0.5 * a_mv * t_acc * t_acc + (tc - t_acc) * v_mv;
    }
    else if( tc<=t_dacc)
    {
        dc = a_mv * t_acc * t_acc + (t_con - t_acc) * v_mv - 0.5 * a_mv * (t_dacc - tc) * (t_dacc - tc);
    }
    else
    {
        dc = a_mv * t_acc * t_acc + (t_con - t_acc) * v_mv;
    }
    return dc+Xc;
}