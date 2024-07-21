#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Quantization : public HW {
    Q_OBJECT
public:
    Quantization   (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setLevels(int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    int                 m_numofpoints;
    int                 m_level;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider;
    QSpinBox           *m_spinBox;
};

#endif // QUANTIZATION_H
