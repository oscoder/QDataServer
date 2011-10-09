TEMPLATE = lib
TARGET = Utils
DEFINES += UTILS_LIBRARY

#include(../../uitools.shared/uitools_library.pri)
#include(utils_dependencies.pri)

HEADERS += utils_global.h \
    toolbutton.h \
    splashscreen.h \
    iprogressmonitor.h

HEADERS += aggregation11.h

HEADERS += configuration.h configuration_p.h
SOURCES += configuration.cpp \
    toolbutton.cpp \
    splashscreen.cpp

HEADERS += debugger.h
SOURCES += debugger.cpp

HEADERS += dependencygraph.h

HEADERS += uniqueid.h
SOURCES += uniqueid.cpp

HEADERS += filehelper.h
SOURCES += filehelper.cpp

HEADERS += stylesheetloader.h
SOURCES += stylesheetloader.cpp

HEADERS += filenamedelegate.h
SOURCES += filenamedelegate.cpp

HEADERS += filesystemwatcher.h
SOURCES += filesystemwatcher.cpp

HEADERS += pimpl.h

HEADERS += tracefn.h
