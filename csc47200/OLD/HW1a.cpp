// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW1a.cpp - HW1a class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#include "HW1a.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1a::HW1a:
//
// HW1a constructor.
//
HW1a::HW1a(QWidget *parent):HW(parent)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1a::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW1a::initializeGL()
{
    // init state variables
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 0.0);		// set foreground color
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1a::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW1a::resizeGL(int w, int h)
{
    // save window dimensions
    m_winW = w;
    m_winH = h;

    // compute aspect ratio
    float xmax, ymax;
    float ar = (float) w / h;
    if(ar > 1.0) {		// wide screen
        xmax = ar;
        ymax = 1.;
    } else {		// tall screen
        xmax = 1.;
        ymax = 1/ar;
    }

    // set viewport to occupy full canvas
    //glViewport(0, 0, w, h);

    // init viewing coordinates for orthographic projection
    glLoadIdentity();
    glOrtho(-xmax, xmax, -ymax,  ymax, -1.0, 1.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1a::paintGL:
//
// Update GL scene.
//
void
HW1a::paintGL()
{

    // clear canvas with background values
    glClear(GL_COLOR_BUFFER_BIT);

    int w,h;
    w = m_winW / 3; //width  per viewport
    h = m_winH / 3; //height per viewport

    glMatrixMode(GL_PROJECTION);
    for(int i = 0; i < 9; i++){
        if(i == 3){
            glLoadIdentity();
            glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);
        }
        else{
            glLoadIdentity();
            glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
        }
        glViewport(i%3 * w, i/3 * h, w, h);
        glBegin(g_drawModes[i]);
            for(int j = 0; j < 16; j++){
                glVertex2f(g_vectors[j][0], g_vectors[j][1]);
            }
        glEnd();
    }
    glFlush();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1a::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW1a::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 1a");
    groupBox->setMinimumWidth(300);

    return(groupBox);
}
