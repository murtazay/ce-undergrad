// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW3b.h - Header file for HW3b class
//
// Written by: George Wolberg, 2015
// ======================================================================

#ifndef HW3B_H
#define HW3B_H


#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>

#define MAXGRID 64

// ----------------------------------------------------------------------
// standard include files
//

class HW3b : public HW {
	Q_OBJECT
public:
	HW3b		(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();		// create control panel
	void		reset		();		// reset parameters

public slots:
	void		changeDisplay	(int);		// change display mode
	void		changeGeom	(int);		// change geometry
	void		changeSize	(int);		// change grid size
	void		changeSpeed	(int);		// change animation speed
	void		startAnimation  ();
	void		stopAnimation   ();

protected:
	void		initializeGL	();		// init GL state
	void		resizeGL	(int, int);	// resize GL widget
	void		paintGL		();		// render GL scene
	void		initTexture	();		// init texture image
	void		initShaders	();		// init shaders
	void		initShader1	();		// texture mapping shader
	void		initShader2	();		// wireframe rendering shader
	void		resetMesh	();		// reset height of surface (z)
	void		getForce	();		// used to compute new height
	void		getVelocity	();		// used to compute new height
	void		getPosition	();		// get surface height
	void		setDT		();		// used to compute new height
	void		initVertices	();		// init XY coords of mesh
	void		initBuffers	();		// init vertices
	void		flatten2D	();		// copy 2D array into 1D array
	void		uploadVertices	();		// copy 3D vertices to GPU
	void		uploadTexCoords	();		// copy 2D texture coords to GPU
	void		uploadIndices	();		// copy vertex indices to GPU
	void		mousePressEvent(QMouseEvent *);
	void		mouseMoveEvent(QMouseEvent *);
	void		timerEvent(QTimerEvent *e);

private:
	int		  m_winW;			// window width
	int		  m_winH;			// window height
	QGLShaderProgram  m_program[2];			// GLSL programs
	int		  m_grid;
	int		  m_displayMode;
	int		  m_geometryMode;
	int		  m_sizeMode;
	int		  m_speed;
	QComboBox	 *m_comboBoxDisplay;		// display mode combobox
	QComboBox	 *m_comboBoxGeom;		// geometry combobox
	QSlider		 *m_sliderSize;
	QSpinBox	 *m_spinBoxSize;
	QSlider		 *m_sliderSpeed;		// speed slider
	QSpinBox	 *m_spinBoxSpeed;		// speed combobox
	QPushButton	 *m_buttonStart;
	QPushButton	 *m_buttonStop;
	std::vector<vec3> m_points;			
	std::vector<vec2> m_coords;
	int		  m_numPoints;			 // number of 2D points
	QMatrix4x4	  m_projection;			 // 4x4 projection matrix
	QMatrix4x4	  m_modelview;			 // 4x4 modelview  matrix
	QImage		  m_image;			 // texture image
	GLuint		  m_texture;			 // shader index to texture unit
	GLint		  m_uniform[2][16];		 // uniform vars for two shaders and <16 vars
	QVector3D	  m_vertices [MAXGRID][MAXGRID]; // vector of 2D points (XY)
	QVector2D	  m_texCoords[MAXGRID][MAXGRID]; // vector of 2D coords (UV)
	float		  m_force    [MAXGRID][MAXGRID];
	float		  m_veloc    [MAXGRID][MAXGRID];
	std::vector<ushort>m_indices_triangles;
	std::vector<ushort>m_indices_wireframe;
	int		  m_resetMode;
	GLuint		  m_vertexBuffer;
	GLuint		  m_texBuffer;
	GLuint		  m_indicesBuffer[2];
	float		  m_angularSpeed;
	QQuaternion	  m_rotation;
	float		  m_dt;
	QVector2D	  m_mousePressPosition;
	QVector3D	  m_rotationAxis;
	QBasicTimer	 *m_timer;
	bool		  m_wave;

};

#endif // HW3B_H
