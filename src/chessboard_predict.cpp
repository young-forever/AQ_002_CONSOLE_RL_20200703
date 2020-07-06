//
// Created by wangjiantao on 20-6-2.
//

#include "chessboard_predict.h"
#include <unistd.h>
#include <thread>
#include <string>
#include <time.h>

#include <sys/time.h>
struct timeval tpstart,tpend;
double timeuse;


namespace MatrixRobot {
    ChessboardPredict::ChessboardPredict() {
        /// default value, must change , different camera has different value
        camera_distortion_coefficients_ = cv::Mat(1, 5, CV_32FC1);
        camera_distortion_coefficients_.at<float>(0,0) = 0;
        camera_distortion_coefficients_.at<float>(0,1) = 0;
        camera_distortion_coefficients_.at<float>(0,2) = 0;
        camera_distortion_coefficients_.at<float>(0,3) = 0;
        camera_distortion_coefficients_.at<float>(0,4) = 0;

        camera_intrinsic_matrix_ = cv::Mat(3, 3, CV_32FC1);
        float f_x = 1009.8326; // Focal length in x axis
        float f_y = 1010.2724; // Focal length in y axis (usually the same?)
        float c_x = 633.09295; // Camera primary point x
        float c_y = 361.32333; // Camera primary point y

        camera_intrinsic_matrix_.at<float>(0, 0) = f_x;
        camera_intrinsic_matrix_.at<float>(0, 1) = 0.0;
        camera_intrinsic_matrix_.at<float>(0, 2) = c_x;
        camera_intrinsic_matrix_.at<float>(1, 0) = 0.0;
        camera_intrinsic_matrix_.at<float>(1, 1) = f_y;
        camera_intrinsic_matrix_.at<float>(1, 2) = c_y;
        camera_intrinsic_matrix_.at<float>(2, 0) = 0.0;
        camera_intrinsic_matrix_.at<float>(2, 1) = 0.0;
        camera_intrinsic_matrix_.at<float>(2, 2) = 1.0;

        set_chessboard_parameters(3, 3, 0.0086);

        set_camera_distortion_coefficients(-1.1593606025999008e-01, -5.3122243060847052e-01,
                                           1.1901540027947982e-04, -6.4743941582823185e-04,
                                           6.8388455295807686e+00);

        set_camera_intrinsic_matrix_fxcxfycy(7.3503309133272805e+03, 1.2741689215216645e+03, 7.3503309133272805e+03, 9.8817511854682357e+02);
        set_world_frame_points();

        //start thread:
        std::thread t(std::mem_fn(&MatrixRobot::ChessboardPredict::run),this);
        t.detach();
    }

    int ChessboardPredict::set_chessboard_parameters(int chessboard_width,  int chessboard_height, double chessboard_length) {
        chessboard_width_ = chessboard_width;
        chessboard_height_ = chessboard_height;
        chessboard_length_ = chessboard_length;
        //// find corners, in gray img
        pattern_size.width = chessboard_width_;
        pattern_size.height = chessboard_height_;
        find_corner_criteria = cv::CALIB_CB_ADAPTIVE_THRESH
                               + cv::CALIB_CB_NORMALIZE_IMAGE
                               + cv::CALIB_CB_FAST_CHECK;
        return 0;
    }

    int ChessboardPredict::set_camera_distortion_coefficients(float c1, float c2, float c3, float c4, float c5) {
        camera_distortion_coefficients_.at<float>(0,0) = c1;
        camera_distortion_coefficients_.at<float>(0,1) = c2;
        camera_distortion_coefficients_.at<float>(0,2) = c3;
        camera_distortion_coefficients_.at<float>(0,3) = c4;
        camera_distortion_coefficients_.at<float>(0,4) = c5;
        return 0;
    }

    int ChessboardPredict::set_camera_intrinsic_matrix_fxcxfycy(float fx, float cx, float fy, float cy) {
        camera_intrinsic_matrix_.at<float>(0, 0) = fx;
        camera_intrinsic_matrix_.at<float>(0, 2) = cx;
        camera_intrinsic_matrix_.at<float>(1, 1) = fy;
        camera_intrinsic_matrix_.at<float>(1, 2) = cy;
        return 0;
    }

    int ChessboardPredict::set_world_frame_points() {
        for (int i = 0; i < chessboard_height_; i++)
        {
            for (int j = 0; j < chessboard_width_; j++)
            {
                world_frame_points_.push_back(cv::Point3f(chessboard_length_ * i, chessboard_length_ * j, 0.0));
            }
        }
        return 0;
    }

    int ChessboardPredict::find_chessboard_corners(cv::Mat &rgb_image) {
        cv::Mat gray_image = rgb_image;
        cv::cvtColor(rgb_image, gray_image, CV_BGR2GRAY);


        bool pattern_found = cv::findChessboardCorners(gray_image,
                                                       pattern_size,
                                                       image_frame_points_,
                                                       find_corner_criteria);

        if(pattern_found) { //// find subcorners, in gray img
            cv::TermCriteria subcorner_criteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.001);
            cornerSubPix(gray_image, image_frame_points_, cv::Size(11, 11), cv::Size(-1, -1), subcorner_criteria);

            //// for show and save
//            cv::drawChessboardCorners(rgb_image, pattern_size, cv::Mat(image_frame_points_), pattern_found);
//            cv::imshow("img_show", rgb_image);
//            cv::waitKey(1);
            //cv::imwrite("../imgs/chessboard_detection.png", rgb_image);

//            std::cout << "FOUND chess board corners!" << std::endl;
//            cv::imwrite("chessboard_detection.png", rgb_image);
        }
        else {
            std::cout << "Can't find chess board corners!" << std::endl;
//            cv::imshow("img_show", gray_image);
//            cv::waitKey(1);
            static int cnt=0;
            time_t tt;
            time( &tt );
            tm* t= gmtime( &tt );
            std::string img_name="debug/";
            img_name.append(std::to_string(t->tm_yday+1837));
            img_name.append("-");
            img_name.append(std::to_string(t->tm_mon+1));
            img_name.append("-");
            img_name.append(std::to_string(t->tm_mday));
            img_name.append("-");
            img_name.append(std::to_string(t->tm_hour+8));
            img_name.append(":");
            img_name.append(std::to_string(t->tm_min));
            img_name.append(":");
            img_name.append(std::to_string(t->tm_sec));
            img_name.append("_");
            img_name.append(std::to_string(cnt));
            img_name.append(".png");
            cv::imwrite(img_name.c_str(), gray_image);
//            cv::imwrite("chessboard_detection_not.png", gray_image);
            cnt++;
            return -1;
        }



