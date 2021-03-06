QT += qml quick multimedia gui
!no_desktop: QT += widgets

QAK_VERSION = 1.3.2
include(gitversion.pri)
message("Qak $$QAK_VERSION git $$QAK_GIT_VERSION/$$QAK_GIT_BRANCH_NAME")

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD

!contains(QAK_CONFIG,"noautoregister") {
    DEFINES += QAK_AUTO_REGISTER
    HEADERS += $$PWD/qak.h
    SOURCES += $$PWD/qak.cpp
}

contains(QAK_CONFIG,"nowarnings") {
    DEFINES += QAK_NO_WARNINGS
    message("Qak supressing warnings")
}

INCLUDEPATH += \
    $$PWD \
    $$PWD/src

HEADERS += \
    $$PWD/src/aid_p.h \
    $$PWD/src/maskedmousearea.h \
    $$PWD/src/resource.h \
    $$PWD/src/store.h \
    $$PWD/src/propertytoggle.h \
    $$PWD/src/mouserotate_p.h \
    $$PWD/src/itemanimation_p.h \
    $$PWD/src/shutdowncheck.h \
    $$PWD/src/env_p.h \
    \

SOURCES += \
    $$PWD/src/aid_p.cpp \
    $$PWD/src/maskedmousearea.cpp \
    $$PWD/src/resource.cpp \
    $$PWD/src/store.cpp \
    $$PWD/src/propertytoggle.cpp \
    $$PWD/src/mouserotate_p.cpp \
    $$PWD/src/itemanimation_p.cpp \
    $$PWD/src/shutdowncheck.cpp \
    $$PWD/src/env_p.cpp \
    \

RESOURCES += \
    $$PWD/qak.qrc

DISTFILES += \
    $$PWD/README.md \
    $$PWD/LICENSE
