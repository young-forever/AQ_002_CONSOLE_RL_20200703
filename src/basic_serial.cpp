//
// Created by zyq on 2020/6/9.
//
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

//my head file
#include "basic_serial.h"

basic_serial::basic_serial()
{

}

basic_serial::~basic_serial()
{

}

int basic_serial::set_interface_attribs(int fd, unsigned int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void basic_serial::set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

int basic_serial::serial_read(char* buffer,const int len)
{
    int rdlen;
    rdlen = read(fd, buffer, len);
    return rdlen;
}

int basic_serial::serial_write(const unsigned char* data,const int len)
{
    return write(fd,data,len);
}

std::vector<char> comm_buffer;
//serial thread:
void basic_serial::run(void)
{
    // set serial fd.
    portname = "/dev/tty";
    portname.append(CommID);
    fd = open(portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC );
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname.c_str(), strerror(errno));
        return;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B460800);
    tcdrain(fd);    /* delay for output */


    char data_temp[2] = {0};//暂时只用到一位值
    int rmcnt=0;
    int test = 0;
    do {
        int rdlen;

        rdlen = serial_read(data_temp,1);//read my serial.

        //***send signal to ui.
        if (rdlen > 0) {
            //  emit serial_ok_signal();//
        }
        else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
            stop();
        }

        //*** put data to buffer
        comm_buffer.push_back(data_temp[0]);

        //*** buffer data process.
        while(comm_buffer.size() >= 8)//juge whether the
        {
            //short command analysis.
            if(comm_buffer.front() == 0x56)//short command.
            {
                if(comm_buffer[7] == 0x0D)
                {
                    unsigned char re_command = comm_buffer[1];//功能码
                    unsigned char re_data[5];//指令数据位, 5位Byte
                    std::copy(comm_buffer.begin()+2, comm_buffer.begin()+7, re_data);

                    command_execute(re_command,re_data);//执行指令解析操作
                    comm_buffer.erase(comm_buffer.begin(), comm_buffer.begin() + 8);//移除执行过的数据帧
                }
                else
                {
                    //移除
                    comm_buffer.erase(comm_buffer.begin());
                }
            }
                //** long command analysis.
            else if(comm_buffer.front() == 0x7F)//long command
            {
                if( comm_buffer.size() < 19)
                {
                    break;
                }

                if(comm_buffer[18] == 0x0D)
                {
                    unsigned char long_re_command = comm_buffer[1];//功能码
                    unsigned char long_re_data[16];//指令数据位, 16位Byte
                    std::copy(comm_buffer.begin()+2, comm_buffer.begin()+18, long_re_data);

                    long_command_execute(long_re_command,long_re_data);//执行指令解析操作

                    comm_buffer.erase(comm_buffer.begin(), comm_buffer.begin() + 19);

                    //std::cout<<"receive long command! "<<std::endl;
                }
                else
                {
                    //std::cout<<"remove byte;"<<std::endl;
                    comm_buffer.erase(comm_buffer.begin());
                }
            }
            else
            {
                comm_buffer.erase(comm_buffer.begin());
                rmcnt++;
//                std::cout<<"remove"<< rmcnt <<" byte;"<<std::endl;
            }
        }

        m_mutex->lock();

        if(ThreadStopFlag)
        {
            m_mutex->unlock();
            return;
        }
        m_mutex->unlock();

    }while(1);
}


