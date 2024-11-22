greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    src/gamescene.cpp \
    src/main.cpp \
    src/starsystem.cpp \
    src/view.cpp

HEADERS += \
    src/planet.h \
    src/gamescene.h \
    src/starsystem.h \
    src/utils.h \
    src/view.h

RESOURCES += \
    resource.qrc
