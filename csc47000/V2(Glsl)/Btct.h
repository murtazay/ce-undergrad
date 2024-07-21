#ifndef BTCT_H
#define BTCT_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class BTCT : public HW{
    Q_OBJECT
public:
    BTCT           (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setBrightness(int);
    void        setContrast  (int);
    void        setRefrence  (int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    int                 m_numofpoints;
    float               m_brightness;
    float               m_contrast;
    float               m_refrence;
    GLuint              m_texture;
    GLuint              m_lut;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider[3];
    QSpinBox           *m_spinBox[3];
};

#endif // BTCT_H
