// ===============================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW4.cpp - HW4 class
//
// Written by: George Wolberg, 2015
// Modified by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ===============================================================

#include "HW4.h"

// shader ID
enum {TEX_SHADER, WIRE_SHADER, FLAT_SHADER, SMOOTH_SHADER, SMOOTH_TEX};

// uniform ID
enum { MODEL, VIEW, PROJ, LIGHTDIR, SAMPLER };

// wave program enums
enum { WIREFRAME, TEXTURED, TEXTURED_WIREFRAME, FLAT_COLOR, SMOOTH_COLOR, SMOOTH_TEXTURE };
enum {WEAK, NORMAL, STRONG};
enum {FLAT, SPIKE, DIAGONALWALL, SIDEWALL, HOLE, MIDDLEBLOCK, DIAGONALBLOCK, CORNERBLOCK, HILL, HILLFOUR};

#define SQRTOFTWOINV 1.0 / 1.414213562


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::HW4:
//
// HW4 constructor.
//
HW4::HW4(QWidget *parent)
	: HW(parent)
{
	m_timer = new QBasicTimer;

	// reset parameters
	m_grid		= 32;
	m_displayMode	= TEXTURED_WIREFRAME;
	m_geometryMode  = MIDDLEBLOCK;
	m_speed		= 1;
	m_angularSpeed	= -30.0f;
	m_wave		= false;

	// light position and direction	
	m_lightEye      = vec3(1.0f, -2.0f, 2.0f);
	m_lightTarget   = vec3(0, 0, 0);
	m_lightRotation = cartesianToSpherical(m_lightEye);

	// keep phi within -2PI to +2PI for easy 'up' comparison;
	// if phi is between 0 to PI or -PI to -2PI, make 'up' be +Y, otherwise make it -Y
	float phi = m_lightRotation.y();
	checkPhi(phi, m_lightUp);
	m_lightRotation.setY(phi);

	// camera position and direction
	m_cameraEye      = vec3(1.0f, -4.0f, 3.0f);
	m_cameraTarget   = vec3(0, 0, 0);
	m_cameraRotation = cartesianToSpherical(m_cameraEye);

	// keep phi within -2PI to +2PI for easy 'up' comparison;
	// If phi is between 0 to PI or -PI to -2PI, make 'up' be +Y, otherwise make it -Y
	phi   = m_cameraRotation.y();
	checkPhi(phi, m_cameraUp);
	m_cameraRotation.setY(phi);

	// init camera axes: right (u), up (v), dir (n)
    vec3 dir     = (m_cameraTarget - m_cameraEye).normalized();
	vec3 worldUp = vec3(0.0f, m_cameraUp, 0.0f);
	vec3 right   = vec3::crossProduct(dir, worldUp).normalized();
	vec3 up      = vec3::crossProduct(right, dir).normalized();

	// init lookat matrix 
	m_cameraView.setToIdentity();
	m_cameraView.lookAt(m_cameraEye, m_cameraTarget, up);

	// init model matrix
	m_model.setToIdentity();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW4::initializeGL()
{
	// initialize GL function resolution for current context
	initializeGLFunctions();

	// init texture
	initTexture();

	// init vertex and fragment shaders
	initShaders();

	// init XY vertices in mesh and texture coords
	initVertices();

	// init Z values in height field
	resetMesh();

	// set dt based on m_grid size and user selection for m_speed
	setDT();

	// initialize vertex buffer and write positions to vertex shader
	initBuffers();

	glGenBuffers(1, &m_lightVtxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightVtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), &m_lightVtx[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_lightColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), &m_lightColor[0], GL_STATIC_DRAW);

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
	glColor3f   (1.0, 1.0, 0.0);		// set foreground color
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW4::resizeGL(int w, int h)
{
    // save window dimensions
    m_winW = w;
    m_winH = h;

    // set viewport to occupy full canvas
    glViewport(0, 0, w, h);

    // calculate aspect ratio
    float aspect = (float) w / h;

    // init perspective projection
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, aspect, 0.1f, 10.f);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::paintGL:
//
// Update GL scene.
//
void
HW4::paintGL()
{
	// clear canvas with background color and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// move back object and rotate so that z-axis points upward
	// enable buffer to be copied to the attribute vertex variable and specify data format
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, false, 0, NULL);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glBindBuffer(GL_ARRAY_BUFFER, m_texBuffer);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
	glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, false, 0, NULL);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glEnableVertexAttribArray(ATTRIB_COLOR);
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, false, 0, NULL);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, false, 0, NULL);

	// draw textured triangles
	if(m_displayMode == TEXTURED || m_displayMode == TEXTURED_WIREFRAME) {
		// draw textured surface
		glUseProgram(m_program[TEX_SHADER].programId());	// use texture glsl progam
		glUniformMatrix4fv(m_uniform[TEX_SHADER][MODEL],1,GL_FALSE,m_model.constData     ());
		glUniformMatrix4fv(m_uniform[TEX_SHADER][VIEW ],1,GL_FALSE,m_cameraView.constData());
		glUniformMatrix4fv(m_uniform[TEX_SHADER][PROJ ],1,GL_FALSE,m_projection.constData());
		glUniform1i(m_uniform[TEX_SHADER][SAMPLER], GL_TEXTURE0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) m_indices_triangles.size(), GL_UNSIGNED_SHORT, 0);
	}

	// draw wireframe
	if(m_displayMode == WIREFRAME || m_displayMode == TEXTURED_WIREFRAME) {
		// draw wireframe
		glUseProgram(m_program[WIRE_SHADER].programId());	// use wireframe glsl progam
		glUniformMatrix4fv(m_uniform[WIRE_SHADER][MODEL], 1, GL_FALSE,  m_model.constData());
		glUniformMatrix4fv(m_uniform[WIRE_SHADER][VIEW],1,GL_FALSE,m_cameraView.constData());
		glUniformMatrix4fv(m_uniform[WIRE_SHADER][PROJ],1,GL_FALSE,m_projection.constData());
		glLineWidth(2.0f);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[1]);
		glDrawElements(GL_LINES, (GLsizei) m_indices_wireframe.size(), GL_UNSIGNED_SHORT, 0);
	}

	// flat shading
	if(m_displayMode == FLAT_COLOR) {
		glUseProgram(m_program[FLAT_SHADER].programId());	// use wireframe glsl progam
		glUniformMatrix4fv(m_uniform[FLAT_SHADER][MODEL], 1, GL_FALSE,  m_model.constData());
		glUniformMatrix4fv(m_uniform[FLAT_SHADER][VIEW],1,GL_FALSE,m_cameraView.constData());
		glUniformMatrix4fv(m_uniform[FLAT_SHADER][PROJ],1,GL_FALSE,m_projection.constData());
		glUniform3f(m_uniform[FLAT_SHADER][LIGHTDIR], m_lightEye.x(), m_lightEye.y(), m_lightEye.z());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) m_indices_triangles.size(), GL_UNSIGNED_SHORT, 0);
	}

	// smooth shading
	if(m_displayMode == SMOOTH_COLOR) {
		glUseProgram(m_program[SMOOTH_SHADER].programId());	// use wireframe glsl progam
		glUniformMatrix4fv(m_uniform[SMOOTH_SHADER][MODEL], 1,GL_FALSE, m_model.constData());
		glUniformMatrix4fv(m_uniform[SMOOTH_SHADER][VIEW],1,GL_FALSE,m_cameraView.constData());
		glUniformMatrix4fv(m_uniform[SMOOTH_SHADER][PROJ],1,GL_FALSE,m_projection.constData());
		glUniform3f(m_uniform[SMOOTH_SHADER][LIGHTDIR], m_lightEye.x(), m_lightEye.y(), m_lightEye.z());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) m_indices_triangles.size(), GL_UNSIGNED_SHORT, 0);
	}

	// smooth shading and texture mapping
	if(m_displayMode == SMOOTH_TEXTURE) {
		glUseProgram(m_program[SMOOTH_TEX].programId());	// use wireframe glsl progam
		glUniformMatrix4fv(m_uniform[SMOOTH_TEX][MODEL], 1, GL_FALSE,   m_model.constData());
		glUniformMatrix4fv(m_uniform[SMOOTH_TEX][VIEW],1,GL_FALSE, m_cameraView.constData());
		glUniformMatrix4fv(m_uniform[SMOOTH_TEX][PROJ],1,GL_FALSE, m_projection.constData());
		glUniform3f(m_uniform[SMOOTH_TEX][LIGHTDIR], m_lightEye.x(), m_lightEye.y(), m_lightEye.z());
		glUniform1i(m_uniform[SMOOTH_TEX][SAMPLER], GL_TEXTURE0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) m_indices_triangles.size(), GL_UNSIGNED_SHORT, 0);
	}

	// draw line between light position and origin
	glUseProgram(m_program[WIRE_SHADER].programId());	// use wireframe glsl progam
	glUniformMatrix4fv(m_uniform[WIRE_SHADER][MODEL], 1, GL_FALSE, m_model.constData());
	glUniformMatrix4fv(m_uniform[WIRE_SHADER][VIEW], 1, GL_FALSE, m_cameraView.constData());
	glUniformMatrix4fv(m_uniform[WIRE_SHADER][PROJ], 1, GL_FALSE, m_projection.constData());
	glLineWidth(2.0f);

	// enable buffer to be copied to the attribute vertex variable and specify data format
	// assign the buffer object to the attribute vertex variable
	glBindBuffer(GL_ARRAY_BUFFER, m_lightVtxBuffer);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer	 (ATTRIB_VERTEX, 3, GL_FLOAT, false, 0, NULL);

	// enable the assignment of attribute vertex variable
    glBindBuffer(GL_ARRAY_BUFFER, m_lightColorBuffer);
	glEnableVertexAttribArray(ATTRIB_COLOR);

	// assign the buffer object to the attribute vertex variable
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, false, 0, NULL);

	// draw line between light source and origin (lookat position)
	glDrawArrays(GL_LINES, 0, 2);
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW4::controlPanel()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Homework 4");

    // create labels
    QLabel *label[5];
    label[0] = new QLabel("Display");
    label[1] = new QLabel("Geometry");
    label[2] = new QLabel("Size");
    label[3] = new QLabel("Speed");
    label[4] = new QLabel("Animation");


	// create Display comboBox
	m_comboBoxDisplay = new QComboBox;
	m_comboBoxDisplay->addItem("Wireframe");
	m_comboBoxDisplay->addItem("Textured");
	m_comboBoxDisplay->addItem("Textured+Wireframe");
	m_comboBoxDisplay->addItem("Flat Shading");
	m_comboBoxDisplay->addItem("Smooth Shading");
	m_comboBoxDisplay->addItem("Smooth Shading+Texture");
	m_comboBoxDisplay->setCurrentIndex(m_displayMode);

    // create Geometry comboBox
    m_comboBoxGeom = new QComboBox;
    m_comboBoxGeom->addItem("Flat");
    m_comboBoxGeom->addItem("Spike");
    m_comboBoxGeom->addItem("Diagonal Wall");
    m_comboBoxGeom->addItem("Side Wall");
    m_comboBoxGeom->addItem("Hole");
    m_comboBoxGeom->addItem("Middle Block");
    m_comboBoxGeom->addItem("Diagonal Block");
    m_comboBoxGeom->addItem("Corner Block");
    m_comboBoxGeom->addItem("Hill");
    m_comboBoxGeom->addItem("Hill Four");
    m_comboBoxGeom->setCurrentIndex(m_geometryMode);

    // create Size slider
    m_sliderSize = new QSlider(Qt::Horizontal);
    m_sliderSize->setRange(2, MAXGRID);
    m_sliderSize->setValue(m_grid);

    // create Size spinbox
    m_spinBoxSize = new QSpinBox;
    m_spinBoxSize->setRange(2, MAXGRID);
    m_spinBoxSize->setValue(m_grid);

    // create Speed slider
    m_sliderSpeed = new QSlider(Qt::Horizontal);
    m_sliderSpeed->setRange(1, 20);
    m_sliderSpeed->setValue(m_speed);

    // create Speed spinbox
    m_spinBoxSpeed = new QSpinBox;
    m_spinBoxSpeed->setRange(1, 20);
    m_spinBoxSpeed->setValue(m_speed);

    // create animation start/stop button
    m_buttonStart = new QPushButton("Start", this);
    m_buttonStop  = new QPushButton("Stop" , this);

    // layout for assembling widgets
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label[0],		0, 0);
    layout->addWidget(m_comboBoxDisplay,	0, 1);
    layout->addWidget(label[1],		1, 0);
    layout->addWidget(m_comboBoxGeom,	1, 1);
    layout->addWidget(label[2],		2, 0);
    layout->addWidget(m_sliderSize,		2, 1);
    layout->addWidget(m_spinBoxSize,	2, 2);
    layout->addWidget(label[3],		3, 0);
    layout->addWidget(m_sliderSpeed,	3, 1);
    layout->addWidget(m_spinBoxSpeed,	3, 2);
    layout->addWidget(label[4],		4, 0);
    layout->addWidget(m_buttonStart,        5, 0);
    layout->addWidget(m_buttonStop,         5, 1);

    // assign layout to group box
    groupBox->setLayout(layout);

    // init signal/slot connections
    connect(m_comboBoxDisplay,SIGNAL(currentIndexChanged(int)), this, SLOT(changeDisplay(int)));
    connect(m_comboBoxGeom,   SIGNAL(currentIndexChanged(int)), this, SLOT(changeGeom   (int)));
    connect(m_sliderSize,	  SIGNAL(valueChanged(int)),	    this, SLOT(changeSize(int)));
    connect(m_spinBoxSize,    SIGNAL(valueChanged(int)),        this, SLOT(changeSize(int)));
    connect(m_sliderSpeed,    SIGNAL(valueChanged(int)),        this, SLOT(changeSpeed(int)));
    connect(m_buttonStart,    SIGNAL(released()),               this, SLOT(startAnimation()));
    connect(m_buttonStop,     SIGNAL(released()),               this, SLOT(stopAnimation()));

	return(groupBox);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::reset:
//
// Reset parameters.
//
void
HW4::reset()
{
    // reset parameters
    m_grid		= 32;
    m_displayMode	= TEXTURED_WIREFRAME;
    m_geometryMode	= MIDDLEBLOCK;
    m_speed		= 1;
    m_angularSpeed	= -30.0f;
    m_wave		= false;

    m_comboBoxDisplay->setCurrentIndex(m_displayMode);
    m_comboBoxGeom->setCurrentIndex(m_geometryMode);
    m_sliderSize->setValue(m_grid);
    m_sliderSpeed->setValue(m_speed);

    // light position and direction
    m_lightEye      = vec3(1.0f, -2.0f, 2.0f);
    m_lightTarget   = vec3(0, 0, 0);
    m_lightRotation = cartesianToSpherical(m_lightEye);

    // keep phi within -2PI to +2PI for easy 'up' comparison;
    // if phi is between 0 to PI or -PI to -2PI, make 'up' be +Y, otherwise make it -Y
    float phi = m_lightRotation.y();
    checkPhi(phi, m_lightUp);
    m_lightRotation.setY(phi);

    // camera position and direction
    m_cameraEye      = vec3(1.0f, -4.0f, 3.0f);
    m_cameraTarget   = vec3(0, 0, 0);
    m_cameraRotation = cartesianToSpherical(m_cameraEye);

    // keep phi within -2PI to +2PI for easy 'up' comparison;
    // If phi is between 0 to PI or -PI to -2PI, make 'up' be +Y, otherwise make it -Y
    phi   = m_cameraRotation.y();
    checkPhi(phi, m_cameraUp);
    m_cameraRotation.setY(phi);

    // init camera axes: right (u), up (v), dir (n)
    vec3 dir     = (m_cameraTarget - m_cameraEye).normalized();
    vec3 worldUp = vec3(0.0f, m_cameraUp, 0.0f);
    vec3 right   = vec3::crossProduct(dir, worldUp).normalized();
    vec3 up      = vec3::crossProduct(right, dir).normalized();

    // init lookat matrix
    m_cameraView.setToIdentity();
    m_cameraView.lookAt(m_cameraEye, m_cameraTarget, up);

    stopAnimation();

    // init XY vertices in mesh and texture coords
    initVertices();

    // init Z values in height field
    resetMesh();

    // set dt based on m_grid size and user selection for m_speed
    setDT();

    // copy to GPU and redraw
    uploadVertices();
    uploadIndices();
    uploadColors();
    uploadNormals();
    uploadTexCoords();
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::resetMesh:
//
// Reset height field based on geometry mode flag.
//
void
HW4::resetMesh()
{
	// pause animation to reset grid without interruption by timer
	if(m_wave) m_timer->stop();

	for(int i=0; i< m_grid; ++i) {
	   for(int j=0; j< m_grid; ++j) {
		m_force[i][j] = 0.0f;
		m_veloc[i][j] = 0.0f;
		QVector3D &vec = m_vertices[i][j];
		switch(m_geometryMode) {
			case FLAT:
				vec.setZ(0.0f);
				break;
			case SPIKE:
				vec.setZ((i==j && i==m_grid/2) ? 1.0f : 0.0f);
				break;
			case HOLE:
                vec.setZ(!((i > m_grid/3 && j > m_grid/3)&&(i < m_grid*2/3 && j < m_grid*2/3)) ? 0.0f : -1.0f);
				break;
			case DIAGONALWALL:
                vec.setZ(((m_grid-i-j<3) && (m_grid-i-j>0)) ? 1.0f : 0.0f);
				break;
            case SIDEWALL:
                vec.setZ((i==1) ? 1.0f : 0.0f);
                break;
            case DIAGONALBLOCK:
                vec.setZ((m_grid-i-j<3) ? 1.0f : 0.0f);
                break;
            case MIDDLEBLOCK:
                vec.setZ(((i > m_grid/3 && j > m_grid/3)&&(i < m_grid*2/3 && j < m_grid*2/3)) ? 1.0f : 0.0f);
                break;
            case CORNERBLOCK:
                vec.setZ(((i > m_grid*3/4 && j > m_grid*3/4)) ? 1.0f : 0.0f);
                break;
            case HILL:
                float s1, s2;
                s1 = sin((M_PI * i) / (m_grid - 1));
                s2 = sin((M_PI * j) / (m_grid - 1));
                vec.setZ((s1 + s2 ) * 0.3f);
                break;
            case HILLFOUR:
                s1 = sin((2*M_PI * i) / (m_grid - 1));
                s2 = sin((2*M_PI * j) / (m_grid - 1));
                vec.setZ((s1 + s2) * 0.3f);
                break;
            }
	   }
	}

	// get face normals and vertex normals: for smooth shading only
	getFaceNorms();
	getVertNorms();

	// copy 2D vertex array to 1D array that will be passed to GPU
	flatten2D();

	// restart animation
	if(m_wave) m_timer->start(10, this);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::getForce:
//
// Compute force values based on differences with eight neighbors
// around each pixel.
//
void HW4::getForce(void)
{
	float d;

	for(int i=0; i<m_grid; ++i)
	    for(int j = 0; j<m_grid; ++j)
		m_force[i][j] = 0.0f;

	for(int i=1; i<m_grid-1; ++i) {
	    for(int j = 1; j<m_grid-1; ++j) {
		d = m_vertices[i][j].z() - m_vertices[i][j-1].z();
		m_force[i][j]   -= d;
		m_force[i][j-1] += d;

        // repeat this for all remaining seven neighbors around m_vertices[i][j]

        d = m_vertices[i][j].z() - m_vertices[i][j+1].z();
        m_force[i][j]   -= d;
        m_force[i][j+1] += d;

        d = m_vertices[i][j].z() - m_vertices[i-1][j].z();
        m_force[i][j]   -= d;
        m_force[i-1][j] += d;

        d = m_vertices[i][j].z() - m_vertices[i+1][j].z();
        m_force[i][j]   -= d;
        m_force[i+1][j] += d;

        d = (m_vertices[i][j].z() - m_vertices[i-1][j-1].z()) * SQRTOFTWOINV;
        m_force[i][j]     -= d;
        m_force[i-1][j-1] += d;

        d = (m_vertices[i][j].z() - m_vertices[i+1][j-1].z()) * SQRTOFTWOINV;
        m_force[i][j]     -= d;
        m_force[i+1][j-1] += d;

        d = (m_vertices[i][j].z() - m_vertices[i+1][j+1].z()) * SQRTOFTWOINV;
        m_force[i][j]     -= d;
        m_force[i+1][j+1] += d;

        d = (m_vertices[i][j].z() - m_vertices[i-1][j+1].z()) * SQRTOFTWOINV;
        m_force[i][j]     -= d;
        m_force[i-1][j+1] += d;

	    }
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::getVelocity:
//
// Compute velocity values based on forces at each pixel.
//
void HW4::getVelocity()
{
	for(int i=0; i<m_grid; ++i)
	   for(int j=0; j<m_grid; ++j)
		m_veloc[i][j] += m_force[i][j] * m_dt;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::getPosition:
//
// Compute vertex positions values based on velocity at each pixel.
//
void HW4::getPosition()
{
	for(int i=0; i<m_grid; ++i) {
	   for(int j=0; j<m_grid; ++j)
		m_vertices[i][j].setZ(m_vertices[i][j].z() + m_veloc[i][j]);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::timerEvent:
//
// Virtual function called when timer times out.
// We use it to stop timer, recompute surface heights, and restart timer.
//
void 
HW4::timerEvent(QTimerEvent *e)
{
	// avoid compiler warning for unused event e
	Q_UNUSED(e);

	// pause animation to reset grid without interruption by timer
	m_timer->stop();

	// recompute surface height and face/vertex normals
	getForce();
	getVelocity();
	getPosition();
	getFaceNorms();
	getVertNorms();

	// copy 2D vertex array to 1D array that will be passed to GPU
	flatten2D();

	// copy to GPU and redraw
	uploadVertices();
	uploadNormals();
	updateGL();

	// restart animation
	m_timer->start(10, this);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initTexture:
//
// Initialize texture.
//
void
HW4::initTexture()
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initShaders:
//
// Initialize vertex and fragment shaders.
//
void
HW4::initShaders()
{
    QMap<QString,GLuint> uniforms;

	// init uniform hash table based on uniform variable names and location IDs
	uniforms["u_Model"     ] = MODEL;
	uniforms["u_View"      ] = VIEW;
	uniforms["u_Projection"] = PROJ;
	uniforms["u_Sampler"   ] = SAMPLER;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
    initShader(TEX_SHADER, QString(":/vshader4a.glsl"), QString(":/fshader4a.glsl"), uniforms);

	// reset uniform hash table for next shader
	uniforms.clear();
	uniforms["u_Model"     ] = MODEL;
	uniforms["u_View"      ] = VIEW;
	uniforms["u_Projection"] = PROJ;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
    initShader(WIRE_SHADER, QString(":/vshader4b.glsl"), QString(":/fshader4b.glsl"), uniforms);

	// reset uniform hash table for next shader
	uniforms.clear();
	uniforms["u_Model"         ] = MODEL;
	uniforms["u_View"          ] = VIEW;
	uniforms["u_Projection"    ] = PROJ;
	uniforms["u_LightDirection"] = LIGHTDIR;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
    initShader(FLAT_SHADER, QString(":/vshader4c.glsl"), QString(":/fshader4c.glsl"), uniforms);

	uniforms.clear();
	uniforms["u_Model"         ] = MODEL;
	uniforms["u_View"          ] = VIEW;
	uniforms["u_Projection"    ] = PROJ;
	uniforms["u_LightDirection"] = LIGHTDIR;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
    initShader(SMOOTH_SHADER, QString(":/vshader4d.glsl"), QString(":/fshader4d.glsl"), uniforms);

	uniforms.clear();
	uniforms["u_Model"         ] = MODEL;
	uniforms["u_View"          ] = VIEW;
	uniforms["u_Projection"    ] = PROJ;
	uniforms["u_LightDirection"] = LIGHTDIR;
	uniforms["u_Sampler"       ] = SAMPLER;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
    initShader(SMOOTH_TEX, QString(":/vshader4e.glsl"), QString(":/fshader4e.glsl"), uniforms);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initShader:
//
// Initialize vertex and fragment shaders.
//
void
HW4::initShader(int number, QString vertexShader, QString fragmentShader, QMap<QString,GLuint> map){
    //Complie Vertex Shader
    if(!m_program[number].addShaderFromSourceFile(QGLShader::Vertex, vertexShader)){
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        qDebug() << m_program[number].log();
        QApplication::quit();
    }
    //Compile fragment Shader
    if(!m_program[number].addShaderFromSourceFile(QGLShader::Fragment, fragmentShader)){
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        qDebug() << m_program[number].log();
        QApplication::quit();
    }

    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program[number].programId(), ATTRIB_VERTEX, "a_Position");

    if(number == TEX_SHADER || number ==SMOOTH_TEX){
        // bind the attribute variable in the glsl program with a generic vertex attribute index;
        // values provided via ATTRIB_TEXCOORD will modify the value of "a_TexCoord")
        glBindAttribLocation(m_program[number].programId(), ATTRIB_TEXCOORD, "a_TexCoord");
    }

    if(number == FLAT_SHADER || number == SMOOTH_SHADER || number == SMOOTH_TEX){
        // bind the attribute variable in the glsl program with a generic vertex attribute index;
        // values provided via ATTRIB_NORMAL will modify the value of "a_Normal")
        glBindAttribLocation(m_program[number].programId(), ATTRIB_NORMAL, "a_Normal");
    }

    if(number == FLAT_SHADER || number == SMOOTH_SHADER){
        // bind the attribute variable in the glsl program with a generic vertex attribute index;
        // values provided via ATTRIB_COLOR will modify the value of "a_Color")
        glBindAttribLocation(m_program[number].programId(), ATTRIB_COLOR, "a_Color");
    }

    // link shader pipeline; attribute bindings go into effect at this point
    if(!m_program[number].link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }

    QMap<QString, GLuint>::iterator i;
    for(i = map.begin(); i != map.end(); ++i){
        //QString name = i.key();
        const char* name = i.key().toLatin1().data();
        m_uniform[number][i.value()] = glGetUniformLocation(m_program[number].programId(), name);
        if((int) m_uniform[number][i.value()] < 0){
            qDebug() << "Failed to get the storage location of " << i.key() << " " << number;
            exit(-1);
        }
    }

    // bind the glsl program
    glUseProgram(m_program[number].programId());
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initVertices:
//
// Initialize  (x,y) positions of vertices so that they lie in [-1,1] range.
// Initialize texture coordinates.
// Initialize vertex indices for texture mapped triangles and wireframe.
// Surface height values (z) are evaluated in resetMesh().
//
void
HW4::initVertices()
{
	// map the grid coordinates to [-1.0, 1.0]; function called when m_grid changes
	// flip y-coordinates because array orientation is upside down wrt graphics coordinates
	int size = m_grid - 1;
	for(int y=0; y < m_grid; ++y) {
		for(int x=0; x < m_grid; ++x) {
			QVector3D &vec = m_vertices[y][x];
			vec.setX(-1 + (2. * x / size));
			vec.setY(-1 + (2. * (size-y) / size));
			m_colors.push_back(vec3(0.2f, 0.9f, 1.0f));

			// init texture coordinates
			QVector2D &texCoord = m_texCoords[y][x];
			texCoord.setX((float) x / size);
			texCoord.setY((float) (size-y) / size);
		}
	}

	// create indices with degenerate triangles (stitching)
	m_indices_triangles.clear();
	for(int y=0; y < m_grid-1; ++y) {
		ushort base = y * m_grid;
		if(y > 0) {
			// Degenerate begin: repeat first vertex
			m_indices_triangles.push_back(base);
		}
		for(int x=0; x < m_grid; ++x){
			m_indices_triangles.push_back(base + x);
			m_indices_triangles.push_back(base + m_grid + x);
		}
		if(y < m_grid-2)
			m_indices_triangles.push_back((y+1) * m_grid + (m_grid - 1));
	}

	// create indices for wireframe rendering (horizontal lines)
	m_indices_wireframe.clear();
	for(int y=0; y < m_grid; ++y) {
		ushort base = y * m_grid;
		for(int x = 0; x < m_grid-1; ++x){
			m_indices_wireframe.push_back(base + x);
			m_indices_wireframe.push_back(base + x + 1);
		}
	}

	// create indices for wireframe rendering (vertical lines)
	for(int x=0; x < m_grid; ++x) {
		for(int y=0; y < m_grid-1; ++y){
			ushort base = y * m_grid;
			m_indices_wireframe.push_back(base + x);
			m_indices_wireframe.push_back(base + x + m_grid);
		}
	}

	m_numPoints = m_grid * m_grid;		// save number of vertices
	m_points.resize(m_numPoints);
	m_coords.resize(m_numPoints);
	m_normals.resize(m_numPoints);

	// set light position
	m_lightVtx.resize(2);
	m_lightVtx[0] = m_lightEye;
	m_lightVtx[1] = m_lightTarget;

	// set light color
	m_lightColor.resize(2);
	m_lightColor[0] = vec3(1.0f, 1.0f, 1.0f);
	m_lightColor[1] = vec3(1.0f, 1.0f, 1.0f);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::getFaceNorms:
//
// Compute face normals for flat shading.
//
void HW4::getFaceNorms()
{
	for(int i = 0; i < m_grid-1; ++i) {
		for(int j = 0; j < m_grid-1; ++j) {
			// get vectors from geometry points
            vec3 &p0 = m_vertices[i][j];
            vec3 &p1 = m_vertices[i][j+1];
            vec3 &p2 = m_vertices[i+1][j];
            vec3 &p3 = m_vertices[i+1][j+1];

            vec3 v0 = p1-p0;
            vec3 v1 = p1-p2;
            vec3 v2 = p1-p3;

			// get triangle face normals from vectors & normalize them
            vec3 norm0 = vec3::crossProduct(v0,v1);
			norm0.normalize();

			// get triangle face normals from vectors & normalize them
            vec3 norm1 = vec3::crossProduct(v1,v2);
			norm1.normalize();

			m_faceNorms[0][i][j] = norm0;
			m_faceNorms[1][i][j] = norm1;
		}
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::getVertNorms:
//
// Compute vertex normals for smooth shading.
//
void HW4::getVertNorms()
{
	for(int i = 0; i < m_grid; ++i) {
		for(int j = 0; j < m_grid; ++j) {
            // for each vertex, average normals from all faces sharing vertex;
			// check each quadrant in turn
			vec3 avg;
			// right & above
			if(j < m_grid-1 && i < m_grid-1)
				avg += m_faceNorms[0][i][j];

			// right & below
			if(j < m_grid-1 && i > 0) {
				avg += m_faceNorms[0][i-1][j];
				avg += m_faceNorms[1][i-1][j];
			}

			// left & above
			if(j > 0 && i < m_grid-1 ) {
				avg += m_faceNorms[0][i][j-1];
				avg += m_faceNorms[1][i][j-1];
			}

			// left & below
			if(j > 0 && i > 0)
				avg += m_faceNorms[1][i-1][j-1];

			// normalize
			m_vertNorms[i][j] = avg.normalized();
		}
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::initBuffers:
//
// Initialize vertex buffer.
//
void
HW4::initBuffers()
{
	glGenBuffers(1, &m_vertexBuffer);
	uploadVertices();

	glGenBuffers(1, &m_texBuffer);
	uploadTexCoords();

	glGenBuffers(2, m_indicesBuffer);
	uploadIndices();

	glGenBuffers(1, &m_normalBuffer);
	uploadNormals();

	glGenBuffers(1, &m_colorBuffer);
	uploadColors();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::uploadVertices:
//
// Copy vertices (QVector3D) to GPU.
//
void
HW4::uploadVertices()
{
	// bind vertex buffer to the GPU and copy the vertices from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec3), &m_points[0], GL_DYNAMIC_DRAW);

	// enable buffer to be copied to the attribute vertex variable and specify data format
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer	 (ATTRIB_VERTEX, 3, GL_FLOAT, false, 0, NULL);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::uploadTexCoords:
//
// Copy texture coordinates (QVector2D) to GPU.
//
void
HW4::uploadTexCoords()
{
	// bind texture coord buffer to the GPU and copy the texture coordinates from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_texBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec2), &m_coords[0], GL_STATIC_DRAW);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
	glVertexAttribPointer	 (ATTRIB_TEXCOORD, 2, GL_FLOAT, false, 0, NULL);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::uploadNormals:
//
// Copy normals (QVector3D) to GPU.
//
void
HW4::uploadNormals()
{
	// bind texture coord buffer to the GPU and copy the texture coordinates from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec3), &m_normals[0], GL_DYNAMIC_DRAW);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, false, 0, NULL);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::uploadColors:
//
// Copy colors (QVector3D) to GPU.
//
void
HW4::uploadColors()
{
	// bind texture coord buffer to the GPU and copy the texture coordinates from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec3), &m_colors[0], GL_STATIC_DRAW);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glEnableVertexAttribArray(ATTRIB_COLOR);
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, false, 0, NULL);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::uploadIndices:
//
// Copy vertex indices to GPU for texture mapped triangles and wireframe.
//
void
HW4::uploadIndices()
{
	// bind indices buffer to the GPU and copy the indices from CPU to GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices_triangles.size() * sizeof(GLushort),
			&m_indices_triangles[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices_wireframe.size() * sizeof(GLushort),
			&m_indices_wireframe[0], GL_STATIC_DRAW);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::flatten:
//
// Copy 2D vertex array to 1D array that will be passed to GPU
//
void
HW4::flatten2D()
{
	for(int k=0, i=0; i < m_grid; ++i) {
		for(int j=0;  j < m_grid; ++j, k++) {
			m_points[k]  = m_vertices [i][j];
			m_coords[k]  = m_texCoords[i][j];
			m_normals[k] = m_vertNorms[i][j];
		}
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::changeDisplay:
//
// Change display mode.
//
void
HW4::changeDisplay(int val)
{
	m_displayMode = val;
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::changeGeom:
//
// Slot function to change geometry.
//
void
HW4::changeGeom(int val)
{
	// pause animation if waving
	if(m_wave) m_timer->stop();

	m_geometryMode = val;
	resetMesh();
	uploadVertices();
	uploadNormals();
	uploadTexCoords();
	uploadIndices();

	// draw
	updateGL();

	// restart animation if it was waving
	if(m_wave) m_timer->start(10, this);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::changeSize:
//
// Slot function to change grid size.
//
void
HW4::changeSize(int val)
{
    // pause animation if waving
    if(m_wave) m_timer->stop();

    // update slider and spinbox
    m_sliderSize->blockSignals(true);
    m_sliderSize->setValue(val);
    m_sliderSize->blockSignals(false);

    m_spinBoxSize->blockSignals(true);
    m_spinBoxSize->setValue(val);
    m_spinBoxSize->blockSignals(false);

    // update variable
    m_grid = val;

    // recompute new vertices on 2D grid and triangle indices
    initVertices();

    // recompute height field
    resetMesh();
    changeSpeed(m_speed);

    // copy to GPU
    uploadVertices();
    uploadTexCoords();
    uploadIndices();
    uploadColors();
    uploadNormals();

    // draw
    updateGL();

    // restart animation if it was waving
    if(m_wave) m_timer->start(10, this);
    //todo
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::setDT:
//
// Change dt based on animation speed and mesh size;
//
void
HW4::setDT()
{
	float scale = (float) m_grid / MAXGRID;
	scale *= scale;
	m_dt = (m_speed/100.) * scale;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::changeSpeed:
//
// Slot function to change animation speed.
//
void
HW4::changeSpeed(int val)
{
    m_speed = val;
    setDT();

    // update slider and spinbox
    m_sliderSpeed->blockSignals(true);
    m_sliderSpeed->setValue(val);
    m_sliderSpeed->blockSignals(false);

    m_spinBoxSpeed->blockSignals(true);
    m_spinBoxSpeed->setValue(val);
    m_spinBoxSpeed->blockSignals(false);
    // todo
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::startAnimation:
//
// Start animation timer.
//
void 
HW4::startAnimation()
{
	m_wave = true;
	m_timer->start(10, this);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::stopAnimation:
//
// Stop animation timer.
//
void
HW4::stopAnimation()
{
	m_wave = false;
	m_timer->stop();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::mousePressEvent:
//
// Mouse press event handler.
//
void HW4::mousePressEvent(QMouseEvent *e)
{
	// Saving mouse press position
	m_mousePressPosition = QVector2D(e->pos());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::mouseMoveEvent:
//
// Mouse move event handler.
//
void HW4::mouseMoveEvent(QMouseEvent *e)
{

	vec2 currentMouse = vec2(e->pos());
	vec2 diff = (currentMouse - m_mousePressPosition) * 0.001f;

	// Mouse release position - mouse press position
	m_mousePressPosition = currentMouse;
	if (!(e->modifiers() & Qt::CTRL)) {
		float theta = m_cameraRotation.x();
		if(m_cameraUp > 0.0f) theta += diff.x();
		else                  theta -= diff.x();
		float phi = m_cameraRotation.y();
		phi -= diff.y();
		// Keep phi within -2PI to +2PI for easy 'up' comparison
		// If phi is between 0 to PI or -PI to -2PI, 
		// make 'up' be positive Y, other wise make it negative Y
		checkPhi(phi, m_cameraUp);
		m_cameraRotation.setX(theta);
		m_cameraRotation.setY(phi);
		sphericalToCartesian(theta, phi, m_cameraEye);
		vec3 dir = (m_cameraTarget - m_cameraEye).normalized();
		vec3 worldUp = vec3(0.0f, m_cameraUp, 0.0f);
		vec3 right = vec3::crossProduct(dir, worldUp).normalized();
		vec3 up = vec3::crossProduct(right, dir).normalized();
		m_cameraView.setToIdentity();
		m_cameraView.lookAt(m_cameraEye, m_cameraTarget, up);
	} else {
		float theta = m_lightRotation.x();
		if(m_lightUp > 0.0f) theta -= diff.x();
		else                 theta += diff.x();
		float phi = m_lightRotation.y();
		phi += diff.y();
		// Keep phi within -2PI to +2PI for easy 'up' comparison
		// If phi is between 0 to PI or -PI to -2PI, 
		// make 'up' be positive Y, other wise make it negative Y
		checkPhi(phi, m_lightUp);
		m_lightRotation.setX(theta);
		m_lightRotation.setY(phi);
		sphericalToCartesian(theta, phi, m_lightEye);
		m_lightVtx[0] = m_lightEye;
		glBindBuffer(GL_ARRAY_BUFFER, m_lightVtxBuffer);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), &m_lightVtx[0], GL_STATIC_DRAW);
	}

	// update scene
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::wheelEvent:
//
// Mouse wheel event handler.
//
void
HW4::wheelEvent(QWheelEvent *event)
{
	float theta = m_cameraRotation.x();
	float phi   = m_cameraRotation.y();
	vec3 dir    = m_cameraTarget - m_cameraEye;

	// find camera position on sphere
	float cameraDistance = dir.length();
	cameraDistance -= event->delta() * 0.001f;
	float x = cameraDistance * cos(theta) * sin(phi);
	float z = cameraDistance * sin(theta) * sin(phi);
	float y = cameraDistance * cos(phi);

	// set m_cameraEye with the camera position
	m_cameraEye.setX(x);
	m_cameraEye.setY(y);
	m_cameraEye.setZ(z);

	// build camera axes: right (u), up (v), and normal (dir)
	dir.normalize();
	vec3 worldUp = vec3(0.0f, m_cameraUp, 0.0f);
	vec3 right = vec3::crossProduct(dir, worldUp).normalized();
	vec3 up = vec3::crossProduct(right, dir).normalized();

	// set camera lookat function
	m_cameraView.setToIdentity();
	m_cameraView.lookAt(m_cameraEye, m_cameraTarget, up);

	// update scene
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::cartesionToSpherical:
//
// Cartesian to spherical coordinate system transformation.
// Input: 3D point p.
// Output: latitude and longitude angles.
//
vec2
HW4::cartesianToSpherical(vec3 &p)
{
	vec2 angles;
	double dist = sqrt(p.x()*p.x() + p.z()*p.z());
	angles.setX(atan2(p.z(), p.x()));
	angles.setY(atan2(dist,  p.y()));
	return angles;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::sphericaltoCartesian:
//
// Spherical to Cartesian coordinate system transformation.
// Input: latitude and longitude angles.
// Output: Cartesian coordinate in 3D vector eye.
//
void
HW4::sphericalToCartesian(float theta, float phi, vec3 &eye)
{
	vec3 dir = -eye;
	float r = dir.length();

	float x = r * cos(theta) * sin(phi);
	float z = r * sin(theta) * sin(phi);
	float y = r * cos(phi);

	eye.setX(x);
	eye.setY(y);
	eye.setZ(z);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW4::checkPhi:
//
// Constrain phi to -2pi to pi for easy 'up' comparison.
//
void 
HW4::checkPhi(float &phi, float &up)
{
	// keep phi within -2PI to +2PI for easy 'up' comparison
	float PI2 = (float)(2 * M_PI);
	if(phi > PI2) {
		phi -= (float)(PI2);
	}
	else if(phi < -PI2)
		phi += PI2;

	if((phi > 0 && phi < M_PI) || (phi < -M_PI && phi > -PI2))
		up =  1.0f;
	else    up = -1.0f;
}
