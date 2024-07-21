#include "Blur.h"
#include <time.h>

enum{
    XSIZE,
    YSIZE,
    XSTEP,
    YSTEP,
    SAMPLER
};

Blur::Blur(QWidget *parent):HW(parent)
{
    m_xsize = 1.f/512.f;
    m_ysize = 1.f/512.f;
}

QGroupBox *Blur::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Blur");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *labelx = new QLabel;
    labelx->setText("X Size");
    QLabel *labely = new QLabel;
    labely->setText("Y Size");

    // create sliders
    m_slider[0] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[0]->setMinimum(1);
    m_slider[0]->setMaximum(7);
    m_slider[0]->setValue(1);
    m_slider[0]->setTickPosition(QSlider::TicksBelow);
    m_slider[0]->setTickInterval(1);

    m_slider[1] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[1]->setMinimum(1);
    m_slider[1]->setMaximum(7);
    m_slider[1]->setValue(1);
    m_slider[1]->setTickPosition(QSlider::TicksBelow);
    m_slider[1]->setTickInterval(1);

    // create spinbox
    m_spinBox[0] = new QSpinBox(m_ctrlGrp);
    m_spinBox[0]->setSingleStep(2);
    m_spinBox[0]->setMinimum(3);
    m_spinBox[0]->setMaximum(15);
    m_spinBox[0]->setValue(3);

    m_spinBox[1] = new QSpinBox(m_ctrlGrp);
    m_spinBox[1]->setSingleStep(2);
    m_spinBox[1]->setMinimum(3);
    m_spinBox[1]->setMaximum(15);
    m_spinBox[1]->setValue(3);

    // create checkbox
    m_checkBox = new QCheckBox("Link");
    m_checkBox->setChecked(false);

    // init signal/slot connections
    connect(m_slider[0]  , SIGNAL(valueChanged(int)),         this,   SLOT(setXSlider(int)));
    connect(m_spinBox[0] , SIGNAL(valueChanged(int)),         this,   SLOT(setXSpinBox(int)));

    connect(m_slider[1]  , SIGNAL(valueChanged(int)),         this,   SLOT(setYSlider(int)));
    connect(m_spinBox[1] , SIGNAL(valueChanged(int)),         this,   SLOT(setYSpinBox(int)));

    connect(m_checkBox   , SIGNAL(clicked(bool))    ,         this,   SLOT(setLink(bool)));

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(      labelx, 1, 0);
    layout->addWidget( m_slider[0], 1, 1);
    layout->addWidget(m_spinBox[0], 1, 2);
    layout->addWidget(      labely, 2, 0);
    layout->addWidget( m_slider[1], 2, 1);
    layout->addWidget(m_spinBox[1], 2, 2);
    layout->addWidget(m_checkBox  , 3, 0);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

void Blur::reset()
{
    m_slider[0]->setValue(1);
    m_slider[1]->setValue(1);
}

void Blur::initVertexBuffer()
{
    // set flag for creating buffers (1st time only)
    static bool flag = 1;

    // verify that we have valid vertex/color buffers
    static GLuint vertexBuffer = -1;
    static GLuint textureBuffer = -1;
    if(flag) {	// create vertex and color buffers
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &textureBuffer);
        flag = 0;	// reset flag
    }

    // init geometry data
    const vec2 vertices[] = {
        vec2( .90f, .90f),
        vec2(-.90f, .90f),
        vec2(-.90f,-.90f),
        vec2( .90f,-.90f)
    };
    const vec2 coords[] = {
        vec2(1.f,1.f),
        vec2(0.f,1.f),
        vec2(0.f,0.f),
        vec2(1.f,0.f)
    };

    // record number of points
    m_numofpoints = 4;

    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // copy the vertices from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numofpoints*sizeof(vec2), &vertices[0], GL_STATIC_DRAW);

    // enable the assignment of attribute vertex variable
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // assign the buffer object to the attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);


    // bind texture buffer to the GPU; all future buffer ops operate on this buffer
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    // copy the texture from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, m_numofpoints*sizeof(vec2), &coords[0], GL_STATIC_DRAW);

    // enable the assignment of attribute texture variable
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);

    // assign the buffer object to the attribute texture variable
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL);
}

