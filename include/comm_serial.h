#ifndef COMM_SERIAL_H
#define COMM_SERIAL_H
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>

//my head file
#include "comm2iLogic_interface.h"
#include "m_thread.h"
#include "basic_serial.h"
#include "iLogic2comm_interface.h"
#include "main.h"

#define WAITTIME    10
#define NOWAIT      0

using namespace std;

class comm_serial : public basic_serial, public comm2ilogic_interface
{
private:
    ilogic2comm_interface* m_ilogic;
    const int decimal_size=1000;
    unsigned char backid;
    int rewatt;

    unsigned char sID;
    unsigned char rID[2];

public:
    comm_serial();
    ~comm_serial();

    int GetBcakId(unsigned char sendid);
    int my_serial_write(const unsigned char* data,const int len, int waitus);

    int comm_start(void); //comm thread.

    void setCommID(std::string ID){CommID=ID;};
    void set_ilogicinterface(ilogic2comm_interface* ilogic){m_ilogic = ilogic;};

    void shakehand();
    // rewrite command excute:
    void command_execute(const unsigned char re_command, const unsigned char* re_data);// 指令解析函数
    void long_command_execute(const unsigned char long_re_command,const unsigned char* re_data);//long command analysis.

    /****** COMMAND LIST  *******/
    int COMM_ShakeHand(void);
    int COMM_Close(void);
    int COMM_Reset(void);

    int COMM_CommandBackModeSet(bool flag);
    int COMM_RequestModeSet(bool flag);
    int COMM_CameraSet(bool flag, short sampltime);
    int COMM_MotBrakeSet(bool flag);
    int COMM_BrakeSet(bool flag,const unsigned char brekenum);
    int COMM_RmoteSet(bool flag);
    int COMM_LimitVelSet(float limtvel,unsigned char num);
    int COMM_StopStateSet(void);//Stop state set.
    int COMM_RunningStateSet(void);//Running state set.

    int COMM_RequirL(void);
    int COMM_RequirWorkState(void);
    int COMM_RequirMotmod(const unsigned char num);
    int COMM_RequirSensor(void);
    int COMM_RequirExppos(const unsigned char num);
    int COMM_RequirExpvel(const unsigned char num);
    int COMM_RequirExpcur(const unsigned char num);

    // motor command list:
    int COMM_DriverModeSet(const unsigned char mode, const unsigned char num);//driver mode set.
    int COMM_DriverExpectPositionSend(const float position,const unsigned char num);//driver position set.
    int COMM_DriverExpectVelocitySend(const float velocity,const unsigned char num);//driver velocity set.
    int COMM_DriverExpectCurrentSend(const float current,const unsigned char num);//driver current set.

    int COMM_SendTest(void);

};

#endif // COMM_SERIAL_H
