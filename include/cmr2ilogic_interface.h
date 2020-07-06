//
// Created by zyq on 2020/6/12.
//

#ifndef CAMERATEST_CMR2ILOGIC_H
#define CAMERATEST_CMR2ILOGIC_H
#include <string>


class cmr2ilogic_interface
{
public:
    ~cmr2ilogic_interface(){};

public:
    virtual void setformat(std::string format_)=0;
    virtual void setsavemode(unsigned char mode)=0;
    virtual void camera_satrt(void)=0;
    virtual void camera_stop(void)=0;
    virtual void setCameraID(int CameraID)=0;

};

#endif //CAMERATEST_CMR2ILOGIC_H
