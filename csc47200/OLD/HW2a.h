// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW2a.h - Header file for HW2a class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#ifndef HW2A_H
#define HW2A_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class HW2a : public HW {
    Q_OBJECT

    int g_drawModes[9] = {
        GL_POINTS,
        GL_LINES,
        GL_LINE_STRIP,
        GL_LINE_LOOP,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
        GL_QUADS,
        GL_POLYGON
    };

public:
    HW2a            (QWidget *parent = 0);  // Constructor
    QGroupBox*      controlPanel       ();  // Create Control Panel
    void            initVertexBuffer   ();  // init vertices
    void            initShaders        ();  // init shaders
protected:
    void            initializeGL       ();
    void            resizeGL   (int, int);
    void            paintGL            ();
private:
    int                     m_winW;
    int                     m_winH;
    QGLShaderProgram        m_program;
};

#endif // HW2A_H
