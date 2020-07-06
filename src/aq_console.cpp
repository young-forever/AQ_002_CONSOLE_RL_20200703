//
// Created by zyq on 2020/6/5.
//

#include "../include/aq_console.h"
#include <string>
#include <unistd.h>

aq_console::aq_console()
{
    cmd_line = new cmdline::parser();

    add_cmdline();
    m_Data = new console_data();

//    DataBase* m_DataBase = new DataBase();
//    set_DataBase(m_DataBase);
}

void aq_console::add_cmdline()
{
    cmd_line->add<std::string>("cmd", 0, "conmmand choose", false, "unknow");
    cmd_line->add<int>("motnum", 'm', "motor number", false, 1);
    cmd_line->add<int>("brknum", 0, "magnet brake number", false, 1);
    cmd_line->add<bool>("flag", 'f', "flag", false, 0);
    cmd_line->add<int>("zvmod", 'z', "zvd mode set", false, 0);
    cmd_line->add<int>("cmrfre", 0, "camera frequent set", false, 10);
    cmd_line->add<int>("cmrmod", 0, "camera mode", false, 0);
    cmd_line->add<std::string>("cmrfmat", 0, "camera image format", false, "jpeg");
    cmd_line->add<int>("smpmod", 0, "sample mode", false, 0);
    cmd_line->add<std::string>("smpdir", 0, "sample save directoty", false, "sample/default");
    cmd_line->add<double>("mpos", 0, "motor expect position ", false, 0.0);
    cmd_line->add<double>("mvel", 0, "motor expect velocity ", false, 0.0);
    cmd_line->add<double>("mcur", 0, "motor expect current ", false, 0.0);
    cmd_line->add<double>("drvkp", 0, "set driver kp", false, 0.0);
    cmd_line->add<double>("drvki", 0, "set driver ki", false, 0.0);
    cmd_line->add<double>("drvkd", 0, "set driver kd", false, 0.0);
    cmd_line->add<double>("amax", 0, "set max acc of S planner", false, 2000);//mm/s^2
    cmd_line->add<double>("vmax", 0, "set max velocity of S planner", false, 500);//mm/s
    cmd_line->add<double>("xd", 0, "set expect xd of S planner", false, 0.0);
    cmd_line->add<double>("ld", 0, "set expect ld of S planenr", false, 500);//mm
    cmd_line->add<double>("veld", 0, "set expect velocity pf step velocity", false, 0.0);
    cmd_line->add<double>("td", 0, "set td of planenr", false, 20);
    cmd_line->add<double>("ctrkp", 0, "set pid controller kp", false, 7);
    cmd_line->add<double>("ctrki", 0, "set pid controller ki", false, 0.0);
    cmd_line->add<double>("ctrkd", 0, "set pid controller kd", false, 10);
    cmd_line->add<double>("drphet", 0, "set drop height of cable", false, 800);
    cmd_line->add<bool>("drpflag", 0, "set aotu drop weight tragectory", false, 0);
    cmd_line->add<float>("limmotvel", 0, "limit velocity of motor. rpm", false, 300);
    cmd_line->add<float>("Lc", 0, "Current length of cable.", false, 500);
}

