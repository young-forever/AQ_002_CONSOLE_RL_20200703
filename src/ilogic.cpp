#include <iostream>
#include "ilogic.h"
#include "comm_serial.h"
#include "cameraworkthread.h"
#include "sample.h"
#include "aq_control.h"

iLogic::iLogic()
{
    CURRENT_STATE = AQ_State::PREPARE_STATE;
    CURRENT_DRIVERMOD[0] = UNKNOW_MODE;
    CURRENT_DRIVERMOD[1] = UNKNOW_MODE;
    smpmod = 0x00;

    /*** modules manage***/
    //serials:
    comm_serial* serial = new comm_serial();
    serial->set_ilogicinterface(this);
    set_comm_interface(serial);

    //camera:
    CameraWorkThread* camera = new CameraWorkThread();
    camera->set_ilogic_interface(this);
    set_camera(camera);

    //sample
    sample* sample_ = new sample();
    set_sample(sample_);

    //control
    aq_control* control = new aq_control();
    control->set_ilogic_interface(this);
    set_control(control);

    //chessboard predict
    MatrixRobot::ChessboardPredict* chebdp = new MatrixRobot::ChessboardPredict();
    set_chessboardPredict(chebdp);

    DataBase* m_DataBase = new DataBase();
    set_DataBase(m_DataBase);

    //init(0, "USB0");
}

iLogic::~iLogic()
{
    delete m_comm;

}

int iLogic::init(unsigned int CameraID, std::string CommID)
{
    m_camera->setCameraID(CameraID);
    m_camera->camera_satrt();

    m_comm->setCommID(CommID);
    m_comm->comm_start();

    int ret=0;

    sleep(2);
    ret = COMM_ShakeHand();
    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: shake hand ok."<<std::endl;

    ret = COMM_CommandBackModeSet(1);

    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: set command back mode ok."<<std::endl;

    ret = COMM_RequestModeSet(1);
    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: set request mode ok."<<std::endl;

    CameraSet(1,10,0,"jpeg");
    usleep(10000);

    std::cout<<"MESSAGE: Camera set ok."<<std::endl;
    return 1;
}

int iLogic::RL_init(unsigned int CameraID, std::string CommID)
{
    m_camera->setCameraID(CameraID);
    m_camera->camera_satrt();

    m_comm->setCommID(CommID);
    m_comm->comm_start();

    int ret=0;

    sleep(2);
    ret = COMM_ShakeHand();
    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: shake hand ok."<<std::endl;

    ret = COMM_CommandBackModeSet(1);
    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: set command back mode ok."<<std::endl;

    ret = COMM_RequestModeSet(1);

    usleep(20000);
    if(ret != 1)
        return 0;

    std::cout<<"MESSAGE: set request mode ok."<<std::endl;

    CameraSet(1,30,2,"jpeg");
    usleep(10000);

    std::cout<<"MESSAGE: Camera set ok."<<std::endl;
    return 1;
}

void iLogic::set_DataBase(DataBase* data)
{
    m_DataBase = data;
}

// set models:
void iLogic::set_comm_interface(comm2ilogic_interface* comm)
{
    m_comm = comm;
}

void iLogic::ResetOk()
{
    if(m_console != NULL)
    {
        m_console->ResetOk();
    }

    CURRENT_STATE = AQ_State::STOP_STATE;
}

void iLogic::RecieveBack()
{
    if(m_console != NULL)
    {
        m_console->RecieveBack();
    }
}

void iLogic::RecShakeHandOk()
{
    CURRENT_STATE = AQ_State::STOP_STATE;
    if(m_console != NULL)
    {
        m_console->RecShakeHandOk();
    }
}

void iLogic::RecWorkStateBack(unsigned char workstate)
{
    m_DataBase->WorkState=workstate;
    if(m_console != NULL)
    {
        m_console->RecWorkStateBack(workstate);
    }
}

void iLogic::RecMotmodBack(unsigned char motmod)
{
    if(m_console != NULL)
    {
        m_console->RecMotmodBack(motmod);
    }
}


void iLogic::RecSensorData(std::vector<float> sdata)
{
    m_DataBase->x=sdata[0];
    m_DataBase->dx=sdata[1];
    m_DataBase->theta=sdata[2];
    m_DataBase->dtheta=sdata[3];

    if(m_console != NULL)
    {
        m_console->RecSensorData(sdata);
    }
}

