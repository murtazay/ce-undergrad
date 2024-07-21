#ifndef SHARPEN_H
#define SHARPEN_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Sharpen : public HW{
    Q_OBJECT
public:
    Sharpen        (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setSizeSlider(int);
    void        setSizeSpinBox(int);
    void        setFactor(int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    float               m_size;
    float               m_factor;
    int                 m_numofpoints;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider [2];
    QSpinBox           *m_spinBox[2];
};

#endif // SHARPEN_H
