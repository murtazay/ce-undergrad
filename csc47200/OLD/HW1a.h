// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW1a.h - Header file for HW1a class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#ifndef HW1A_H
#define HW1A_H

#include "HW.h"

class HW1a : public HW{
    Q_OBJECT

    // initial data
    QVector2D g_vectors[16] = {
        QVector2D( .70, .75),
        QVector2D( .75, .65),
        QVector2D( .70, .55),
        QVector2D( .25, .55),
        QVector2D( .25, .45),
        QVector2D( .75, .45),
        QVector2D( .80, .55),
        QVector2D( .85, .65),
        QVector2D( .80, .75),
        QVector2D( .75, .85),//Set this to be last keeping order looks cool
        QVector2D( .15, .85),
        QVector2D( .15, .15),
        QVector2D( .10, .10),
        QVector2D( .30, .10),//Interesting as well
        QVector2D( .25, .15),//Probably the best
        QVector2D( .25, .75) //Pretty good too.
    };
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
    HW1a            (QWidget *parent = 0);
    QGroupBox*      controlPanel       ();
protected:
    void            initializeGL    ();
    void            resizeGL(int, int);
    void            paintGL         ();
private:
    int             m_winW;
    int             m_winH;
};

#endif // HW1A_H
