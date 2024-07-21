// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW3a.h - Header file for HW3a class
//
// Written by: George Wolberg, 2015
// Modified by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ======================================================================

#ifndef HW3A_H
#define HW3A_H


#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>


// ----------------------------------------------------------------------
// standard include files
//

class HW3a : public HW {
	Q_OBJECT
public:
	HW3a		(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image

public slots:
    void		changeTheta      (int);		// respond to rotation slider
    void		changeSubdiv	 (int);		// respond to subdivision slider
    void		changeTwist      (int);		// respond to twist checkbox
    void		changeWire     	 (int);		// respond to wireframe checkbox

protected:
    void		initializeGL        ();                 // init GL state
    void		resizeGL	(int, int);                 // resize GL widget
    void		paintGL             ();                 // render GL scene
	void		divideTriangle(vec2, vec2, vec2, int);	// subdivide triangle
    void		triangle(vec2, vec2, vec2);             // process single triangle
    void		initShader1         ();                 // texture mapping shader
    void		initShader2         ();                 // wireframe rendering shader

private:
    int                 m_winW;             // window width
    int                 m_winH;             // window height
    bool                m_twist;			// twist flag
    bool                m_wire;             // wireframe flag
    float               m_theta;			// rotation angle
    int                 m_subdivisions;		// triangle subdivisions
    std::vector<vec2>   m_points;			// vector of 2D points (XY)
    std::vector<vec2>   m_coords;			// vector of 2D coords (UV)
    QSlider            *m_sliderTheta;		// rotation slider
    QSlider            *m_sliderSubdiv;		// subdivision slider
    QSpinBox           *m_spinBoxTheta;		// rotation spinbox
    QSpinBox           *m_spinBoxSubdiv;	// subdivision spinbox
    QCheckBox          *m_checkBoxTwist;	// twist checkbox
    QCheckBox          *m_checkBoxWire;		// wireframe checkbox
    int                 m_numPoints;		// number of 2D points
    QMatrix4x4          m_ModelMatrix;		// 4x4 transformation matrix
    QImage              m_image;			// texture image
    GLuint              m_texture;			// shader index to texture unit
    GLuint              m_uniform[2][16];	// uniform vars for two shaders and <16 vars
    QGLShaderProgram    m_program[2];		// GLSL programs
};

#endif // HW3A_H
