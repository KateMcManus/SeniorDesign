QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    emcsettings.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    emcsettings.h \
    mainwindow.h

FORMS += \
    emcsettings.ui \
    mainwindow.ui

INCLUDEPATH += C:\Users\Kate\Documents\Qt Projects\EMCGUI\include\EMCMessageDefinitions.hpp \
                C:\Users\Kate\Documents\Qt Projects\EMCGUI\include\main.hpp \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
