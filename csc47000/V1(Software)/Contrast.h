// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Contrast.h - Contrast widget
//
// Written by: George Wolberg, 2016
// Edited  by: Murtaza Yaqoob, 2016
// ======================================================================

#ifndef CONTRAST_H
#define CONTRAST_H

#include "ImageFilter.h"

class Contrast : public ImageFilter {
	Q_OBJECT

    enum{BRIGHTNESS,CONTRAST,REFRENCE};
public:
	Contrast	(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();		// create control panel
	bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
	void		reset		();		// reset parameters

protected:
    void contrast(ImagePtr I1, double brightness, double contrast, double refrence, ImagePtr I2);

protected slots:
    void setBrightness(int);
    void setContrast(int);
    void setRefrence(int);

private:
    // brightness/contrast controls
    QSlider     *m_slider[3] ;
    QSpinBox    *m_spinBox[3];
	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif	// CONTRAST_H
