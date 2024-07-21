// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Qunatization.cpp - Quantization widget.
//
// Written by: Murtaza Yaqoob, 2016
// ======================================================================

#include "MainWindow.h"
#include "Quantization.h"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::Quantization:
//
// Constructor.
//
Quantization::Quantization(QWidget *parent)
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::controlPanel:
//
// Create group box for control panel.
//
QGroupBox *Quantization::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Quantization");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *label = new QLabel;
    label->setText("Level");

    // create sliders
    m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider->setMinimum( 1);
    m_slider->setMaximum(15);
    m_slider->setValue(3);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setTickInterval(1);

    // create spinbox
    m_spinBox = new QSpinBox(m_ctrlGrp);
    m_spinBox->setMinimum( 1);
    m_spinBox->setMaximum(15);
    m_spinBox->setValue(3);

    // init signal/slot connections
    connect(m_slider , SIGNAL(valueChanged(int)),      this, SLOT(setLevels(int)));
    connect(m_slider , SIGNAL(valueChanged(int)), m_spinBox,  SLOT(setValue(int)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider ,  SLOT(setValue(int)));

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(    label, 0, 0);
    layout->addWidget( m_slider, 0, 1);
    layout->addWidget(m_spinBox, 0, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool Quantization::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    int level = m_slider->value();
    double l = (double) level;


    // apply filter
    quantize(I1, l, I2);

    return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::reset:
//
// Reset parameters.
//
void Quantization::reset()
{
    m_slider->setValue(3);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// quantize:
//
// INSERT YOUR CODE HERE.
//
void Quantization::quantize(ImagePtr I1, double level, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();
    int total = w * h;

    // compute lut[]
    int i, lut[MXGRAY];
    int scale = MXGRAY / level;
    for(i=0;i<MXGRAY;++i){
        lut[i] = scale * (int) (i/scale);
    }

    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
    t = clock() -t;
    qDebug() << "Quant:" << (float) t / CLOCKS_PER_SEC;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::changeLevels:
//
// Slot to process change in lev caused by moving the slider.
//
void Quantization::setLevels(int lev)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