void aq_console::get_all_para()
{
    cmd = cmd_line->get<std::string>("cmd");

    m_Data->cmr_sampletime = (short)cmd_line->get<int>("cmrfre");
    m_Data->cmrfmat = (std::string)cmd_line->get<std::string>("cmrfmat");
    m_Data->cmrmod = cmd_line->get<int>("cmrmod");
    m_Data->smpmod = cmd_line->get<int>("smpmod");
    m_Data->smpdir = (std::string)cmd_line->get<std::string>("smpdir");
    m_Data->motposition = cmd_line->get<double>("mpos");
    m_Data->motvelocity = cmd_line->get<double>("mvel");
    m_Data->motcurrent = cmd_line->get<double>("mcur");
    m_Data->drvkp = cmd_line->get<double>("drvkp");
    m_Data->drvki = cmd_line->get<double>("drvki");
    m_Data->drvkd = cmd_line->get<double>("drvkd");
    m_Data->motnum = cmd_line->get<int>("motnum");
    m_Data->brknum = cmd_line->get<int>("brknum");
    m_Data->flag = cmd_line->get<bool>("flag");
    m_Data->zvmod = cmd_line->get<int>("zvmod");
    m_Data->xd = cmd_line->get<double>("xd")/1000.0;
    m_Data->ld = cmd_line->get<double>("ld")/1000.0;
    m_Data->td = cmd_line->get<double>("td");
    m_Data->ctrkp = cmd_line->get<double>("ctrkp");
    m_Data->ctrki = cmd_line->get<double>("ctrki");
    m_Data->ctrkd = cmd_line->get<double>("ctrkd");
    m_Data->drphet = cmd_line->get<double>("drphet");
    m_Data->drpflag = cmd_line->get<bool>("drpflag");
    m_Data->limmotvel = cmd_line->get<float>("limmotvel");
    m_Data->Lc = cmd_line->get<float>("Lc")/1000.0;
}

void aq_console::set_ilogic(iLogic2console_InterFace* logic)
{
    m_ilogic = logic;
}

int cnt = 0;

int ctrokflag = 0;
void aq_console::run()
{
    std::string cmd_str;
    std::string cmd_head="cmd --cmd=";

    while(1)
    {
        std::cout << "please input command:" << std::endl;
        std::getline(std::cin, cmd_str);
        cmd_str.insert(0,cmd_head);

        // check input legality:
        bool input_ok_flag=cmd_line->parse_check_yang(cmd_str);

        if(!input_ok_flag)
        {
            std::cout << "ERROR:input command error,please input again!" << std::endl<< std::endl;
            continue;
        }

        //read all paraments:
        get_all_para();

        //analysis command:
        if(cmd == "comm")
        {
            Comm();
        }
        else if(cmd == "init")
        {
            InitialPos();
        }
        else if(cmd == "reset")
        {
            Reset();
        }
        else if(cmd == "cbmset")
        {
            CommandBackSet();
        }
        else if(cmd == "rmset")
        {
            RequestModeSet();
        }
        else if(cmd == "cmrset")
        {
            CameraModeSet();
        }
        else if(cmd == "smpset")
        {
            SampleSet();
        }
        else if(cmd == "motpos")
        {
            SglMotPos();
        }
        else if(cmd == "motvel")
        {
            SglMotVel();
        }
        else if(cmd == "motcur")
        {
            SglMotCur();
        }
        else if(cmd == "stop")
        {
            Stop();
        }
        else if(cmd == "drvpidset")
        {
            DriverPidSet();
        }
        else if(cmd == "splan")
        {
            Splan();
        }
        else if(cmd == "vplan")
        {
            VelPlan();
        }
        else if(cmd == "demo")
        {
            DemoPlan();
        }
        else if(cmd == "motbrkset")
        {
            MotBrakeSet();
        }
        else if(cmd == "brkset")
        {
            BrakeSet();
        }
        else if(cmd == "limvelset")
        {
            LimitVelSet();
        }
        else if(cmd == "rmtmodset")
        {
            RemoteModeSet();
        }
        else if(cmd == "rewx")
        {
            RequireWeightX();
        }
        else if(cmd == "recl")
        {
            RequireCableLen();
        }
        else if(cmd == "rewkst")
        {
            RequireWorkState();
        }
        else if(cmd == "remotmod")
        {
            RequireMotMode();
        }
        else if(cmd == "reexppos")
        {
            RequireExpectPos();
        }
        else if(cmd == "reexpvel")
        {
            RequireExpectVel();
        }
        else if(cmd == "reexpcur")
        {
            RequireExpectCur();
        }
        else if(cmd == "ress")
        {
            RequireSensor();
        }
        else if(cmd == "testcomm")
        {
            TestComm();
        }
        else if(cmd == "resettest")
        {
            ResetTest();
        }
        else if(cmd == "prtall")//print all database.
        {
            printfall();
        }
        else if(cmd == "exit()")
        {
            ThreadStopFlag = true;
        }

        else
        {
            std::cout << "you must input correct command!" << std::endl;
        }
        std::cout<< std::endl<< std::endl;

        //
        m_mutex->lock();

        if(ThreadStopFlag)
        {
            m_mutex->unlock();
            return;
        }
        m_mutex->unlock();

    }
}

