#include "Sharpen.h"
#include <time.h>

enum{
    SIZE,
    STEP,
    FACTOR,
    SAMPLER
};

Sharpen::Sharpen(QWidget *parent) : HW(parent)
{
    m_size = 1.f/512.f;
    m_factor = 1.f;
}

QGroupBox *Sharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Sharpen");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *labelx = new QLabel;
    labelx->setText("Filter Size");
    QLabel *labely = new QLabel;
    labely->setText("Factor");

    // create sliders
    m_slider[0] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[0]->setMinimum(1);
    m_slider[0]->setMaximum(7);
    m_slider[0]->setValue(1);
    m_slider[0]->setTickPosition(QSlider::TicksBelow);
    m_slider[0]->setTickInterval(1);

    m_slider[1] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[1]->setMinimum(1);
    m_slider[1]->setMaximum(15);
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
    m_spinBox[1]->setMinimum(1);
    m_spinBox[1]->setMaximum(15);
    m_spinBox[1]->setValue(1);

    // init signal/slot connections
    connect(m_slider[0]  , SIGNAL(valueChanged(int)),         this,   SLOT(setSizeSlider(int)));
    connect(m_spinBox[0] , SIGNAL(valueChanged(int)),         this,   SLOT(setSizeSpinBox(int)));

    connect(m_slider[1]  , SIGNAL(valueChanged(int)),         this,   SLOT(setFactor(int)));
    connect(m_slider[1]  , SIGNAL(valueChanged(int)), m_spinBox[1],   SLOT(setValue(int)));
    connect(m_spinBox[1] , SIGNAL(valueChanged(int)), m_slider[1] ,   SLOT(setValue(int)));
    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(      labelx, 1, 0);
    layout->addWidget( m_slider[0], 1, 1);
    layout->addWidget(m_spinBox[0], 1, 2);
    layout->addWidget(      labely, 2, 0);
    layout->addWidget( m_slider[1], 2, 1);
    layout->addWidget(m_spinBox[1], 2, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

void Sharpen::reset()
{
    m_slider[0]->setValue(1);
    m_slider[1]->setValue(1);
}

void Sharpen::initVertexBuffer()
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

void Sharpen::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderSharpen.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderSharpen.glsl")) {
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
    m_uniforms[SIZE] = glGetUniformLocation(m_program.programId(),"u_size");
    if((int) m_uniforms[SIZE] < 0){
        qDebug() << "Failed to get the storage location of u_size";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[STEP] = glGetUniformLocation(m_program.programId(),"u_step");
    if((int) m_uniforms[STEP] < 0){
        qDebug() << "Failed to get the storage location of u_step";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[FACTOR] = glGetUniformLocation(m_program.programId(),"u_factor");
    if((int) m_uniforms[FACTOR] < 0){
        qDebug() << "Failed to get the storage location of u_factor";
        exit(-1);
    }

    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[SIZE]   , m_size);
    glUniform1f(m_uniforms[STEP]   , 1.f/512.f);
    glUniform1f(m_uniforms[FACTOR] , m_factor);
    glUniform1f(m_uniforms[SAMPLER], GL_TEXTURE0);
}

void Sharpen::initTexture()
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

void Sharpen::setSizeSlider(int)
{
    m_spinBox[0]->setValue((m_slider[0]->value() * 2) + 1);
    m_size = ((m_spinBox[0]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[SIZE]  , m_size);
    updateGL();
}

void Sharpen::setSizeSpinBox(int)
{
    m_slider[0]->setValue((m_spinBox[0]->value()-1)/2);
    m_size = ((m_spinBox[0]->value()-1)/2)/512.f;
    glUniform1f(m_uniforms[SIZE]  , m_size);
    updateGL();
}

void Sharpen::setFactor(int)
{
    m_factor = m_slider[1]->value();
    glUniform1f(m_uniforms[FACTOR], m_factor);
    updateGL();
}

void Sharpen::initializeGL()
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

void Sharpen::resizeGL(int w, int h)
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

void Sharpen::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quadss
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "SHARPEN:" <<(float)t/CLOCKS_PER_SEC;
}
