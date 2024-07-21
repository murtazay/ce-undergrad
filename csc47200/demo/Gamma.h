#ifndef GAMMA_H
#define GAMMA_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Gamma : public HW {
    Q_OBJECT
public:
    Gamma          (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setGamma(int);
    void        setGamma(double);
protected:
    void        initializeGL();
    void        resizeGL(int, int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    int                 m_numofpoints;
    float               m_gamma;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider;
    QDoubleSpinBox     *m_spinBox;
};

#endif // GAMMA_H
