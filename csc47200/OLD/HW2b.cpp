// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW2a.cpp - HW2a class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================

#include "HW2b.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::HW2b:
//
// HW2b constructor.
//
HW2b::HW2b(QWidget *parent):HW(parent)
{
    m_theta         = 0.0;
    m_subdivide     = 0;
    m_twist         = false;
    srand(time(0));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox *HW2b::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 2b");
    groupBox->setMinimumWidth(300);

    // layout for assembling widgets
    QGridLayout *layout = new QGridLayout;

    // create slider to rotate
    m_sliderTheta = new QSlider(Qt::Horizontal);
    m_sliderTheta->setRange(0,360);
    m_sliderTheta->setValue(0);

    // create spinbox to rotate
    m_spinboxTheta = new QSpinBox();
    m_spinboxTheta->setRange(0,360);
    m_spinboxTheta->setValue(0);

    // create slider to subdivide
    m_sliderSubDivide = new QSlider(Qt::Horizontal);
    m_sliderSubDivide->setRange(0,6);
    m_sliderSubDivide->setValue(0);

    // create spinbox to subdivide
    m_spinboxSubDivide = new QSpinBox();
    m_spinboxSubDivide->setRange(0,6);
    m_spinboxSubDivide->setValue(0);

    // create radio buttons to twist
    m_radioButtonYesTwist = new QRadioButton("Yes");
    m_radioButtonNoTwist  = new QRadioButton("No" );
    m_radioButtonNoTwist->setChecked(true);

    // button group for radio buttons
    QButtonGroup *twistGroup = new QButtonGroup();
    twistGroup->addButton(m_radioButtonYesTwist);
    twistGroup->addButton(m_radioButtonNoTwist);

    // assemble widgets into layout
    layout->addWidget(new QLabel("Theta:"),     0, 0);
    layout->addWidget(m_sliderTheta,            0, 1);
    layout->addWidget(m_spinboxTheta,             0, 2);
    layout->addWidget(new QLabel("Subdivide:"), 1, 0);
    layout->addWidget(m_sliderSubDivide,        1, 1);
    layout->addWidget(m_spinboxSubDivide,         1, 2);
    layout->addWidget(new QLabel("Twist?"),     2, 0);
    layout->addWidget(m_radioButtonYesTwist,    2, 1);
    layout->addWidget(m_radioButtonNoTwist,     2, 2);

    // assign layout to group box
    groupBox->setLayout(layout);

    // init signal/slot connections
    connect(m_sliderTheta,          SIGNAL(valueChanged(int)), this, SLOT(setTheta(int))    );
    connect(m_spinboxTheta,         SIGNAL(valueChanged(int)), this, SLOT(setTheta(int))    );
    connect(m_sliderSubDivide,      SIGNAL(valueChanged(int)), this, SLOT(setSubDivide(int)));
    connect(m_spinboxSubDivide,     SIGNAL(valueChanged(int)), this, SLOT(setSubDivide(int)));
    connect(m_radioButtonYesTwist,  SIGNAL(toggled(bool))    , this, SLOT(setTwist(bool))   );
    // cheap way to avoid more code to set functions
    connect(m_sliderTheta,          SIGNAL(valueChanged(int)), m_spinboxTheta,     SLOT(setValue(int)));
    connect(m_sliderSubDivide,      SIGNAL(valueChanged(int)), m_spinboxSubDivide, SLOT(setValue(int)));
    connect(m_spinboxTheta,         SIGNAL(valueChanged(int)), m_sliderTheta,     SLOT(setValue(int)));
    connect(m_spinboxSubDivide,     SIGNAL(valueChanged(int)), m_sliderSubDivide, SLOT(setValue(int)));

    return(groupBox);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::reset:
//
// Reset parameters.
//
void HW2b::reset()
{
    // reset sliders and radio
    m_sliderTheta->setValue(0);
    m_sliderSubDivide->setValue(0);
    m_radioButtonNoTwist->setChecked(true);

    // redraw
    if(m_subdivide!=0)
        initVertexBuffer();
    updateGL();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::initVertexBuffer:
//
// Initialize vertex buffer.
//
void HW2b::initVertexBuffer()
{
    // set flag for creating buffers (1st time only)
    static bool flag = 1;

    // verify that we have valid vertex/color buffers
    static GLuint vertexBuffer = -1;
    static GLuint colorBuffer  = -1;
    if(flag) {	// create vertex and color buffers
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &colorBuffer );
        flag = 0;	// reset flag
    }

    // initialize data
    divideTriangle(vec2( 0.0 ,  0.75 ),
                   vec2(-0.75, -0.375),
                   vec2( 0.75, -0.375),
                   m_subdivide);

    // record number of points
    m_numofpoints = m_points.size();

    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // copy the vertices from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numofpoints*sizeof(vec2), &m_points[0], GL_STATIC_DRAW);

    // enable the assignment of attribute vertex variable
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // assign the buffer object to the attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);

    // bind color buffer to the GPU; all future buffer ops operate on this buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);

    // copy the color from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numofpoints*sizeof(vec3), &m_colors[0], GL_STATIC_DRAW);

    // enable the assignment of attribute color variable
    glEnableVertexAttribArray(ATTRIB_COLOR);

    // assign the buffer object to the attribute color variable
    glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, false, 0, NULL);

    // clear vertex and color vectors because they have already been copied into GPU
    m_points.clear();
    m_colors.clear();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::initShaders:
//
// Initialize vertex and fragment shaders.
//
void HW2b::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader2b.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader2b.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }

    // link shader pipeline
    if(!m_program.link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }

    // bind the glsl progam
    glUseProgram(m_program.programId());

    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program.programId(), ATTRIB_VERTEX, "a_Position");

    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_COLOR will modify the value of "a_Color")
    glBindAttribLocation(m_program.programId(), ATTRIB_COLOR,  "a_Color");

    // get storage location of u_Twist in vertex shader
    m_u_Twist = glGetUniformLocation(m_program.programId(),"u_Twist");
    if((int) m_u_Twist < 0){
        qDebug() << "Failed to get the storage location of u_Twist";
        exit(-1);
    }

    // get storage location of u_Theta in vertex shader
    m_u_Theta = glGetUniformLocation(m_program.programId(),"u_Theta");
    if((int) m_u_Theta < 0){
        qDebug() << "Failed to get the storage location of u_Theta";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1i(m_u_Twist, m_twist);
    glUniform1f(m_u_Theta, m_theta);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::triangle(vec2,vec2,vec2)
//
//
//
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void HW2b::triangle(vec2 a, vec2 b, vec2 c)
{
    m_points.push_back(a);
    m_points.push_back(b);
    m_points.push_back(c);

    vec3 color = vec3( (float) rand()/ (float) RAND_MAX,
                       (float) rand()/ (float) RAND_MAX,
                       (float) rand()/ (float) RAND_MAX);
    m_colors.push_back(color);
    m_colors.push_back(color);
    m_colors.push_back(color);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::divideTriangle(vec2,vec2,vec2,int)
//
//
//
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void HW2b::divideTriangle(vec2 a, vec2 b, vec2 c, int count)
{
    if(count == 0)
        triangle(a,b,c);
    else{
        vec2 ab = vec2((b[0]+a[0])/2, (b[1]+a[1])/2);
        vec2 ac = vec2((c[0]+a[0])/2, (c[1]+a[1])/2);
        vec2 bc = vec2((b[0]+c[0])/2, (b[1]+c[1])/2);

        divideTriangle( a, ab, ac, count-1);
        divideTriangle( b, ab, bc, count-1);
        divideTriangle( c, ac, bc, count-1);
        divideTriangle(ab, ac, bc, count-1);
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::setTheta:
//
// Slot function to change angle of rotation
//
void HW2b::setTheta(int theta)
{
    // compute m_theta
    m_theta = (theta * M_PI / 180);

    // update m_u_theta
    glUniform1f(m_u_Theta, m_theta);

    // redraw
    updateGL();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::setSubDivide:
//
// Slot function to set division of triangles
//
void HW2b::setSubDivide(int subdivide)
{
    // update m_subdivide
    m_subdivide = subdivide;

    // update geometry
    initVertexBuffer();

    // redraw
    updateGL();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::setTwist:
//
// Slot function to rotate data.
//
void HW2b::setTwist(bool twist)
{
    // update m_twist
    m_twist = twist;

    // update m_u_twist
    glUniform1i(m_u_Twist, m_twist);

    // redraw
    updateGL();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void HW2b::initializeGL()
{
    // initialize GL function resolution for current context
    initializeGLFunctions();

    // init vertex and fragment shaders
    initShaders();

    // initialize vertex buffer and write positions to vertex shader
    initVertexBuffer();

    // init state variables
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (0.0, 1.0, 0.0);		// set foreground color
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW2b::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void HW2b::resizeGL(int w, int h)
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

void HW2b::paintGL()
{
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangles
    glDrawArrays(GL_TRIANGLES, 0, m_numofpoints);
}

