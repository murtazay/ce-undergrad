// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Blur.h - Blur widget
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef BLUR_H
#define BLUR_H

#include "ImageFilter.h"

class Blur : public ImageFilter {
    Q_OBJECT
public:
    Blur(QWidget *parent = 0);                      // constructor
    QGroupBox*	controlPanel	             ();	// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
    void		reset		                 ();	// reset parameters

protected:
    void blur(ImagePtr I1, double xSize, double ySize, ImagePtr I2);
    void blr(ChannelPtr<uchar> src, ChannelPtr<uchar> dest, int stride, double filterSize, int amount);

protected slots:
    void setXSize(int);
    void setXSpin(int);
    void setYSize(int);
    void setYSpin(int);
    void setLink(bool);

private:
    QSlider     *m_slider[2];
    QSpinBox    *m_spinBox[2];
    QCheckBox   *m_checkBox;
    QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif // BLUR_H
