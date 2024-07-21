#include "Correlation.h"
#include <time.h>

enum{
    SAMPLER,
    TEMPLATE,
    TEMPLATEW,
    TEMPLATEH,
    TEMPLATEX,
    TEMPLATEY,
    XSTEP,
    YSTEP,
    XPAD,
    YPAD
};

Correlation::Correlation(QWidget *parent) : HW(parent)
{

}

QGroupBox *Correlation::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Correlation");
    return m_ctrlGrp;
}

void Correlation::reset()
{
    //No reset?
}

void Correlation::initVertexBuffer()
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

void Correlation::initShaders()
{

    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderCorrelation.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }

    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderCorrelation.glsl")) {
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

    // get storage location of u_Sampler in fragment shader
    m_uniforms[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniforms[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[TEMPLATE] = glGetUniformLocation(m_program.programId(), "u_Template");
    if((int) m_uniforms[TEMPLATE] < 0) {
        qDebug() << "Failed to get the storage location of u_Template";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[TEMPLATEW] = glGetUniformLocation(m_program.programId(), "u_TemplateWidth");
    if((int) m_uniforms[TEMPLATEW] < 0) {
        qDebug() << "Failed to get the storage location of u_TemplateWidth";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[TEMPLATEH] = glGetUniformLocation(m_program.programId(), "u_TemplateHeight");
    if((int) m_uniforms[TEMPLATEH] < 0) {
        qDebug() << "Failed to get the storage location of u_TemplateHeight";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[TEMPLATEX] = glGetUniformLocation(m_program.programId(), "u_TemplateXStep");
    if((int) m_uniforms[TEMPLATEX] < 0) {
        qDebug() << "Failed to get the storage location of u_TemplateXStep";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[TEMPLATEY] = glGetUniformLocation(m_program.programId(), "u_TemplateYStep");
    if((int) m_uniforms[TEMPLATEY] < 0) {
        qDebug() << "Failed to get the storage location of u_TemplateYStep";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[XSTEP] = glGetUniformLocation(m_program.programId(), "u_XStep");
    if((int) m_uniforms[XSTEP] < 0) {
        qDebug() << "Failed to get the storage location of u_XStep";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[YSTEP] = glGetUniformLocation(m_program.programId(), "u_YStep");
    if((int) m_uniforms[YSTEP] < 0) {
        qDebug() << "Failed to get the storage location of u_YStep";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[XPAD] = glGetUniformLocation(m_program.programId(), "u_XPad");
    if((int) m_uniforms[XPAD] < 0) {
        qDebug() << "Failed to get the storage location of u_XPad";
        exit(-1);
    }

    // get storage location of u_Template in fragment shader
    m_uniforms[YPAD] = glGetUniformLocation(m_program.programId(), "u_YPad");
    if((int) m_uniforms[YPAD] < 0) {
        qDebug() << "Failed to get the storage location of u_YPad";
        exit(-1);
    }

    // init uniform variables and pass
    glUniform1f(m_uniforms[SAMPLER]  , GL_TEXTURE0);
    glUniform1f(m_uniforms[TEMPLATE] , GL_TEXTURE1);
    glUniform1f(m_uniforms[TEMPLATEW], (float)m_templateWidth);
    glUniform1f(m_uniforms[TEMPLATEH], (float)m_templateHeight);
    glUniform1f(m_uniforms[TEMPLATEX], 1.f/(float)m_templateWidth);
    glUniform1f(m_uniforms[TEMPLATEY], 1.f/(float)m_templateHeight);
    glUniform1f(m_uniforms[XSTEP]    , 1.f/(float)m_imageWidth);
    glUniform1f(m_uniforms[YSTEP]    , 1.f/(float)m_imageHeight);
    glUniform1f(m_uniforms[XPAD]     , (float)((m_templateWidth-1)/2.f)/512.f);
    glUniform1f(m_uniforms[YPAD]     , (float)((m_templateHeight-1)/2.f)/512.f);
}

void Correlation::initTexture()
{

    // read image from file
    m_image.load(QString(":/mandrill.jpg"));

    // convert jpg to GL formatted image
    QImage qImage = QGLWidget::convertToGLFormat(m_image);

    // init vars
    int w = qImage.width ();
    int h = qImage.height();
    m_imageWidth = w;
    m_imageHeight= h;

    // generate texture name and bind it
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture0);
    glBindTexture(GL_TEXTURE_2D, m_texture0);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());

    // read image from file
    m_image.load(QString(":/template.jpg"));

    // convert jpg to GL formatted image
    qImage = QGLWidget::convertToGLFormat(m_image);

    // init vars
    w = qImage.width ();
    h = qImage.height();
    m_templateWidth  = w;
    m_templateHeight = h;

    // generate texture name and bind it
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &m_texture1);
    glBindTexture(GL_TEXTURE_2D, m_texture1);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());

}

void Correlation::initializeGL()
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

void Correlation::resizeGL(int w, int h)
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

void Correlation::paintGL()
{
    clock_t t = clock();
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quadss
    glUseProgram(m_program.programId());
    glDrawArrays(GL_QUADS, 0, (GLsizei)m_numofpoints);
    t = clock() - t;
    qDebug() << "CORRELATION:" <<(float)t/CLOCKS_PER_SEC;
}
