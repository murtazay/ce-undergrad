#include "Btct.h"
#include <time.h>

enum{
    BRIGHTNESS,
    CONTRAST,
    REFRENCE,
    SAMPLER
};


BTCT::BTCT(QWidget *parent) : HW(parent)
{
    m_brightness = 0.f;
    m_contrast   = 1.f;
    m_refrence   = 128.f/256.f;
}

QGroupBox *BTCT::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Contrast");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label[i]
    QLabel* label[3];
    label[BRIGHTNESS] = new QLabel;
    label[CONTRAST] = new QLabel;
    label[REFRENCE] = new QLabel;

    label[BRIGHTNESS]->setText("Brightness");
    label[CONTRAST]->setText("Contrast");
    label[REFRENCE]->setText("Refrence Point");

    // create sliders
    m_slider[BRIGHTNESS] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[BRIGHTNESS]->setMinimum(-100);
    m_slider[BRIGHTNESS]->setMaximum( 100);
    m_slider[BRIGHTNESS]->setValue(0);
    m_slider[CONTRAST]   = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[CONTRAST]->setMinimum(-50);
    m_slider[CONTRAST]->setMaximum( 50);
    m_slider[CONTRAST]->setValue(0);
    m_slider[REFRENCE]   = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[REFRENCE]->setMinimum(0);
    m_slider[REFRENCE]->setMaximum(255);
    m_slider[REFRENCE]->setValue(128);
    for(int i = 0; i < 3; i++){
        m_slider[i]->setTickPosition(QSlider::TicksBelow);
        m_slider[i]->setTickInterval(25);
    }

    // create spinbox
    m_spinBox[BRIGHTNESS] = new QSpinBox(m_ctrlGrp);
    m_spinBox[BRIGHTNESS]->setMinimum(-100);
    m_spinBox[BRIGHTNESS]->setMaximum( 100);
    m_spinBox[BRIGHTNESS]->setValue(0);
    m_spinBox[CONTRAST] = new QSpinBox(m_ctrlGrp);
    m_spinBox[CONTRAST]->setMinimum(-50);
    m_spinBox[CONTRAST]->setMaximum( 50);
    m_spinBox[CONTRAST]->setValue(0);
    m_spinBox[REFRENCE] = new QSpinBox(m_ctrlGrp);
    m_spinBox[REFRENCE]->setMinimum(  0);
    m_spinBox[REFRENCE]->setMaximum(255);
    m_spinBox[REFRENCE]->setValue(128);

    // init signal/slot connections
    connect(m_slider[BRIGHTNESS], SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
    connect(m_slider[CONTRAST], SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
    connect(m_slider[REFRENCE], SIGNAL(valueChanged(int)), this, SLOT(setRefrence(int)));
    for(int i = 0; i < 3; i++){
        connect(m_slider [i], SIGNAL(valueChanged(int)), m_spinBox[i], SLOT(setValue(int)));
        connect(m_spinBox[i], SIGNAL(valueChanged(int)), m_slider [i], SLOT(setValue(int)));
    }

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(    label[BRIGHTNESS], 0, 0);
    layout->addWidget( m_slider[BRIGHTNESS], 0, 1);
    layout->addWidget(m_spinBox[BRIGHTNESS], 0, 2);
    layout->addWidget(      label[CONTRAST], 1, 0);
    layout->addWidget(   m_slider[CONTRAST], 1, 1);
    layout->addWidget(  m_spinBox[CONTRAST], 1, 2);
    layout->addWidget(      label[REFRENCE], 2, 0);
    layout->addWidget(   m_slider[REFRENCE], 2, 1);
    layout->addWidget(  m_spinBox[REFRENCE], 2, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

void BTCT::reset()
{
    m_slider[BRIGHTNESS]->setValue(0);
    m_slider[CONTRAST]->setValue(0);
    m_slider[REFRENCE]->setValue(128);
}

void BTCT::initVertexBuffer()
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

void BTCT::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderBtct.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderBtct.glsl")) {
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
    m_uniforms[BRIGHTNESS] = glGetUniformLocation(m_program.programId(),"u_brightness");
    if((int) m_uniforms[BRIGHTNESS] < 0){
        qDebug() << "Failed to get the storage location of u_brightness";
        exit(-1);
    }


    // get storage location of u_brightness in fragment shader
    m_uniforms[CONTRAST] = glGetUniformLocation(m_program.programId(),"u_contrast");
    if((int) m_uniforms[CONTRAST] < 0){
        qDebug() << "Failed to get the storage location of u_contrast";
        exit(-1);
    }


    // get storage location of u_brightness in fragment shader
    m_uniforms[REFRENCE] = glGetUniformLocation(m_program.programId(),"u_refrence");
    if((int) m_uniforms[REFRENCE] < 0){
        qDebug() << "Failed to get the storage location of u_refrence";
        exit(-1);
    }


    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[BRIGHTNESS], m_brightness);
    glUniform1f(m_uniforms[CONTRAST]  ,   m_contrast);
    glUniform1f(m_uniforms[REFRENCE]  ,   m_refrence);
    glUniform1f(m_uniforms[SAMPLER]   ,  GL_TEXTURE0);
}

void BTCT::initTexture()
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

void BTCT::setBrightness(int brt)
{
    m_brightness = brt/100.f;
    glUniform1f(m_uniforms[BRIGHTNESS],m_brightness);
    updateGL();
}

void BTCT::setContrast(int cnt)
{
    if(cnt >= 0){
        m_contrast = (cnt/25.) + 1;
    }
    else{
        m_contrast = (cnt/50.) + 1;
    }
    glUniform1f(m_uniforms[CONTRAST],m_contrast);
    updateGL();
}

void BTCT::setRefrence(int ref)
{
    m_refrence = ref/256.f;
    glUniform1f(m_uniforms[REFRENCE],m_refrence);
    updateGL();
}

void BTCT::initializeGL()
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

void BTCT::resizeGL(int w, int h)
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

void BTCT::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quadss
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "Contrast:" << (float) t / CLOCKS_PER_SEC;
}
