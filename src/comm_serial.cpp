#include "comm_serial.h"
#include <iostream>
#include <thread>
#include "math.h"

comm_serial::comm_serial()
{
    rewatt = 0;
    CommID="USB0";
}

comm_serial::~comm_serial()
{

}

int comm_serial::GetBcakId(const unsigned char sendid)
{
    if((sendid & 0x0F) == 0X0A)//requeset command.
    {
        if(rID[0]==sendid)
        {
            return 1;
        }
    }
    else//other command.
    {
         if(rID[0]==0XBC && rID[1]==sendid)
             return 1;
    }
    return 0;
}

int comm_serial::my_serial_write(const unsigned char* data,const int len, int waitms)
{
    #if DEBUG_ENABLE
        std::cout<<"DEBUG: Send command ID: "<<std::hex<<(unsigned int)data[1]<<std::dec<<std::endl<<std::endl;
    #endif

    int wret=0;
    int wcnt=0;
    sID = data[1];

    //reset rID;
    rID[0] = 0x77;
    rID[1] = 0x77;

    //only send once(send success)
    if(waitms<1)
    {
        while(!(wret = serial_write(data,len))) {
            wcnt++;
            if (wcnt > 10) {
                return -1;
            }
        }
        return 1;
    }

    // waitting back send mode:
    int wtime = 0;
    for (int i = 0; i < 3;i++)
    {
        wret = serial_write(data,len);
//        cout<<wret<<endl;
//        if(wret != 1)
//        {
//            #if COMM_DEBUG_ENABLE
//                std::cout<<"COMM LOG: COMM send fail."<<std::endl;
//            #endif
//            return -1;
//        }
        for(int i=0;i<waitms;i++)
        {
            usleep(1000);
            wtime++;
            if (GetBcakId(sID)) {
                #if COMM_DEBUG_ENABLE
                std::cout<<"COMM LOG: Waiting "<<wtime<<"ms."<<std::endl;
                #endif
                return 1;
            }
        }
    }
    std::cout<<"ERROR: COMM waiting fail, waiting "<<wtime<<"ms."<<std::endl;
    return 0;
}

int comm_serial::comm_start(void)
{
    start();
}

void comm_serial::shakehand()
{
//    std::cout<<"MESSAGE: Request shake hand."<<std::endl;
//    std::thread t(std::mem_fn(&comm_serial::COMM_ShakeHand),this);
//    t.detach();
}

//short command analysis.
void comm_serial::command_execute(const unsigned char re_command, const unsigned char* re_data)
{
    int data=0;
    float pdata=0;
    short id;//number of motor.

    switch(re_command & 0x0F)
    {
        case 0x00:  //serial port connect ok.
            //shakehand();
            break;

        case 0x01:  //serial port connect ok.
            m_ilogic->RecShakeHandOk();
            break;

        case 0x0A:  //require back
            switch(re_command)
            {
                case 0X0A:
                    m_ilogic->RecWorkStateBack(re_data[0]);
                    break;
                case 0X2A:
                    m_ilogic->RecMotmodBack(re_data[0]);
                    break;
                case 0X4A:
                    memcpy(&data,re_data,4);
                    pdata = (float)data/decimal_size;
                    m_ilogic->RecExppos(pdata);
                    break;
                case 0X6A:
                    memcpy(&data,re_data,4);
                    pdata = (float)data/decimal_size;
                    m_ilogic->RecExpvel(pdata);
                    break;
                case 0X8A:
                    memcpy(&data,re_data,4);
                    pdata = (float)data/decimal_size;
                    m_ilogic->RecExpcur(pdata);
                    break;
                case 0XAA:
                    memcpy(&data,re_data,4);
                    pdata = (float)data/decimal_size;
                    m_ilogic->RecCableL(pdata/1000.0);
                    break;
            }

            //get length of l.
        case 0x0C:
            switch(re_command)
            {
                case 0XBC:
                    m_ilogic->RecieveBack();
                    break;
                case 0XCC:

                    break;
            }

        case 0x0F:
            switch(re_command)
            {
                case 0X2F://
                    m_ilogic->ResetOk();
                    break;
                case 0X02://

                    break;
                case 0X03://

                    break;

                case 0XFF://error flag.
                    m_ilogic->RecErrorBack(re_data[0]);
                    break;
            }
            break;
    }
//    backid = re_command;

    rID[0] = re_command;
    rID[1] = re_data[0];

#if COMM_DEBUG_ENABLE
    std::cout<<"DEBUG: Receive command ID: "<<std::hex<<(unsigned int)re_command<<" "<<(unsigned int)re_data[0]<<std::dec<<std::endl<<std::endl;
#endif
}

