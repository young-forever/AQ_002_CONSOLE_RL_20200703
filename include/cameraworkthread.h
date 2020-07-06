#ifndef CAMERAWORKTHREAD_H
#define CAMERAWORKTHREAD_H
//#include <QThread>
//#include <QMutex>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include "MvCameraControl.h"

//
#include "m_thread.h"
#include "cmr2ilogic_interface.h"
#include "iLogic2cmr_interface.h"

//#define CAMERA_INDEX 0

//camera save mode.
#define NO_SAVE         0X00
#define ONCE_SAVE       0X01
#define SAMPLE_SAVE     0X02

using namespace std;

class CameraWorkThread: public cmr2ilogic_interface, public m_thread
{
    iLogic2cmr_interface* m_ilogic;

    unsigned  char savemod;
    string format;
    unsigned int CAMERA_INDEX;

public:
    CameraWorkThread();
    ~CameraWorkThread();

    void set_ilogic_interface(iLogic2cmr_interface* ilogic);

    void run();

    // self interface
    void setCameraID(int CameraID){CAMERA_INDEX = CameraID;};
    void setformat(string format_);
    void setsavemode(unsigned char mode);
    void camera_satrt(void){start();};
    void camera_stop(void){stop();};

};

#endif // CAMERAWORKTHREAD_H
