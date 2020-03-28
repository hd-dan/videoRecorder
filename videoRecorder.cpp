#include "videoRecorder.h"

videoRecorder::videoRecorder():frameW_(-1),frameH_(-1),fps_(-1),
                                fCamOpen_(false),fClose_(false),
                                fRecord_(false),fShow_(false),{

}
videoRecorder::videoRecorder(std::string videoPath,int camNum,
                             double fps, std::string fourCC, int w,int h):
                                frameW_(w),frameH_(h),fps_(fps),
                                fCamOpen_(false),fClose_(false),
                                fRecord_(false),fShow_(false),{
    videoRecorder::setup(videoPath,camNum,fps,fourCC,w,h);
}

videoRecorder::~videoRecorder(){
    videoRecorder::closeCam();
}

void videoRecorder::setup(std::string videoPath, int camNum, double fps, std::string fourCC, int w, int h){
    path_= videoRecorder::processPath(videoPath);
    videoRecorder::processDirectory(path_);
    fps_= fps;

    cam_= cv::VideoCapture(camNum);
    if (!cam_.isOpened()){
        fCamOpen_= false;
        printf("Camera not opened.\n");
        return;
    }
    frameW_= (w==-1)? int(cam_.get(3)):w;
    frameH_= (h==-1)? int(cam_.get(4)):h;
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

    recorder_= cv::VideoWriter(path_,CV_FOURCC(fourCC.at(0),fourCC.at(1),
                                                   fourCC.at(2),fourCC.at(3)),
                               fps_,cv::Size(frameW_,frameH_));

    camThread_= boost::thread(&videoRecorder::cameraLoop,this);
}

void videoRecorder::cameraLoop(){
    fClose_=false;
    while(!fClose_){
        if (fCamOpen_)
            cam_.read(camFrame_);

        if (camFrame_.empty())
            continue;

        videoRecorder::addText2Frame();

        std::unique_lock<std::mutex> lockRecord(mtxCycle_);
        if (fRecord_)
            recorder_.write(camFrame_);
        if (fShow_){
            cv::imshow("Camera",camFrame_);
            inputKey_= char(cv::waitKey(1));
        }
        lockRecord.unlock();
        boost::this_thread::interruption_point();
    }
}

void videoRecorder::addText2Frame(){

    for (unsigned int i=0;i<textPropBuf_.size();i++){
        textPropStruct textProp= textPropBuf_.at(i);
//        cv::putText(img, text, coordinate, font, scale, bgr);
        cv::putText(camFrame_,textProp.text,
                    cv::Point(int(frameW_*textProp.x),int(frameH_*textProp.y)),
                    textProp.font,textProp.scale,textProp.bgr,textProp.lw);

        double dt= std::chrono::duration_cast<std::chrono::duration<double> >
                (std::chrono::high_resolution_clock::now()-textProp.t0).count();
        if (dt>textProp.timeout){
            textPropBuf_.erase(textPropBuf_.begin()+i);
            i--;
        }
    }
    return;
}

void videoRecorder::addText(std::string text, double x,double y, double timeout, int lw,
                            cv::Scalar bgr, double scale, int font){
    x= (x>1)? x/frameW_:x;
    y= (y>1)? y/frameH_:y;
    lw= (lw<0)? 1:lw;
    scale= (scale<0)? 1:scale;
    std::chrono::high_resolution_clock::time_point t0= std::chrono::high_resolution_clock::now();
    textPropStruct textProp= {text, x, y, lw, bgr, scale, font,t0,timeout};
    int i=videoRecorder::searchTextPropIndFromXY(x,y);
    if (i==-1)
        textPropBuf_.push_back(textProp);
    else
        textPropBuf_.at(unsigned(i))=textProp;
    return;
}

int videoRecorder::searchTextPropIndFromXY(double x, double y){
    for (unsigned int i=0;i<textPropBuf_.size();i++){
        if (fabs(x-textPropBuf_.at(i).x)<1e-3 && fabs(y-textPropBuf_.at(i).y)<1e-3)
            return int(i);
    }
    return -1;
}

void videoRecorder::clearText(){
    textPropBuf_.clear();
    return;
}


void videoRecorder::startRecord(){
    if (fRecord_==true)
        return;
    std::unique_lock<std::mutex> lockRecord(mtxCycle_);
    fRecord_= true;
//    if (fRecord_) printf("Recording Cam\n");
    return;
}
void videoRecorder::stopRecord(){
    if (fRecord_==false)
        return;
    std::unique_lock<std::mutex> lockRecord(mtxCycle_);
    fRecord_= false;
    return;
}

void videoRecorder::show(bool fshow){
    if (fShow_==fshow)
        return;
    std::unique_lock<std::mutex> lockRecord(mtxCycle_);
    fShow_= fshow;
//    if (fShow_) printf("Showing Cam\n");
    return;
}

cv::Mat videoRecorder::getCamFrame(){
    return camFrame_;
}

std::string videoRecorder::processPath(std::string path){
    if (path.at(0)=='~')
        path= getenv("HOME")+path.substr(1);
    if (path.find("..")==0){
//        std::string pwd= getenv("PWD");
        char tuh[PATH_MAX];
        std::string pwd=getcwd(tuh,sizeof(tuh));
        do{
            pwd= pwd.substr(0,pwd.rfind("/"));
            path= path.substr(path.find("..")+2);
        }while(path.find("..")<2);
        path= pwd+path;

    }else if(path.at(0)=='.')
        path= getenv("PWD")+path.substr(1);
    if (path.find("/")==path.npos)
        path= getenv("PWD")+std::string("/")+path;
    return path;
}
void videoRecorder::processDirectory(std::string dir){
    dir= dir.substr(0,dir.rfind('/'));
    struct stat st;
    if (stat(dir.c_str(),&st)!=0){
        videoRecorder::processDirectory(dir);
        mkdir(dir.c_str(),S_IRWXU);
    }
    return;
}


void videoRecorder::closeCam(){
    if (!fCamOpen_)
        return;
    fClose_=1;
    usleep(1e5);
    camThread_.interrupt();
    camThread_.join();
    cam_.release();
    recorder_.release();
    return;
}
