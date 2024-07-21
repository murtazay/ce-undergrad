// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW2b.h - Header file for HW2b class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#ifndef HW2B_H
#define HW2B_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>
#include <time.h>

class HW2b : public HW {
    Q_OBJECT

public:
    HW2b            (QWidget *parent = 0             );
    QGroupBox*      controlPanel                    ();
    void            initVertexBuffer                ();
    void            initShaders                     ();
    void            triangle  (vec2,vec2,vec2        );
    void            divideTriangle(vec2,vec2,vec2,int);
    void            reset                           ();
public slots:
    void            setTheta    (int);
    void            setSubDivide(int);
    void            setTwist   (bool);
protected:
    void            initializeGL       ();
    void            resizeGL    (int,int);
    void            paintGL            ();
private:
    int                 m_winW;
    int                 m_winH;
    int                 m_subdivide;
    int                 m_numofpoints;
    bool                m_twist;
    float               m_theta;
    QSlider            *m_sliderTheta;
    QSlider            *m_sliderSubDivide;
    QSpinBox           *m_spinboxTheta;
    QSpinBox           *m_spinboxSubDivide;
    QRadioButton       *m_radioButtonYesTwist;
    QRadioButton       *m_radioButtonNoTwist;
    std::vector<vec2>   m_points;
    std::vector<vec3>   m_colors;
    GLuint              m_u_Twist;
    GLfloat             m_u_Theta;
    QGLShaderProgram    m_program;
};

#endif // HW2B_H
