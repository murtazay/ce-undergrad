// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramMatching.h - Histogram Matching widget
//
// Written by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef HISTOGRAMMATCHING_H
#define HISTOGRAMMATCHING_H

#include "ImageFilter.h"

class HistogramMatching : public ImageFilter{
    Q_OBJECT
public:
    HistogramMatching     (QWidget *parent = 0);    // constructor
    QGroupBox*	controlPanel	             ();	// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
    void		reset		                 ();	// reset parameters

protected:
    void histmatch(ImagePtr I1, int deg, ImagePtr I2);

protected slots:
    void setExp(int);

private:
    QSlider  *m_slider;
    QSpinBox *m_spinBox;

    // widgets and groupbox
    QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif // HISTOGRAMMATCHING_H