void iLogic::RecCableL(float L)
{
    m_DataBase->l=L;

    if(m_console != NULL)
    {
        m_console->RecCableL(L);
    }
}

void iLogic::RecExppos(float pos)
{
    if(m_console != NULL)
    {
        m_console->RecExppos(pos);
    }
}

void iLogic::RecExpvel(float vel)
{
    if(m_console != NULL)
    {
        m_console->RecExpvel(vel);
    }
}

void iLogic::RecExpcur(float cur)
{
    if(m_console != NULL)
    {
        m_console->RecExpcur(cur);
    }
}


void iLogic::RecErrorBack(unsigned char error)
{
    if(error != 0XCE)
    {
        CURRENT_STATE = AQ_State::FAULT_STATE;
    }
    if(m_console != NULL)
    {
        m_console->RecErrorBack(error);
    }
}


//to console interface realize:
int iLogic::COMM_ShakeHand()//communication shake hand command.
{
    int ret = m_comm->COMM_ShakeHand();

    if(ret==1)
        CURRENT_STATE = AQ_State::STOP_STATE;

    return ret;
}

void iLogic::COMM_Close()
{
    m_comm->COMM_Close();
}

int iLogic::COMM_Reset()//machine reset command.
{
    int ret = 0;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret = m_comm->COMM_Reset();
            if(ret)
            {
                CURRENT_STATE = AQ_State::PREPARE_STATE;
            }
            break;
        case AQ_State::FAULT_STATE:
            ret = m_comm->COMM_Reset();
            if(ret)
            {
                CURRENT_STATE = AQ_State::PREPARE_STATE;
            }
            break;
        default:
            std::cout<<"WARNNING: can't reset machine in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_CommandBackModeSet(bool flag)
{
    if(CURRENT_STATE == AQ_State::PREPARE_STATE)
    {
        std::cout<<"WARNNING: can't set mode in prepare state."<<endl<<endl;
        return 0;
    }
    else
    {
        m_DataBase->state_cbm_flag=flag;
        return m_comm->COMM_CommandBackModeSet(flag);
    }
}

int iLogic::COMM_RequestModeSet(bool flag)
{
    if(CURRENT_STATE == AQ_State::PREPARE_STATE)
    {
        std::cout<<"WARNNING: can't set mode in prepare state."<<endl<<endl;
        return 0;
    }
    else
    {
        m_DataBase->state_rm_flag=flag;
        return m_comm->COMM_RequestModeSet(flag);
    }
}

int iLogic::COMM_MotBrakeSet(bool flag)
{
    int ret = 0;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            m_comm->COMM_MotBrakeSet(flag);
            ret =1;
            break;
        default:
            std::cout<<"WARNNING: can't set motor brake in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_BrakeSet(bool flag,const unsigned char brekenum)
{
    int ret = 1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret=m_comm->COMM_BrakeSet(flag,brekenum);
            break;
        default:
            std::cout<<"WARNNING: can't set magnet brake in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_RemoteSet(bool flag)
{
    int ret = 1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret= m_comm->COMM_RmoteSet(flag);
            CURRENT_STATE = AQ_State::REMOTERUNNING_STATE;
            break;
        default:
            std::cout<<"WARNNING: can't set remote mode in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_LimitVelSet(float limtvel,unsigned char num)
{
    int ret = 1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret = m_comm->COMM_LimitVelSet(limtvel,num);
            if(!ret)
            {
                std::cout<<"ERROR: Set limit motvel fail."<<endl<<endl;
            }
            break;
        default:
            std::cout<<"WARNNING: can't set motor limit velocity in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_RequirL(void)
{
    int ret = 1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret=m_comm->COMM_RequirL();
            break;
        default:
            std::cout<<"WARNNING: can't requir cable length in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_RequirWorkState(void)
{
    return  m_comm->COMM_RequirWorkState();
}

int iLogic::COMM_RequirMotmod(const unsigned char num)
{
    return m_comm->COMM_RequirMotmod(num);
}

int iLogic::COMM_RequirSensor(void)
{
    if(m_DataBase->state_rm_flag) {
        return m_comm->COMM_RequirSensor();
    }
    return 1;
}

int iLogic::COMM_RequirExppos(const unsigned char num)
{
    return m_comm->COMM_RequirExppos(num);
}

int iLogic::COMM_RequirExpvel(const unsigned char num)
{
    return m_comm->COMM_RequirExpvel(num);
}

int iLogic::COMM_RequirExpcur(const unsigned char num)
{
    return m_comm->COMM_RequirExpcur(num);
}

int iLogic::SingleMotorPositionSend(unsigned char num, float pos)
{
    int ret=1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            ret = m_comm->COMM_DriverModeSet(POSITION_MODE, num);
            if (!ret) {
                std::cout << "set mot mode fail!" << std::endl;
                ret = 0;
                return ret;
            }
            CURRENT_DRIVERMOD[num-1] = POSITION_MODE;
            usleep(10000);
            ret = m_comm->COMM_RunningStateSet();
            if (!ret) {
                std::cout << "set running state fail!" << std::endl;
                ret = 0;
                return ret;
            }
            CURRENT_STATE = AQ_State::SGRUNNING_STATE;

            usleep(10000);
            cout<<pos<<endl;
            ret = m_comm->COMM_DriverExpectPositionSend(pos, num);
            if (!ret) {
                std::cout << "set mot position fail!" << std::endl;
                ret = 0;
                return ret;
            }
            break;

        case AQ_State::SGRUNNING_STATE:
            if (CURRENT_DRIVERMOD[num-1] != POSITION_MODE) {
                std::cout << "please set mot mode first!" << std::endl;
                return 0;
            }

            ret = m_comm->COMM_DriverExpectPositionSend(pos, num);
            if (!ret) {
                std::cout << "set mot position fail!" << std::endl;
                ret = 0;
                return ret;
            }
            break;

        default:
            std::cout << "WARNNING: can't execute single motor position mode in current sate." << endl << endl;
            ret = 0;
            break;
    }
    return ret;
}

int iLogic::SingleMotorVelocitySend(unsigned char num, float vel)
{
    int ret=1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE: {
            ret = m_comm->COMM_DriverModeSet(VELOCITY_MODE, num);
            if (!ret) {
                std::cout << "set mot mode fail!" << std::endl;
                ret = 0;
                return ret;
            }
            CURRENT_DRIVERMOD[num-1] = VELOCITY_MODE;

            usleep(10000);
            ret = m_comm->COMM_RunningStateSet();
            if (!ret) {
                std::cout << "set running state fail!" << std::endl;
                ret = 0;
                return ret;
            }

            usleep(10000);
            ret = m_comm->COMM_DriverExpectVelocitySend(vel, num);
            if (!ret) {
                std::cout << "set mot velocity fail!" << std::endl;
                ret = 0;
                return ret;
            }
            CURRENT_STATE = AQ_State::SGRUNNING_STATE;
            break;
        }
        case AQ_State::SGRUNNING_STATE: {
            if (CURRENT_DRIVERMOD[num-1] != VELOCITY_MODE) {
                std::cout << "please set mot mode first!" << std::endl;
                return 0;
            }

            ret = m_comm->COMM_DriverExpectVelocitySend(vel, num);
            if (!ret) {
                std::cout << "set mot velocity fail!" << std::endl;
                ret = 0;
                return ret;
            }
            break;
        }
        default: {
            std::cout << "WARNNING: can't execute single motor velocity mode in current sate." << endl << endl;
            ret =0;
            break;
        }
    }
    return ret;
}

int iLogic::SingleMotorCurrentSend(unsigned char num, float cur)
{
    int ret = 1;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE: {
            ret = m_comm->COMM_DriverModeSet(CURRRNT_MODE, num);
            CURRENT_DRIVERMOD[num-1] = CURRRNT_MODE;
            usleep(10000);
            ret = m_comm->COMM_RunningStateSet();
            CURRENT_STATE = AQ_State::SGRUNNING_STATE;
            usleep(10000);
            ret = m_comm->COMM_DriverExpectCurrentSend(cur, num);
            break;
        }
        case AQ_State::SGRUNNING_STATE: {
            usleep(10000);
            ret = m_comm->COMM_DriverExpectCurrentSend(cur, num);
            break;
        }
        default: {
            std::cout << "WARNNING: can't execute single motor current mode in current sate." << endl << endl;
            ret = 0;
            break;
        }
    }
    return ret;
}


//Stop state set.
int iLogic::Stop(void)
{
    int ret = 0;

    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE:
            m_comm->COMM_StopStateSet();
            ret =1;
            break;
        case AQ_State::SGRUNNING_STATE:
            m_comm->COMM_StopStateSet();
            CURRENT_STATE = AQ_State::STOP_STATE;
            ret =1;
            break;
        case AQ_State::PCRUNNING_STATE:
            m_control->StopMoving();
            m_comm->COMM_StopStateSet();
            CURRENT_STATE = AQ_State::STOP_STATE;
            ret =1;
            break;
        case AQ_State::REMOTERUNNING_STATE:
            m_comm->COMM_StopStateSet();
            CURRENT_STATE = AQ_State::STOP_STATE;
            ret =1;
            break;
        default:
            std::cout<<"WARNNING: can't stop machine in current sate."<<endl<<endl;
            break;
    }
    return ret;
}

int iLogic::COMM_SendTest()
{
    return m_comm->COMM_SendTest();
}

void iLogic::CameraSet(bool flag, int frequency, unsigned char mode, string format)
{
    if(CURRENT_STATE == AQ_State::PREPARE_STATE)
    {
        std::cout<<"WARNNING: can't set camera in prepare state."<<endl<<endl;
    }
    else
    {
        m_DataBase->state_cmr_flag=flag;
        int time = 1000/frequency;
        m_comm->COMM_CameraSet(flag,time);
        if(!flag)
        {
            return;
        }
        //set mode
        m_camera->setsavemode(mode);
        //set format
        m_camera->setformat(format);
    }
}

void iLogic::SampleSet(bool flag,unsigned char mod,std::string dir)
{
    //start/close sample
    smpmod = mod;
    m_sample->set_save_dir(dir);
    if(mod==0x01) {
        return;
    }
    else
    {
        if(flag)
        {
            m_sample->startsample();
        }
        else
        {
            m_sample->stopsample();
        }
    }
}

int iLogic::Splan(float xd, float ld, float td)
{
    int ret = 0;
    int comm_ret =0;
    float xc;
    float lc;
    //action
    switch (CURRENT_STATE) {
        case AQ_State::PREPARE_STATE:

            ret =-1;
            break;
        case AQ_State::STOP_STATE:
            //set driver mode
            comm_ret=m_comm->COMM_DriverModeSet(VELOCITY_MODE,1);
            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: set driver 1 mode fail."<<std::endl;
                return -1;
            }
            usleep(10000);
            comm_ret=m_comm->COMM_DriverModeSet(VELOCITY_MODE,2);
            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: set driver 2 mode fail."<<std::endl;
                return -1;
            }
            usleep(10000);

            //set PDO mode.
            comm_ret=COMM_RequestModeSet(0);

            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: set request mode fail."<<std::endl;
                return -1;
            }
            usleep(10000);

            xc = m_DataBase->x;
            comm_ret = COMM_RequirL();
            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: get L fail."<<std::endl;
                return -1;
            }
            lc = m_DataBase->l;

            m_control->PlannerParaSet(xd,xc,ld,lc,td);
            usleep(10000);

            comm_ret = COMM_CommandBackModeSet(0);

            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: set cbm mode fail."<<std::endl;
                return -1;
            }

            //set running mode:
            comm_ret=m_comm->COMM_RunningStateSet();
            if(comm_ret == 0 || comm_ret == -1)
            {
                std::cout<<"ERROR: set running state fail."<<std::endl;
                return -1;
            }
            usleep(10000);

            std::cout<<"SPLAN PARA: xd: "<<xd<<"    "
                     <<"xc: "<<xc<<" "
                     <<"ld: "<<ld<<" "
                     <<"lc: "<<lc<<" "
                     <<"td: "<<td<<" "<<std::endl;

            m_control->StartMoving();

            if(smpmod==0x01)
            {
                m_sample->startsample();
            }

            CURRENT_STATE=AQ_State::PCRUNNING_STATE;
            ret =1;
            break;
        case AQ_State::SGRUNNING_STATE:

            ret =-1;
            break;
        case AQ_State::PCRUNNING_STATE:

            ret =-1;
            break;
        case AQ_State::REMOTERUNNING_STATE:

            ret =-1;
            break;
        case AQ_State::FAULT_STATE:

            ret =-1;
            break;
    }
    return ret;

}

