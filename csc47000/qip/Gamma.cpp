// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Gamma.cpp - Gamma widget.
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================

#include "MainWindow.h"
#include "Gamma.h"
#include "math.h"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::Gamma:
//
// Constructor.
//
Gamma::Gamma(QWidget *parent)
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::controlPanel:
//
// Create group box for control panel.
//
QGroupBox *Gamma::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Gamma Correction");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *label = new QLabel;
    label->setText("Gamma");

    // create sliders
    m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider->setMinimum(-30);
    m_slider->setMaximum( 30);
    m_slider->setValue(30);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setSingleStep(1);
    m_slider->setTickInterval(5);

    // create spinbox
    m_spinBox = new QDoubleSpinBox(m_ctrlGrp);
    m_spinBox->setDecimals(1);
    m_spinBox->setSingleStep(.1f);
    m_spinBox->setMinimum(-3.f);
    m_spinBox->setMaximum( 3.f);
    m_spinBox->setValue(3.f);

    // init signal/slot connections
    connect(m_slider  , SIGNAL(valueChanged(int)),         this,  SLOT(setGamma(int)));
    connect(m_spinBox , SIGNAL(valueChanged(double)),      this,  SLOT(setGamma(double)));

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(    label, 1, 0);
    layout->addWidget( m_slider, 1, 1);
    layout->addWidget(m_spinBox, 1, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool Gamma::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    double g = (double) m_slider->value()/10;


    // apply filter
    gamma(I1, g, I2);

    return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::reset:
//
// Reset parameters.
//
void Gamma::reset()
{
    m_slider->setValue(3);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gamma:
//
// INSERT YOUR CODE HERE.
//
void Gamma::gamma(ImagePtr I1, double gamma, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();
    int total = w * h;

    // compute lut[]
    int i;
    double lut[MXGRAY];
    for(i=0;i<MXGRAY;++i){
        lut[i] = pow((double)i/MXGRAY, 1/gamma) * MXGRAY;
    }

    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
    t = clock() - t;
    qDebug() << "Gamma:" << (float) t / CLOCKS_PER_SEC;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::changeGamma:
//
// Slot to process change in gam caused by moving the slider.
//
void Gamma::setGamma(int gam)
{
    double g = (double) gam/10;
    m_spinBox->setValue(g);

    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::changeGamma:
//
// Slot to process change in gam caused by moving the slider.
//
void Gamma::setGamma(double gam)
{
    int g = (int) (gam*10);
    m_slider->setValue(g);

    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}
