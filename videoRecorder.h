#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <string>
#include <sys/stat.h>

#include <opencv/highgui.h>
#include <boost/thread.hpp>
#include <chrono>

class videoRecorder{
private:
    std::string savePath_;
    std::string checkPath(std::string path);

    cv::VideoCapture cam_;
    cv::VideoWriter recorder_;
    cv::Mat camFrame_;
    int frameW_;
    int frameH_;
    double fps_;

    boost::thread camThread_;
    void cameraLoop();
    bool fClose_;
    bool fRecord_;
    bool fShow_;
    char inputKey_;

    bool fFrameAddText_;
    struct{
        std::string text;
        double x;
        double y;
        int lw;
        cv::Scalar bgr;
        double scale;
        int font;
    }textProp_;
    void addText2Frame();
    double textTimeout_;
    std::chrono::high_resolution_clock::time_point text_t0_;

public:
    videoRecorder(std::string videoPath,int cameraNum,
                  double fps=-1, std::string fourCC="MJPG", int w=-1, int h=-1);
    ~videoRecorder();

    void addText(std::string text, double x,double y, int lw=1,
                 cv::Scalar bgr=cv::Scalar(0,0,0),double scale=1,
                 int font= cv::FONT_HERSHEY_PLAIN);
    void setTextTimeout(double textTimeout);
    void record(bool frecord=1);
    void show(bool fshow=1);
    cv::Mat getCamFrame();

    void closeCam();

};

#endif // VIDEORECORDER_H
