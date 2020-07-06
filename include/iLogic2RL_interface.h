//
// Created by zyq on 2020/6/30.
//

#ifndef AQ_002_CONSOLE_ILOGIC2RL_INTERFACE_H
#define AQ_002_CONSOLE_ILOGIC2RL_INTERFACE_H

#include <iostream>
#include <vector>

class ilogic2RL_interface
{
public:
    ~ilogic2RL_interface(){};

    /*
     * param: null
     * return: data[0] = 0/1, data[1]=pos_x, data[2]=dx, data[3]=dz;
     */
    virtual std::vector<float> RL_GetData(void)=0;

    /*
     * param: vel[0] = vx, vel[1]=vl; unit: m/s;
     * return: 0/1
     */
    virtual std::vector<float> RL_SetVel(std::vector<float> vel)=0;

    /*
     * param: null
     * return: 0/1
     */
    virtual std::vector<float> RL_Reset(void)=0;

    /*
     * param: pos[0] = pos_x, pos[1]=pos_l; unit: m;
     * return: 0/1
     */
    virtual std::vector<float> RL_SetPos(std::vector<float> pos)=0;

};

class RL_aq
{
    ilogic2RL_interface* m_ilogic;
public:
    RL_aq();
    ~RL_aq(){};

    void set_ilogic2RL_interface(ilogic2RL_interface* ilogic){m_ilogic = ilogic;};

    std::vector<float> RL_GetData(void){return m_ilogic->RL_GetData();};
    std::vector<float> RL_SetVel(std::vector<float> vel){return m_ilogic->RL_SetVel(vel);};
    std::vector<float> RL_Reset(void){return m_ilogic->RL_Reset();};
    std::vector<float> RL_SetPos(std::vector<float> pos){return m_ilogic->RL_SetPos(pos);};

};


#endif //AQ_002_CONSOLE_ILOGIC2RL_INTERFACE_H
