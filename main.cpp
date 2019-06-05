#include <iostream>

#include "videoRecorder.h"
#include "../util/joystick.h"

void recordVideo(){
    int camNum=1;
    videoRecorder recorder("~/Desktop/testVideo.avi",camNum);
    joystick js(0);
    js_state state= js.getState();

    bool frecord=0;
    bool fshow=0;
    while(!state.button.at(7)){
        state= js.getState();

        if (state.button.at(0))
            frecord=1;
        else if (state.button.at(1))
            frecord=0;

        if (state.button.at(2))
            fshow=1;
        else if (state.button.at(3))
            fshow=0;

        if (state.button.at(5))
            recorder.addText("haha",0.7,0.5,2,cv::Scalar(100,100,50));

        recorder.record(frecord);
        recorder.show(fshow);
        usleep(1e5);
    }

}

int main(){
    std::cout << "Hello World!" << std::endl;

    recordVideo();

    return 0;
}
