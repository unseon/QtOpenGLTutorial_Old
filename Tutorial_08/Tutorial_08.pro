TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    rendersurface.cpp

RESOURCES += qml.qrc \
    textures.qrc \
    shaders.qrc \
    assets.qrc

INCLUDEPATH += 3rdparty/assimp/include
LIBS += $$_PRO_FILE_PWD_/3rdparty/assimp/lib/osx/libassimp.dylib

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    rendersurface.h

DISTFILES +=
