// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Contrast.cpp - Brightness/Contrast widget.
//
// Written by: George Wolberg, 2016
// Edited  by: Murtaza Yaqoob, 2016
// ======================================================================

#include "MainWindow.h"
#include "Contrast.h"
#include "IPdefs.h"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::Contrast:
//
// Constructor.
//
Contrast::Contrast(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Contrast::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get brightness value
    int bri = m_slider[BRIGHTNESS]->value();
    double b = (double) bri;

    // get contrast value
    int con = m_slider[CONTRAST]->value();
    // map the [-50,50] to (0,3]
    // positive mapping
    double c = 0.;
    if(con >= 0){
        c = (con/25.) + 1.;
    }
    else{
        c = (con/51.) + 1;
    }

    // get refrence value
    int ref = m_slider[REFRENCE]->value();
    double r = (double) ref;

    // apply filter
    contrast(I1, b, c, r, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::createGroupBox:
//
// Create group box for control panel.
//
QGroupBox*
Contrast::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Contrast");

	// INSERT YOUR CODE HERE
    // init widgets
    // create label[i]
    QLabel* label[3];
    label[BRIGHTNESS] = new QLabel;
    label[CONTRAST] = new QLabel;
    label[REFRENCE] = new QLabel;

    label[BRIGHTNESS]->setText("Brightness");
    label[CONTRAST]->setText("Contrast");
    label[REFRENCE]->setText("Refrence Point");

    // create sliders
    m_slider[BRIGHTNESS] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[BRIGHTNESS]->setMinimum(-100);
    m_slider[BRIGHTNESS]->setMaximum( 100);
    m_slider[BRIGHTNESS]->setValue(0);
    m_slider[CONTRAST]   = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[CONTRAST]->setMinimum(-50);
    m_slider[CONTRAST]->setMaximum( 50);
    m_slider[CONTRAST]->setValue(0);
    m_slider[REFRENCE]   = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[REFRENCE]->setMinimum(0);
    m_slider[REFRENCE]->setMaximum(255);
    m_slider[REFRENCE]->setValue(128);
    for(int i = 0; i < 3; i++){
        m_slider[i]->setTickPosition(QSlider::TicksBelow);
        m_slider[i]->setTickInterval(25);
    }

    // create spinbox
    m_spinBox[BRIGHTNESS] = new QSpinBox(m_ctrlGrp);
    m_spinBox[BRIGHTNESS]->setMinimum(-100);
    m_spinBox[BRIGHTNESS]->setMaximum( 100);
    m_spinBox[BRIGHTNESS]->setValue(0);
    m_spinBox[CONTRAST] = new QSpinBox(m_ctrlGrp);
    m_spinBox[CONTRAST]->setMinimum(-50);
    m_spinBox[CONTRAST]->setMaximum( 50);
    m_spinBox[CONTRAST]->setValue(0);
    m_spinBox[REFRENCE] = new QSpinBox(m_ctrlGrp);
    m_spinBox[REFRENCE]->setMinimum(  0);
    m_spinBox[REFRENCE]->setMaximum(255);
    m_spinBox[REFRENCE]->setValue(128);

    // init signal/slot connections
    connect(m_slider[BRIGHTNESS], SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
    connect(m_slider[CONTRAST], SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
    connect(m_slider[REFRENCE], SIGNAL(valueChanged(int)), this, SLOT(setRefrence(int)));
    for(int i = 0; i < 3; i++){
        connect(m_slider [i], SIGNAL(valueChanged(int)), m_spinBox[i], SLOT(setValue(int)));
        connect(m_spinBox[i], SIGNAL(valueChanged(int)), m_slider [i], SLOT(setValue(int)));
    }

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(    label[BRIGHTNESS], 0, 0);
    layout->addWidget( m_slider[BRIGHTNESS], 0, 1);
    layout->addWidget(m_spinBox[BRIGHTNESS], 0, 2);
    layout->addWidget(      label[CONTRAST], 1, 0);
    layout->addWidget(   m_slider[CONTRAST], 1, 1);
    layout->addWidget(  m_spinBox[CONTRAST], 1, 2);
    layout->addWidget(      label[REFRENCE], 2, 0);
    layout->addWidget(   m_slider[REFRENCE], 2, 1);
    layout->addWidget(  m_spinBox[REFRENCE], 2, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// contrast:
//
// INSERT YOUR CODE HERE.
//
void
Contrast::contrast(ImagePtr I1, double brightness, double contrast, double refrence, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();
    int total = w * h;

    // compute lut[]
    int i, lut[MXGRAY];
    for(i=0;i<MXGRAY;++i){
        lut[i] = CLIP((i - refrence) * contrast + refrence + brightness,0,255);
    }

    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
    t = clock() - t;
    qDebug()<< "Contrast:" <<(float) t/CLOCKS_PER_SEC;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::changeBrightness:
//
// Slot to process change in bri caused by moving the slider.
//
void Contrast::setBrightness(int bri)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::changeThr:
//
// Slot to process change in thr caused by moving the slider.
//
void Contrast::setContrast(int con)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Threshold::changeThr:
//
// Slot to process change in thr caused by moving the slider.
//
void Contrast::setRefrence(int ref)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::reset:
//
// Reset parameters.
//
void
Contrast::reset() {
    m_slider[BRIGHTNESS]->setValue(0);
    m_slider[CONTRAST]->setValue(0);
    m_slider[REFRENCE]->setValue(128);
}
