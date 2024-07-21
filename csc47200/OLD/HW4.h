// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW4.h - Header file for HW4 class
//
// Written by: George Wolberg, 2015
// Modified by: Lukas Rascius,Murtaza Yaqoob and Ethan Graber, 2015
// ======================================================================

#ifndef HW4_H
#define HW4_H


#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

#define MAXGRID 64

// ----------------------------------------------------------------------
// standard include files
//

class HW4 : public HW {
	Q_OBJECT
public:
    HW4                         (QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();		// create control panel
    void		reset           ();		// reset parameters

public slots:
	void		changeDisplay	(int);		// change display mode
    void		changeGeom      (int);		// change geometry
    void		changeSize      (int);		// change grid size
    void		changeSpeed     (int);		// change animation speed
	void		startAnimation  ();
	void		stopAnimation   ();

protected:
	void		initializeGL	();		// init GL state
    void		resizeGL        (int, int);	// resize GL widget
    void		paintGL         ();		// render GL scene
    void		initTexture     ();		// init texture image
    void		initShaders     ();		// init shaders
    void		resetMesh       ();		// reset height of surface (z)
    void		getForce        ();		// used to compute new height
    void		getVelocity     ();		// used to compute new height
    void		getPosition     ();		// get surface height
    void		setDT           ();		// used to compute new height
	void		initVertices	();		// init XY coords of mesh
    void		initBuffers     ();		// init vertices
    void		flatten2D       ();		// copy 2D array into 1D array
	void		uploadVertices	();		// copy 3D vertices to GPU
	void		uploadTexCoords	();		// copy 2D texture coords to GPU
	void		uploadIndices	();		// copy vertex indices to GPU
	void		uploadNormals   ();		// copy normals to GPU
	void		uploadColors    ();		// copy colors to GPU
    void        initShader      (int, QString, QString, QMap<QString, GLuint>);
	void		mousePressEvent	(QMouseEvent *);
	void		mouseMoveEvent	(QMouseEvent *);
    void		wheelEvent      (QWheelEvent *event);
    void		timerEvent      (QTimerEvent *e);
	void		getFaceNorms	();
	void		getVertNorms	();
	vec2		cartesianToSpherical(vec3&);
	void		sphericalToCartesian(float,  float, vec3&);
	void		checkPhi	    (float&, float&);


private:
	int		  m_winW;			// window width
	int		  m_winH;			// window height
	int		  m_grid;
	int		  m_displayMode;
	int		  m_geometryMode;
	int		  m_sizeMode;
	int		  m_speed;
    int		  m_numPoints;			 // number of 2D points

    std::vector<vec3>  m_points;
    std::vector<vec2>  m_coords;
    std::vector<vec3>  m_normals;
    std::vector<vec3>  m_colors;
    std::vector<ushort>m_indices_triangles;
    std::vector<ushort>m_indices_wireframe;
    QGLShaderProgram   m_program[10];
    GLint              m_uniform[10][16];

    QComboBox	 *m_comboBoxDisplay;    // display mode combobox
    QComboBox	 *m_comboBoxGeom;		// geometry combobox
    QSlider		 *m_sliderSize;
    QSpinBox	 *m_spinBoxSize;
    QSlider		 *m_sliderSpeed;		// speed slider
    QSpinBox	 *m_spinBoxSpeed;		// speed combobox
    QPushButton	 *m_buttonStart;
    QPushButton	 *m_buttonStop;

	QMatrix4x4	  m_projection;			 // 4x4 projection matrix
    QMatrix4x4	  m_model;               // 4x4 model matrix
	QMatrix4x4	  m_cameraView;			 // 4x4 view  matrix
    QImage		  m_image;               // texture image
	GLuint		  m_texture;			 // shader index to texture unit
	QVector3D	  m_vertices [MAXGRID][MAXGRID]; // vector of 3D points (XYZ)
	QVector2D	  m_texCoords[MAXGRID][MAXGRID]; // vector of 2D coords (UV)
	QVector3D	  m_vertNorms[MAXGRID][MAXGRID]; // vector of normals
	QVector3D	  m_faceNorms[2][MAXGRID][MAXGRID]; // vector of normals
	float		  m_force    [MAXGRID][MAXGRID];
    float		  m_veloc    [MAXGRID][MAXGRID];
    int           m_resetMode;
	GLuint		  m_vertexBuffer;
	GLuint		  m_texBuffer;
	GLuint		  m_normalBuffer;
	GLuint		  m_colorBuffer;
	GLuint		  m_indicesBuffer[2];
	float		  m_angularSpeed;
	QQuaternion	  m_rotation;
	float		  m_dt;
	QVector2D	  m_mousePressPosition;
	QVector3D	  m_rotationAxis;
	QBasicTimer	 *m_timer;
	bool		  m_wave;

	vec3		  m_lightEye;
	vec3		  m_lightTarget;
	float		  m_lightUp;
	vec2		  m_lightRotation;
	std::vector<vec3> m_lightVtx;
	std::vector<vec3> m_lightColor;
	GLuint		  m_lightVtxBuffer;
	GLuint		  m_lightColorBuffer;


	vec2		  m_cameraRotation;
	vec3		  m_cameraTarget;
	vec3		  m_cameraEye;
	float		  m_cameraUp;
};

#endif // HW4_H
