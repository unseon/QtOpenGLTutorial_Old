TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    rendersurface.cpp \
    node.cpp \
    mesh.cpp \
    material.cpp \
    scene.cpp \
    fbxparser.cpp \
    directionallight.cpp

RESOURCES += qml.qrc \
    textures.qrc \
    shaders.qrc \
    assets.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    rendersurface.h \
    node.h \
    mesh.h \
    material.h \
    scene.h \
    fbxparser.h \
    directionallight.h

DISTFILES +=

app_launch_images.files += $$PWD/assets/plane.fbx
app_launch_images.files += $$PWD/assets/monkey.fbx
app_launch_images.files += $$PWD/assets/cube.fbx
app_launch_images.files += $$PWD/assets/solid_cube.fbx
app_launch_images.files += $$PWD/assets/objects.fbx
app_launch_images.files += $$PWD/assets/objects_01.fbx
app_launch_images.files += $$PWD/assets/uvtemplate.png
app_launch_images.files += $$PWD/assets/normalmap.png
app_launch_images.files += $$PWD/assets/house01_org.png

QMAKE_BUNDLE_DATA += app_launch_images

macx: LIBS += -L$$PWD/3rdparty/fbxsdk/lib/clang/release/ -lfbxsdk

INCLUDEPATH += $$PWD/3rdparty/fbxsdk/include
DEPENDPATH += $$PWD/3rdparty/fbxsdk/include