void Blur::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderBlur.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderBlur.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }


    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program.programId(), ATTRIB_VERTEX, "a_Position");


    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_TEXTURE_POSITION will modify the value of "a_TexCoord")
    glBindAttribLocation(m_program.programId(), ATTRIB_TEXTURE_POSITION, "a_TexCoord");

    // link shader pipeline
    if(!m_program.link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }

    // bind the glsl progam
    glUseProgram(m_program.programId());

    // get storage location of u_brightness in fragment shader
    m_uniforms[XSIZE] = glGetUniformLocation(m_program.programId(),"u_xsize");
    if((int) m_uniforms[XSIZE] < 0){
        qDebug() << "Failed to get the storage location of u_xsize";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[YSIZE] = glGetUniformLocation(m_program.programId(),"u_ysize");
    if((int) m_uniforms[YSIZE] < 0){
        qDebug() << "Failed to get the storage location of u_ysize";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[XSTEP] = glGetUniformLocation(m_program.programId(),"u_xstep");
    if((int) m_uniforms[XSTEP] < 0){
        qDebug() << "Failed to get the storage location of u_xstep";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[YSTEP] = glGetUniformLocation(m_program.programId(),"u_ystep");
    if((int) m_uniforms[YSTEP] < 0){
        qDebug() << "Failed to get the storage location of u_ystep";
        exit(-1);
    }

    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[XSIZE]  , m_xsize);
    glUniform1f(m_uniforms[YSIZE]  , m_ysize);
    glUniform1f(m_uniforms[XSTEP]  , 1.f/512.f);
    glUniform1f(m_uniforms[YSTEP]  , 1.f/512.f);
    glUniform1f(m_uniforms[SAMPLER], GL_TEXTURE0);
}

void Blur::initTexture()
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

void Blur::setXSlider(int)
{
    m_spinBox[0]->setValue((m_slider[0]->value() * 2) + 1);
    m_xsize = ((m_spinBox[0]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[XSIZE]  , m_xsize);
    updateGL();
}

void Blur::setYSlider(int)
{
    m_spinBox[1]->setValue((m_slider[1]->value() * 2) + 1);
    m_ysize = ((m_spinBox[1]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[YSIZE]  , m_ysize);
    updateGL();
}

void Blur::setXSpinBox(int)
{
    m_slider[0]->setValue((m_spinBox[0]->value()-1)/2);
    m_xsize = ((m_spinBox[0]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[XSIZE]  , m_xsize);
    updateGL();
}

void Blur::setYSpinBox(int)
{
    m_slider[1]->setValue((m_spinBox[1]->value()-1)/2);
    m_ysize = ((m_spinBox[1]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[YSIZE]  , m_ysize);
    updateGL();
}

void Blur::setLink(bool)
{
    if(m_checkBox->isChecked()){
        m_slider[1]->setValue(m_slider[0]->value());
        m_slider[1]->setEnabled(false);
        m_spinBox[1]->setEnabled(false);
        connect(m_slider[0], SIGNAL(valueChanged(int)), m_slider[1], SLOT(setValue(int)));
        m_ysize = ((m_spinBox[1]->value()-1)/2)/512.f;
        glUniform1f(m_uniforms[YSIZE]  , m_ysize);
        updateGL();
    }
    else{
        m_slider[1]->setEnabled(true);
        m_spinBox[1]->setEnabled(true);
        disconnect(m_slider[0], SIGNAL(valueChanged(int)), m_slider[1], SLOT(setValue(int)));
    }
}

void Blur::initializeGL()
{
    // initialize GL function resolution for current context
    initializeGLFunctions();

    // init texture
    initTexture();

    // init vertex and fragment shaders
    initShaders();

    // initialize vertex buffer and write positions to vertex shader
    initVertexBuffer();
}

void Blur::resizeGL(int w, int h)
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

void Blur::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quads
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "BLUR:" <<(float)t/CLOCKS_PER_SEC;
}
