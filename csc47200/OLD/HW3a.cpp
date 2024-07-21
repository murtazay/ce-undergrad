// ================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW3a.cpp - HW3a class
//
// Written by: George Wolberg, 2015
// Modified by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ================================================================

#include "HW3a.h"

enum {
    MV,
	THETA,
	SUBDIV,
	TWIST,
	SAMPLER
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::HW3a:
//
// HW3a constructor.
//
HW3a::HW3a(QWidget *parent)
	: HW(parent)
{
	// init vars
	m_theta = 0.0f;
	m_subdivisions = 4;
	m_twist = true;
	m_wire  = true;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW3a::controlPanel()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Homework 3a");
    groupBox->setMinimumWidth(300);

    // add your code here
    // layout for assembling widgets
    QGridLayout *layout = new QGridLayout;

    // create slider to rotate
    m_sliderTheta = new QSlider(Qt::Horizontal);
    m_sliderTheta->setRange(0,360);
    m_sliderTheta->setValue(0);

    // create spinbox to rotate
    m_spinBoxTheta = new QSpinBox();
    m_spinBoxTheta->setRange(0,360);
    m_spinBoxTheta->setValue(0);

    // create slider to subdivide
    m_sliderSubdiv = new QSlider(Qt::Horizontal);
    m_sliderSubdiv->setRange(0,6);
    m_sliderSubdiv->setValue(4);

    // create spinbox to subdivide
    m_spinBoxSubdiv = new QSpinBox();
    m_spinBoxSubdiv->setRange(0,6);
    m_spinBoxSubdiv->setValue(4);

    // create check box to twist
    m_checkBoxTwist = new QCheckBox();
    m_checkBoxTwist->setChecked(true);

    // create check box to wire
    m_checkBoxWire = new QCheckBox();
    m_checkBoxWire->setChecked(true);

    // assemble widgets into layout
    layout->addWidget(new QLabel("Theta:"),     0, 0);
    layout->addWidget(m_sliderTheta,            0, 1);
    layout->addWidget(m_spinBoxTheta,           0, 2);
    layout->addWidget(new QLabel("Subdivide:"), 1, 0);
    layout->addWidget(m_sliderSubdiv,           1, 1);
    layout->addWidget(m_spinBoxSubdiv,          1, 2);
    layout->addWidget(new QLabel("Twist?"),     2, 0);
    layout->addWidget(m_checkBoxTwist,          2, 1);
    layout->addWidget(new QLabel("Wire?"),      3, 0);
    layout->addWidget(m_checkBoxWire,           3, 1);

    // assign layout to group box
    groupBox->setLayout(layout);

    // init signal/slot connections
    connect(m_sliderTheta,          SIGNAL(valueChanged(int)), this, SLOT(changeTheta(int)) );
    connect(m_spinBoxTheta,         SIGNAL(valueChanged(int)), this, SLOT(changeTheta(int)) );
    connect(m_sliderSubdiv,         SIGNAL(valueChanged(int)), this, SLOT(changeSubdiv(int)));
    connect(m_spinBoxSubdiv,        SIGNAL(valueChanged(int)), this, SLOT(changeSubdiv(int)));
    connect(m_checkBoxTwist,        SIGNAL(stateChanged(int)), this, SLOT(changeTwist(int)) );
    connect(m_checkBoxWire,         SIGNAL(stateChanged(int)), this, SLOT(changeWire(int))  );
    // cheap way to avoid more code to set functions
    connect(m_sliderTheta,          SIGNAL(valueChanged(int)), m_spinBoxTheta,      SLOT(setValue(int)));
    connect(m_sliderSubdiv,         SIGNAL(valueChanged(int)), m_spinBoxSubdiv,     SLOT(setValue(int)));
    connect(m_spinBoxTheta,         SIGNAL(valueChanged(int)), m_sliderTheta,       SLOT(setValue(int)));
    connect(m_spinBoxSubdiv,        SIGNAL(valueChanged(int)), m_sliderSubdiv,      SLOT(setValue(int)));

	return(groupBox);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW3a::initializeGL()
{
	// initialize GL function resolution for current context
	initializeGLFunctions();

	// init texture
	initTexture();

	// init vertex and fragment shaders
	initShaders();

	// initialize vertex buffer and write positions to vertex shader
	initVertexBuffer();

	// init state variables
	glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
	glColor3f   (1.0, 1.0, 0.0);		// set foreground color
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initTexture:
//
// Initialize texture.
//
void
HW3a::initTexture()
{
	// read image from file
	m_image.load(QString(":/mandrill.jpg"));

	// convert jpg to GL formatted image
	QImage qImage = QGLWidget::convertToGLFormat(m_image);

	// init vars
	int w = qImage.width ();
	int h = qImage.height();

	// generate texture name and bind it
	glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initShaders:
//
// Initialize vertex and fragment shaders.
//
void
HW3a::initShaders()
{
	initShader1();
	initShader2();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initShader1:
//
// Initialize vertex and fragment shaders for texture mapping.
//
void
HW3a::initShader1()
{
	// compile vertex shader
	if(!m_program[0].addShaderFromSourceFile(QGLShader::Vertex, ":/vshader3a1.glsl")) {
		QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok); 
		QApplication::quit();
	}

	// compile fragment shader
	if(!m_program[0].addShaderFromSourceFile(QGLShader::Fragment, ":/fshader3a1.glsl")) {
		QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok); 
		QApplication::quit();
	}

	// bind the attribute variable in the glsl program with a generic vertex attribute index;
	// values provided via ATTRIB_VERTEX will modify the value of "a_position")
	glBindAttribLocation(m_program[0].programId(), ATTRIB_VERTEX, "a_Position");

	// bind the attribute variable in the glsl program with a generic vertex attribute index;
	// values provided via ATTRIB_TEXTURE_POSITION will modify the value of "a_TexCoord")
	glBindAttribLocation(m_program[0].programId(), ATTRIB_TEXTURE_POSITION, "a_TexCoord");

	// link shader pipeline; attribute bindings go into effect at this point
	if(!m_program[0].link()) {
		QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok); 
		QApplication::quit();
	}

	// get storage location of u_ModelMatrix in vertex shader
    m_uniform[0][MV] = glGetUniformLocation(m_program[0].programId(), "u_ModelMatrix");
    if((int) m_uniform[0][MV] < 0) {
        qDebug() << "Failed to get the storage location of u_ModelMatrix";
        exit(-1);
    }

	// get storage location of u_Theta in vertex shader
    m_uniform[0][THETA] = glGetUniformLocation(m_program[0].programId(), "u_Theta");
    if((int) m_uniform[0][THETA] < 0) {
        qDebug() << "Failed to get the storage location of u_Theta";
        exit(-1);
    }

	// get storage location of u_Twist in vertex shader
    m_uniform[0][TWIST] = glGetUniformLocation(m_program[0].programId(), "u_Twist");
    if((int) m_uniform[0][TWIST] < 0) {
        qDebug() << "Failed to get the storage location of u_Twist";
        exit(-1);
    }

	// get storage location of u_Sampler in fragment shader
	m_uniform[0][SAMPLER] = glGetUniformLocation(m_program[0].programId(), "u_Sampler");
	if((int) m_uniform[0][SAMPLER] < 0) {
		qDebug() << "Failed to get the storage location of u_Sampler";
		exit(-1);
	}


    // bind the glsl program
    glUseProgram(m_program[0].programId());

	// init model matrix; pass it to vertex shader along with theta and twist flag
    m_ModelMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[0][MV], 1, GL_FALSE, m_ModelMatrix.constData());
    glUniform1f(m_uniform[0][THETA], m_theta);
    glUniform1i(m_uniform[0][TWIST], m_twist);
	glUniform1i(m_uniform[0][SAMPLER], GL_TEXTURE0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initShader2:
//
// Initialize vertex and fragment shaders for wireframe rendering.
//
void
HW3a::initShader2()
{
	// compile vertex shader
	if(!m_program[1].addShaderFromSourceFile(QGLShader::Vertex, ":/vshader3a2.glsl")) {
		QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok); 
		QApplication::quit();
	}

	// compile fragment shader
	if(!m_program[1].addShaderFromSourceFile(QGLShader::Fragment, ":/fshader3a2.glsl")) {
		QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok); 
		QApplication::quit();
	}

	// bind the attribute variable in the glsl program with a generic vertex attribute index;
	// values provided via ATTRIB_VERTEX will modify the value of "a_position")
	glBindAttribLocation(m_program[1].programId(), ATTRIB_VERTEX, "a_Position");

	// bind the attribute variable in the glsl program with a generic vertex attribute index;
	// values provided via ATTRIB_TEXTURE_POSITION will modify the value of "a_TexCoord")
	glBindAttribLocation(m_program[1].programId(), ATTRIB_TEXTURE_POSITION, "a_TexCoord");

	// link shader pipeline; attribute bindings go into effect at this point
	if(!m_program[1].link()) {
		QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok); 
		QApplication::quit();
	}