void iLogic::Vplan(float veld, float td)
{


}

float iLogic::GetChebdpPosX(void)
{
    return m_chessboardPredict->get_current_displace();
}

float iLogic::GetChebdpPosZ(void)
{
//    m_chessboardPredict->setUcimg(0x00);
//    usleep(200000);
    return m_chessboardPredict->get_current_height();
}


// to camera interface:
void iLogic::sample_add_image(unsigned char* p,unsigned int len,string format)
{
    std::vector<unsigned char> ucImg(len);
    memcpy(&ucImg[0],p,(int)len);
    m_chessboardPredict->setUcimg(ucImg);
    m_sample->add_image(p,len,format);


    std::string label;
    label = to_string(m_DataBase->x*1000.0);
    label.append(",");
    label.append(to_string(m_DataBase->l*1000.0));
    label.append(",");
    label.append(to_string(m_DataBase->theta));
    label.append(",");
    label.append(to_string(m_DataBase->dx*1000.0));
    label.append(",");
    label.append(to_string(m_DataBase->dl*1000.0));
    label.append(",");
    label.append(to_string(m_DataBase->dtheta));
    label.append(",");
    label.append(to_string(m_DataBase->vx));
    label.append(",");
    label.append(to_string(m_DataBase->vl));
    m_sample->add_label(label);
}


