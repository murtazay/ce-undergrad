// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Sharpen.h - Sharpen widget
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================
#ifndef SHARPEN_H
#define SHARPEN_H

#include "ImageFilter.h"

class Sharpen : public ImageFilter{
    Q_OBJECT
public:
    Sharpen(QWidget *parent = 0);                   // constructor
    QGroupBox*	controlPanel	             ();	// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
    void		reset		                 ();	// reset parameters

protected:
    void sharpen(ImagePtr I1, double size, double factor, ImagePtr I2);
    void blur(ImagePtr I1, double xSize, double ySize, ImagePtr I2);
    void blr(ChannelPtr<uchar> src, ChannelPtr<uchar> dest, int stride, double filterSize, int amount);

protected slots:
    void setSize(int);
    void setSpin(int);
    void setFactor(int);

private:
    QSlider     *m_slider[2];
    QSpinBox    *m_spinBox[2];
    QGroupBox	*m_ctrlGrp;	// groupbox for panel

};

#endif // SHARPEN_H
