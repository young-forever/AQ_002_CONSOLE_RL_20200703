//
// Created by zyq on 2020/6/10.
//

#ifndef AQ_002_CONSOLE_COMM2ILOGIC_INTERFACE_H
#define AQ_002_CONSOLE_COMM2ILOGIC_INTERFACE_H
#include "DataBase.h"
#include "iLogic2comm_interface.h"

class comm2ilogic_interface
{
public:
    //comm2ilogic_interface();
    ~comm2ilogic_interface(){};

public:
    virtual void setCommID(std::string ID)=0;
    virtual void set_ilogicinterface(ilogic2comm_interface* ilogic)=0;

    virtual int comm_start(void)=0; //run thread.

    /**** COMM COMMAND LIST  ****/
    virtual int COMM_ShakeHand(void)=0;//communication shake hand command.
    virtual int COMM_Close(void)=0;//communication close command.

    //machine mode set & data reqest:
    virtual int COMM_Reset(void)=0;     //reset machine.
    virtual int COMM_CommandBackModeSet(bool flag)=0;
    virtual int COMM_RequestModeSet(bool flag)=0;
    virtual int COMM_CameraSet(bool flag, short sampltime)=0;
    virtual int COMM_MotBrakeSet(bool flag)=0;
    virtual int COMM_BrakeSet(bool flag,const unsigned char brekenum)=0;
    virtual int COMM_RmoteSet(bool flag)=0;
    virtual int COMM_LimitVelSet(float limtvel,unsigned char num)=0;
    virtual int COMM_RequirL(void)=0;
    virtual int COMM_RequirWorkState(void)=0;
    virtual int COMM_RequirMotmod(const unsigned char num)=0;
    virtual int COMM_RequirSensor(void)=0;
    virtual int COMM_RequirExppos(const unsigned char num)=0;
    virtual int COMM_RequirExpvel(const unsigned char num)=0;
    virtual int COMM_RequirExpcur(const unsigned char num)=0;
    virtual int COMM_StopStateSet(void)=0;//Machine Stop Work State set.
    virtual int COMM_RunningStateSet(void)=0;//Machine Running work state set.

    // motor command list:
    virtual int COMM_DriverModeSet(const unsigned char mode, const unsigned char num)=0;//driver mode set.
    virtual int COMM_DriverExpectPositionSend(const float position,const unsigned char num)=0;//driver position set.
    virtual int COMM_DriverExpectVelocitySend(const float velocity,const unsigned char num)=0;//driver velocity set.
    virtual int COMM_DriverExpectCurrentSend(const float current,const unsigned char num)=0;//driver current set.

    virtual int COMM_SendTest(void)=0;//driver current set.
};
#endif //AQ_002_CONSOLE_COMM2ILOGIC_INTERFACE_H
