#ifndef BLUR_H
#define BLUR_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Blur : public HW{
    Q_OBJECT
public:
    Blur(QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setXSlider(int);
    void        setYSlider(int);
    void        setXSpinBox(int);
    void        setYSpinBox(int);
    void        setLink(bool);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    float               m_xsize;
    float               m_ysize;
    bool                m_link;
    int                 m_numofpoints;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider [2];
    QSpinBox           *m_spinBox[2];
    QCheckBox          *m_checkBox;
};

#endif // BLUR_H
