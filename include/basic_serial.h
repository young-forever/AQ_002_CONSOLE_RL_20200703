//
// Created by zyq on 2020/6/9.
//

#ifndef AQ_002_CONSOLE_BASIC_SERIAL_H
#define AQ_002_CONSOLE_BASIC_SERIAL_H
#include "m_thread.h"

class basic_serial : public m_thread
{
private:
    std::string portname;
    int fd;

public:
    std::string CommID;
    basic_serial();
    ~basic_serial();

    void run(void);

    int set_interface_attribs(int fd, unsigned int speed);
    void set_mincount(int fd, int mcount);
    int serial_read(char* buffer,const int len);
    int serial_write(const unsigned char* data,const int len);

    // command excute:
    virtual void command_execute(const unsigned char re_command, const unsigned char* re_data)=0;//指令解析函数
    virtual void long_command_execute(const unsigned char long_re_command,const unsigned char* re_data)=0;//long command analysis.
};


#endif //AQ_002_CONSOLE_BASIC_SERIAL_H