void aq_console::Comm()
{
    int ret;
    if(m_Data->flag == 1)
    {
        ret = m_ilogic->COMM_ShakeHand();

        if(ret==1)
            std::cout<<"open comm ok!"<<std::endl;
    }
    else
    {
        m_ilogic->COMM_Close();
        std::cout<<"close comm!"<<std::endl;
    }
}

void aq_console::InitialPos()
{
    int ret;
    m_Data->flag = 1;
    Comm();
    usleep(10000);

    CommandBackSet();
    usleep(10000);
    RequestModeSet();
    usleep(10000);

    m_Data->cmr_sampletime = 20;
    m_Data->cmrmod = 0x02;
    m_Data->cmrfmat = "jpeg";
    CameraModeSet();
    usleep(200000);

    //request Lc:
    float Lc = m_Data->Lc;
    m_Data->motnum=2;
    m_Data->motposition=1000.0f-Lc*1000.0f;
    ret = m_ilogic->SingleMotorPositionSend(m_Data->motnum,m_Data->motposition);
    if(ret != 1)
    {
        std::cout<<"initial fail!"<<std::endl;
        return;
    }
    std::cout<<"weight height reset ing!"<<std::endl;
    usleep(1000000*fabs(Lc-0.5)*1000/52.3+2000000);
    ret = m_ilogic->Stop();
    if(ret != 1)
    {
        std::cout<<"stop fail!"<<std::endl;
        return;
    }
    std::cout<<"height reset ok!"<<std::endl;

    usleep(10000);
    ret = m_ilogic->COMM_Reset();
    if(ret == 1)
    {
        std::cout<<"reset ing!"<<std::endl;
    }
}

void aq_console::Reset()
{
    int ret = m_ilogic->COMM_Reset();
    if(ret == 1)
    {
        std::cout<<"reset ing!"<<std::endl;
    }
}

void aq_console::CommandBackSet()
{
    int ret;
    if(m_Data->flag == true)
    {
        ret = m_ilogic->COMM_CommandBackModeSet(1);
        if(ret==1)
            std::cout<<"set comm back mode ok!"<<std::endl;
    }
    else
    {
        ret = m_ilogic->COMM_CommandBackModeSet(0);
        if(ret==1)
        std::cout << "close comm back mode." << std::endl;
    }
}

void aq_console::RequestModeSet()
{
    int ret;
    if(m_Data->flag == true)
    {
        ret = m_ilogic->COMM_RequestModeSet(1);
        if(ret==1)
        {
            rm_flag=1;
            std::cout<<"set require mode ok!"<<std::endl;
        }
    }
    else {
        ret = m_ilogic->COMM_RequestModeSet(0);
        if(ret==1)
        {
            rm_flag=0;
            std::cout << "close require mode." << std::endl;
        }
    }
}

void aq_console::CameraModeSet()
{
    bool flag = m_Data->flag;
    int fre = m_Data->cmr_sampletime;
    unsigned char mod = m_Data->cmrmod;
    std::string fmat = m_Data->cmrfmat;

    m_ilogic->CameraSet(flag,fre,mod,fmat);
    std::cout<<"set camera ok!"<<std::endl;
}

void aq_console::SampleSet()
{
    bool flag=m_Data->flag;
    unsigned char mode=m_Data->smpmod;
    std::string dir = m_Data->smpdir;
    m_ilogic->SampleSet(flag,mode,dir);
}

