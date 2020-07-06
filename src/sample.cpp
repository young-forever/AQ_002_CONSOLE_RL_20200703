//
// Created by zyq on 2020/6/12.
//

#include "sample.h"
#include <sstream>
#include <fstream>
#include <memory.h>
#include <sys/stat.h>

sample::sample()
{
    m_queue_camera = new Circle_Queue<ImageInfo*>(1000);

    int len = m_queue_camera->GetQueneFullLength();
    str_buffer_len = len;
    label_buffer = new string[str_buffer_len];

    for(int i=0;i<len;i++)
    {
        image_buffer[i] = (unsigned char*)malloc(2448*2048*4+2048);
    }
    smpenflag = false;
    label_cnt = 0;
    image_cnt = 0;

    save_dir = "sample/default";
}

sample::~sample()
{

}

void sample::set_save_dir(string dir)
{
    save_dir = dir;

    //creat sample folder
    int isCreate = mkdir(save_dir.c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if( !isCreate )
        std::cout<<"create path:"<<save_dir.c_str()<<std::endl;
    else
        std::cout<<"create path failed! error code : "<<save_dir.c_str()<<std::endl;
}

bool sample::add_label(string label)
{

    if(!smpenflag || label_cnt>=str_buffer_len) { return false; }
    label_buffer[label_cnt] = label;

    label_cnt++;
    return true;
}

void sample::add_image(unsigned char* p,unsigned int len,string format)
{
    if(!smpenflag)
        return;

    cout<<"add_image in."<<endl;

    int i = m_queue_camera->get_tail();
    memcpy(image_buffer[i],p,len);
    ImageInfo* image_ = new ImageInfo;
    image_->image_p=image_buffer[i];
    image_->len = len;
    image_->name = to_string(image_cnt);
    image_cnt++;
    image_->format = format;

    m_mutex->lock();
    m_queue_camera->EnQueue(image_);
    m_mutex->unlock();
    cout<<"add_image out."<<endl;
}

void sample::run()
{
    if(!smpenflag)
        return;

    string image_dir;
    ImageInfo* image_;

    do {
        m_mutex->lock();

        bool ret = m_queue_camera->DeQueue(image_);
        if(ret)
        {
            cout<<"111"<<endl;
            /*********  save images  *********/
            image_dir = save_dir;
            image_dir.append("/");
            image_dir.append(image_->name);
            image_dir.append(".");
            image_dir.append(image_->format);

            FILE *fp = fopen(image_dir.c_str(), "wb");
            if (NULL == fp) {
                printf("fopen failed\n");
                break;
            }
            fwrite(image_->image_p, 1, image_->len, fp);
            fclose(fp);
            delete(image_);
            image_= nullptr;
//            printf("save image succeed\n");
        }

        if(ThreadStopFlag)
        {
            cout<<"sample return!"<<endl;
            m_mutex->unlock();
            return;
        }
        m_mutex->unlock();

    } while (1);

}

void sample::startsample()
{
    smpenflag = true;
    label_cnt = 0;
    image_cnt = 0;
    m_queue_camera->ClearQueue();
    start();

    std::cout<<"sample start!"<<std::endl;
}

void sample::stopsample()
{
    smpenflag = false;
    string csv_dir;
    csv_dir = save_dir;
    csv_dir.append("/");
    csv_dir.append("label.csv");

    ofstream outfile(csv_dir.c_str(),ios::app);

    // print title:
    outfile << "image,x(mm),l(mm),theta(deg),dx(mm/s),dl(mm/s),dtheta(deg/s),Vx(mm/s),Vl(mm/s)" << "\n";

    //print data:
    for(int i=0; i<label_cnt;i++)
    {
        outfile <<i+1<<","<<label_buffer[i] << "\n";
    }
    outfile.close();

    stop();

    label_cnt = 0;
    std::cout<<"sample stop!"<<std::endl;
}

