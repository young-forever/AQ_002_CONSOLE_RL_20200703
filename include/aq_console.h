//
// Created by zyq on 2020/6/5.
//

#ifndef AQ_002_CONSOLE_AQ_CONSOLE_H
#define AQ_002_CONSOLE_AQ_CONSOLE_H
#include "iLogic2console_interface.h"
#include "DataBase.h"
#include "m_thread.h"
#include "cmdline.h"
#include "console_interface.h"
#include "main.h"
#include "math.h"


typedef struct console_data_
{
    float x;
    float dx;
    float wx;
    float wz;
    float l;

    int cmr_sampletime ;
    std::string cmrfmat ;
    int cmrmod ;
    int smpmod ;
    std::string smpdir ;
    double motposition ;
    double motvelocity ;
    double motcurrent ;
    double drvkp ;
    double drvki ;
    double drvkd ;
    int motnum;
    int brknum ;
    bool flag ;
    int zvmod ;
    double xd ;
    double ld ;
    double td ;
    double ctrkp ;
    double ctrki;
    double ctrkd;
    double drphet;
    bool drpflag ;
    float limmotvel ;
    float Lc ;
}console_data;

class aq_console : public m_thread , public console_interface
{
private:
    console_data* m_Data; // state data

    iLogic2console_InterFace* m_ilogic;// logic layer
    cmdline::parser* cmd_line;
    std::string cmd;
    int test_scnt;
    int rm_flag=1;

public:
    aq_console();
    ~aq_console(){};

    void get_all_para();
    void set_ilogic(iLogic2console_InterFace* logic);
    void add_cmdline();

    void run();

public://command list:
    void Comm();
    void InitialPos();
    void Reset();
    void CommandBackSet();
    void RequestModeSet();
    void CameraModeSet();
    void SampleSet();
    void SglMotPos();
    void SglMotVel();
    void SglMotCur();
    void Stop();
    void DriverPidSet();
    void Splan();
    void VelPlan();
    void DemoPlan();
    void MotBrakeSet();
    void BrakeSet();
    void LimitVelSet();
    void RemoteModeSet();
    void RequireWeightX();
    void RequireCableLen();
    void RequireWorkState();
    void RequireMotMode();
    void RequireExpectPos();
    void RequireExpectVel();
    void RequireExpectCur();
    void RequireSensor();
    void TestComm();
    void ResetTest();
    void printfall();
public:
    //to ilogic interface func:
    void RecShakeHandOk();
    void ResetOk();
    void RecieveBack();
    void RecWorkStateBack(unsigned char workstate);
    void RecMotmodBack(unsigned char motmod);
    void RecSensorData(std::vector<float> sdata);
    void RecCableL(float L);
    void RecExppos(float pos);
    void RecExpvel(float vel);
    void RecExpcur(float cur);
    void RecErrorBack(unsigned char error);
    void ControlFinished(void);

};


#endif //AQ_002_CONSOLE_AQ_CONSOLE_H
