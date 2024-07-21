// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW1b.h - Header file for HW1b class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#ifndef HW1B_H
#define HW1B_H

#include "HW.h"
#include <time.h>

class HW1b : public HW{
    Q_OBJECT

public:
    HW1b            (QWidget *parent = 0                            );
    QGroupBox*      controlPanel                                   ();
    void            reset                                          ();
    void            triangle(QVector2D, QVector2D, QVector2D        );
    void            divideTriangle(QVector2D,QVector2D,QVector2D,int);

public slots:
    void            setTheta        (int) ;
    void            setSubDivide    (int) ;
    void            setTwist        (bool);

protected:
    void            initializeGL    ();
    void            resizeGL(int, int);
    void            paintGL         ();

private:
    int                 m_winW;
    int                 m_winH;
    int                 m_subdivide;
    bool                m_twist;
    bool                m_updateColor;
    float               m_theta;
    QList<QVector2D>    m_Points;
    QList<QVector3D>    m_Colors;
    QSlider            *m_sliderTheta;
    QSlider            *m_sliderSubDivide;
    QSpinBox           *m_spinboxTheta;
    QSpinBox           *m_spinboxSubDivide;
    QRadioButton       *m_radioButtonYesTwist;
    QRadioButton       *m_radioButtonNoTwist;
};

#endif // HW1B_H
