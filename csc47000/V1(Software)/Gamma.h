// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Gamma.h - Gamma widget
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef GAMMA_H
#define GAMMA_H

#include "ImageFilter.h"

class Gamma : public ImageFilter {
    Q_OBJECT

public:
    Gamma       (QWidget *parent = 0          );    // constructor
    QGroupBox*	controlPanel	             ();	// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
    void		reset		                 ();	// reset parameters

protected:
    void gamma(ImagePtr I1, double alpha, ImagePtr I2);

protected slots:
    void setGamma(int);
    void setGamma(double);

private:

    QSlider             *m_slider ;
    QDoubleSpinBox      *m_spinBox;
    // widgets and groupbox
    QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif // GAMMA_H