//long command analysis.
void comm_serial::long_command_execute(const unsigned char long_re_command, const unsigned char* re_data)
{
    std::vector<float> sdata(4);
    unsigned char temp_data[4];
    int data=0;
    float pdata=0;

    //get position_x.
    temp_data[0] = re_data[0];
    temp_data[1] = re_data[1];
    temp_data[2] = re_data[2];
    temp_data[3] = re_data[3];

    memcpy(&data,temp_data,4);//put recieve data to temp data.
    pdata = (float)data/decimal_size;
    sdata[0] = pdata/1000.0;

    //get d_position_x.
    temp_data[0] = re_data[4];
    temp_data[1] = re_data[5];
    temp_data[2] = re_data[6];
    temp_data[3] = re_data[7];

    memcpy(&data,temp_data,4);//put recieve data to temp data.
    pdata = (float)data/decimal_size;
    sdata[1] = pdata/1000.0;

    //get theta.
    temp_data[0] = re_data[8];
    temp_data[1] = re_data[9];
    temp_data[2] = re_data[10];
    temp_data[3] = re_data[11];

    memcpy(&data,temp_data,4);//put recieve data to temp data.
    pdata = (float)data/decimal_size;
    sdata[2] = pdata/1000.0;

    //get d_theta.
    temp_data[0] = re_data[12];
    temp_data[1] = re_data[13];
    temp_data[2] = re_data[14];
    temp_data[3] = re_data[15];

    memcpy(&data,temp_data,4);//put recieve data to temp data.
    pdata = (float)data/decimal_size;
    sdata[3] = pdata/1000.0;

    m_ilogic->RecSensorData(sdata);

    if(long_re_command == 0x00) {
        return;
    }
//    backid = long_re_command;
    rID[0] = long_re_command;
}

/****** COMMAND LIST  *******/
int comm_serial::COMM_ShakeHand(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x01;
    data[2] = 0x01;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,50);
    return wlen;
}

int comm_serial::COMM_Close()
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    //std::cout<<"shake hand command send ok!"<<std::endl;

    int wlen = my_serial_write(data,8,5);
    return wlen;
}

int comm_serial::COMM_Reset(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0xAC;
    data[2] = 0x55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
//    std::cout<<"reset command send ok!"<<std::endl;
    return wlen;
}

int comm_serial::COMM_CommandBackModeSet(bool flag)
{
    if(flag==true)
    {
        rewatt = WAITTIME ;
    }
    else
    {
        rewatt=NOWAIT;
    }

    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0xBC;
    data[2] = (unsigned char)flag;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    //std::cout<<"command back mode set command send ok!"<<std::endl;
    return wlen;
}


int comm_serial::COMM_RequestModeSet(bool flag)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0xDC;
    data[2] = (unsigned char)flag;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
//    std::cout<<"Request mode set command send ok!"<<std::endl;
    return wlen;
}

