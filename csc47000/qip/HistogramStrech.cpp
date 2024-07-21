// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// HistogramStrech.cpp - Histogram Strech widget.
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================

#include "MainWindow.h"
#include "HistogramStrech.h"
#include "IPdefs.h"

extern MainWindow *g_mainWindowP;

HistogramStrech::HistogramStrech(QWidget *parent)
{
    m_minAuto = false;
    m_maxAuto = false;
}

QGroupBox *HistogramStrech::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Histogram Streching");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *label[2] = {new QLabel,new QLabel};
    label[0]->setText("Min");
    label[1]->setText("Max");

    // create sliders
    m_slider[MIN] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[MIN]->setMinimum(0);
    m_slider[MIN]->setMaximum(254);
    m_slider[MIN]->setTickPosition(QSlider::TicksBelow);
    m_slider[MIN]->setTickInterval(32);
    m_slider[MIN]->setValue(0);

    m_slider[MAX] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[MAX]->setMinimum(1);
    m_slider[MAX]->setMaximum(255);
    m_slider[MAX]->setTickPosition(QSlider::TicksBelow);
    m_slider[MAX]->setTickInterval(32);
    m_slider[MAX]->setValue(255);

    // create spinboxes
    m_spinBox[MIN] = new QSpinBox(m_ctrlGrp);
    m_spinBox[MIN]->setMinimum(0);
    m_spinBox[MIN]->setMaximum(254);
    m_spinBox[MIN]->setValue(0);

    m_spinBox[MAX] = new QSpinBox(m_ctrlGrp);
    m_spinBox[MAX]->setMinimum(1);
    m_spinBox[MAX]->setMaximum(255);
    m_spinBox[MAX]->setValue(255);

    // create checkboxes
    m_checkBox[MIN] = new QCheckBox("Auto");
    m_checkBox[MAX] = new QCheckBox("Auto");

    // init signal/slot connections
    connect( m_slider [MIN], SIGNAL(valueChanged(int)), this, SLOT(setMin(int)));
    connect( m_spinBox[MIN], SIGNAL(valueChanged(int)), this, SLOT(setMin(int)));
    connect(m_checkBox[MIN], SIGNAL(toggled(bool))    , this, SLOT(setMinAuto(bool)));

    connect( m_slider [MAX], SIGNAL(valueChanged(int)), this, SLOT(setMax(int)));
    connect( m_spinBox[MAX], SIGNAL(valueChanged(int)), this, SLOT(setMax(int)));
    connect(m_checkBox[MAX], SIGNAL(toggled(bool))    , this, SLOT(setMaxAuto(bool)));

    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(     label[MIN], 0, 0);
    layout->addWidget(  m_slider[MIN], 0, 1);
    layout->addWidget( m_spinBox[MIN], 0, 2);
    layout->addWidget(m_checkBox[MIN], 0, 3);
    layout->addWidget(     label[MAX], 1, 0);
    layout->addWidget(  m_slider[MAX], 1, 1);
    layout->addWidget( m_spinBox[MAX], 1, 2);
    layout->addWidget(m_checkBox[MAX], 1, 3);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);

}

bool HistogramStrech::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    int max = m_slider[MAX]->value();
    int min = m_slider[MIN]->value();
    if(min > max || min == max){
        return 0;
    }

    // apply filter
    histstrech(I1, min, max, I2);

    return 1;
}

void HistogramStrech::reset()
{
    m_slider[MIN]->setValue(0);
    m_slider[MAX]->setValue(255);
}

void HistogramStrech::histstrech(ImagePtr I1, int minGray, int maxGray, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++){
//        int h1[MXGRAY];
//        // Generate H1 histogram
//        for(int i = 0; i < MXGRAY; ++i){
//            h1[i]=0;
//        }
//        for(endd = p1 + total; p1<endd;){
//           h1[*p1++]++;
//        }
//        if(m_maxAuto){
//            int i = MXGRAY - 1;
//            while(h1[i] <= 0){
//                --i;
//            }
//            maxGray = i;
//        }
//        if(m_minAuto){
//            int i = 0;
//            while(h1[i] <= 0){
//                ++i;
//            }
//            minGray = i;
//        }
        // compute lut[]
        int i;
        double lut[MXGRAY];
        for(i=0;i<MXGRAY;++i){
            lut[i] = CLIP((255*(i-minGray)/(maxGray-minGray)),0,255);
        }
        IP_getChannel(I1, ch, p1, type);
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) {
            *p2++ = lut[*p1++];
        }
    }
    t = clock()-t;
    qDebug()<<"Hist S:" << (float) t / CLOCKS_PER_SEC;

}

void HistogramStrech::setMin(int min)
{
    if(min >= m_slider[MAX]->value()){
        min = m_slider[MAX]->value()-1;
    }
    m_slider[MIN]->setValue(min);
    m_spinBox[MIN]->setValue(min);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void HistogramStrech::setMax(int max)
{
    if(max <= m_slider[MIN]->value()){
        max = m_slider[MIN]->value()+1;
    }
    m_slider[MAX]->setValue(max);
    m_spinBox[MAX]->setValue(max);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void HistogramStrech::setMinAuto(bool min)
{
    m_minAuto = min;
    if(m_minAuto){
        m_slider [MIN]->setEnabled(false);
        m_spinBox[MIN]->setEnabled(false);
    }
    else{
        m_slider [MIN]->setEnabled(true);
        m_spinBox[MIN]->setEnabled(true);
    }

    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void HistogramStrech::setMaxAuto(bool max)
{
    m_maxAuto = max;
    if(m_maxAuto){
        m_slider [MAX]->setEnabled(false);
        m_spinBox[MAX]->setEnabled(false);
    }
    else{
        m_slider [MAX]->setEnabled(true);
        m_spinBox[MAX]->setEnabled(true);
    }

    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}
