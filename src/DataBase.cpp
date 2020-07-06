//
// Created by zyq on 2020/6/9.
//

#include "DataBase.h"
DataBase::DataBase()
{
    x = 0.0;
    dx = 0.0;
    theta = 0.0;
    dtheta =0.0;
    l=0.0;
    dl=0.0;
    WorkState = 0;
    motmod = 0;

    vx = 0.0;
    vl = 0.0;

//    rec_sensor_flag = false;

    state_cbm_flag = false;//command back mode flag;
    state_rm_flag = false;//request mpode flag;
    state_cmr_flag = false;//camera open flag;
//    state_rewatt = 0;//respond waiting time,ms.

//    cmrmod = 0x00;
//    cmrfmat="jpeg";
//    smpmod = 0x00;
//    smpdir = "sample/default";
//    Lc = 1000;
//    motnum =0;
//    flag = false;
//
//    xd = 0;
//    ld = 300;
//    veld = 0;
//    td = 10;
//    ctrkp = 7;
//    ctrki = 0;
//    ctrkd = 10;
//    drphet = 800;
//    drpflag = false;
//    zvmod = 0x00;
//
//    cmr_sampletime = 10;
//    motposition = 0.0;
//    motvelocity = 0.0;
//    motcurrent = 0.0;
//    drvkp = 0.0;
//    drvki = 0.0;
//    drvkd = 0.0;
}

