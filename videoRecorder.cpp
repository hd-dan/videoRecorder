#include "videoRecorder.h"

videoRecorder::videoRecorder(std::string videoPath,int camNum,
                             double fps, std::string fourCC, int w,int h):
                                frameW_(w),frameH_(h),fps_(fps),
                                fClose_(0),fRecord_(0),fShow_(0),
                                fFrameAddText_(0),textTimeout_(1){
    savePath_= videoRecorder::checkPath(videoPath);

    cam_= cv::VideoCapture(camNum);
    if (!cam_.isOpened()){
        printf("Camera not opened. Quit Now\n");
        exit(1);
    }
    frameW_= (frameW_==-1)? int(cam_.get(3)):frameW_;
    frameH_= (frameH_==-1)? int(cam_.get(4)):frameH_;
    cam_.read(camFrame_);

    if (int(fps_)==-1){
        printf("Detecting framerate..\n");
        std::chrono::high_resolution_clock::time_point t0,t1;
        t0=std::chrono::high_resolution_clock::now();
        double n=50;
        for (int i=0;i<n;i++){
            cam_.read(camFrame_);
        }
        t1=std::chrono::high_resolution_clock::now();
        double t=std::chrono::duration_cast<std::chrono::duration<double> >(t1-t0).count();
        fps_= n/t;
        printf("Detected fps: %.2f\n",fps_);
    }

    recorder_= cv::VideoWriter(savePath_,CV_FOURCC(fourCC.at(0),fourCC.at(1),
                                                   fourCC.at(2),fourCC.at(3)),
                               fps_,cv::Size(frameW_,frameH_));

    camThread_= boost::thread(&videoRecorder::cameraLoop,this);
}

videoRecorder::~videoRecorder(){
    videoRecorder::closeCam();
}

void videoRecorder::cameraLoop(){
    while(!fClose_){
        cam_.read(camFrame_);

        if (camFrame_.empty()){
            printf("Cannot Receive Camera Frame\n");
            continue;
        }
        if (fFrameAddText_)
            videoRecorder::addText2Frame();

        if (fRecord_)
            recorder_.write(camFrame_);
        if (fShow_){
            cv::imshow("Camera",camFrame_);
            inputKey_= char(cv::waitKey(1));
        }
        boost::this_thread::interruption_point();
    }
}

void videoRecorder::addText2Frame(){
    if (fFrameAddText_){
//        cv::putText(img, text, coordinate, font, scale, bgr);
        cv::putText(camFrame_,textProp_.text,
                    cv::Point(int(frameW_*textProp_.x),int(frameH_*textProp_.y)),
                    textProp_.font,textProp_.scale,textProp_.bgr,textProp_.lw);
    }

    double dt= std::chrono::duration_cast<std::chrono::duration<double> >
            (std::chrono::high_resolution_clock::now()-text_t0_).count();
    if (dt>textTimeout_){
        fFrameAddText_=0;
        return;
    }
    return;
}

void videoRecorder::addText(std::string text, double x,double y, int lw,
                            cv::Scalar bgr, double scale, int font){
    fFrameAddText_=1;
    x= (x>1)? x/frameW_:x;
    y= (y>1)? y/frameH_:y;
    lw= (lw<0)? 1:lw;
    scale= (scale<0)? 1:scale;
    textProp_= {text, x, y, lw, bgr, scale, font};
    text_t0_= std::chrono::high_resolution_clock::now();
    return;
}

void videoRecorder::setTextTimeout(double textTimeout){
    textTimeout_= textTimeout;
    return;
}


void videoRecorder::record(bool frecord){
    if (fRecord_!=frecord){
        fRecord_= frecord;
        if (fRecord_) printf("Record Cam Now\n");
        else printf("Stop Record\n");
    }
    return;
}

void videoRecorder::show(bool fshow){
    if (fShow_!=fshow){
        fShow_= fshow;
        if (fShow_) printf("Show Cam Now\n");
        else printf("Stop show cam\n");
    }
    return;
}

cv::Mat videoRecorder::getCamFrame(){
    return camFrame_;
}

std::string videoRecorder::checkPath(std::string path){
    if (path.at(0)=='~'){
        path.erase(0,1);
        path= std::string("/home/")+getenv("LOGNAME")+path;
    }

    std::string dir= path.substr(0,path.find_last_of('/'));
    struct stat st;
    if (stat(dir.c_str(),&st)!=0){
        mkdir(dir.c_str(),S_IRWXU);
    }
    return path;
}

void videoRecorder::closeCam(){
    fClose_=1;
    usleep(1e5);
    camThread_.interrupt();
    camThread_.join();
    cam_.release();
    recorder_.release();
}