void aq_console::SglMotPos()
{
    int ret;
    ret = m_ilogic->SingleMotorPositionSend(m_Data->motnum,m_Data->motposition);

    if(ret==1)
    std::cout<<"single motor position moving!"<<std::endl;
}

void aq_console::SglMotVel()
{
    int ret;
    ret=m_ilogic->SingleMotorVelocitySend(m_Data->motnum,m_Data->motvelocity);

    if(ret==1)
    std::cout<<"single motor velocity mode moving!"<<std::endl;
}

void aq_console::SglMotCur()
{
    int ret;
    ret = m_ilogic->SingleMotorCurrentSend(m_Data->motnum,m_Data->motcurrent);

    if(ret==1)
    std::cout<<"single motor current mode moving!"<<std::endl;
}

void aq_console::Stop()
{
    int ret = m_ilogic->Stop();
    if(ret==1)
        std::cout<<"stop machine ok!"<<std::endl;
}

void aq_console::DriverPidSet()
{
    std::cout<<"set driver pid!"<<std::endl;
}

void aq_console::Splan()
{
    float xd = m_Data->xd;
    float ld = m_Data->ld;
    float td = m_Data->td;

    m_ilogic->ZVModeSet(m_Data->zvmod);
    int ret = m_ilogic->Splan(xd,ld,td);

    if(ret !=1)
    {
        std::cout<<"s planner fail!"<<std::endl;
    }
}

void aq_console::VelPlan()
{

    std::cout<<"step velocity run!"<<std::endl;
}

void aq_console::DemoPlan()
{
    std::string cmd_str;
    do
    {
        float xd = m_Data->xd;
        float ld = m_Data->ld;
        float td = m_Data->td;

// trj 1
        m_ilogic->ZVModeSet(m_Data->zvmod);
        int ret = m_ilogic->Splan(xd,ld,td);

        if(ret !=1)
        {
            std::cout<<"s planner fail!"<<std::endl;
            break;
        }
        else
        {
            std::cout<<"s planner ok!"<<std::endl;
        }

//judge if trj1 finished and if need to execute trj2(drop height).
        while (1)
        {
            if(ctrokflag)
            {
                ctrokflag=0;
                if(m_Data->drpflag)
                {
                    usleep(1000000);
                    m_ilogic->ZVModeSet(0x00);
                    ret = m_ilogic->Splan(m_Data->xd,m_Data->drphet/1000.0,3);
                    if(!ret)
                    {
                        std::cout<<"s planner fail!"<<std::endl;
                    }
                    else
                    {
                        std::cout<<"s planner ok!"<<std::endl;
                    }
                    break;
                }
                else
                {
                    break;
                }
            }
            usleep(1000);
        }

        while (1)
        {
            if(ctrokflag)
            {
                ctrokflag=0;
                break;
            }
            usleep(1000);
        }

        std::cout<<"press 'Enter' back to zero."<<std::endl;
        std::getline(std::cin, cmd_str);

        ret = m_ilogic->Splan(0.2,0.5,3);
        if(!ret)
        {
            std::cout<<"s planner fail!"<<std::endl;
            break;
        }
        else
        {
            std::cout<<"s planner ok!"<<std::endl;
        }
        while (1)
        {
            if(ctrokflag)
            {
                ctrokflag=0;
                break;
            }
            usleep(1000);
        }
    }while (0);
}

void aq_console::MotBrakeSet()
{
    int ret;
    ret=m_ilogic->COMM_MotBrakeSet(m_Data->flag);

    if(ret==1)
    std::cout<<"Motor brake set ok!"<<std::endl;
}

void aq_console::BrakeSet()
{
    int ret;
    ret = m_ilogic->COMM_BrakeSet(m_Data->flag,m_Data->brknum);

    if(ret==1)
    std::cout<<"Magnet brake set ok!"<<std::endl;
}

