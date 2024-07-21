// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Median.cpp - Median widget.
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================
#include "Median.h"
#include "MainWindow.h"
#include <algorithm>

extern MainWindow *g_mainWindowP;

Median::Median(QWidget *parent)
{

}

QGroupBox*
Median::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Median");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *labelx = new QLabel;
    labelx->setText("Filter Size");
    QLabel *labely = new QLabel;
    labely->setText("Average");

    // create sliders
    m_slider[0] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[0]->setMinimum(1);
    m_slider[0]->setMaximum(3);
    m_slider[0]->setValue(1);
    m_slider[0]->setTickPosition(QSlider::TicksBelow);
    m_slider[0]->setTickInterval(1);

    m_slider[1] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[1]->setMinimum(0);
    m_slider[1]->setMaximum(10);
    m_slider[1]->setValue(0);
    m_slider[1]->setTickPosition(QSlider::TicksBelow);
    m_slider[1]->setTickInterval(1);

    // create spinbox
    m_spinBox[0] = new QSpinBox(m_ctrlGrp);
    m_spinBox[0]->setSingleStep(2);
    m_spinBox[0]->setMinimum(3);
    m_spinBox[0]->setMaximum(7);
    m_spinBox[0]->setValue(3);

    m_spinBox[1] = new QSpinBox(m_ctrlGrp);
    m_spinBox[1]->setMinimum(0);
    m_spinBox[1]->setMaximum(10);
    m_spinBox[1]->setValue(0);

    // init signal/slot connections
    connect(m_slider[0]  , SIGNAL(valueChanged(int)),         this,   SLOT(setSize(int)));
    connect(m_spinBox[0] , SIGNAL(valueChanged(int)),         this,   SLOT(setSpin(int)));

    connect(m_slider[1]  , SIGNAL(valueChanged(int)),         this,   SLOT(setAverage(int)));
    connect(m_slider[1]  , SIGNAL(valueChanged(int)), m_spinBox[1],   SLOT(setValue(int)));
    connect(m_spinBox[1] , SIGNAL(valueChanged(int)), m_slider[1] ,   SLOT(setValue(int)));
    // assemble dialog
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(      labelx, 1, 0);
    layout->addWidget( m_slider[0], 1, 1);
    layout->addWidget(m_spinBox[0], 1, 2);
    layout->addWidget(      labely, 2, 0);
    layout->addWidget( m_slider[1], 2, 1);
    layout->addWidget(m_spinBox[1], 2, 2);

    // assign layout to group box
    m_ctrlGrp->setLayout(layout);
    return(m_ctrlGrp);
}

bool Median::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    int sz = m_spinBox[0]->value();
    int av = m_spinBox[1]->value();

    // apply filter
    median(I1,sz,av,I2);
    return 1;
}

void Median::reset()
{
    m_spinBox[0]->setValue(3);
    m_spinBox[1]->setValue(0);
}

void Median::median(ImagePtr I1, double size, double average, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();

    int type;
    ChannelPtr<uchar> p1, p2;
    QVector<uchar*> buffer;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ++ch){
         IP_getChannel(I2,ch,p2,type);
         for(int i = 0; i < h; ++i){
             med(p1, p2, size, w, h, average, i, buffer);
             p1 += w;
             p2 += w;
         }
         buffer.clear();
    }
    t = clock() - t;
    qDebug() << "Median:" << (float) t / CLOCKS_PER_SEC;
}

void Median::med(ChannelPtr<uchar> src, ChannelPtr<uchar> dest, double size, int amount, int height,double average, int flag, QVector<uchar*>& buffer)
{
    int pad     = (size - 1)/2;
    int tmpSize = (pad*2) + amount;
    //Make Pad Size array
    uchar tmp[tmpSize];
    // inital buffer
    if(flag == 0){
        // add padding rows
        for(int i = 0; i < pad; ++i){
            for(int j = 0; j < amount; ++j){
                tmp[pad+j] = *(src+j);
            }
            for(int j = pad; j > 0; --j){
                tmp[j-1] = tmp[j];
                tmp[tmpSize -j] = tmp[tmpSize - j - 1];
            }
            buffer.push_back(tmp);
        }
        // add the other rows as needed
        for(int i = 0; i < size-pad; ++i){
            for(int j = 0; j < amount; ++j){
                tmp[pad+j] = *(src+(amount*i)+j);
            }
            for(int j = pad; j > 0; --j){
                tmp[j-1] = tmp[j];
                tmp[tmpSize -j] = tmp[tmpSize - j - 1];
            }
            buffer.push_back(tmp);
        }
    }
    // Update buffer with padding near end of file
    else if(flag > height-pad-1){
        buffer.pop_front();
        for(int i = 0; i < amount; ++i){
            tmp[pad+i] = *(src+((height-flag-1)*amount)+i);
        }
        for(int i = pad; i > 0; --i){
            tmp[i-1] = tmp[i];
            tmp[tmpSize -i] = tmp[tmpSize - i - 1];
        }
        buffer.push_back(tmp);
    }
    //update buffer
    else{
        buffer.pop_front();
        for(int i = 0; i < amount; ++i){
            tmp[pad+i] = *(src+amount+i);
        }
        for(int i = pad; i > 0; --i){
            tmp[i-1] = tmp[i];
            tmp[tmpSize -i] = tmp[tmpSize - i - 1];
        }
        buffer.push_back(tmp);
    }
    // start computing the median
    int tSize = (size*size-1)/2;
    int denom = (2*average) + 1;
    QVector<uchar> median;
    for(int column = pad; column < amount+pad; ++column){
        median.clear();
        int sum = 0;
        for(int i = 0; i < size; ++i){
            for(int j = -pad; j <= pad; ++j){
                median.push_back(buffer[i][j + column]);
            }
        }
        std::sort(median.begin(),median.end());
        for(int i = -average; i <= average; ++i){
            sum += median[i+tSize];
        }
        *(dest+column-pad) = (double) sum / (double)denom;
    }
}

void Median::setSize(int)
{
    m_spinBox[0]->setValue((m_slider[0]->value() * 2) + 1);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void Median::setSpin(int)
{
    m_slider[0]->setValue((m_spinBox[0]->value()-1)/2);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void Median::setAverage(int)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}
