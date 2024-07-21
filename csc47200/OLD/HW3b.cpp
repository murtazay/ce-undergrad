// ================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW3b.cpp - HW3b class
//
// Written by: George Wolberg, 2015
// Modified by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ================================================================

#include "HW3b.h"

enum {MV, PROJ, SAMPLER};
enum {WIREFRAME, TEXTURED, TEXTURED_WIREFRAME};
enum {WEAK, NORMAL, STRONG};
enum {FLAT, SPIKE, DIAGONALWALL, SIDEWALL, HOLE, MIDDLEBLOCK, DIAGONALBLOCK, CORNERBLOCK, HILL, HILLFOUR};
#define SQRTOFTWOINV 1.0 / 1.414213562


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::HW3b:
//
// HW3b constructor.
//
HW3b::HW3b(QWidget *parent)
	: HW(parent)
{
	m_timer = new QBasicTimer;

	// reset parameters
    m_grid		= 32;
	m_displayMode	= TEXTURED_WIREFRAME;
	m_geometryMode	= MIDDLEBLOCK;
	m_speed		= 1;
	m_angularSpeed	= -30.0f;
	m_rotation	= QQuaternion::fromAxisAndAngle(QVector3D(1.0f,1.0f,0.0f), m_angularSpeed);
	m_wave		= false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
HW3b::initializeGL()
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

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
	glColor3f   (1.0, 1.0, 0.0);		// set foreground color
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
HW3b::resizeGL(int w, int h)
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

	// copy matrix to vertex shader
	glUseProgram(m_program[0].programId());
	glUniformMatrix4fv(m_uniform[0][MV],   1, GL_FALSE, m_modelview.constData());
	glUniformMatrix4fv(m_uniform[0][PROJ], 1, GL_FALSE, m_projection.constData());
	glUseProgram(m_program[1].programId());
	glUniformMatrix4fv(m_uniform[1][MV],   1, GL_FALSE, m_modelview.constData());
	glUniformMatrix4fv(m_uniform[1][PROJ], 1, GL_FALSE, m_projection.constData());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::paintGL:
//
// Update GL scene.
//
void
HW3b::paintGL()
{
	// clear canvas with background color and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// move back object and rotate so that z-axis points upward
	m_modelview.setToIdentity();
	m_modelview.translate(0.0f, 0.0f, -5.0f);
	m_modelview.rotate(m_rotation);

	// draw textured triangles
	if(m_displayMode == TEXTURED || m_displayMode == TEXTURED_WIREFRAME) {
		// draw textured surface
        glUseProgram(m_program[0].programId());		// bind the glsl progam 0
        glUniformMatrix4fv(m_uniform[0][MV], 1, GL_FALSE, m_modelview.constData());	// pass m_modelview
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[0]);
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) m_indices_triangles.size(), GL_UNSIGNED_SHORT, 0);
	}

	// draw wireframe
	if(m_displayMode == WIREFRAME || m_displayMode == TEXTURED_WIREFRAME) {
		// draw wireframe
        glUseProgram(m_program[1].programId());		// bind the glsl progam 1
        glUniformMatrix4fv(m_uniform[1][MV], 1, GL_FALSE, m_modelview.constData());	// pass m_modelview
		glLineWidth(2.0f);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer[1]);
		glDrawElements(GL_LINES, (GLsizei) m_indices_wireframe.size(), GL_UNSIGNED_SHORT, 0);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
