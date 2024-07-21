// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW2a.cpp - HW2a class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================

#include "HW2a.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::HW2a:
//
// HW2a constructor.
//
HW2a::HW2a(QWidget *parent):HW(parent)
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW2a::initializeGL()
{
    // initialize GL function resolution for current context
    initializeGLFunctions();

    // init vertex and fragment shaders
    initShaders();

    // initialize vertex buffer and write positions to vertex shader
    initVertexBuffer();

    // init state variables
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::initShaders:
//
// Initialize vertex and fragment shaders.
//
void
HW2a::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader2a.glsl")){
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader2a.glsl")){
        QMessageBox::critical(0, "Error", "Fragment shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // link shader pipeline
    if(!m_program.link()){
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }

    // bind the glsl program
    glUseProgram(m_program.programId());
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
HW2a::initVertexBuffer()
{
    // set flag for creating buffers (1st time only)
    static bool flag = 1;

    // verify that we have valid vertex/color buffers
    static GLuint vertexBuffer = -1;
    static GLuint colorBuffer  = -1;

    // create vertex and color buffers
    if(flag) {
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &colorBuffer );
        flag = 0;	// reset flag
    }

    // init geometry data
    const vec2 vertices[] = {
        vec2( .70, .75),
        vec2( .75, .65),
        vec2( .70, .55),
        vec2( .25, .55),
        vec2( .25, .45),
        vec2( .75, .45),
        vec2( .80, .55),
        vec2( .85, .65),
        vec2( .80, .75),
        vec2( .75, .85),//Set this to be last keeping order looks cool
        vec2( .15, .85),
        vec2( .15, .15),
        vec2( .10, .10),
        vec2( .30, .10),//Interesting as well
        vec2( .25, .15),//Probably the best
        vec2( .25, .75) //Pretty good too.
    };


    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // copy the vertices from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, 16*sizeof(vec2), &vertices[0], GL_STATIC_DRAW);

    // enable the assignment of attribute vertex variable
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // assign the buffer object to the attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW2a::resizeGL(int w, int h)
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
    glViewport(0, 0, w, h);

    // init viewing coordinates for orthographic projection
    glLoadIdentity();
    glOrtho(-xmax, xmax, -ymax,  ymax, -1.0, 1.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::paintGL:
//
// Update GL scene.
//
void
HW2a::paintGL()
{

    // clear canvas with background values
    glClear(GL_COLOR_BUFFER_BIT);

    // viewport settings
    int w,h;
    w = m_winW / 3; //width  per viewport
    h = m_winH / 3; //height per viewport

    // draw P
    for(int i = 0 ; i < 9; i++){
        if( i == 3){ 
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
        }
        if( i == 4){
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
        }
        glViewport(i % 3 * w, i / 3 * h, w, h);
        glDrawArrays(g_drawModes[i], 0, 16);
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2a::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW2a::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 2a");
    groupBox->setMinimumWidth(300);

    return(groupBox);
}
