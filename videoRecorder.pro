TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    videoRecorder.h \
    ../util/joystick.h

SOURCES += \
        main.cpp \
    videoRecorder.cpp \
    ../util/joystick.cpp


LIBS += -lopencv_core -lopencv_highgui -lboost_system -lboost_thread