        return 0;
    }

    int ChessboardPredict::solve_transformation(cv::Mat& rotation_rpy_vector, cv::Mat& translation_xyz_vextor) {
        bool solve_done = cv::solvePnP(world_frame_points_,
                                       image_frame_points_,
                                       camera_intrinsic_matrix_,
                                       camera_distortion_coefficients_,
                                       rotation_rpy_vector,
                                       translation_xyz_vextor,
                                       false,
                                       cv::SOLVEPNP_ITERATIVE);

        if(solve_done)
            return 0;
        else {
            std::cout << "Solve PNP Wrong!" << std::endl;
            return -1;
        }
    }

    int ChessboardPredict::record_states(cv::Mat rgb_image) {
        cv::Mat rvec(3, 1, CV_32F);
        cv::Mat tvec(3, 1, CV_32F);

        int found_flag = find_chessboard_corners(rgb_image);/// find chessboard corners
        if(found_flag==-1)
            return -1;

        //// for the first time, save the corners direction
        int last_point_pos = chessboard_height_ * chessboard_width_ - 1;
        chessboard_x_sign_ = image_frame_points_[0].x - image_frame_points_[last_point_pos].x;
        chessboard_y_sign_ = image_frame_points_[0].y - image_frame_points_[last_point_pos].y;

        int solve_flag = solve_transformation(rvec, tvec);   /// solve pnp
        if(solve_flag == -1)
            return -1;

        /// record start pos
        current_x_pos_ = tvec.at<double>(1)/compensate_tehta_;
        current_z_height_ = tvec.at<double>(2);

        return 0;
    }

    int ChessboardPredict::record_states(std::vector<unsigned char> ucImg) {
//        cv::Mat rgb_image = cv::Mat(img_height_, img_width_, CV_8UC1);// = cv::Mat(2048, 2448, cv::CV_8UC1, ucImg, 0);	// unsigned char*  => Mat
        cv::Mat rgb_image = cv::imdecode(ucImg,CV_LOAD_IMAGE_COLOR);// = cv::Mat(2048, 2448, cv::CV_8UC1, ucImg, 0);	// unsigned char*  => Mat
//        memcpy(rgb_image.data, ucImg,  img_height_ * img_width_*4 + 2048);


        cv::Mat rvec(3, 1, CV_32F);
        cv::Mat tvec(3, 1, CV_32F);

        int found_flag = find_chessboard_corners(rgb_image);/// find chessboard corners
        if(found_flag==-1)
            return -1;

        //// for the first time, save the corners direction
        int last_point_pos = chessboard_height_ * chessboard_width_ - 1;
        chessboard_x_sign_ = image_frame_points_[0].x - image_frame_points_[last_point_pos].x;
        chessboard_y_sign_ = image_frame_points_[0].y - image_frame_points_[last_point_pos].y;

        int solve_flag = solve_transformation(rvec, tvec);   /// solve pnp
        if(solve_flag == -1)
            return -1;

//        std::cout<<tvec.at<double>(0)<<", "<<tvec.at<double>(1)<<", "<<tvec.at<double>(2)<<std::endl;

        /// record start pos
        current_x_pos_ = tvec.at<double>(0)/compensate_tehta_;

        current_z_height_ = tvec.at<double>(2);

        return 0;
    }

    float ChessboardPredict::get_current_displace() {
        return -(current_x_pos_ - start_x_pos_);
    }

    float ChessboardPredict::get_current_height() {
        return current_z_height_ + start_z_height_;
    }

    int ChessboardPredict::get_file_names(std::string path,std::vector<std::string>& filenames)
    {
        DIR *pDir;
        struct dirent* ptr;
        if(!(pDir = opendir(path.c_str()))){
            std::cout<<"Folder doesn't Exist!"<<std::endl;
            return -1;
        }
        while((ptr = readdir(pDir))!= 0) {
            if (std::strcmp(ptr->d_name, ".") !=0  && std::strcmp(ptr->d_name, "..") != 0){
                filenames.push_back(path + "/" + ptr->d_name);
            }
        }
        closedir(pDir);

        return  0;
    }

    void ChessboardPredict::setUcimg(std::vector<unsigned char> ucImg)
    {
        ucImg_ = ucImg;
        img_update_flag=1;
    }

    void ChessboardPredict::run()
    {
//        cv::namedWindow("img_show", 0);
//        cv::resizeWindow("img_show", 1280,720);
        while(1)
        {
            if(img_update_flag==1)
            {
//                std::cout<<"111"<<std::endl;
                img_update_flag=0;

//                cv::Mat test_img_start = cv::imread("ai_serve/image.jpeg", cv::IMREAD_COLOR);
//                record_states(test_img_start);
                record_states(ucImg_);

                usleep(200);
            }

            if(0)
            {
                break;
            }
        }
    }
}