void aq_console::LimitVelSet()
{
    int ret;
    ret=m_ilogic->COMM_LimitVelSet(m_Data->limmotvel,m_Data->motnum);

    if(ret==1)
    std::cout<<"motor limit velocity set ok!"<<std::endl;
}

void aq_console::RemoteModeSet()
{
    int ret;
    ret=m_ilogic->COMM_RemoteSet(m_Data->flag);

    if(ret==1)
    std::cout<<"remote mode set ok!"<<std::endl;
}

void aq_console::RequireWeightX()
{
    float real_wx = m_ilogic->GetChebdpPosX();

    std::cout<<"Real weight x is :"<<real_wx<<std::endl;

}

void aq_console::RequireCableLen()
{
    int ret;
    ret=m_ilogic->COMM_RequirL();

    float real_lc = m_ilogic->GetChebdpPosZ();

    std::cout<<"Real lc is :"<<real_lc<<std::endl;

    if(ret==1)
        std::cout<<"require cable length send ok!"<<std::endl;
}

void aq_console::RequireWorkState()
{
    int ret;
    ret=m_ilogic->COMM_RequirWorkState();

    if(ret==1)
            std::cout<<"require for work state of machine!"<<std::endl;
}

void aq_console::RequireMotMode()
{
    int ret;
    ret = m_ilogic->COMM_RequirMotmod(m_Data->motnum);

    if(ret==1)
    std::cout<<"require for motor mode."<<std::endl;
}

void aq_console::RequireExpectPos()
{
    int ret;
    ret =m_ilogic->COMM_RequirExppos(m_Data->motnum);

    if(ret==1)
    std::cout<<"require for expect position."<<std::endl;
}

void aq_console::RequireExpectVel()
{
    int ret;
    ret=m_ilogic->COMM_RequirExpvel(m_Data->motnum);

    if(ret==1)
    std::cout<<"require for expect velocity."<<std::endl;
}

void aq_console::RequireExpectCur()
{
    int ret;
    ret=m_ilogic->COMM_RequirExpcur(m_Data->motnum);

    if(ret==1)
    std::cout<<"require for expect current."<<std::endl;
}

void aq_console::RequireSensor()
{
    int ret;
    ret=m_ilogic->COMM_RequirSensor();

    if(ret==1)
    std::cout<<"require for sensor data."<<std::endl;
}

void aq_console::TestComm()
{
    int sendfailcnt=0;
    std::cout<<"test begin:"<<std::endl;

    for(int cnt=0;cnt<1000;cnt++)
    {
        if(!m_ilogic->COMM_SendTest())
            sendfailcnt++;
        else
        {
        }
        usleep(1000);
    }
    usleep(20000);

    std::cout<<"test finished!"<<std::endl;
    std::cout<<"recieve back command "<<std::dec<<test_scnt<<";   "<<"send fail:"<<sendfailcnt<<"."<<std::endl;
}

void aq_console::ResetTest()
{
    test_scnt=0;
    cnt = 0;
}

