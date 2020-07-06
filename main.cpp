#include <iostream>
#include "ilogic.h"
#include "DataBase.h"
#include "aq_console.h"
#include "iLogic2RL_interface.h"
#include <unistd.h>
#include <random>
using namespace std;

#define CONSOLE_ENABLE 0

#if CONSOLE_ENABLE
int main()
{
    /*** 1: set object of layer  ***/
    iLogic* ilogic = new iLogic();              //ilogic layer:
    ilogic->init(0,"USB0");
    aq_console* m_console = new aq_console();   //console layer:

    /*** 2: set interface for each other  ***/
    m_console->set_ilogic(ilogic);
    ilogic->set_Console(m_console);

    /*** 3: start all thread  ***/
    m_console->start();

    while(1)
    {
    };

    return 0;
}
#else
// RL example:
int main()
{
    std::string cmd_str;
    RL_aq* ilogic = new RL_aq();

    while(1)
    {
        std::cout << "please input command:" << std::endl;
        std::getline(std::cin, cmd_str);

        if(cmd_str == "getdata")
        {
            std::vector<float> data;
            data = ilogic->RL_GetData();
            for(int i =0;i<data.size();i++)
            {
                std::cout<<data[i]<<std::endl;
            }
        }
        else if(cmd_str == "setvel")
        {
            std::vector<float>vel(2);
            std::cout << "please input vel vx(m/s): " << std::endl;
            std::getline(std::cin, cmd_str);

            std::vector<float> pos(2);
            vel[0] = std::atof(cmd_str.c_str());
            vel[1] = 0;

            std::vector<float>ret = ilogic->RL_SetVel(vel);
            if(ret[0]==0.0)
            {
                std::cout << "ERROR: setvel fail. " << std::endl;
            }
        }
        else if(cmd_str == "radv")
        {
            std::vector<float>vel(2);
            vel[0] = 0.2;
            vel[1] = 0;

            std::vector<float> ret;
            for(int i=0;i<100;i++)
            {
                vel[0] = -1*vel[0];
                ret = ilogic->RL_SetVel(vel);
                usleep(80000);

                std::vector<float> data;
                data = ilogic->RL_GetData();
                for(int i =0;i<data.size();i++)
                {
                    std::cout<<data[i]<<std::endl;
                }
                usleep(20000);
            }

            vel[0] = 0;
            ret = ilogic->RL_SetVel(vel);
            if(ret[0]==0.0)
            {
                std::cout << "ERROR: setvel fail. " << std::endl;
            }
        }
        else if(cmd_str == "reset")
        {
            std::vector<float>ret = ilogic->RL_Reset();
            if(ret[0]==0.0)
            {
                std::cout << "Reset fail. " << std::endl;
            }
        }
        else if(cmd_str == "setpos")
        {
            std::cout << "please input pos x(mm): " << std::endl;
            std::getline(std::cin, cmd_str);

            std::vector<float> pos(2);
            pos[0] = std::atof(cmd_str.c_str())/1000.0;
            pos[1] = 500/1000.0;
            std::vector<float>ret =ilogic->RL_SetPos(pos);
            if(ret[0]==0.0)
            {
                std::cout << "setpos fail. " << std::endl;
            }
        }
        else if(cmd_str == "radx")
        {
            default_random_engine e;
            uniform_real_distribution<double> u(1, 2.5); //随机数分布对象

            std::cout << "how much times do you want to dandom : " << std::endl;
            std::getline(std::cin, cmd_str);

            int cnt = std::atoi(cmd_str.c_str());
            std::vector<float> pos(2);
            for(int i=0;i<cnt;i++) {
                pos[0] = u(e);
                pos[1] = 500 / 1000.0;
                cout<<"pos[0]:"<<pos[0]<<endl;
                std::vector<float> ret = ilogic->RL_SetPos(pos);
                if (ret[0] == 0.0) {
                    std::cout << "setpos fail. " << std::endl;
                }
            }
        }
        else if(cmd_str == "quit")
        {
            ilogic->RL_Reset();
            break;
        }
        else
        {
            std::cout << "please input correct cmd." << std::endl;
        }
    };

    return 0;
}
#endif