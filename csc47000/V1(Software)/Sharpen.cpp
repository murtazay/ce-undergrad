// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by Murtaza Yaqoob
//
// Sharpen.cpp - Sharpen widget.
//
// Written  by: Murtaza Yaqoob, 2016
// ======================================================================
#include "MainWindow.h"
#include "Sharpen.h"
#include "Blur.h"

extern MainWindow *g_mainWindowP;

Sharpen::Sharpen(QWidget *parent)
{

}

QGroupBox *Sharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Sharpen");

    // INSERT YOUR CODE HERE
    // init widgets
    // create label
    QLabel *labelx = new QLabel;
    labelx->setText("Filter Size");
    QLabel *labely = new QLabel;
    labely->setText("Factor");

    // create sliders
    m_slider[0] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[0]->setMinimum(1);
    m_slider[0]->setMaximum(7);
    m_slider[0]->setValue(1);
    m_slider[0]->setTickPosition(QSlider::TicksBelow);
    m_slider[0]->setTickInterval(1);

    m_slider[1] = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider[1]->setMinimum(1);
    m_slider[1]->setMaximum(15);
    m_slider[1]->setValue(1);
    m_slider[1]->setTickPosition(QSlider::TicksBelow);
    m_slider[1]->setTickInterval(1);

    // create spinbox
    m_spinBox[0] = new QSpinBox(m_ctrlGrp);
    m_spinBox[0]->setSingleStep(2);
    m_spinBox[0]->setMinimum(3);
    m_spinBox[0]->setMaximum(15);
    m_spinBox[0]->setValue(3);

    m_spinBox[1] = new QSpinBox(m_ctrlGrp);
    m_spinBox[1]->setMinimum(1);
    m_spinBox[1]->setMaximum(15);
    m_spinBox[1]->setValue(1);

    // init signal/slot connections
    connect(m_slider[0]  , SIGNAL(valueChanged(int)),         this,   SLOT(setSize(int)));
    connect(m_spinBox[0] , SIGNAL(valueChanged(int)),         this,   SLOT(setSpin(int)));

    connect(m_slider[1]  , SIGNAL(valueChanged(int)),         this,   SLOT(setFactor(int)));
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

bool Sharpen::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE
    // error checking
    if(I1.isNull()) return 0;

    // get level value
    int sz = m_spinBox[0]->value();
    int fc = m_spinBox[1]->value();

    // apply filter
    sharpen(I1,sz,fc,I2);

    return 1;
}

void Sharpen::reset()
{
    m_slider[0]->setValue(3);
    m_slider[1]->setValue(1);
}

void Sharpen::sharpen(ImagePtr I1, double size, double factor, ImagePtr I2)
{
    clock_t t = clock();
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I2->height();
    int total = w * h;

    blur(I1,size,size,I2);
    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ++ch){
        IP_getChannel(I2,ch,p2,type);
        for(endd = p1 + total; p1 < endd;){
            float tmp = factor * (*p1-*p2);
            tmp += *p1;
            if(tmp < 0)
                *p2 = 0;
            else if(tmp > 255)
                *p2 = 255;
            else
                *p2 = (uchar) tmp;
            p1++;
            p2++;
        }
    }
    t = clock() - t;
    qDebug() << "Sharpen:" << (float) t/CLOCKS_PER_SEC;
}

void Sharpen::blur(ImagePtr I1, double xSize, double ySize, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
       int w = I1->width();
       int h = I2->height();

       int type;
       ChannelPtr<uchar> p1, p2;
       for(int ch = 0; IP_getChannel(I1, ch, p1, type); ++ch){
           IP_getChannel(I2,ch,p2,type);
           ChannelPtr<uchar> tmp = p2;
           // horizontal blur
           for(int i = 0; i < h; ++i){
               blr(p1, p2, w, xSize, w);
               p1 += w;
               p2 += w;
           }
           p1 = tmp;
           p2 = tmp;
           for(int i = 0; i < w; ++i){
               blr(p1, p2, 1, ySize, h);
               p1++;
               p2++;
           }
       }
}

void Sharpen::blr(ChannelPtr<uchar> src, ChannelPtr<uchar> dest, int stride, double filterSize, int amount)
{
    int     pad = (filterSize - 1)/2;
    int     tmpSize = (pad*2) + amount;
    uchar   tmp[tmpSize];
    if(stride == 1){
        for(int i = 0; i < amount; ++i){
            tmp[pad+i] = *(src+(amount*i));
        }
        for(int i = pad; i > 0; --i){
            tmp[i-1] = tmp[i];
            tmp[tmpSize - i] = tmp[tmpSize - i - 1];
        }
        for(int i = pad; i < amount+pad; ++i){
            int sum = 0;
            for(int j = i-pad; j <= i+pad; ++j){
                sum += tmp[j];
            }
            *(dest+(amount*(i-pad))) = (double) sum / (double) filterSize;
        }
    }
    else{
        for(int i = 0; i < amount; ++i){
            tmp[pad+i] = *(src+i);
        }
        for(int i = pad; i > 0; --i){
            tmp[i-1] = tmp[i];
            tmp[tmpSize - i] = tmp[tmpSize - i - 1];
        }
        for(int i = pad; i < amount+pad; ++i){
            int sum = 0;
            for(int j = i-pad; j <= i+pad; ++j){
                sum += tmp[j];
            }
            *(dest+i-pad) = (double) sum / (double) filterSize;
        }
    }
}

void Sharpen::setSize(int)
{
    m_spinBox[0]->setValue(m_slider[0]->value()*2+1);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void Sharpen::setSpin(int)
{
    m_slider[0]->setValue((m_spinBox[0]->value()-1)/2);
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}

void Sharpen::setFactor(int)
{
    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}