//set camera.
int comm_serial::COMM_CameraSet(bool flag, short sampltime)
{
    unsigned char data[8];
    int pdata = sampltime*decimal_size;

    data[0] = 0x56;
    data[1] = 0x0C;
    data[2] = (unsigned char)(pdata & 0xff);
    data[3] = (unsigned char)(pdata>>8 & 0xff);
    data[4] = (unsigned char)(pdata>>16 & 0xff);
    data[5] = (unsigned char)(pdata>>24 & 0xff);
    data[6] = (unsigned char)flag;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_MotBrakeSet(bool flag)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x2C;
    data[2] = (unsigned char)flag;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_BrakeSet(bool flag,const unsigned char brekenum)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x4C;
    data[2] = (unsigned char)flag;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = brekenum;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}


int comm_serial::COMM_RmoteSet(bool flag)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x8C;
    data[2] = (unsigned char)flag;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_LimitVelSet(float limtvel,unsigned char num)
{
    unsigned char data[8];
    int pdata = limtvel*decimal_size;

    data[0] = 0x56;
    data[1] = 0xAD;
    data[2] = (unsigned char)(pdata & 0xff);
    data[3] = (unsigned char)(pdata>>8 & 0xff);
    data[4] = (unsigned char)(pdata>>16 & 0xff);
    data[5] = (unsigned char)(pdata>>24 & 0xff);
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_RequirL(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0xAA;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,200);
    return wlen;
}

int comm_serial::COMM_RequirWorkState(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x0A;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

int comm_serial::COMM_RequirMotmod(const unsigned char num)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x2A;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

int comm_serial::COMM_RequirSensor(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0xEA;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

int comm_serial::COMM_RequirExppos(const unsigned char num)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x4A;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

int comm_serial::COMM_RequirExpvel(const unsigned char num)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x6A;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

int comm_serial::COMM_RequirExpcur(const unsigned char num)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x8A;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,10);
    return wlen;
}

//Work State set.
int comm_serial::COMM_StopStateSet(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x8D;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_RunningStateSet(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x9D;
    data[2] = 0X55;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

// motor command list:
//driver mode set.
int comm_serial::COMM_DriverModeSet(const unsigned char mode, const unsigned char num)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x0D;
    data[2] = mode;
    data[3] = 0X55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

//driver position set.
int comm_serial::COMM_DriverExpectPositionSend(const float position,const unsigned char num)
{
    unsigned char data[8];
    int pdata = position*decimal_size;

    data[0] = 0x56;
    data[1] = 0x2D;
    data[2] = (unsigned char)(pdata & 0xff);
    data[3] = (unsigned char)(pdata>>8 & 0xff);
    data[4] = (unsigned char)(pdata>>16 & 0xff);
    data[5] = (unsigned char)(pdata>>24 & 0xff);
    data[6] = (unsigned char)num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

//driver velocity set.
int comm_serial::COMM_DriverExpectVelocitySend(const float velocity,const unsigned char num)
{
    unsigned char data[8];
    int pdata = velocity*decimal_size;

    data[0] = 0x56;
    data[1] = 0x4D;
    data[2] = (unsigned char)(pdata & 0xff);
    data[3] = (unsigned char)(pdata>>8 & 0xff);
    data[4] = (unsigned char)(pdata>>16 & 0xff);
    data[5] = (unsigned char)(pdata>>24 & 0xff);
    data[6] = (unsigned char)num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

//driver current set.
int comm_serial::COMM_DriverExpectCurrentSend(const float current,const unsigned char num)
{
    unsigned char data[8];
    int pdata = current*decimal_size;

    data[0] = 0x56;
    data[1] = 0x4D;
    data[2] = (unsigned char)(pdata & 0xff);
    data[3] = (unsigned char)(pdata>>8 & 0xff);
    data[4] = (unsigned char)(pdata>>16 & 0xff);
    data[5] = (unsigned char)(pdata>>24 & 0xff);
    data[6] = (unsigned char)num;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}

int comm_serial::COMM_SendTest(void)
{
    unsigned char data[8];

    data[0] = 0x56;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x0D;

    int wlen = my_serial_write(data,8,rewatt);
    return wlen;
}