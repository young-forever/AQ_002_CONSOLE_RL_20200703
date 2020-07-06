//
// Created by zyq on 2020/6/9.
//
#ifndef AQ_002_CONSOLE_DATABASE_H
#define AQ_002_CONSOLE_DATABASE_H

#include <iostream>

class DataBase {
public:
    //serial recieve data:
    //only comm module can modify.
    float x;
    float dx;
    float theta;
    float dtheta;
    float l;
    float dl;
    unsigned char WorkState;
    unsigned char motmod;

    // control value:
    //only control module can modify.
    float vx;
    float vl;

    //system mode:
    //only logic can modify.
    bool state_cbm_flag;//command back mode flag;
    bool state_rm_flag;//request mpode flag;
    bool state_cmr_flag;//camera open flag;

//    int rewatt;//respond waiting time,ms.

public:
    DataBase();
    ~DataBase(){};

};


#endif //AQ_002_CONSOLE_DATABASE_H
