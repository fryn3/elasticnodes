QT += widgets

HEADERS += \
        edge.h \
        node.h \
        graphwidget.h \
    win.h

SOURCES += \
        edge.cpp \
        main.cpp \
        node.cpp \
        graphwidget.cpp \
    win.cpp

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/elasticnodes
#INSTALLS += target
