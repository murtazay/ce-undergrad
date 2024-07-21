#include "Convolution.h"
#include <time.h>

enum{
    TL,
    TM,
    TR,
    L,
    M,
    R,
    BL,
    BM,
    BR,
    PAD,
    STEP,
    SAMPLER
};

Convolution::Convolution(QWidget *parent) : HW(parent)
{
    for(int i=0;i<9;++i){
        m_kernal[i] = 0.f;
    }
    m_kernal[M] = 1.f;
}

QGroupBox *Convolution::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Convolution");

    for(int i = 0; i < 9; ++i){
        m_spinBox[i] = new QSpinBox(m_ctrlGrp);
        m_spinBox[i]->setMinimum(-10);
        m_spinBox[i]->setMaximum( 10);
        m_spinBox[i]->setValue(0);
    }
    m_spinBox[M]->setValue(1);

    m_button = new QPushButton("Update");

    connect(m_spinBox[TL], SIGNAL(valueChanged(int)), this, SLOT(setTL(int)));
    connect(m_spinBox[TM], SIGNAL(valueChanged(int)), this, SLOT(setTM(int)));
    connect(m_spinBox[TR], SIGNAL(valueChanged(int)), this, SLOT(setTR(int)));
    connect(m_spinBox[L] , SIGNAL(valueChanged(int)), this, SLOT(setL(int)));
    connect(m_spinBox[M] , SIGNAL(valueChanged(int)), this, SLOT(setM(int)));
    connect(m_spinBox[R] , SIGNAL(valueChanged(int)), this, SLOT(setR(int)));
    connect(m_spinBox[BL], SIGNAL(valueChanged(int)), this, SLOT(setBL(int)));
    connect(m_spinBox[BM], SIGNAL(valueChanged(int)), this, SLOT(setBM(int)));
    connect(m_spinBox[BR], SIGNAL(valueChanged(int)), this, SLOT(setBR(int)));
    connect(m_button     , SIGNAL(pressed())        , this, SLOT(updateGL()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_spinBox[TL], 0, 0);
    layout->addWidget(m_spinBox[TM], 0, 1);
    layout->addWidget(m_spinBox[TR], 0, 2);
    layout->addWidget(m_spinBox[ L], 1, 0);
    layout->addWidget(m_spinBox[ M], 1, 1);
    layout->addWidget(m_spinBox[ R], 1, 2);
    layout->addWidget(m_spinBox[BL], 2, 0);
    layout->addWidget(m_spinBox[BM], 2, 1);
    layout->addWidget(m_spinBox[BR], 2, 2);
    layout->addWidget(m_button     , 3, 0, 3, 3);

    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

void Convolution::reset()
{
    for(int i = 0; i < 9; ++i){
        m_spinBox[i]->setValue(0);
    }
    m_spinBox[M]->setValue(1);
    m_button->pressed();
}

void Convolution::initVertexBuffer()
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

void Convolution::initShaders()
{

    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderConvolution.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderConvolution.glsl")) {
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
    m_uniforms[TL] = glGetUniformLocation(m_program.programId(),"u_topl");
    if((int) m_uniforms[TL] < 0){
        qDebug() << "Failed to get the storage location of u_topl";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[TM] = glGetUniformLocation(m_program.programId(),"u_topm");
    if((int) m_uniforms[TM] < 0){
        qDebug() << "Failed to get the storage location of u_topm";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[TR] = glGetUniformLocation(m_program.programId(),"u_topr");
    if((int) m_uniforms[TR] < 0){
        qDebug() << "Failed to get the storage location of u_topr";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[L] = glGetUniformLocation(m_program.programId(),"u_left");
    if((int) m_uniforms[L] < 0){
        qDebug() << "Failed to get the storage location of u_left";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[M] = glGetUniformLocation(m_program.programId(),"u_mid");
    if((int) m_uniforms[M] < 0){
        qDebug() << "Failed to get the storage location of u_mid";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[R] = glGetUniformLocation(m_program.programId(),"u_right");
    if((int) m_uniforms[R] < 0){
        qDebug() << "Failed to get the storage location of u_right";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[BL] = glGetUniformLocation(m_program.programId(),"u_botl");
    if((int) m_uniforms[BL] < 0){
        qDebug() << "Failed to get the storage location of u_botl";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[BM] = glGetUniformLocation(m_program.programId(),"u_botm");
    if((int) m_uniforms[BM] < 0){
        qDebug() << "Failed to get the storage location of u_botm";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[BR] = glGetUniformLocation(m_program.programId(),"u_botr");
    if((int) m_uniforms[BR] < 0){
        qDebug() << "Failed to get the storage location of u_botr";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[PAD] = glGetUniformLocation(m_program.programId(),"u_pad");
    if((int) m_uniforms[PAD] < 0){
        qDebug() << "Failed to get the storage location of u_pad";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[STEP] = glGetUniformLocation(m_program.programId(),"u_step");
    if((int) m_uniforms[STEP] < 0){
        qDebug() << "Failed to get the storage location of u_step";
        exit(-1);
    }

    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[TL]     , m_kernal[TL]);
    glUniform1f(m_uniforms[TM]     , m_kernal[TM]);
    glUniform1f(m_uniforms[TR]     , m_kernal[TR]);
    glUniform1f(m_uniforms[L]      , m_kernal[L]);
    glUniform1f(m_uniforms[M]      , m_kernal[M]);
    glUniform1f(m_uniforms[R]      , m_kernal[R]);
    glUniform1f(m_uniforms[BL]     , m_kernal[BL]);
    glUniform1f(m_uniforms[BM]     , m_kernal[BM]);
    glUniform1f(m_uniforms[BR]     , m_kernal[BR]);
    glUniform1f(m_uniforms[PAD]    , 1.f/512.f);
    glUniform1f(m_uniforms[STEP]   , 1.f/512.f);
    glUniform1f(m_uniforms[SAMPLER], GL_TEXTURE0);

}

void Convolution::initTexture()
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

void Convolution::setTL(int)
{
    m_kernal[TL] = m_spinBox[TL]->value();
    glUniform1f(m_uniforms[TL], m_kernal[TL]);
}

void Convolution::setTM(int)
{
    m_kernal[TM] = m_spinBox[TM]->value();
    glUniform1f(m_uniforms[TM], m_kernal[TM]);
}

void Convolution::setTR(int)
{
    m_kernal[TR] = m_spinBox[TR]->value();
    glUniform1f(m_uniforms[TR], m_kernal[TR]);
}

void Convolution::setL(int)
{
    m_kernal[L] = m_spinBox[L]->value();
    glUniform1f(m_uniforms[L], m_kernal[L]);
}

void Convolution::setM(int)
{
    m_kernal[M] = m_spinBox[M]->value();
    glUniform1f(m_uniforms[M], m_kernal[M]);
}

void Convolution::setR(int)
{
    m_kernal[R] = m_spinBox[R]->value();
    glUniform1f(m_uniforms[R], m_kernal[R]);
}

void Convolution::setBL(int)
{
    m_kernal[BL] = m_spinBox[BL]->value();
    glUniform1f(m_uniforms[BL], m_kernal[BL]);
}

void Convolution::setBM(int)
{
    m_kernal[BM] = m_spinBox[BM]->value();
    glUniform1f(m_uniforms[BM], m_kernal[BM]);
}

void Convolution::setBR(int)
{
    m_kernal[BR] = m_spinBox[BR]->value();
    glUniform1f(m_uniforms[BR], m_kernal[BR]);
}

void Convolution::initializeGL()
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

void Convolution::resizeGL(int w, int h)
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

void Convolution::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quadss
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "CONVOLUTION:" <<(float)t/CLOCKS_PER_SEC;

}