//to control interface:
float iLogic::GetXc(void)
{
    return m_DataBase->x;
}
void iLogic::ZVModeSet(unsigned char mode)
{
    m_control->ZVModeSet(mode);
}

int iLogic::SendControlValue(int motnum,float target_vx)//rpm
{
    m_comm->COMM_DriverExpectVelocitySend(target_vx,(unsigned char)motnum);
}

void iLogic::ControlFinished(void)
{
//    m_comm->COMM_CommandBackModeSet(1);

    if(m_console != NULL)
    {
        m_console->ControlFinished();
    }

    usleep(10000);

    if(m_DataBase->state_rm_flag==1)
    {
        m_comm->COMM_RequestModeSet(1);
    }
    else
    {
        m_comm->COMM_RequestModeSet(0);
    }

    if(smpmod==0x01)
    {
        m_sample->stopsample();
    }
    smpmod = 0x00;

    usleep(10000);
    m_comm->COMM_StopStateSet();

    CURRENT_STATE = AQ_State::STOP_STATE;
}

void iLogic::SetTargetValue(float vx, float vl)
{
    m_DataBase->vx=vx;
    m_DataBase->vl=vl;
}

// to RL interface realize:
std::vector<float> iLogic::RL_GetData(void)
{
    int ret;
    std::vector<float>data(4);
    ret = COMM_RequirSensor();

    if(ret !=1)
    {
        data[0]=0;
        return data;
    }

    data[0] = 1;
    data[1] = m_DataBase->x;
    data[2] = m_chessboardPredict->get_current_displace();
    data[3] = m_chessboardPredict->get_current_height();
    return data;
}

