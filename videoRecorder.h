#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <string>
#include <sys/stat.h>

#include <opencv/highgui.h>
#include <boost/thread.hpp>
#include <mutex>
#include <chrono>
#include <math.h>
#include <vector>

class videoRecorder{
private:
    std::string path_;
    std::string processPath(std::string path);
    void processDirectory(std::string path);

    cv::VideoCapture cam_;
    cv::VideoWriter recorder_;
    cv::Mat camFrame_;
    int frameW_;
    int frameH_;
    double fps_;
    std::string fourCC_;

    std::mutex mtxCycle_;
    boost::thread camThread_;
    void cameraLoop();
    bool fCamOpen_;
    bool fClose_;
    bool fRecord_;
    bool fShow_;
    char inputKey_;

    struct textPropStruct{
        std::string text;
        double x;
        double y;
        int lw;
        cv::Scalar bgr;
        double scale;
        int font;
        std::chrono::high_resolution_clock::time_point t0;
        double timeout;
    };
//    textPropStruct textProp_;
    std::vector<textPropStruct> textPropBuf_;
    void addText2Frame();
    int searchTextPropIndFromXY(double x,double y);

public:
    videoRecorder();
    videoRecorder(std::string videoPath,int camNum,
                  double fps=-1, std::string fourCC="MJPG", int w=-1, int h=-1);
    ~videoRecorder();

    void setup(std::string videoPath,int camNum,
               double fps=-1, std::string fourCC="MJPG", int w=-1, int h=-1);

    void addText(std::string text, double x,double y, double timeout=5,int lw=1,
                 cv::Scalar bgr=cv::Scalar(0,0,0),double scale=1,
                 int font= cv::FONT_HERSHEY_PLAIN);
    void clearText();

    bool startRecord();
    bool pauseRecord();
    bool stopRecord();
    void show(bool fshow=1);
    cv::Mat getCamFrame();

    void closeCam();

    bool mvVideoTo(std::string mvPath);

};

#endif // VIDEORECORDER_H
