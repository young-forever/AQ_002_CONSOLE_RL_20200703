//
// Created by zyq on 2020/6/5.
//

#include "m_thread.h"

m_thread::m_thread()
{
    ThreadStopFlag=false;
    m_mutex = new std::mutex();
}

void m_thread::start(void)
{
    ThreadStopFlag = false;
    mxt_thread = new std::thread(&m_thread::run,this);
}

void m_thread::stop(void)
{
    m_mutex->lock();
    ThreadStopFlag = true;
    m_mutex->unlock();
}