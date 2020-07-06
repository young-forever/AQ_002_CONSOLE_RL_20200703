//
// Created by zyq on 2020/6/12.
//

#ifndef CAMERATEST_SAMPLE_H
#define CAMERATEST_SAMPLE_H
#include <iostream>
#include "sample_interface.h"
#include "m_thread.h"
#include "circle_queue.h"
using namespace std;

typedef struct image_info
{
    unsigned char* image_p;
    unsigned int len;
    string name;
    string format;
}ImageInfo;

class sample : public sample_interface, public m_thread
{
    bool smpenflag;
    std::__cxx11::string save_dir;
    int str_buffer_len;
    int label_cnt;
    int image_cnt;
    string* label_buffer;

    unsigned char*  image_buffer[1000]; //store image buffer.
    Circle_Queue<ImageInfo*>* m_queue_camera;

public:
    sample();
    ~sample();

    void set_save_dir(const string dir);
    bool add_label(const string label);

    void add_image(unsigned char* p,unsigned int len,string format);

    void startsample();
    void stopsample();

    void run();
};


#endif //CAMERATEST_SAMPLE_H
