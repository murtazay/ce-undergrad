#ifndef HISTROGRAMSTRECH_H
#define HISTROGRAMSTRECH_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class HistrogramStrech : public HW{
    Q_OBJECT
public:
    HistrogramStrech(QWidget *parent=0);
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setMin(int);
    void        setMax(int);
protected:
    void        initializeGL();
    void        resizeGL(int,int);
    void        paintGL();
private:
    int                 m_winW;
    int                 m_winH;
    float               m_min;
    float               m_max;
    int                 m_numofpoints;
    GLuint              m_texture;
    GLuint              m_uniforms[16];
    QImage              m_image;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider [2];
    QSpinBox           *m_spinBox[2];
    QCheckBox          *m_checkBox[2];
//    bool                m_minAuto;
//    bool                m_maxAuto;
};

#endif // HISTROGRAMSTRECH_H
