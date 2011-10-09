TEMPLATE = app
include(app_common.pri)
DESTDIR = $${QDATASERVER_BINS_DIR}
TARGET = APP
DEFINES += APP_BINARY
QT += network

include(qtsingleapplication/qtsingleapplication.pri)

SOURCES += \
    main.cpp





