#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

class Threshold : public HW{
    Q_OBJECT
public:
    Threshold(QWidget *parent = 0    );
    QGroupBox*	controlPanel        ();		// create control panel
    void		reset               ();		// reset parameters
    void		initVertexBuffer    ();		// init vertices
    void		initShaders         ();		// init shaders
    void		initTexture         ();		// init texture image
public slots:
    void        setThreshold(int);
protected:
    void		initializeGL        ();                 // init GL state
    void		resizeGL	(int, int);                 // resize GL widget
    void		paintGL             ();                 // render GL scene
private:
    int                 m_winW;
    int                 m_winH;
    float               m_threshold;
    int                 m_numofpoints;
    QImage              m_image;
    GLuint              m_u_threshold;
    GLuint              m_texture;
    GLuint              m_u_sampler;
    QGLShaderProgram    m_program;
    std::vector<vec2>   m_points;
    QGroupBox          *m_ctrlGrp;
    QSlider            *m_slider ;	// Threshold sliders
    QSpinBox           *m_spinBox;	// Threshold spin boxes
};

#endif // THRESHOLD_H