std::vector<float> iLogic::RL_SetVel(std::vector<float> vel)
{
    int ret=1;
    std::vector<float> error={0};
    std::vector<float> succeed={1};

    if(fabs(vel[0])>0.8 || fabs(vel[1]>0.8))
    {
        std::cout<<"ERROR: setvel out of range, vel can't big than 0.8m/s!"<<std::endl;
        return error;
    }

    //action
    switch (CURRENT_STATE) {
        case AQ_State::STOP_STATE: {
            ret = m_comm->COMM_DriverModeSet(VELOCITY_MODE, 1);
            if (!ret) {
                std::cout << "set mot mode fail!" << std::endl;
                ret = 0;
                return error;
            }
            usleep(10000);
            CURRENT_DRIVERMOD[0] = VELOCITY_MODE;
            ret = m_comm->COMM_DriverModeSet(VELOCITY_MODE, 2);
            if (!ret) {
                std::cout << "set mot mode fail!" << std::endl;
                ret = 0;
                return error;
            }
            CURRENT_DRIVERMOD[1] = VELOCITY_MODE;
            usleep(10000);
            ret = m_comm->COMM_RunningStateSet();
            if (!ret) {
                std::cout << "set running state fail!" << std::endl;
                ret = 0;
                return error;
            }
            usleep(10000);

            ret = m_comm->COMM_DriverExpectVelocitySend(vel[0]*1000.0/125.0*60.0, 1);
            if (!ret) {
                std::cout << "set mot 1 velocity fail!" << std::endl;
                ret = 0;
                return error;
            }
            usleep(2000);

            ret = m_comm->COMM_DriverExpectVelocitySend(vel[1]*1000.0/78.5*60, 2);
            if (!ret) {
                std::cout << "set mot 2 velocity fail!" << std::endl;
                ret = 0;
                return error;
            }

            CURRENT_STATE = AQ_State::SGRUNNING_STATE;
            break;
        }
        case AQ_State::SGRUNNING_STATE: {
            if (CURRENT_DRIVERMOD[0] != VELOCITY_MODE || CURRENT_DRIVERMOD[1] != VELOCITY_MODE) {
                std::cout << "please set mot mode first!" << std::endl;
                return error;
            }

            ret = m_comm->COMM_DriverExpectVelocitySend(vel[0]*1000.0/125.0*60.0, 1);
            if (!ret) {
                std::cout << "set mot 1 velocity fail!" << std::endl;
                ret = 0;
                return error;
            }
            usleep(2000);

            ret = m_comm->COMM_DriverExpectVelocitySend(vel[1]*1000.0/78.5*60.0, 2);
            if (!ret) {
                std::cout << "set mot 2 velocity fail!" << std::endl;
                ret = 0;
                return error;
            }

            break;
        }
        default: {
            std::cout << "WARNNING: can't execute single motor velocity mode in current sate." << endl << endl;
            return error;
            break;
        }
    }
    return succeed;
}

