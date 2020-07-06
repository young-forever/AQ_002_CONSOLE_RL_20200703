//
// Created by zyq on 2020/6/5.
//

#ifndef AQ_002_CONSOLE_M_THREAD_H
#define AQ_002_CONSOLE_M_THREAD_H
#include <thread>
#include <mutex>

class m_thread {
private:
    std::thread* mxt_thread;

protected:
    bool ThreadStopFlag;
    std::mutex* m_mutex;

public:
    m_thread();
    ~m_thread(){};

    virtual void run(void){};
    void start(void);
    void stop(void);
};


#endif //AQ_002_CONSOLE_M_THREAD_H