	// get storage location of u_ModelMatrix in vertex shader
    m_uniform[1][MV] = glGetUniformLocation(m_program[1].programId(), "u_ModelMatrix");
    if((int) m_uniform[1][MV] < 0) {
        qDebug() << "Failed to get the storage location of u_ModelMatrix";
        exit(-1);
    }

	// get storage location of u_Theta in vertex shader
    m_uniform[1][THETA] = glGetUniformLocation(m_program[1].programId(), "u_Theta");
    if((int) m_uniform[1][THETA] < 0) {
        qDebug() << "Failed to get the storage location of u_Theta";
        exit(-1);
    }

	// get storage location of u_Twist in vertex shader
    m_uniform[1][TWIST] = glGetUniformLocation(m_program[1].programId(), "u_Twist");
    if((int) m_uniform[1][TWIST] < 0) {
        qDebug() << "Failed to get the storage location of u_Twist";
        exit(-1);
    }

    // bind the glsl program
    glUseProgram(m_program[1].programId());

	// init model matrix; pass it to vertex shader along with theta and twist flag
    m_ModelMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[1][MV], 1, GL_FALSE, m_ModelMatrix.constData());
	glUniform1f(m_uniform[1][THETA], m_theta);
    glUniform1i(m_uniform[1][TWIST], m_twist);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
