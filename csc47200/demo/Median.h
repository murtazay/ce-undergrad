#ifndef MEDIAN_H
#define MEDIAN_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Median : public HW{
    Q_OBJECT
public:
    Median         (QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setSizeSlider(int);
    void        setSizeSpinBox(int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    float               m_pad;
    int                 m_size;
    int                 m_numofpoints;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider ;
    QSpinBox           *m_spinBox;
};

#endif // MEDIAN_H
