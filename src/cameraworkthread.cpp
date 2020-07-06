#include "cameraworkthread.h"
#include <memory>
#include <X11/Xlib.h>
#include <string.h>
#include <stdlib.h>

#define NIL (0)

using namespace std;

CameraWorkThread::CameraWorkThread()
{
    savemod = 0x00;
    format = "jpeg";
    CAMERA_INDEX=0;
}

CameraWorkThread::~CameraWorkThread()
{

}
void CameraWorkThread::set_ilogic_interface(iLogic2cmr_interface* ilogic)
{
    m_ilogic = ilogic;
}

void CameraWorkThread::setformat(string format_)
{
    format = format_;
}

void CameraWorkThread::setsavemode(unsigned char mode)
{
    savemod = mode;
}

void PressEnterToExit(void)
{
    int c;
    while ( (c = getchar()) != '\n' && c != EOF );
    fprintf( stderr, "\nPress enter to exit.\n");
    while( getchar() != '\n');
}


bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }

    return true;
}

//unsigned char *pDataForSaveImage = NULL;
//该线程执行  任务
void CameraWorkThread::run()
{
    int camera_cnt=0;

    Window w;
    Display *dpy;

    memset(&w, 0, sizeof(Window));
    dpy    = NULL;

    // 打开连接到X服务器的连接
    // open the connection to the display 0
    dpy = XOpenDisplay(NIL);

    if (NULL == dpy)
    {
        printf("please run with screan environment\n");
        return;
    }

    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

    w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                            752, 480, 0, 0xffff00ff, 0xff00ffff);

    // 获取改变窗口大小事件
    // we want to get MapNotify events
    XSelectInput(dpy, w, StructureNotifyMask);

    // 使窗口可见
    // "Map" the window (that is, make it appear on the screen)
    XMapWindow(dpy, w);

    // 创建图像上下文给出绘图函数的属性
    // Create a "Graphics Context"
    GC gc = XCreateGC(dpy, w, 0, NIL);

    // 告诉GC使用白色
    // Tell the GC we draw using the white color
    XSetForeground(dpy, gc, whiteColor);

    // 等待事件的到来
    // Wait for the MapNotify event
    for(;;)
    {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }

    int nRet = MV_OK;
    void* handle = NULL;
    //    unsigned char * pData = NULL;
    //    unsigned char *pDataForRGB = NULL;
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    do
    {
        // 枚举设备
        // enum device
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
            stop();//stop my thread.
            break;
        }
        if (stDeviceList.nDeviceNum > 0)
        {
            for (int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                printf("[device %d]:\n", i);
                MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (NULL == pDeviceInfo)
                {
                    break;
                }
                PrintDeviceInfo(pDeviceInfo);
            }
        }
        else
        {
            printf("Find No Devices!\n");
            stop();
            break;
        }

        //    printf("Please Intput camera index: ");
        unsigned int nIndex = CAMERA_INDEX;
        //    scanf("%d", &nIndex);

        if (nIndex >= stDeviceList.nDeviceNum)
        {
            printf("Intput error!\n");
            stop();
            break;
        }

        // 选择设备并创建句柄
        // select device and create handle
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
        if (MV_OK != nRet)
        {
            printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        // 打开设备
        // open device
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
        if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
        {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            if (nPacketSize > 0)
            {
                nRet = MV_CC_SetIntValue(handle,"GevSCPSPacketSize",nPacketSize);
                if(nRet != MV_OK)
                {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!\n", nRet);
                }
            }
            else
            {
                printf("Warning: Get Packet Size fail nRet [0x%x]!\n", nPacketSize);
            }
        }

        nRet = MV_CC_SetBoolValue(handle, "AcquisitionFrameRateEnable", false);
        if (MV_OK != nRet)
        {
            printf("set AcquisitionFrameRateEnable fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        // 设置触发模式为on
        // set trigger mode as on
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 1);
        if (MV_OK != nRet)
        {
            printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        // 设置触发源
        // set trigger source
        //    nRet = MV_CC_SetEnumValue(handle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);

        nRet = MV_CC_SetEnumValue(handle, "TriggerSource", MV_TRIGGER_SOURCE_LINE0);
        if (MV_OK != nRet)
        {
            printf("MV_CC_SetTriggerSource fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        // 开始取流
        // start grab image
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
            stop();
            break;
        }

        nRet= MV_CC_Display(handle, (void*)w);
        if (MV_OK != nRet)
        {
            printf("MV_CC_Display fail! nRet [%x]\n", nRet);
            break;
        }
        printf("Display succeed\n");

        // ch:获取数据包大小 | en:Get payload size
        // ch:获取数据包大小 | en:Get payload size
        MVCC_INTVALUE stParam;
        memset(&stParam, 0, sizeof(MVCC_INTVALUE));
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
        if (MV_OK != nRet)
        {
            printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
            return;
        }

        MV_FRAME_OUT_INFO_EX stImageInfo = {0};
        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
        unsigned char * pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
        if (NULL == pData)
        {
            return;
        }

        do {
            unsigned int nDataSize = stParam.nCurValue;
            nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 1000);
            if (nRet == MV_OK)
            {
                static  unsigned char*  m_pDataForSaveImage = (unsigned char*)malloc(stImageInfo.nWidth * stImageInfo.nHeight * 4 + 2048);

                if (NULL == m_pDataForSaveImage)
                {
                    break;
                }
                // 填充存图参数
                // fill in the parameters of save image
                MV_SAVE_IMAGE_PARAM_EX stSaveParam;
                memset(&stSaveParam, 0, sizeof(MV_SAVE_IMAGE_PARAM_EX));
                // 从上到下依次是：输出图片格式，输入数据的像素格式，提供的输出缓冲区大小，图像宽，
                // 图像高，输入数据缓存，输出图片缓存，JPG编码质量

                enum MV_SAVE_IAMGE_TYPE FORMAT;
                if(format == "bmp")
                {
                    FORMAT = MV_Image_Bmp;
                }
                else if(format == "jpeg")
                {
                    FORMAT = MV_Image_Jpeg;
                }

                switch(savemod)
                {
                    case NO_SAVE:
                        break;
                    case ONCE_SAVE://save 1 image, bmp;
                    {
                        savemod = 0x00;//reset save mod.
                        stSaveParam.enImageType = FORMAT;
                        stSaveParam.enPixelType = stImageInfo.enPixelType;
                        stSaveParam.nBufferSize = stImageInfo.nWidth * stImageInfo.nHeight * 4 + 2048;
                        stSaveParam.nWidth = stImageInfo.nWidth;
                        stSaveParam.nHeight = stImageInfo.nHeight;
                        stSaveParam.pData = pData;
                        stSaveParam.nDataLen = stImageInfo.nFrameLen;
                        stSaveParam.pImageBuffer = m_pDataForSaveImage;
                        stSaveParam.nJpgQuality = 80;

                        nRet = MV_CC_SaveImageEx2(handle, &stSaveParam);
                        if (MV_OK != nRet) {
                            printf("failed in MV_CC_SaveImage,nRet[%x]\n", nRet);
                            break;
                        }

                        string image_dir;
                        image_dir = "ai_serve/";
                        image_dir.append("image");
                        image_dir.append(".");
                        image_dir.append(format);

                        FILE *fp = fopen(image_dir.c_str(), "wb");

//                        FILE *fp = fopen("ai_serve/image.bmp", "wb");
                        if (NULL == fp) {
                            printf("fopen failed\n");
                            break;
                        }
                        fwrite(m_pDataForSaveImage, 1, stSaveParam.nImageLen, fp);
                        fclose(fp);
                        printf("save image succeed\n");
                        break;
                    }
                    case SAMPLE_SAVE://sample save.
                    {
                        //save iamge.
                        stSaveParam.enImageType = FORMAT;
                        stSaveParam.enPixelType = stImageInfo.enPixelType;
                        stSaveParam.nBufferSize = stImageInfo.nWidth * stImageInfo.nHeight * 4 + 2048;
                        stSaveParam.nWidth = stImageInfo.nWidth;
                        stSaveParam.nHeight = stImageInfo.nHeight;
                        stSaveParam.pData = pData;
                        stSaveParam.nDataLen = stImageInfo.nFrameLen;
                        stSaveParam.pImageBuffer = m_pDataForSaveImage;
                        stSaveParam.nJpgQuality = 80;

                        nRet = MV_CC_SaveImageEx2(handle, &stSaveParam);
                        if (MV_OK != nRet) {
                            printf("failed in MV_CC_SaveImage,nRet[%x]\n", nRet);
                            break;
                        }
//                        cout<<stSaveParam.nImageLen<<endl;
                        m_ilogic->sample_add_image(m_pDataForSaveImage,stSaveParam.nImageLen,format);
                        break;
                    }
                }
            }

            m_mutex->lock();
            if(ThreadStopFlag)
            {
                m_mutex->unlock();
                // 停止取流
                // stop grab image
                nRet = MV_CC_StopGrabbing(handle);
                if (MV_OK != nRet)
                {
                    printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
                    break;
                }

                // 关闭设备
                // close device
                nRet = MV_CC_CloseDevice(handle);
                if (MV_OK != nRet)
                {
                    printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
                    break;
                }

                // 销毁句柄
                // destroy handle
                nRet = MV_CC_DestroyHandle(handle);
                if (MV_OK != nRet)
                {
                    printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
                    break;
                }
                return;
            }
            m_mutex->unlock();

        } while (1);
    }while(0);
}

//void CameraWorkThread::open_iamge_save(void)
//{
//    image_path = 1;
//    save_enable_flag  = true;
//    m_image_save_thread->start();
//}

//void CameraWorkThread::close_image_save(void)
//{
//    image_path = 1;
//    save_enable_flag  = false;
//    m_image_save_thread->stopImmediately();
//}
//
//void CameraWorkThread::set_save_dir(string dir)
//{
//    m_image_save_thread->set_save_dir(dir);
//}
