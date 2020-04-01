TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
#CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

CONFIG += qt

HEADERS += \
    videoRecorder.h \
    ../util/joystick.h

SOURCES += \
        main.cpp \
    videoRecorder.cpp \
    ../util/joystick.cpp


LIBS += -lopencv_core -lopencv_highgui -lboost_system -lboost_thread -pthread

