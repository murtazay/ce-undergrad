// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramStrech.h - Histogram Strech widget
//
// Written by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef HISTOGRAMSTRECH_H
#define HISTOGRAMSTRECH_H

#include "ImageFilter.h"

enum{MIN,MAX};

class HistogramStrech : public ImageFilter {
    Q_OBJECT
public:
    HistogramStrech       (QWidget *parent = 0);    // constructor
    QGroupBox*	controlPanel	             ();	// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
    void		reset		                 ();	// reset parameters

protected:
    void histstrech(ImagePtr I1, int minGray, int maxGray, ImagePtr I2);

protected slots:
    void setMin(int);
    void setMax(int);
    void setMinAuto(bool);
    void setMaxAuto(bool);

private:
    QSlider     *m_slider [2];
    QSpinBox    *m_spinBox[2];
    QCheckBox   *m_checkBox[2];
    bool         m_minAuto;
    bool         m_maxAuto;
    // widgets and groupbox
    QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif // HISTOGRAMSTRECH_H
