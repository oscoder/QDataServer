include($$PWD/utils_dependencies.pri)

include($$PWD/../../uitools.shared/uitools_functions.pri)
LIBS *= -l$$qtLibraryTarget(Utils)

INCLUDEPATH *= $$PWD/..
DEPENDPATH  *= $$PWD/..
VPATH       *= $$PWD/..
