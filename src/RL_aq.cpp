//
// Created by zyq on 2020/6/30.
//

#include "iLogic2RL_interface.h"
#include "ilogic.h"

RL_aq::RL_aq()
{
    iLogic* ilogic = new iLogic();
    int ret = ilogic->RL_init(0,"USB0");
    if(ret != 1)
    {
        std::cout<<"RL init fail!"<<std::endl;
    } else
    {
        std::cout<<"RL init succeed!"<<std::endl;
    }

    set_ilogic2RL_interface(ilogic);
}