void aq_console::printfall()
{
    std::cout<<"DataBase:"<<std::endl;
    std::cout<<"x: "<<m_Data->x<<std::endl;
    std::cout<<"dx: "<<m_Data->dx<<std::endl;
    std::cout<<"wx: "<<m_Data->wx<<std::endl;
    std::cout<<"wz: "<<m_Data->wz<<std::endl;
    std::cout<<"l: "<<m_Data->l<<std::endl;

    std::cout<<"motnum: "<<m_Data->motnum<<std::endl;
    std::cout<<"limmotvel: "<<m_Data->limmotvel<<std::endl;
    std::cout<<"flag: "<<m_Data->flag<<std::endl;
    std::cout<<"xd: "<<m_Data->xd<<std::endl;
    std::cout<<"ld: "<<m_Data->ld<<std::endl;
//    std::cout<<"veld: "<<m_Data->veld<<std::endl;
    std::cout<<"td: "<<m_Data->td<<std::endl;
    std::cout<<"ctrkp: "<<m_Data->ctrkp<<std::endl;
    std::cout<<"ctrki: "<<m_Data->ctrki<<std::endl;
    std::cout<<"ctrkd: "<<m_Data->ctrkd<<std::endl;
    std::cout<<"drphet: "<<m_Data->drphet<<std::endl;
    std::cout<<"drpflag: "<<m_Data->drpflag<<std::endl;
    std::cout<<(std::hex)<<"zvmod: "<<(unsigned int)m_Data->zvmod<<std::dec<<std::endl;
    std::cout<<"cmr_sampletime: "<<m_Data->cmr_sampletime<<std::endl;
    std::cout<<"cmrmod: "<<(std::hex)<<m_Data->cmrmod<<std::dec<<std::endl;
    std::cout<<"cmrfmat: "<<m_Data->cmrfmat.c_str()<<std::endl;
    std::cout<<"smpmod: "<<(std::hex)<<m_Data->smpmod<<std::dec<<std::endl;
    std::cout<<"smpdir: "<<m_Data->smpdir.c_str()<<std::endl;
    std::cout<<"motposition: "<<m_Data->motposition<<std::endl;
    std::cout<<"motvelocity: "<<m_Data->motvelocity<<std::endl;
    std::cout<<"motcurrent: "<<m_Data->motcurrent<<std::endl;
    std::cout<<"drvkp: "<<m_Data->drvkp<<std::endl;
    std::cout<<"drvki: "<<m_Data->drvki<<std::endl;
    std::cout<<"drvkd: "<<m_Data->drvkd<<std::endl;
}


// receive call back function:
void aq_console::RecShakeHandOk()
{
    std::cout<<"MESSAGE: Receive shake hand ok!"<<std::endl<<std::endl;
}

void aq_console::ResetOk()
{
    std::cout<<"MESSAGE: Receive Reset finished!"<<std::endl<<std::endl;
}

void aq_console::RecieveBack()
{
#if DEBUG_ENABLE
    std::cout << "MESSAGE: Receive back!" << std::endl << std::endl;
#endif
}


void aq_console::RecWorkStateBack(unsigned char workstate)
{
    std::cout<<std::hex<<"ANSER: Slave computer work state is : "\
    <<(unsigned int)workstate<<std::endl<<std::endl;
}

void aq_console::RecMotmodBack(unsigned char motmod)
{
    std::cout<<std::hex<<"ANSER: Motor mode is : "<<(unsigned int)motmod<<std::endl<<std::endl;
}

void aq_console::RecSensorData(std::vector<float> sdata)
{
    if(rm_flag==1) {
        std::cout << "ANSER: Get sensor data " << ":"
                  << sdata[0] << ",    "
                  << sdata[1] << ",    "
                  << sdata[2] << ",    "
                  << sdata[3] << ",    "
                  << std::endl;
    }
}

void aq_console::RecCableL(float L)
{
    std::cout<<"ANSER: Receive cable length L: "<<L<<std::endl<<std::endl;
}

void aq_console::RecExppos(float pos)
{
    std::cout<<"ANSER: Receive expect position : "<<pos<<std::endl<<std::endl;
}

void aq_console::RecExpvel(float vel)
{
    std::cout<<"ANSER: Receive expect velocity : "<<vel<<std::endl<<std::endl;
}

void aq_console::RecExpcur(float cur)
{
    std::cout<<"ANSER: receive expect current : "<<cur<<std::endl<<std::endl;
}

void aq_console::RecErrorBack(unsigned char error)
{
    std::cout<<"MESSAGE: Receive ERROR back : "<<std::hex<<(unsigned int)error<<std::dec<<std::endl<<std::endl;
}

void aq_console::ControlFinished(void)
{
    ctrokflag = 1;
    std::cout<<"MESSAGE: Splan trjectory finished!"<<std::endl<<std::endl;
}