#include <iostream>

#include "videoRecorder.h"
#include "../util/joystick.h"

void recordVideo(){
    int camNum=1;
    std::string path= "/home/airportvision/Desktop/testVideo.avi";
    videoRecorder recorder(path,camNum);
    joystick js(0);

    recorder.startRecord();

    bool f1st=1;
    bool fshow=1;
    double t=0;
    while(!js.getState().button.at(7) && t<3){
        js_state state= js.getState();

        recorder.addText("hehe",0.2,0.9,0.5, 0.7,1,cv::Scalar(0,0,0));
        if (t>1)
            recorder.addText("haha",0.1,0.9,0.5, 0.7,3,cv::Scalar(0,0,0));


        if (t>2 && recorder.stopRecord()){
            printf("stopped\n");
            if (recorder.mvVideoTo("~/Desktop/success.avi") )
                printf("\n\n\nRenamed\n\n\n");
//            recorder.startRecord();
            f1st=0;
        }

        printf("t:%.3f\n",t);

//        if (state.button.at(5))
//            recorder.addText("haha",0.7,0.5,2,cv::Scalar(100,100,50));

//        if (state.button.at(0))
//            recorder.startRecord();
//        else if (state.button.at(1))
//            recorder.stopRecord();

//        if (state.button.at(2))
//            fshow=1;
//        else if (state.button.at(3))
//            fshow=0;


        recorder.show(fshow);
        t+=0.1;
        usleep(1e5);
    }
    printf("Done\n");
}

int main(){
    std::cout << "Hello World!" << std::endl;

    recordVideo();

//    std::cout<<cv::getBuildInformation();

    return 0;
}
