//
// Created by zyq on 2020/6/12.
//

#ifndef CAMERATEST_IAMGESAVE2CAMERA_INTERFACE_H
#define CAMERATEST_IAMGESAVE2CAMERA_INTERFACE_H

#include <iostream>

using namespace std;

class sample_interface
{
public:
    ~sample_interface(){};

public:
    virtual void set_save_dir(const string dir)=0;
    virtual bool add_label(const string label)=0;

    virtual void add_image(unsigned char* p,unsigned int len,string format)=0;
    virtual void stopsample()=0;
    virtual void startsample()=0;
};




#endif //CAMERATEST_IAMGESAVE2CAMERA_INTERFACE_H
