// ====================================================================
// Computer Graphics Homework
// Copyright (C) 2015 by Lukas Rascius,Murtaza Yaqoob and Ethan Graber
//
// HW1b.cpp - HW1b class
//
// Written by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ====================================================================
#include "HW1b.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::HW1b:
//
// HW1b constructor.
//
HW1b::HW1b(QWidget *parent):HW(parent)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::triangle(QVector2D,QVector2D,QVector2D)
//
//
//
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
HW1b::triangle(QVector2D a, QVector2D b, QVector2D c){

    // base distance multipliers
    float ad = 1.;
    float bd = 1.;
    float cd = 1.;

    // twist multipliers calculations
    if(m_twist){
        ad = sqrt(a[0]*a[0]+a[1]*a[1]);
        bd = sqrt(b[0]*b[0]+b[1]*b[1]);
        cd = sqrt(c[0]*c[0]+c[1]*c[1]);
    }

    // points adding
    m_Points.append(QVector2D(a[0]*cos(m_theta*ad) - a[1]*sin(m_theta*ad),
                              a[0]*sin(m_theta*ad) + a[1]*cos(m_theta*ad)));
    m_Points.append(QVector2D(b[0]*cos(m_theta*bd) - b[1]*sin(m_theta*bd),
                              b[0]*sin(m_theta*bd) + b[1]*cos(m_theta*bd)));
    m_Points.append(QVector2D(c[0]*cos(m_theta*cd) - c[1]*sin(m_theta*cd),
                              c[0]*sin(m_theta*cd) + c[1]*cos(m_theta*cd)));
    //colors adding
    if(m_updateColor){
        m_Colors.append(QVector3D((float) rand() / (float) RAND_MAX,
                                  (float) rand() / (float) RAND_MAX,
                                  (float) rand() / (float) RAND_MAX));
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::divideTriangle(QVector2D,QVector2D,QVector2D,int)
//
//
//
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
HW1b::divideTriangle(QVector2D a, QVector2D b, QVector2D c, int count){
    if(count == 0){
        //save point
        triangle(a,b,c);
    }
    else{
        // compute midpoints
        QVector2D ab =  QVector2D((b[0]+a[0])/2,
                                  (b[1]+a[1])/2);
        QVector2D ac =  QVector2D((c[0]+a[0])/2,
                                  (c[1]+a[1])/2);
        QVector2D bc =  QVector2D((b[0]+c[0])/2,
                                  (b[1]+c[1])/2);

        // recursive
        divideTriangle(a , ab, ac, count-1);
        divideTriangle(b , ab, bc, count-1);
        divideTriangle(c , ac, bc, count-1);
        divideTriangle(ab, ac, bc, count-1);
    }
}

// HW1b::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW1b::initializeGL()
{
    // init state variables
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    m_theta = 0.;
    m_subdivide = 4;
    m_twist = false;
    m_updateColor = true;
    srand(time(0));

    // set default points
    divideTriangle(QVector2D(.0,.75),QVector2D(-.75,-.375),QVector2D(.75,-.375), m_subdivide);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW1b::resizeGL(int w, int h)
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
// HW1b::paintGL:
//
// Update GL scene.
//
void
HW1b::paintGL()
{

    // clear canvas with background values
    glClear(GL_COLOR_BUFFER_BIT);

    // triangle drawing
    glViewport(0,0,m_winW,m_winH);

    glBegin(GL_TRIANGLES);
        for(int i = 0; i < m_Points.size();i++){
            if(i%3==0){
                glColor3f(m_Colors.at(i/3)[0],
                          m_Colors.at(i/3)[1],
                          m_Colors.at(i/3)[2]);
            }
            glVertex2f(m_Points.at(i)[0],
                       m_Points.at(i)[1]);
        }
    glEnd();
    glFlush();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW1b::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 1b");
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
    m_sliderSubDivide->setValue(4);

    // create spinbox to rotate
    m_spinboxSubDivide = new QSpinBox();
    m_spinboxSubDivide->setRange(0,6);
    m_spinboxSubDivide->setValue(4);

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
// HW1b::setTheta:
//
// Slot function to change angle of rotation
//
void
HW1b::setTheta(int theta)
{
    // update rotation varible
    m_theta = (float) (theta * M_PI / 180);

    // clear points
    m_Points.clear();

    // do not update colors
    m_updateColor = false;

    // call divide to update
    divideTriangle(QVector2D(.0,.75),QVector2D(-.75,-.375),QVector2D(.75,-.375), m_subdivide);

    // call resizeGL() to reset coordinate system
    resizeGL(m_winW, m_winH);

    // redraw
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::setSubDivide:
//
// Slot function to set division of triangles
//
void
HW1b::setSubDivide(int subdivide)
{
    // update subDivide
    m_subdivide = subdivide;

    // clear current
    m_Points.clear();
    m_Colors.clear();

    // update colors
    m_updateColor = true;

    // call divideTriangle with initialize points
    divideTriangle(QVector2D(.0,.75),QVector2D(-.75,-.375),QVector2D(.75,-.375), m_subdivide);

    // call resizeGL() to reset coordinate system
    resizeGL(m_winW, m_winH);

    // redraw
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::setTwist:
//
// Slot function to rotate data.
//
void
HW1b::setTwist(bool twiston)
{
    // update twist flag
    m_twist = twiston;

    // clear current
    m_Points.clear();

    // do not update colors
    m_updateColor = false;

    // call divideTriangle with initialize points
    divideTriangle(QVector2D(.0,.75),QVector2D(-.75,-.375),QVector2D(.75,-.375), m_subdivide);

    // redraw
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW1b::reset:
//
// Reset parameters.
//
void
HW1b::reset()
{
    // reset sliders and radio
    m_sliderTheta->setValue(0);
    m_sliderSubDivide->setValue(4);
    m_radioButtonNoTwist->setChecked(true);
    resizeGL(m_winW, m_winH);

    // reset angle and slider/spinbox settings
    m_theta     = 0.;
    m_subdivide = 4;
    m_twist     = false;
    m_updateColor = true;

    // redraw
    updateGL();
}
