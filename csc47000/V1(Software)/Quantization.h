// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Quantization.h - Quantization widget
//
// Written by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include "ImageFilter.h"

class Quantization : public ImageFilter {
    Q_OBJECT

public:
    Quantization    (QWidget *parent = 0);
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters

protected:
    void quantize(ImagePtr I1, double level, ImagePtr I2);

protected slots:
    void setLevels(int);

private:
    QSlider     *m_slider;
    QSpinBox    *m_spinBox;
    QGroupBox   *m_ctrlGrp;

};

#endif // QUANTIZATION_H
