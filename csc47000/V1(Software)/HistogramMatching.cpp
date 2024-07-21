// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// HistogramMatching.cpp - Histogram Matching widget.
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================

#include "MainWindow.h"
#include "HistogramMatching.h"
#include <math.h>

extern MainWindow *g_mainWindowP;

HistogramMatching::HistogramMatching(QWidget *parent)
{

}

QGroupBox *HistogramMatching::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Histogram Matching");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *label = new QLabel;
    label->setText("Exp");

    // create sliders
    m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider->setMinimum(-5);
    m_slider->setMaximum( 5);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setTickInterval(1);
    m_slider->setValue(0);

    // create spinboxes
    m_spinBox = new QSpinBox(m_ctrlGrp);
    m_spinBox->setMinimum(-5);
    m_spinBox->setMaximum( 5);
    m_spinBox->setValue(0);

    // init signal/slot connections
    connect(m_slider , SIGNAL(valueChanged(int)), this,       SLOT(setExp(int)));
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

bool HistogramMatching::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    int exp = m_slider->value();

    // apply filter
    histmatch(I1, exp, I2);

    return 1;
}

void HistogramMatching::reset()
{
    m_slider->setValue(1);
}

void HistogramMatching::histmatch(ImagePtr I1, int exp, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();
    int total = w * h;

    // Generate H2 and scale
    double h2[MXGRAY];
    for(int i = 0; i < MXGRAY; ++i){
        h2[i] = 0;
    }
    double Havg = 0;
    if(exp == 0){
        for(int i = 0; i < MXGRAY; ++i){
            h2[i] = (double) total / (double) MXGRAY;
            Havg += h2[i];
        }
    }
    else if(exp < 0){
        for(int i = 0; i < MXGRAY; ++i){
            double a = (double) i / (double)(MXGRAY - 1);
            double b = (1. - pow(a, (double) -exp));
            h2[i] = b;
            Havg += h2[i];
        }
    }
    else{
        for(int i = 0; i < MXGRAY; ++i){
            double a = (double) i / (double) MXGRAY;
            double b = pow(a, (double) exp);
            h2[i] = b;
            Havg += h2[i];
        }
    }
    // Scale h2
    double scale = ((double) total/Havg);
    if(scale != 1){
        for(int i = 0; i < MXGRAY; ++i){
            h2[i] = round(h2[i] * scale);
        }
    }
    int type;
    ChannelPtr<uchar> p1, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        // Generate H1 histogram
        int h1[MXGRAY];
        for(int i = 0; i < MXGRAY; ++i){
            h1[i]=0;
        }
        for(endd = p1 + total; p1<endd;){
           h1[*p1++]++;
        }
        int left[MXGRAY], right[MXGRAY];
        int reserves[MXGRAY][MXGRAY];
        // init reserves
        for(int i = 0; i < MXGRAY; ++i){
            for(int j = 0; j < MXGRAY; ++j){
                reserves[i][j] = 0;
            }
        }
        // Calculate limits and reserves
        int tot = 0;
        int Hsum = 0;
        int R = 0;
        for(int i = 0; i < MXGRAY; ++i){
            left[i] = R;
            Hsum = h1[i];
            int Pr = R;
            while(Hsum > 0){
                if(R != Pr){
                    tot = 0;
                    Pr = R;
                }
                if(tot+Hsum > h2[R] && R < MXGRAY){
                    if(R != MXGRAY - 1 || i != MXGRAY - 1){
                        int in = h2[R] - tot;
                        reserves[R][i] = in;
                        Hsum -= in;
                        R++;
                    }
                    else{
                        reserves[R][i] = Hsum;
                        Hsum -= Hsum;
                    }
                }
                else{
                    reserves[R][i] = Hsum;
                    tot  += Hsum;
                    Hsum -= Hsum;
                }
                right[i] = R;
            }
        }

        // Do histogram matching
        ChannelPtr<uchar> p2,p3;
        IP_getChannel(I1,ch,p3,type);
        IP_getChannel(I2,ch,p2,type);
        for(endd = p3 + total; p3<endd;){
            int p = left[*p3];
            if(reserves[p][*p3] > 0){
                *p2 = p;
                reserves[p][*p3]--;
            }
            else{
                if(p+1 < right[*p3]){
                    left[*p3] = p+1;
                }
                else{
                    left[*p3] = right[*p3];
                }
                reserves[left[*p3]][*p3]--;
                *p2 = p = left[*p3];
            }
            *p3++;
            *p2++;
        }
    }
}

void HistogramMatching::setExp(int)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}