HW3a::initVertexBuffer()
{
	// init geometry data
    divideTriangle(
		vec2( 0.0f,   0.75f ),
		vec2( 0.65f, -0.375f),
        vec2(-0.65f, -0.375f),
              m_subdivisions);

	// add your code here
    // set flag for creating buffers (1st time only)
    static bool flag = 1;

    // verify that we have valid vertex/color buffers
    static GLuint vertexBuffer   = -1;
    static GLuint textureBuffer  = -1;
    if(flag) {	// create vertex and color buffers
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &textureBuffer );
        flag = 0;	// reset flag
    }
    // record number of points
    m_numPoints = m_points.size();

    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // copy the vertices from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec2), &m_points[0], GL_STATIC_DRAW);

    // enable the assignment of attribute vertex variable
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // assign the buffer object to the attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);

    // bind texture buffer to the GPU; all future buffer ops operate on this buffer
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    // copy the texture from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec2), &m_coords[0], GL_STATIC_DRAW);

    // enable the assignment of attribute texture variable
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);

    // assign the buffer object to the attribute texture variable
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL);

    // clear vertex and color vectors because they have already been copied into GPU
    m_points.clear();
    m_coords.clear();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::divideTriangle:
//
// Recursive subdivision of triangle (a,b,c). Recurse count times.
//
void
HW3a::divideTriangle(vec2 a, vec2 b, vec2 c, int count)
{
	// add your code here
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
// HW3a::triangle:
//
// Push positions and colors of the three triangle vertices.
//
void
HW3a::triangle(vec2 v1, vec2 v2, vec2 v3)
{
	// init geometry
	m_points.push_back(v1);
	m_points.push_back(v2);
	m_points.push_back(v3);

	// init texture coordinates
	m_coords.push_back(vec2((v1.x()+.65)/1.3, (v1.y()+.375)/1.125));
	m_coords.push_back(vec2((v2.x()+.65)/1.3, (v2.y()+.375)/1.125));
	m_coords.push_back(vec2((v3.x()+.65)/1.3, (v3.y()+.375)/1.125));
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW3a::resizeGL(int w, int h)
{
	// save window dimensions
	m_winW = w;
	m_winH = h;

	// compute aspect ratio
	float ar = (float) w / h;
	float xmax, ymax;
	if(ar > 1.0) {		// wide screen
		xmax = ar;
		ymax = 1.;
	}
	else {			// tall screen
		xmax = 1.;
		ymax = 1 / ar;
	}

	// set viewport to occupy full canvas
	glViewport(0, 0, w, h);

	// init viewing coordinates for orthographic projection
	glLoadIdentity();
	glOrtho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::paintGL:
//
// Update GL scene.
//
void
HW3a::paintGL()
{
	// clear canvas with background color
	glClear(GL_COLOR_BUFFER_BIT);

	// draw texture mapped triangles
	glUseProgram(m_program[0].programId());		// bind the glsl progam
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) m_numPoints);

	// draw wireframe, if necessary
	if(m_wire) {
		glUseProgram(m_program[1].programId());	// bind the glsl progam
		for(int i=0; i<m_numPoints; i+=3)
			glDrawArrays(GL_LINE_LOOP, i, (GLsizei) 3);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::reset:
//
// Reset parameters.
//
void
HW3a::reset()
{
    // reset parameters
    m_sliderTheta->setValue(0);
    m_sliderSubdiv->setValue(4);
    m_checkBoxTwist->setChecked(true);
    m_checkBoxWire->setChecked(true);

	// copy data to vertex shader
	for(int i=0; i<2; i++) {
		glUseProgram(m_program[i].programId());		// bind the glsl progam
        glUniformMatrix4fv(m_uniform[i][MV], 1, GL_FALSE, m_ModelMatrix.constData());
		glUniform1f(m_uniform[i][THETA], m_theta);
		glUniform1i(m_uniform[i][THETA], m_twist);
	}

	// draw
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::changeTheta:
//
// Slot function to change rotation angle.
//
void
HW3a::changeTheta(int angle)
{
	// update model's rotation matrix
	m_ModelMatrix.setToIdentity();
	m_ModelMatrix.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));
    m_theta = (angle * M_PI / 180);
	for(int i=0; i<2; i++) {
		glUseProgram(m_program[i].programId());		// bind the glsl progam
        glUniformMatrix4fv(m_uniform[i][MV], 1, GL_FALSE, m_ModelMatrix.constData());
        glUniform1f(m_uniform[i][THETA], m_theta);
	}

	// draw
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::changeSubdiv:
//
// Slot function to change number of recursive subdivisions.
//
void
HW3a::changeSubdiv(int subdivisions)
{
	
	// init vars
	m_subdivisions = subdivisions;

	// compute new vertices and texture coordinates
	initVertexBuffer();

	// draw
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::changeTwist:
//
// Slot function to turn on/off m_twist;
//
void
HW3a::changeTwist(int twist)
{
	// init vars
	m_twist = twist;

	// redraw with new twist flag
	for(int i=0; i<2; i++) {
		glUseProgram(m_program[i].programId());		// bind the glsl progam
		glUniform1i(m_uniform[i][TWIST], m_twist);
	}
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3a::changeWire:
//
// Slot function to turn on/off m_wire;
//
void
HW3a::changeWire(int wire)
{
	// init vars
	m_wire = wire;

	// redraw with new wire flag
	updateGL();
}
