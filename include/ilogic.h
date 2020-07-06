#ifndef ILOGIC_H
#define ILOGIC_H
#include "DataBase.h"
#include "iLogic2console_interface.h"
#include "iLogic2comm_interface.h"
#include "comm2iLogic_interface.h"
#include "console_interface.h"
#include "sample_interface.h"
#include "cmr2ilogic_interface.h"
#include "iLogic2cmr_interface.h"
#include "iLogic2control_interface.h"
#include "control2ilogic_interface.h"
#include "chessboard_predict.h"
#include "iLogic2RL_interface.h"

// master computer state
enum class AQ_State{
    PREPARE_STATE,
    STOP_STATE,
    PCRUNNING_STATE,
    SGRUNNING_STATE,
    REMOTERUNNING_STATE,
    FAULT_STATE
};

/* DRIVER MODE */
#define UNKNOW_MODE     0X00
#define POSITION_MODE   0XD0
#define VELOCITY_MODE   0XC4
#define CURRRNT_MODE    0XC1

class iLogic : public iLogic2console_InterFace, public ilogic2comm_interface, public iLogic2cmr_interface, public iLogic2ctr_interface ,
               public ilogic2RL_interface
{
private:
    console_interface* m_console = NULL; //console interface
    comm2ilogic_interface* m_comm; //comm interface
    sample_interface* m_sample;
    cmr2ilogic_interface* m_camera;
    ctr2ilogic_interface* m_control;
    MatrixRobot::ChessboardPredict_interface* m_chessboardPredict;

    DataBase* m_DataBase;
    AQ_State CURRENT_STATE;
    unsigned char CURRENT_DRIVERMOD[2];

    unsigned char smpmod;//0x00 sample by hand,0x01 sample auto;

public:
    iLogic();
    ~iLogic();

    int init(unsigned int CameraID, std::string CommID);
    int RL_init(unsigned int CameraID, std::string CommID);

    //others interface
    void set_DataBase(DataBase* data);
    void set_Console(console_interface* console){m_console = console;};
    void set_comm_interface(comm2ilogic_interface* comm);

    void set_sample(sample_interface* sample){m_sample = sample;};
    void set_camera(cmr2ilogic_interface* camera){m_camera=camera;};
    void set_control(ctr2ilogic_interface* control){m_control=control;};
    void set_chessboardPredict(MatrixRobot::ChessboardPredict_interface* chessbdp){m_chessboardPredict = chessbdp;};

public:
    // to comm interface realize:
    void ResetOk();
    void RecieveBack();
    void RecShakeHandOk();
    void RecWorkStateBack(unsigned char workstate);
    void RecMotmodBack(unsigned char motmod);
    void RecSensorData(std::vector<float> sdata);
    void RecCableL(float L);
    void RecExppos(float pos);
    void RecExpvel(float vel);
    void RecExpcur(float cur);
    void RecErrorBack(unsigned char error);


    //to console interface realize:
    int COMM_ShakeHand();//communication shaekhan command.
    void COMM_Close();  //close comm.
    int COMM_Reset();
    int COMM_CommandBackModeSet(bool flag);
    int COMM_RequestModeSet(bool flag);

    int COMM_MotBrakeSet(bool flag);
    int COMM_BrakeSet(bool flag,const unsigned char brekenum);
    int COMM_RemoteSet(bool flag);
    int COMM_LimitVelSet(float limtvel,unsigned char num);
    int COMM_RequirL(void);
    int COMM_RequirWorkState(void);
    int COMM_RequirMotmod(const unsigned char num);
    int COMM_RequirSensor(void);
    int COMM_RequirExppos(const unsigned char num);
    int COMM_RequirExpvel(const unsigned char num);
    int COMM_RequirExpcur(const unsigned char num);
    int Stop(void);

    int COMM_SendTest(void);

    int SingleMotorPositionSend(unsigned char num, float pos);
    int SingleMotorVelocitySend(unsigned char num, float vel);
    int SingleMotorCurrentSend(unsigned char num, float cur);

    void CameraSet(bool flag, int frequency, unsigned char mode, string format);
    void SampleSet(bool flag,unsigned char mod,std::string dir);
    void ZVModeSet(unsigned char mode);

    int Splan(float xd, float ld, float td);
    void Vplan(float veld, float td);

    float GetChebdpPosX(void);
    float GetChebdpPosZ(void);

    //to camera interface realize:
    void sample_add_image(unsigned char* p,unsigned int len,string format);

    //to control interface realize:
    float GetXc(void);
    int SendControlValue(int motnum,float target_vx);
    void ControlFinished(void);
    void SetTargetValue(float vx, float vl);

    //to RL interface realize:
    std::vector<float> RL_GetData(void);
    std::vector<float> RL_SetVel(std::vector<float> vel);
    std::vector<float> RL_Reset(void);
    std::vector<float> RL_SetPos(std::vector<float> pos);
};

#endif // ILOGIC_H
