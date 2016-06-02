TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    rendersurface.cpp

RESOURCES += qml.qrc \
    textures.qrc \
    shaders.qrc \
    assets.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    rendersurface.h

DISTFILES +=

app_launch_images.files = $$files($$PWD/assets/cube.fbx)
QMAKE_BUNDLE_DATA += app_launch_images

macx: LIBS += -L$$PWD/3rdparty/fbxsdk/lib/clang/debug/ -lfbxsdk

INCLUDEPATH += $$PWD/3rdparty/fbxsdk/include
DEPENDPATH += $$PWD/3rdparty/fbxsdk/include
