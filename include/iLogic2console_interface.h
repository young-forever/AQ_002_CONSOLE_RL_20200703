//
// Created by zyq on 2020/6/10.
//

#ifndef AQ_002_CONSOLE_ILOGIC2CONSOLE_INTERFACE_H
#define AQ_002_CONSOLE_ILOGIC2CONSOLE_INTERFACE_H

#include <iostream>

class iLogic2console_InterFace
{
public:
    ~iLogic2console_InterFace(){};

public:
    virtual int COMM_ShakeHand()=0;//communication shaekhan command.
    virtual void COMM_Close()=0;    //clse comm.
    virtual int COMM_Reset()=0;    //reset machine of aq.
    virtual int COMM_CommandBackModeSet(bool flag)=0;
    virtual int COMM_RequestModeSet(bool flag)=0;
    virtual int COMM_MotBrakeSet(bool flag)=0;
    virtual int COMM_BrakeSet(bool flag,const unsigned char brekenum)=0;
    virtual int COMM_RemoteSet(bool flag)=0;
    virtual int COMM_LimitVelSet(float limtvel,unsigned char num)=0;
    virtual int COMM_RequirL(void)=0;
    virtual int COMM_RequirWorkState(void)=0;
    virtual int COMM_RequirMotmod(const unsigned char num)=0;
    virtual int COMM_RequirSensor(void)=0;
    virtual int COMM_RequirExppos(const unsigned char num)=0;
    virtual int COMM_RequirExpvel(const unsigned char num)=0;
    virtual int COMM_RequirExpcur(const unsigned char num)=0;
    virtual int Stop(void)=0;

    virtual int COMM_SendTest()=0;

    virtual int SingleMotorPositionSend(unsigned char num, float pos)=0;
    virtual int SingleMotorVelocitySend(unsigned char num, float vel)=0;
    virtual int SingleMotorCurrentSend(unsigned char num, float cur)=0;

    virtual int Splan(float xd, float ld, float td)=0;
    virtual void Vplan(float veld, float td)=0;
    virtual void ZVModeSet(unsigned char mode)=0;

    virtual void CameraSet(bool flag, int frequency, unsigned char mode, std::string format)=0;
    virtual void SampleSet(bool flag,unsigned char mod,std::string dir)=0;

    virtual float GetChebdpPosX(void)=0;
    virtual float GetChebdpPosZ(void)=0;
};

#endif //AQ_002_CONSOLE_ILOGIC2CONSOLE_INTERFACE_H