std::vector<float> iLogic::RL_Reset(void)
{
    int ret;
    int cnt=0;
    std::vector<float> error={0};
    std::vector<float> succeed={1};

    ret = COMM_RequirWorkState();
    if(ret != 1)
    {
        return error;
    }
    usleep(100000);
    if(m_DataBase->WorkState == 0x01)
    {
        ret = Stop();
        if(ret !=1)
            return error;
    }
    usleep(100000);

    // l reset:


    //x reset:
    ret = COMM_Reset();
    if(ret !=1)
        return error;

    std::cout<<"MESSAGE: waitting for RESET finish."<<std::endl;

    while (CURRENT_STATE != AQ_State::STOP_STATE)
    {
        usleep(100000);
        cnt++;
        if(cnt>=300)
            return error;
    }

    usleep(100000);
//    ret = RL_init();
    if(ret !=1)
        return error;

    std::cout<<"MESSAGE: RESET finish."<<std::endl;
    return succeed;
}


std::vector<float> iLogic::RL_SetPos(std::vector<float> pos)
{
    int ret;
    int cnt=0;
    std::vector<float> error={0};
    std::vector<float> succeed={1};

    if(pos[0]>3.5 || pos[0]<-0.1 || pos[1]>2 || pos[1]<0)
    {
        std::cout<<"ERROR: setpos out of range!"<<std::endl;
        return error;
    }

    int wkst = COMM_RequirWorkState();
    usleep(10000);
    if(m_DataBase->WorkState == 0x01)
    {
        ret = Stop();
        if(ret !=1)
            return error;
    }
    usleep(10000);
    COMM_RequirSensor();
    usleep(10000);
    // compute td:
    float td1 = fabs(pos[0]-m_DataBase->x)*1000.0/200.0;

//    float td2 = fabs(pos[1]-m_DataBase->l)*1000.0/80.0;
    float td2 = 0;
    float td = (td1>td2)? td1 :td2;

    cout<<"td: "<<td<<endl;
    ret = Splan(pos[0],pos[1],td);

    if(ret != 1)
        return error;

    std::cout<<"MESSAGE: waitting for splan finish."<<std::endl;

    while (CURRENT_STATE != AQ_State::STOP_STATE)
    {
        usleep(100000);
        cnt++;
        if(cnt>=300)
            return error;
    }

    m_comm->COMM_CommandBackModeSet(1);

    std::cout<<"MESSAGE: Splan finish."<<std::endl;
    return succeed;
}