HW3b::controlPanel()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Homework 3b");

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
// HW3b::reset:
//
// Reset parameters.
//
void
HW3b::reset()
{
	// reset parameters
	m_grid		= 32;
	m_displayMode	= TEXTURED_WIREFRAME;
	m_geometryMode	= MIDDLEBLOCK;
	m_speed		= 1;
    m_angularSpeed	= -30.0f;
	m_rotation	= QQuaternion::fromAxisAndAngle(QVector3D(1.0f,1.0f,0.0f), m_angularSpeed);
	m_wave		= false;

	m_comboBoxDisplay->setCurrentIndex(m_displayMode);
	m_comboBoxGeom->setCurrentIndex(m_geometryMode);
	m_sliderSize->setValue(m_grid);
	m_sliderSpeed->setValue(m_speed);

    stopAnimation();

	// init XY vertices in mesh and texture coords
    initVertices();

	// init Z values in height field
	resetMesh();

	// set dt based on m_grid size and user selection for m_speed
    setDT();

    // copy to GPU and redraw
    uploadVertices();
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::resetMesh:
//
// Reset height field based on geometry mode flag.
//
void
HW3b::resetMesh()
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

	// copy 2D vertex array to 1D array that will be passed to GPU
	flatten2D();

	// restart animation
	if(m_wave) m_timer->start(10, this);
    updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::getForce:
//
// Compute force values based on differences with eight neighbors
// around each pixel.
//
void HW3b::getForce(void)
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
// HW3b::getVelocity:
//
// Compute velocity values based on forces at each pixel.
//
void HW3b::getVelocity()
{
	// update m_veloc[][]
    for(int y = 0; y < m_grid; ++y){
        for(int x = 0; x < m_grid; ++x){
            m_veloc[y][x] += m_force[y][x] * m_dt;
        }
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::getPosition:
//
// Compute vertex positions values based on velocity at each pixel.
//
void HW3b::getPosition()
{
	// update m_vertices[][]
    for(int y = 0; y < m_grid; ++y){
        for(int x = 0; x < m_grid; ++x){
            m_vertices[y][x].setZ(m_vertices[y][x].z() + m_veloc[y][x]);
        }
    }

	// copy 2D vertex array to 1D array that will be passed to GPU
	flatten2D();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::timerEvent:
//
// Virtual function called when timer times out.
// We use it to stop timer, recompute surface heights, and restart timer.
//
void 
HW3b::timerEvent(QTimerEvent *e)
{
	// avoid compiler warning for unused event e
	Q_UNUSED(e);

	// pause animation to reset grid without interruption by timer
	m_timer->stop();

	// recompute surface height
	getForce();
	getVelocity();
    getPosition();

	// copy to GPU and redraw
	uploadVertices();
	updateGL();

	// restart animation
	m_timer->start(10, this);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initTexture:
//
// Initialize texture.
//
void
HW3b::initTexture()
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
// HW3b::initShaders:
//
// Initialize vertex and fragment shaders.
//
void
HW3b::initShaders()
{
	initShader1();
	initShader2();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initShader1:
//
// Initialize vertex and fragment shaders for texture mapping.
//
void
HW3b::initShader1()
{
    // compile vertex shader
    if(!m_program[0].addShaderFromSourceFile(QGLShader::Vertex, ":/vshader3b1.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program[0].addShaderFromSourceFile(QGLShader::Fragment, ":/fshader3b1.glsl")) {
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

    // get storage location of u_Modelview in vertex shader
    m_uniform[0][MV] = glGetUniformLocation(m_program[0].programId(), "u_Modelview");
    if((int) m_uniform[0][MV] < 0) {
        qDebug() << "Failed to get the storage location of u_ModelMatrix";
        exit(-1);
    }

    // get storage location of u_projection in vertex shader
    m_uniform[0][PROJ] = glGetUniformLocation(m_program[0].programId(), "u_Projection");
    if((int) m_uniform[0][PROJ] < 0) {
        qDebug() << "Failed to get the storage location of u_Projection";
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
    m_modelview.setToIdentity();
    m_projection.setToIdentity();
    glUniformMatrix4fv(m_uniform[0][MV], 1, GL_FALSE, m_modelview.constData());
    glUniformMatrix4fv(m_uniform[0][PROJ], 1, GL_FALSE, m_projection.constData());
    glUniform1i(m_uniform[0][SAMPLER], GL_TEXTURE0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initShader2:
//
// Initialize vertex and fragment shaders for wireframe rendering.
//
void
HW3b::initShader2()
{
    // compile vertex shader
    if(!m_program[1].addShaderFromSourceFile(QGLShader::Vertex, ":/vshader3b2.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program[1].addShaderFromSourceFile(QGLShader::Fragment, ":/fshader3b2.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }

    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program[1].programId(), ATTRIB_VERTEX, "a_Position");

    // link shader pipeline; attribute bindings go into effect at this point
    if(!m_program[1].link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }

    // get storage location of u_Modelview in vertex shader
    m_uniform[1][MV] = glGetUniformLocation(m_program[1].programId(), "u_Modelview");
    if((int) m_uniform[1][MV] < 0) {
        qDebug() << "Failed to get the storage location of u_ModelMatrix";
        exit(-1);
    }

    // get storage location of u_projection in vertex shader
    m_uniform[1][PROJ] = glGetUniformLocation(m_program[1].programId(), "u_Projection");
    if((int) m_uniform[1][PROJ] < 0) {
        qDebug() << "Failed to get the storage location of u_Projection";
        exit(-1);
    }

    // bind the glsl program
    glUseProgram(m_program[1].programId());

    // init model matrix; pass it to vertex shader
    m_modelview.setToIdentity();
    m_projection.setToIdentity();
    glUniformMatrix4fv(m_uniform[1][MV], 1, GL_FALSE, m_modelview.constData());
    glUniformMatrix4fv(m_uniform[1][PROJ], 1, GL_FALSE, m_projection.constData());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initVertices:
//
// Initialize  (x,y) positions of vertices so that they lie in [-1,1] range.
// Initialize texture coordinates.
// Initialize vertex indices for texture mapped triangles and wireframe.
// Surface height values (z) are evaluated in resetMesh().
//
void
HW3b::initVertices()
{
	// map the grid coordinates to [-1.0, 1.0]; function called when m_grid changes
	// flip y-coordinates because array orientation is upside down wrt graphics coordinates
	int size = m_grid - 1;
	for(int y=0; y < m_grid; ++y) {
		for(int x=0; x < m_grid; ++x) {
			QVector3D &vec = m_vertices[y][x];
			vec.setX(-1 + (2. * x / size));
			vec.setY(-1 + (2. * (size-y) / size));

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

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::initBuffers:
//
// Initialize vertex buffer.
//
void
HW3b::initBuffers()
{
	glGenBuffers(1, &m_vertexBuffer);
	uploadVertices();

    glGenBuffers(2, &m_texBuffer);
	uploadTexCoords();

    glGenBuffers(3, m_indicesBuffer);
	uploadIndices();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::uploadVertices:
//
// Copy vertices (QVector3D) to GPU.
//
void
HW3b::uploadVertices()
{
	// bind vertex buffer to the GPU and copy the vertices from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec3), &m_points[0], GL_STATIC_DRAW);

	// enable buffer to be copied to the attribute vertex variable and specify data format
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer	 (ATTRIB_VERTEX, 3, GL_FLOAT, false, 0, NULL);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::uploadTexCoords:
//
// Copy texture coordinates (QVector2D) to GPU.
//
void
HW3b::uploadTexCoords()
{
	// bind texture coord buffer to the GPU and copy the texture coordinates from CPU to GPU
	glBindBuffer(GL_ARRAY_BUFFER, m_texBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_numPoints*sizeof(vec2), &m_coords[0], GL_STATIC_DRAW);

	// enable buffer to be copied to the attribute texture coord variable and specify data format
	glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
    glVertexAttribPointer	 (ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::uploadIndices:
//
// Copy vertex indices to GPU for texture mapped triangles and wireframe.
//
void
HW3b::uploadIndices()
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
// HW3b::flatten:
//
// Copy 2D vertex array to 1D array that will be passed to GPU
//
void
HW3b::flatten2D()
{
	for(int k=0, i=0; i < m_grid; ++i) {
	    for(int j=0;  j < m_grid; ++j, k++) {
            m_points[k] = m_vertices [i][j];
            m_coords[k] = m_texCoords[i][j];
	    }
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::changeDisplay:
//
// Change display mode.
//
void
HW3b::changeDisplay(int val)
{

	m_displayMode = val;
	updateGL();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::changeGeom:
//
// Slot function to change geometry.
//
void
HW3b::changeGeom(int val)
{
	// pause animation if waving
	if(m_wave) m_timer->stop();

	m_geometryMode = val;
	resetMesh();
	uploadVertices();

	// draw
	updateGL();

	// restart animation if it was waving
	if(m_wave) m_timer->start(10, this);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::changeSize:
//
// Slot function to change grid size.
//
void
HW3b::changeSize(int val)
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

	// draw
	updateGL();

	// restart animation if it was waving
	if(m_wave) m_timer->start(10, this);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::setDT:
//
// Change dt based on animation speed and mesh size;
//
void
HW3b::setDT()
{
	float scale = (float) m_grid / MAXGRID;
	scale *= scale;
	m_dt = (m_speed/100.) * scale;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::changeSpeed:
//
// Slot function to change animation speed.
//
void
HW3b::changeSpeed(int val)
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
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::startAnimation:
//
// Start animation timer.
//
void 
HW3b::startAnimation()
{
	m_wave = true;
	m_timer->start(10, this);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::stopAnimation:
//
// Stop animation timer.
//
void
HW3b::stopAnimation()
{
	m_wave = false;
	m_timer->stop();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::mousePressEvent:
//
// Mouse press event handler.
//
void HW3b::mousePressEvent(QMouseEvent *e)
{
	// save mouse press position
	m_mousePressPosition = QVector2D(e->pos());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW3b::mouseMoveEvent:
//
// Mouse move event handler.
//
void HW3b::mouseMoveEvent(QMouseEvent *e)
{

	// mouse release position - mouse press position
	QVector2D diff = QVector2D(e->pos()) - m_mousePressPosition;

	// rotation axis is perpendicular to the mouse position difference vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// accelerate angular speed relative to the length of the mouse sweep
	float acc = diff.length() / 200.0;

	// calculate new rotation axis as weighted sum
	m_rotationAxis = (n * acc).normalized();
	m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, acc) * m_rotation;

	// draw
	updateGL();
}
