#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Convolution : public HW{
    Q_OBJECT
public:
    Convolution    (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image;
public slots:
    void        setTL(int);
    void        setTM(int);
    void        setTR(int);
    void        setL(int);
    void        setM(int);
    void        setR(int);
    void        setBL(int);
    void        setBM(int);
    void        setBR(int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    float               m_kernal[9];
    int                 m_numofpoints;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSpinBox           *m_spinBox[9];
    QPushButton        *m_button;
};

#endif // CONVOLUTION_H
