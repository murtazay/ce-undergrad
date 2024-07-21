#ifndef CORRELATION_H
#define CORRELATION_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Correlation : public HW{
    Q_OBJECT
public:
    Correlation    (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image;
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    int                 m_numofpoints;
    float               m_imageWidth;
    float               m_imageHeight;
    float               m_templateWidth;
    float               m_templateHeight;
    float               m_xStep;
    float               m_yStep;
    GLuint              m_texture0;
    GLuint              m_texture1;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
};

#endif // CORRELATION_H
