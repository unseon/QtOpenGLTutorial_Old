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

app_launch_images.files += $$PWD/assets/monkey.fbx
app_launch_images.files += $$PWD/images/house01_org.png

QMAKE_BUNDLE_DATA += app_launch_images

macx: LIBS += -L$$PWD/3rdparty/fbxsdk/lib/clang/release/ -lfbxsdk

INCLUDEPATH += $$PWD/3rdparty/fbxsdk/include
DEPENDPATH += $$PWD/3rdparty/fbxsdk/include
