//
// Created by wangjiantao on 20-6-2.
//

#ifndef CHESSBOARD_PREDICT_CHESSBOARD_PREDICT_H
#define CHESSBOARD_PREDICT_CHESSBOARD_PREDICT_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <iostream>
#include <sys/types.h>
#include <dirent.h>


namespace MatrixRobot {
    class ChessboardPredict_interface {
    public:
        ChessboardPredict_interface(){};
        ~ChessboardPredict_interface(){};

    public:
        virtual int set_chessboard_parameters(int chessboard_width,  int chessboard_height, double chessboard_length) = 0;
        virtual int set_camera_distortion_coefficients(float c1, float c2, float c3, float c4, float c5) = 0;
        virtual int set_camera_intrinsic_matrix_fxcxfycy(float fx, float cx, float fy, float cy) = 0;
        virtual int set_world_frame_points() =0;
        virtual int record_states(cv::Mat rgb_image) = 0;
        virtual int record_states(std::vector<unsigned char> ucImg) = 0;
        virtual float get_current_displace() = 0;
        virtual float get_current_height() = 0;
        virtual void setUcimg(std::vector<unsigned char> ucImg)=0;
    };

    class ChessboardPredict : public ChessboardPredict_interface
    {
    public:
        ChessboardPredict();
        ~ChessboardPredict(){};

    public:
        int set_chessboard_parameters(int chessboard_width,  int chessboard_height, double chessboard_length);
        int set_camera_distortion_coefficients(float c1, float c2, float c3, float c4, float c5);
        int set_camera_intrinsic_matrix_fxcxfycy(float fx, float cx, float fy, float cy);
        int set_world_frame_points();
        int find_chessboard_corners(cv::Mat& rgb_image);
        int solve_transformation(cv::Mat& rotation_rpy_vector, cv::Mat& translation_xyz_vextor) ;
        int record_states(cv::Mat rgb_image);
        int record_states(std::vector<unsigned char> ucImg);
        float get_current_displace();
        float get_current_height();
        int get_file_names(std::string path,std::vector<std::string>& filenames);
        void setUcimg(std::vector<unsigned char> ucImg);
        void run();

    private:
        int chessboard_width_, chessboard_height_ ;
        double chessboard_length_ ;
        std::vector<cv::Point3f> world_frame_points_;  ///// chessboard corners in the world
        std::vector<cv::Point2f> image_frame_points_;  ///// chessboard corners found in the img
        cv::Mat camera_distortion_coefficients_;   //// camera distortion coefficients
        cv::Mat camera_intrinsic_matrix_;   //// cmaera intrinsic matrix

        //// find corners, in gray img
        cv::Size pattern_size;

        int find_corner_criteria;
        float chessboard_x_sign_ = 0.0;   //// for check the order of chessboard corners
        float chessboard_y_sign_ = 0.0;
        float compensate_tehta_ = 1.0; //// it seems arcsin(dis/length) is proportion to the real result, so add this compensate
        float current_x_pos_ = 0.0;
        float current_z_height_ = 0.0;

        int img_update_flag=0;
        std::vector<unsigned char> ucImg_;
        //set const value
        const float start_x_pos_ = -0.0192;   /////  for save the start pos
        const float start_z_height_ = 0.1033;
        const int img_width_ = 2448;
        const int img_height_ = 2048;
    };
}

#endif //CHESSBOARD_PREDICT_CHESSBOARD_PREDICT_H
