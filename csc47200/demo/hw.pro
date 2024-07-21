TEMPLATE = app
TARGET = hw
QT += widgets opengl
OBJECTS_DIR = ./obj
MOC_DIR     = ./moc
RESOURCES   = hw.qrc

win32-msvc2013 {
	LIBS += -lopengl32 -lglu32
	QMAKE_CXXFLAGS += /MP /Zi
}


macx{
        QMAKE_MAC_SDK = macosx10.11
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

unix:!macx {
	CONFIG += C++11
}

# Input
HEADERS +=	MainWindow.h	\
                HW.h		\
                Threshold.h \
                Btct.h \
                Gamma.h \
                Quantization.h \
                Histrogramstrech.h \
                Blur.h \
                Sharpen.h \
                Median.h \
                Convolution.h \
                Correlation.h
		
SOURCES +=	main.cpp	\ 
		MainWindow.cpp 	\
                HW.cpp		\
                Threshold.cpp \
                Btct.cpp \
                Gamma.cpp \
                Quantization.cpp \
                Histrogramstrech.cpp \
                Blur.cpp \
                Sharpen.cpp \
                Median.cpp \
                Convolution.cpp \
                Correlation.cpp

DISTFILES +=
