#include "Histrogramstrech.h"
#include <time.h>

enum{
    MIN,
    MAX,
    SAMPLER
};

HistrogramStrech::HistrogramStrech(QWidget *parent):HW(parent)
{
    m_max = 1.f;
    m_min = 0.f;
}

QGroupBox *HistrogramStrech::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Histogram Streching");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *label[2] = {new QLabel,new QLabel};
    label[0]->setText("Min");
    label[1]->setText("Max");

    // create sliders
    m_slider[MIN] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[MIN]->setMinimum(0);
    m_slider[MIN]->setMaximum(254);
    m_slider[MIN]->setTickPosition(QSlider::TicksBelow);
    m_slider[MIN]->setTickInterval(32);
    m_slider[MIN]->setValue(0);

    m_slider[MAX] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[MAX]->setMinimum(1);
    m_slider[MAX]->setMaximum(255);
    m_slider[MAX]->setTickPosition(QSlider::TicksBelow);
    m_slider[MAX]->setTickInterval(32);
    m_slider[MAX]->setValue(255);

    // create spinboxes
    m_spinBox[MIN] = new QSpinBox(m_ctrlGrp);
    m_spinBox[MIN]->setMinimum(0);
    m_spinBox[MIN]->setMaximum(254);
    m_spinBox[MIN]->setValue(0);

    m_spinBox[MAX] = new QSpinBox(m_ctrlGrp);
    m_spinBox[MAX]->setMinimum(1);
    m_spinBox[MAX]->setMaximum(255);
    m_spinBox[MAX]->setValue(255);

    // init signal/slot connections
    connect( m_slider [MIN], SIGNAL(valueChanged(int)), this, SLOT(setMin(int)));
    connect( m_spinBox[MIN], SIGNAL(valueChanged(int)), this, SLOT(setMin(int)));

    connect( m_slider [MAX], SIGNAL(valueChanged(int)), this, SLOT(setMax(int)));
    connect( m_spinBox[MAX], SIGNAL(valueChanged(int)), this, SLOT(setMax(int)));

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(     label[MIN], 0, 0);
    layout->addWidget(  m_slider[MIN], 0, 1);
    layout->addWidget( m_spinBox[MIN], 0, 2);
    layout->addWidget(     label[MAX], 1, 0);
    layout->addWidget(  m_slider[MAX], 1, 1);
    layout->addWidget( m_spinBox[MAX], 1, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

void HistrogramStrech::reset()
{
    setMin(0);
    setMax(255);
}

void HistrogramStrech::initVertexBuffer()
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

void HistrogramStrech::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderHistogramStrech.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderHistogramStrech.glsl")) {
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
    m_uniforms[MIN] = glGetUniformLocation(m_program.programId(),"u_min");
    if((int) m_uniforms[MIN] < 0){
        qDebug() << "Failed to get the storage location of u_min";
        exit(-1);
    }

    // get storage location of u_brightness in fragment shader
    m_uniforms[MAX] = glGetUniformLocation(m_program.programId(),"u_max");
    if((int) m_uniforms[MAX] < 0){
        qDebug() << "Failed to get the storage location of u_max";
        exit(-1);
    }

    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[MIN]    ,   m_min);
    glUniform1f(m_uniforms[MAX]    ,   m_max);
    glUniform1f(m_uniforms[SAMPLER], GL_TEXTURE0);
}

void HistrogramStrech::initTexture()
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

void HistrogramStrech::setMin(int min)
{
    m_slider[MIN]->blockSignals(true);
    m_spinBox[MIN]->blockSignals(true);
    if(min >= m_slider[MAX]->value()){
        min = m_slider[MAX]->value()-1;
    }
    m_min = min/255.f;
    m_slider[MIN]->setValue(min);
    m_spinBox[MIN]->setValue(min);
    m_slider[MIN]->blockSignals(false);
    m_spinBox[MIN]->blockSignals(false);
    glUniform1f(m_uniforms[MIN],   m_min);
    updateGL();
}

void HistrogramStrech::setMax(int max)
{
    if(max <= m_slider[MIN]->value()){
        max = m_slider[MIN]->value()+1;
    }
    m_max = max/255.f;
    m_slider[MAX]->setValue(max);
    m_spinBox[MAX]->setValue(max);
    m_slider[MAX]->blockSignals(false);
    m_spinBox[MAX]->blockSignals(false);
    glUniform1f(m_uniforms[MAX],   m_max);
    updateGL();
}

void HistrogramStrech::initializeGL()
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

void HistrogramStrech::resizeGL(int w, int h)
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

void HistrogramStrech::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quadss
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "Hist S:" <<(float)t/CLOCKS_PER_SEC;
}
