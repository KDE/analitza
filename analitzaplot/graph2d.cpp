/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/


#include "graph2d.h"

#include <math.h>


#include <QSvgGenerator>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QList>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFile>
#include <QApplication>
#include <QVector2D>
#include <KLocale>
#include <KDebug>
#include <cmath>
#include <QDebug>
#include <QTimer>

#include "analitza/analyzer.h"

#include "functionsmodel.h"
#include "functionimpl.h"
#include <QClipboard>
#include <QApplication>

#include <KColorUtils>

using namespace std;

QColor const Graph2D::m_axeColor(100,100,255);

QColor const Graph2D::m_derivativeColor(90,90,160);

Graph2D::Graph2D(FunctionsModel *fm, QWidget *parent)
    : QWidget(parent), m_functionsModel(fm)
    ,  mode(None)
    , m_keepRatio(true)
    , m_framed(false)
    , m_readonly(false), m_posText()
    , m_axisXLabel("x"), m_axisYLabel("y")
    , m_showHTicks(true), m_showVTicks(true)
    , m_forceDrawFunction(false)
    , m_tickScaleUseSymbols(true)
    , m_tickScaleNumerator(1)
    , m_tickScaleDenominator(1)
    , m_gridColor(KColorUtils::lighten(QColor(85,190,255), .35))
    , m_meshGridShown(true)
    , m_useCoordSys(0)
    , m_showHAxes(true)
    , m_showVAxes(true)
    , m_showRTang(true)
    , m_showRNorm(true)
    , m_showR(true)
    , zw(0)
    , ubic(0)
    , penNorm(4)
    , penTang(4)
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->setCursor(Qt::CrossCursor);


    this->setMouseTracking(true);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setViewport(QRectF(QPointF(-12., 10.), QSizeF(24., -20.)));
    defViewport = userViewport;
    this->setAutoFillBackground(false);

    ah = new QTimer(this);

    ah->setInterval(10);
    connect(ah,SIGNAL(timeout()),SLOT(timerWork()));



    offX = offY = 0;





















}

Graph2D::~Graph2D() {}

void Graph2D::timerWork()
{
    zw = zw + 10;
    update();

}

double Graph2D::devFuncion(int tipo, double val)
{
    switch(tipo)
    {
    case 1:
        return((-1 * val) + 7);
        break;
    case 2:
        return(val);
        break;
    case 3:
        return(sin(val/10)*40);
        break;
    case 4:
        return(cos(val/10)*40);
        break;
    case 5:
        return(5*log(val)+3);
        break;
    case 6:
        return((-1 * pow(val/4,2)) + 5);
        break;
    default:
        return -40;
        break;

    }

}

double Graph2D::absF(double n1, double n2)
{
    double r;
    r = n1 - n2;
    if(r < 0)
    {
        r = r * -1;
    }

    return r;
}

void Graph2D::forceDrawFunctionsOnBuffer()
{


    drawFunctions(&buffer);

    update();

}


void Graph2D::drawAxes(QPainter *f, Function::Axe a)
{


    f->setRenderHint(QPainter::Antialiasing, true);

    {
        if ((m_useCoordSys == 1) )
            drawPolarAxes(f);
        else if ((m_useCoordSys == 0) )
            drawCartesianAxes(f);


    }


    QString rightBound=QString::number(viewport.right());
    int width=f->fontMetrics().width(rightBound);

    f->drawText(QPointF(3.+this->width()/2., 13.                 ), QString::number(viewport.top()));
    f->drawText(QPointF(3.+this->width()/2., this->height()-5.   ), QString::number(viewport.bottom()));
    f->drawText(QPointF(8.                 , this->height()/2.-5.), QString::number(viewport.left()));
    f->drawText(QPointF(this->width()-width, this->height()/2.-5.), rightBound);

}

void Graph2D::drawPolarAxes(QPainter *w)
{
    qreal until = qMax(viewport.height(), viewport.width());

    QPen ceixos;
    ceixos.setColor(m_gridColor);
    w->setPen(ceixos);
    const QPointF center = toWidget(QPointF(0.,0.));

    ceixos.setColor(m_gridColor);
    ceixos.setStyle(Qt::SolidLine);
    w->setPen(ceixos);

    QString symbol =m_tickScaleSymbol;
    qreal symbolValue = m_tickScaleSymbolValue;
    bool symbolFormat = m_tickScaleUseSymbols;
    int numerator = m_tickScaleNumerator;
    int denominator =m_tickScaleDenominator;


    qreal coef = (qreal)(numerator);
    coef /= (qreal)(denominator);
    bool isSymbol = !symbol.isEmpty();
    symbolFormat = isSymbol? symbolFormat : false;
    qreal tickValue = isSymbol? coef*symbolValue : coef;

    QString numStr = QString::number(numerator);
    QString denStr = QString::number(denominator);



    qreal inc = tickValue;

    int decimalPrecision = denominator == 1 && !isSymbol? 0 : denominator == 2? 1 : 2;




    double xini=floor((viewport.left())/inc)*inc;
    double yini=(floor(viewport.bottom())/inc)*inc;

    double xend=ceil((viewport.right())/inc)*inc;
    double yend=(ceil(viewport.top())/inc)*inc;

    int correctScale = 1;


    if (viewport.width()>50)
    {
        correctScale = 10;
        inc *= correctScale;
        xini = floor(xini/inc)*inc;
        yini = floor(yini/inc)*inc;
    }

    QFontMetrics fm(w->font());

    if (m_meshGridShown)
        for (qreal i = inc; i < until; i +=inc )
        {
            QPointF p(toWidget(QPointF(i,i)));
            QPointF p2(toWidget(QPointF(-i,-i)));
            QRectF er(p.x(),p.y(), p2.x()-p.x(),p2.y()-p.y());
            w->drawEllipse(er);
        }

    int i = 1;

    QPointF p;

    QPen tickpen;
    tickpen.setColor(Qt::black);
    tickpen.setStyle(Qt::SolidLine);

    QFont serifFont("Times", 10, QFont::Bold);
    w->setFont(serifFont);

    if (m_showHTicks)
    {


        for(double x = -inc; x > xini; x -= inc, i+=1)
        {
            p = toWidget(QPointF(x, 0.));

            w->setPen(ceixos);


            {
                w->setPen(tickpen);

                if (!isSymbol || !symbolFormat)
                {
                    w->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));

                    continue;
                }

                int iCorrected = i*correctScale;

                QString sign("");
                if (x <= 0.) sign = QString("-");

                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                w->drawText(p.x(), p.y()+20, sign+QString::number(val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    w->drawText(p.x()-10+fm.width(symbol+sign), p.y()+20+fm.height(), denStr);
                }
            }
        }

        i = 1;

        for(double x = inc; x < xend; x += inc, i+=1)
        {
            p = toWidget(QPointF(x, 0.));

            w->setPen(ceixos);



            {
                w->setPen(tickpen);

                if (!isSymbol || !symbolFormat)
                {
                    w->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));

                    continue;
                }

                int iCorrected = i*correctScale;

                QString sign("");
                if (x <= 0.) sign = QString("-");

                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                w->drawText(p.x(), p.y()+20, sign+QString::number(val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    w->drawText(p.x()-10+fm.width(symbol+sign), p.y()+20+fm.height(), denStr);
                }
            }
        }




    }

    if (m_showVTicks)
    {
        i = 1;

        for(double y = -inc; y > yini; y -= inc, i+=1)
        {
            p = toWidget(QPointF(0., y));

            w->setPen(ceixos);



            {
                w->setPen(tickpen);

                if (!isSymbol || !symbolFormat)
                {
                    w->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));

                    continue;
                }

                int iCorrected = i*correctScale;

                QString sign("");
                if (y <= 0.) sign = QString("-");

                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                qreal correctxpos = fm.width(sign+QString::number(val*numerator)+symbol)+6;

                w->drawText(-correctxpos + p.x(), p.y()+5, sign+QString::number(val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    w->drawText(-correctxpos + p.x()-10+fm.width(symbol+sign), p.y()+5+fm.height(), denStr);
                }
            }
        }

        i = 1;

        for(double y = inc; y < yend; y += inc, i+=1)
        {
            p = toWidget(QPointF(0., y));

            w->setPen(ceixos);



            {
                w->setPen(tickpen);

                if (!isSymbol || !symbolFormat)
                {
                    w->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));

                    continue;
                }

                int iCorrected = i*correctScale;

                QString sign("");
                if (y <= 0.) sign = QString("-");

                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                qreal correctxpos = fm.width(sign+QString::number(val*numerator)+symbol)+6;

                w->drawText(-correctxpos + p.x(), p.y()+5, sign+QString::number(val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    w->drawText(-correctxpos + p.x()-10+fm.width(symbol+sign), p.y()+5+fm.height(), denStr);
                }
            }
        }

    }
    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);
    if (m_showHAxes)
    {

        ceixos.setColor(Qt::darkRed);
        w->setPen(ceixos);
        w->drawLine(QPointF(0., center.y()), Xright);
    }

    if (m_showVAxes)
    {
        ceixos.setColor(Qt::darkGreen);
        w->setPen(ceixos);
        w->drawLine(Ytop, QPointF(center.x(),this->height()));
    }

    QFont labelsfont = w->font();
    labelsfont.setBold(true);



    if (m_showHAxes)
    {
        ceixos.setColor(Qt::darkRed);
        w->setPen(ceixos);
        w->drawText(Xright.x() - fm.width(m_axisXLabel) - 5, center.y() - 20, m_axisXLabel);
    }

    if (m_showVAxes)
    {
        ceixos.setColor(Qt::darkGreen);
        w->setPen(ceixos);
        w->drawText(center.x() + 5, Ytop.y() + fm.height() + 15, m_axisYLabel);
    }

    ceixos.setColor(Qt::darkGreen);
    w->setPen(ceixos);
    w->setBrush(m_axeColor);


    const double width=15., height=4.;
    QPointF dpx(width, height);
    QPointF dpy(height, width);
    QRectF rectX(Xright+dpx, Xright-dpx);
    QRectF rectY(Ytop+dpy, Ytop-dpy);

    int startAngleX = 150*16;
    int startAngleY = 240*16;
    int spanAngle = 60*16;
    if (m_showHAxes)
        w->drawPie(rectX, startAngleX, spanAngle);
    if (m_showVAxes)
        w->drawPie(rectY, startAngleY, spanAngle);


    ceixos.setColor(m_axeColor);
    ceixos.setStyle(Qt::SolidLine);
    w->setPen(ceixos);

}

void Graph2D::drawCartesianAxes(QPainter *finestra)
{
    QPen ceixos;
    QFont natfont = finestra->font();
    const QPointF center = toWidget(QPointF(0.,0.));
    QPointF p;

    ceixos.setColor(m_gridColor);
    ceixos.setStyle(Qt::SolidLine);
    finestra->setPen(ceixos);

    QString symbol =m_tickScaleSymbol;
    qreal symbolValue = m_tickScaleSymbolValue;
    bool symbolFormat = m_tickScaleUseSymbols;
    int numerator = m_tickScaleNumerator;
    int denominator =m_tickScaleDenominator;


    qreal coef = (qreal)(numerator);
    coef /= (qreal)(denominator);
    bool isSymbol = !symbol.isEmpty();
    symbolFormat = isSymbol? symbolFormat : false;
    qreal tickValue = isSymbol? coef*symbolValue : coef;

    QString numStr = QString::number(numerator);
    QString denStr = QString::number(denominator);



    qreal inc = tickValue;

    int decimalPrecision = denominator == 1 && !isSymbol? 0 : denominator == 2? 1 : 2;

    double xini=floor((viewport.left())/inc)*inc;
    double yini=(floor(viewport.bottom())/inc)*inc;

    double xend=ceil((viewport.right())/inc)*inc;
    double yend=(ceil(viewport.top())/inc)*inc;

    int correctScale = 1;


    if (viewport.width()>50)
    {
        correctScale = 10;
        inc *= correctScale;
        xini = floor(xini/inc)*inc;
        yini = floor(yini/inc)*inc;
    }


    QPen tickpen;
    tickpen.setColor(Qt::black);
    tickpen.setStyle(Qt::SolidLine);

    QFont serifFont("Times", 10, QFont::Bold);
    finestra->setFont(serifFont);

    int i = 1;

    QFontMetrics fm(finestra->font());

    for(double x = -inc; x > xini; x -= inc, i+=1)
    {
        p = toWidget(QPointF(x, 0.));

        finestra->setPen(ceixos);

        if(m_meshGridShown)
            finestra->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            finestra->drawLine(p, p+QPointF(0.,-3.));

        if (m_showHTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));

                continue;
            }

            int iCorrected = i*correctScale;

            QString sign("");
            if (x <= 0.) sign = QString("-");

            int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

            finestra->drawText(p.x(), p.y()+20, sign+QString::number(val*numerator)+symbol);

            if ((denominator != 1) && (iCorrected % denominator != 0))
            {
                finestra->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                finestra->drawText(p.x()-10+fm.width(symbol+sign), p.y()+20+fm.height(), denStr);
            }
        }
    }

    i = 1;

    for(double x = inc; x < xend; x += inc, i+=1)
    {
        p = toWidget(QPointF(x, 0.));

        finestra->setPen(ceixos);

        if(m_meshGridShown)
            finestra->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            finestra->drawLine(p, p+QPointF(0.,-3.));

        if (m_showHTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));

                continue;
            }

            int iCorrected = i*correctScale;

            QString sign("");
            if (x <= 0.) sign = QString("-");

            int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

            finestra->drawText(p.x(), p.y()+20, sign+QString::number(val*numerator)+symbol);

            if ((denominator != 1) && (iCorrected % denominator != 0))
            {
                finestra->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                finestra->drawText(p.x()-10+fm.width(symbol+sign), p.y()+20+fm.height(), denStr);
            }
        }
    }

    i = 1;

    for(double y = -inc; y > yini; y -= inc, i+=1)
    {
        p = toWidget(QPointF(0., y));

        finestra->setPen(ceixos);

        if(m_meshGridShown)
            finestra->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        else
            finestra->drawLine(p, p+QPointF(3.,0.));

        if (m_showVTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));

                continue;
            }

            int iCorrected = i*correctScale;

            QString sign("");
            if (y <= 0.) sign = QString("-");

            int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

            qreal correctxpos = fm.width(sign+QString::number(val*numerator)+symbol)+6;

            finestra->drawText(-correctxpos + p.x(), p.y()+5, sign+QString::number(val*numerator)+symbol);

            if ((denominator != 1) && (iCorrected % denominator != 0))
            {
                finestra->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                   -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                finestra->drawText(-correctxpos + p.x()-10+fm.width(symbol+sign), p.y()+5+fm.height(), denStr);
            }
        }
    }

    i = 1;

    for(double y = inc; y < yend; y += inc, i+=1)
    {
        p = toWidget(QPointF(0., y));

        finestra->setPen(ceixos);

        if(m_meshGridShown)
            finestra->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        else
            finestra->drawLine(p, p+QPointF(3.,0.));

        if (m_showVTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));

                continue;
            }

            int iCorrected = i*correctScale;

            QString sign("");
            if (y <= 0.) sign = QString("-");

            int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

            qreal correctxpos = fm.width(sign+QString::number(val*numerator)+symbol)+6;

            finestra->drawText(-correctxpos + p.x(), p.y()+5, sign+QString::number(val*numerator)+symbol);

            if ((denominator != 1) && (iCorrected % denominator != 0))
            {
                finestra->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                   -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                finestra->drawText(-correctxpos + p.x()-10+fm.width(symbol+sign), p.y()+5+fm.height(), denStr);
            }
        }
    }

    finestra->setFont(natfont);
    finestra->setPen(ceixos);

    ceixos.setStyle(Qt::SolidLine);

    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);

    if (m_showHAxes)
    {

        ceixos.setColor(Qt::darkRed);
        finestra->setPen(ceixos);
        finestra->drawLine(QPointF(0., center.y()), Xright);
    }

    if (m_showVAxes)
    {
        ceixos.setColor(Qt::darkGreen);
        finestra->setPen(ceixos);
        finestra->drawLine(Ytop, QPointF(center.x(),this->height()));
    }



    ceixos.setColor(Qt::darkGreen);
    finestra->setPen(ceixos);
    finestra->setBrush(m_axeColor);

    const double width=15., height=4.;
    QPointF dpx(width, height);
    QPointF dpy(height, width);
    QRectF rectX(Xright+dpx, Xright-dpx);
    QRectF rectY(Ytop+dpy, Ytop-dpy);

    int startAngleX = 150*16;
    int startAngleY = 240*16;
    int spanAngle = 60*16;
    if (m_showHAxes)
        finestra->drawPie(rectX, startAngleX, spanAngle);
    if (m_showVAxes)
        finestra->drawPie(rectY, startAngleY, spanAngle);



    QFont labelsfont = finestra->font();
    labelsfont.setBold(true);

    ceixos.setColor(Qt::darkRed);
    finestra->setPen(ceixos);
    finestra->drawText(Xright.x() - fm.width(m_axisXLabel) - 5, center.y() - 20, m_axisXLabel);

    ceixos.setColor(Qt::darkGreen);
    finestra->setPen(ceixos);
    finestra->drawText(center.x() + 5, Ytop.y() + fm.height() + 15, m_axisYLabel);

    ceixos.setColor(m_axeColor);
    ceixos.setStyle(Qt::SolidLine);
    finestra->setPen(ceixos);
}

void Graph2D::drawFunctions(QPaintDevice *qpd)
{
    QPalette p=qApp->palette();
    QPen pfunc;

    if(buffer.isNull() || buffer.width()!=width() || buffer.height()!=height())
        buffer = QPixmap(this->width(), this->height());

    buffer.fill(p.color(QPalette::Active, QPalette::Base));

    pfunc.setColor(QColor(0,150,0));
    pfunc.setWidth(2);

    QPainter finestra;
    finestra.begin(qpd);



    Function::Axe t=Function::Cartesian;
    if(m_functionsModel->hasSelection())
        t=m_functionsModel->currentFunction().axeType();
    drawAxes(&finestra, t);


    finestra.setRenderHint(QPainter::Antialiasing, true);

    int k=0;

    int totalfuncs = m_functionsModel->rowCount();

    if(m_functionsModel->getModAnActivo())
    {
        if(!ah->isActive())
        {
            ah->start();


        }


    }
    else
    {
        if(ah->isActive())
        {
            ah->stop();

        }

    }

    for (int i = 0; i < totalfuncs; i+=1)

    {
        k = i;




        Function *it = m_functionsModel->editFunction(i);

        FunctionImpl2D *solver = static_cast<FunctionImpl2D*>(it->solver());

        if (!it->isShown() || (it->dimension() == 3) || (!solver))
        {
            //punto de control seguro de 2011 was ...
            //qDebug() << "entra dib" << it->isShown() << solver->lambda().toString();
            continue;

        }

        pfunc.setColor(it->color());
        qreal selfunwidthincre= 0.0;

        if (totalfuncs > 1)
            if (m_functionsModel->isSelected(k))
            {
                selfunwidthincre = 0.4;
                if(solver->dimension() == 2)
                {
                    if(solver->arguments() == QStringList("t"))
                        m_showR = false;
                    else
                        m_showR = true;
                }

            }

        if (m_functionsModel->isSelected(k))
        {

            if(solver->dimension() == 2)
            {
                if(solver->arguments() == QStringList("t"))
                    m_showR = false;
                else
                    m_showR = true;
            }


        }


        pfunc.setWidthF(millimetersToPixels(it->lineWidth() + selfunwidthincre, qpd));

        finestra.setPen(pfunc);




        if(m_functionsModel->getModAnActivo() && m_functionsModel->isSelected(k) && solver->arguments() == QStringList("t"))
        {



            if(m_functionsModel->getAnimActiva() != 3)
            {


                if(m_functionsModel->getDirAnim() == 1)
                {

                    if(m_functionsModel->getAnimActiva() == 1)
                    {
                        if(zw >= 10)
                        {

                            zw = 0;
                            if(ubic < solver->paths().size())
                            {
                                ubic = ubic + 1;

                            }


                            for (i =0 ; i < ubic; i++)
                            {
                                finestra.drawLine(toWidget(solver->paths().at(i)));
                            }
                        }
                        else
                        {
                            for (i =0 ; i < ubic; i++)
                            {
                                finestra.drawLine(toWidget(solver->paths().at(i)));
                            }
                        }

                    }
                    else
                    {
                        for (i =0 ; i < ubic; i++)
                        {
                            finestra.drawLine(toWidget(solver->paths().at(i)));
                        }
                    }

                }
                else
                {
                    if(m_functionsModel->getAnimActiva() == 1)
                    {
                        if(zw >= 10)
                        {
                            zw = 0;
                            if(ubic > 0)
                            {
                                ubic = ubic - 1;
                            }

                            for (i = solver->paths().size() - 1 ; i >= ubic; i--)
                            {
                                finestra.drawLine(toWidget(solver->paths().at(i)));
                            }
                        }
                        else
                        {
                            for (i = solver->paths().size() - 1 ; i >= ubic; i--)
                            {
                                finestra.drawLine(toWidget(solver->paths().at(i)));
                            }
                        }

                    }
                    else
                    {
                        for (i = solver->paths().size() - 1 ; i >= ubic; i--)
                        {
                            finestra.drawLine(toWidget(solver->paths().at(i)));
                        }
                    }
                }



            }
            else
            {
                if(m_functionsModel->getDirAnim() == 1)
                    ubic = 0;
                else
                {
                    if(solver->paths().size() > 0)
                        ubic = solver->paths().size() - 1;
                }
            }

        }
        else
        {

            for (i =0 ; i < solver->paths().size(); i++)
            {

                finestra.drawLine(toWidget(solver->paths().at(i)));


            }

        }

    }

    finestra.end();

}

void Graph2D::drawFunctionsAreaE(QPaintDevice* qpd)
{

    QPalette p=qApp->palette();
    QPen pfunc;

    if(buffer.isNull() || buffer.width()!=width() || buffer.height()!=height())
        buffer = QPixmap(this->width(), this->height());

    buffer.fill(p.color(QPalette::Active, QPalette::Base));

    pfunc.setColor(QColor(0,150,0));
    pfunc.setWidth(2);

    QPainter finestra;
    finestra.begin(qpd);



    Function::Axe t=Function::Cartesian;
    if(m_functionsModel->hasSelection())
        t=m_functionsModel->currentFunction().axeType();
    drawAxes(&finestra, t);


    finestra.setRenderHint(QPainter::Antialiasing, true);

    int k=0;



    QVector<QLineF> lineAreaE;

    if(m_functionsModel->getAreaEjemplo() == 0)
    {
        lineAreaE = m_functionsModel->getPathsACre();

    }
    if(m_functionsModel->getAreaEjemplo() == 1)
    {
        lineAreaE = m_functionsModel->getPathsAEnt();

    }
    if(m_functionsModel->getAreaEjemplo() == 2)
    {
        lineAreaE = m_functionsModel->getPathsADec();

    }


    pfunc.setColor(Qt::yellow);
    pfunc.setWidth(2);
    finestra.setPen(pfunc);

    for(k=0; k<lineAreaE.size(); k++)
    {
        finestra.drawLine(toWidget(lineAreaE.at(k)));
    }

    drawAxes(&finestra, t);

    QVector<QLineF> lineFuncE;
    QVector<QLineF> lineFuncE2;

    if(m_functionsModel->getAreaEjemplo() == 0)
    {
        lineFuncE = m_functionsModel->getPathsCre();

    }
    if(m_functionsModel->getAreaEjemplo() == 1)
    {
        lineFuncE = m_functionsModel->getPathsEnt();
        lineFuncE2 = m_functionsModel->getPathsEnt2();

    }
    if(m_functionsModel->getAreaEjemplo() == 2)
    {
        lineFuncE = m_functionsModel->getPathsDec();


    }

    pfunc.setColor(Qt::green);
    pfunc.setWidth(2);
    finestra.setPen(pfunc);

    for(k=0; k<lineFuncE.size(); k++)
    {
        finestra.drawLine(toWidget(lineFuncE.at(k)));
    }

    for(k=0; k<lineFuncE2.size(); k++)
    {
        finestra.drawLine(toWidget(lineFuncE2.at(k)));
    }

    pfunc.setColor(Qt::gray);
    pfunc.setWidth(2);
    finestra.setPen(pfunc);
    finestra.setBrush(QBrush());

    if(m_functionsModel->getAreaEjemplo() == 0)
    {



        double dif;
        double rest;
        int i;

        QRectF recColumn;

        double li;
        double ls;
        int div;
        li = 0.5;
        ls = 6.0;
        div = m_functionsModel->getCantDiv();


        dif = (ls - li) / div;
        dif = abs2(dif);

        rest = ls;

        for(i=0; i<div; ++i)
        {
            recColumn.setTopLeft(toWidget(QPointF(rest-dif,devFuncion(5,rest-dif))));
            recColumn.setTopRight(toWidget(QPointF(rest,devFuncion(5,rest))));
            recColumn.setBottomLeft(toWidget(QPointF(rest-dif,0)));
            recColumn.setBottomRight(toWidget(QPointF(rest,0)));

            finestra.drawRect(recColumn);



            rest -= dif;
        }
    }

    if(m_functionsModel->getAreaEjemplo() == 2)
    {
        finestra.setBrush(QBrush(Qt::blue));

        double dif;
        double rest;
        int i;

        QRectF recColumn;

        double li;
        double ls;
        int div;
        li = 0.5;
        ls = 6.0;
        div = m_functionsModel->getCantDiv();


        dif = (ls - li) / div;
        dif = abs2(dif);
        rest = ls;

        for(i=0; i<div; ++i)
        {
            recColumn.setTopLeft(toWidget(QPointF(rest-dif,devFuncion(6,rest-dif))));
            recColumn.setTopRight(toWidget(QPointF(rest,devFuncion(6,rest))));
            recColumn.setBottomLeft(toWidget(QPointF(rest-dif,0)));
            recColumn.setBottomRight(toWidget(QPointF(rest,0)));
            finestra.drawRect(recColumn);


            rest -= dif;
        }
    }

    if(m_functionsModel->getAreaEjemplo() == 1)
    {
        QRectF recColumn;


        double dif;
        double rest;
        double med;



        double li;
        double ls;
        int div;
        li = 0.5;
        ls = 6.0;
        div = m_functionsModel->getCantDiv();


        dif = (ls - li) / div;
        med = (ls + li) / 2;
        dif = abs2(dif);
        rest = ls;



        recColumn.setTopLeft(toWidget(QPointF(med-dif,devFuncion(5,med))));
        recColumn.setTopRight(toWidget(QPointF(med,devFuncion(5,med))));
        recColumn.setBottomLeft(toWidget(QPointF(med-dif,devFuncion(1,med))));
        recColumn.setBottomRight(toWidget(QPointF(med,devFuncion(1,med))));
        finestra.drawRect(recColumn);

    }
}

double Graph2D::abs2(double n)
{
    if(n >= 0)
        return n;
    else
        return -1*n;
}

void Graph2D::drawFunctionsArea(QPaintDevice* qpd)
{




    QPalette p=qApp->palette();
    QPen pfunc;

    if(buffer.isNull() || buffer.width()!=width() || buffer.height()!=height())
        buffer = QPixmap(this->width(), this->height());

    buffer.fill(p.color(QPalette::Active, QPalette::Base));

    pfunc.setColor(QColor(0,150,0));
    pfunc.setWidth(2);

    QPainter finestra;
    finestra.begin(qpd);



    Function::Axe t=Function::Cartesian;
    if(m_functionsModel->hasSelection())
        t=m_functionsModel->currentFunction().axeType();
    drawAxes(&finestra, t);






    finestra.setRenderHint(QPainter::Antialiasing, true);

    int k=0;



    int totalfuncs = m_functionsModel->rowCount();

    QPen ccursor;

    ccursor.setColor(Qt::yellow);
    ccursor.setStyle(Qt::SolidLine);
    ccursor.setWidth(2);

    QVector<QLineF> lArea;

    lArea = m_functionsModel->getPathsArea();
    int j;

    finestra.setPen(ccursor);




    for(j=0; j<lArea.size(); j++)
    {

        finestra.drawLine(toWidget(lArea.at(j)));
    }

    drawAxes(&finestra, t);

    for (int i = 0; i < totalfuncs; i+=1)

    {
        k = i;




        Function *it = m_functionsModel->editFunction(i);



        FunctionImpl2D *solver = static_cast<FunctionImpl2D*>(it->solver());

        if (!it->isShown() || (it->dimension() == 3) || (!solver))
            continue;






        pfunc.setColor(it->color());
        qreal selfunwidthincre= 0.0;


        pfunc.setWidthF(millimetersToPixels(it->lineWidth() + selfunwidthincre, qpd));

        finestra.setPen(pfunc);






        if((m_functionsModel->getPFuncion1() != -1) && (it->expression().toString() == m_functionsModel->getMFuncion1()))
        {
            for (j =0 ; j < solver->paths().size(); j++)
            {









                finestra.drawLine(toWidget(solver->paths().at(j)));
            }

            m_functionsModel->setPFuncion1(i);


        }

        if((m_functionsModel->getPFuncion2() != -1) && (it->expression().toString() == m_functionsModel->getMFuncion2()))
        {
            for (j =0 ; j < solver->paths().size(); j++)
            {


                finestra.drawLine(toWidget(solver->paths().at(j)));
            }
            m_functionsModel->setPFuncion2(i);
        }




    }

    QVector<QPointF> intPoints = m_functionsModel->getIntPoints();

    QPen pIntersec;



    double px;
    double py;
    pIntersec.setColor(Qt::red);
    pIntersec.setStyle(Qt::SolidLine);
    pIntersec.setWidth(12);
    finestra.setPen(pIntersec);







    for(j=0; j<intPoints.size(); j++)
    {


        px = intPoints.at(j).x();
        py = intPoints.at(j).y();



        finestra.drawLine(toWidget(QPointF(px,py)),toWidget(QPointF(px,py)));


    }



    finestra.end();




}

void Graph2D::paintEvent(QPaintEvent * )
{





    if(!m_functionsModel->getMArea())
    {
        drawFunctions(&buffer);

    }

    else
    {
        if(m_functionsModel->getMAreaE())
        {
            drawFunctionsAreaE(&buffer);
        }
        else
        {
            drawFunctionsArea(&buffer);
        }



    }




    QPainter finestra(this);









    finestra.setRenderHint(QPainter::Antialiasing, true);

    finestra.drawPixmap(0,0,width(),height(), buffer);

    if(!m_functionsModel->getMArea())
    {


        QPen ccursor;
        QPointF ultim;


        QLineF sl;
        QLineF sn;

        if (!m_readonly && mode==None)
        {
            ultim = toWidget(mark);


            ccursor.setColor(m_derivativeColor);
            ccursor.setStyle(Qt::SolidLine);
            sl=slope(mark);
            sn=slopeNormal(mark);

            if(m_showRTang && m_showR)
            {
                if (!sl.isNull())
                {
                    sl.translate(mark);

                    finestra.setPen(ccursor);

                    if(!sl.isNull() && !isnan(sl.length()) && !isinf(sl.length()))
                    {
                        finestra.drawLine(toWidget(sl));
                        penTang = abs2(sl.p1().y() - sl.p2().y())/ abs2(sl.p1().x() - sl.p2().x());

                    }


                    if (!isnan(ultim.x()) && !isnan(ultim.y()) && !isinf(ultim.x()) && !isinf(ultim.y()))
                    {
                        ccursor.setColor(QColor(0xc0,0,0));
                        ccursor.setStyle(Qt::SolidLine);



                        finestra.setPen(ccursor);



                    }
                }
            }

            if(m_showRNorm && m_showR)
            {
                if (!sn.isNull())
                {
                    sn.translate(mark);
                    ccursor.setColor(QColor(Qt::darkYellow));

                    finestra.setPen(ccursor);

                    if(!sn.isNull() && !isnan(sn.length()) && !isinf(sn.length()))
                    {
                        finestra.drawLine(toWidget(sn));
                        penNorm = abs2(sn.p1().y() - sn.p2().y())/ abs2(sn.p1().x() - sn.p2().x());
                    }




                    if (!isnan(ultim.x()) && !isnan(ultim.y()) && !isinf(ultim.x()) && !isinf(ultim.y()))
                    {
                        ccursor.setColor(QColor(0xc0,0,0));
                        ccursor.setStyle(Qt::SolidLine);



                        finestra.setPen(ccursor);



                    }
                }
            }





            int w=finestra.fontMetrics().width(m_posText)+15, h=finestra.fontMetrics().height();

            if(ultim.x()+w > this->width())
                ultim.setX(this->width()-w);
            if(ultim.y()+h > this->height())
                ultim.setY(this->height()-h);
            if(ultim.y() < 0.)
                ultim.setY(0.);

            finestra.setPen(QPen(Qt::black));
            finestra.drawText(QPointF(ultim.x()+15., ultim.y()+15.), m_posText);



        }


        if (mode==Selection)
        {

            ccursor.setColor(QColor(0xc0,0,0));
            ccursor.setStyle(Qt::SolidLine);
            finestra.setPen(ccursor);
            finestra.setBrush(QColor(0xff,0xff, 0,0x90));
            finestra.drawRect(QRect(press,last));
        }

        if(m_framed)
        {
            QPoint p2=QPoint(this->width(), this->height());
            QPen bord(Qt::black);
            finestra.setPen(bord);
            finestra.drawRect(QRect(QPoint(0,0), p2-QPoint(2,2)));
        }









        QString mouseOnCanvas = QString("( %1 , %2 )").arg(QString::number(m_mousePos.x(), 'f')).arg(QString::number(m_mousePos.y(), 'f'));
        QString mouseOnCurvePos = QString("( %1 , %2 )").arg(QString::number(mark.x(), 'f')).arg(QString::number(mark.y(), 'f'));

        finestra.save();

        QPen leyendap = finestra.pen();
        leyendap.setColor(Qt::black);

        QFont leyendaf("Times", 11, QFont::Bold);

        finestra.setFont(leyendaf);
        finestra.setPen(leyendap);

        finestra.drawText(width() - 170  ,height() - 80, "Cursor en:");
        finestra.drawText(width() - 170 ,height() - 60, mouseOnCanvas);


        if (m_functionsModel->rowCount() > 0)
        {
            finestra.drawText(30 ,height() - 80, "Punto sobre la curva:");
            finestra.drawText(30 ,height() - 60, mouseOnCurvePos);

            qreal m = sl.dy()/sl.dx();

            FunctionImpl2D *s2d2 = static_cast<FunctionImpl2D*>(m_functionsModel->currentFunction().solver());


            if(s2d2->arguments() != QStringList("t"))
            {
                if (!std::isnan(m))
                {


                    finestra.drawText(30 ,height() - 40, "Recta tangente:");


                    qreal b_real = mark.y() - m*mark.x();
                    QString b = QString::number(b_real, 'f');

                    if (b_real>= 0.)
                        finestra.drawText(30 ,height() - 20, QString::number(m,'f')+"x"+"+"+b);
                    else
                        finestra.drawText(30 ,height() - 20, QString::number(m,'f')+"x"+b);

                    if(m != 0)
                    {
                        qreal mn = -1/m;

                        finestra.drawText(width() - 170 ,height() - 40, "Recta normal:");


                        qreal b_real = mark.y() - m*mark.x();
                        QString b = QString::number(b_real, 'f');

                        if (b_real>= 0.)
                            finestra.drawText(width() - 170 ,height() - 20, QString::number(mn,'f')+"x"+"+"+b);
                        else
                            finestra.drawText(width() - 170 ,height() - 20, QString::number(mn,'f')+"x"+b);

                    }

                }
                else
                {



                    QLineF lineF;

                    FunctionImpl2D *s2d = static_cast<FunctionImpl2D*>(m_functionsModel->currentFunction().solver());

                    lineF = s2d->derivative(fromWidget(mark.toPoint()));

                    if(lineF.length() == 0.0)
                    {
                        ccursor.setColor(QColor(Qt::blue));
                        ccursor.setStyle(Qt::SolidLine);



                        finestra.setPen(ccursor);

                        QString ar1;
                        QString ar2;

                        if(penTang - 0 < 0.0001)
                        {
                            finestra.drawLine(QPointF(0.,ultim.y()), QPointF(this->width(), ultim.y()));
                            finestra.drawLine(QPointF(ultim.x(),0.), QPointF(ultim.x(), this->height()));
                            finestra.drawText(30 ,height() - 40, "Recta tangente:");
                            ar1.setNum(ceil ( m_mousePos.y() * pow (10, 2) ) / pow (10, 2));
                            finestra.drawText(30 ,height() - 20, "y = " + ar1);
                            finestra.drawText(width() - 170 ,height() - 40, "Recta normal:");
                            ar2.setNum(ceil ( m_mousePos.x() * pow (10, 2) ) / pow (10, 2));
                            finestra.drawText(width() - 170 ,height() - 20, "x = " + ar2);
                        }

                        if(penNorm - 0 < 0.0001)
                        {
                            finestra.drawLine(QPointF(0.,ultim.y()), QPointF(this->width(), ultim.y()));
                            finestra.drawLine(QPointF(ultim.x(),0.), QPointF(ultim.x(), this->height()));
                            finestra.drawText(30 ,height() - 40, "Recta tangente:");
                            ar1.setNum(ceil ( m_mousePos.x() * pow (10, 2) ) / pow (10, 2));
                            finestra.drawText(30 ,height() - 20, "x = " + ar1);
                            finestra.drawText(width() - 170 ,height() - 40, "Recta normal:");
                            ar2.setNum(ceil ( m_mousePos.y() * pow (10, 2) ) / pow (10, 2));
                            finestra.drawText(width() - 170 ,height() - 20, "y = " + ar2);
                        }
                    }




                    finestra.drawLine(lineF);







                }
            }




        }
    }
    else
    {

        if(m_functionsModel->getMAreaE())
        {
            finestra.save();

        }
        else
        {
            int j;

            QVector<QPointF> intPoints = m_functionsModel->getIntPoints();

            QPen pIntersec;



            double px;
            double py;
            QString mouseOnCanvas;

            for(j=0; j<intPoints.size(); j++)
            {


                px = m_mousePos.x();
                py = m_mousePos.y();



                if(absF(intPoints.at(j).x(),px) <= 0.1 && absF(intPoints.at(j).y(),py) <= 0.1)
                {

                    pIntersec.setColor(Qt::black);
                    finestra.setPen(pIntersec);

                    mouseOnCanvas = QString("( %1 , %2 )").arg(QString::number(ceil ( intPoints.at(j).x() * pow (10, 2) ) / pow (10, 2), 'f')).arg(QString::number(ceil ( intPoints.at(j).y() * pow (10, 2) ) / pow (10, 2), 'f'));

                    finestra.drawText(30 ,height() - 80, "Punto de Interseccion:");
                    finestra.drawText(30 ,height() - 60, mouseOnCanvas);


                }
            }

            finestra.save();
        }





    }




    finestra.restore();
    finestra.end();

}

void Graph2D::wheelEvent(QWheelEvent *e)
{
    int d = e->delta()>0 ? -1 : 1;

    if(d>0 || (viewport.width()+d > 2 && viewport.height()+d < 2))
    {
        QPointF p = fromWidget(e->pos());
        QRectF nv;
        nv.setLeft(viewport.left() - d);
        nv.setTop(viewport.top() + d);
        nv.setRight(viewport.right() + d);
        nv.setBottom(viewport.bottom() - d);
        setViewport(nv, false);

        QPointF p2 = p-fromWidget(e->pos());
        nv.translate(p2);
        setViewport(nv);
    }
}

void Graph2D::mousePressEvent(QMouseEvent *e)
{



    emit viewClicked();





    if (!m_readonly)
    {
        last = press = e->pos();
        ant = toViewport(e->pos());





        if (e->button()==Qt::RightButton)
        {
            mode=Selection;
            this->setCursor(QCursor(Qt::PointingHandCursor));
        }

        if (e->button()==Qt::LeftButton)
        {
            mode=Pan;

            this->setCursor(QCursor(Qt::SizeAllCursor));

            lastpos = e->pos();
        }





    }

}

void Graph2D::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_readonly)
        this->setCursor(Qt::ArrowCursor);
    else
        this->setCursor(Qt::CrossCursor);

    if (!m_readonly && mode==Selection)
    {
        QPointF pd = toViewport(e->pos())-toViewport(press);

        QPoint pd2 = e->pos()-press;
        QRect rr(press, QSize(pd2.x(), pd2.y()));

        if(rr.width()>20 && rr.height()>20)
        {
            QRectF r(fromWidget(press), QSizeF(pd.x(), pd.y()));

            if(r.top()<r.bottom())
            {
                double aux = r.bottom();
                r.setBottom(r.top());
                r.setTop(aux);
            }

            if(r.right()<r.left())
            {
                double aux = r.left();
                r.setLeft(r.right());
                r.setRight(aux);
            }

            setViewport(r);
        }
        else
        {

            sendStatus(i18n("Selected viewport too small"));

        }
    }
    mode = None;








    QPoint relp = e->pos();
    if (lastpos == QPoint(0,0))
    {

    }
    else
    {

        int _xdiff = relp.x() - lastpos.x();
        int _ydiff = relp.y() - lastpos.y();
        lastpos = QPoint(0,0);
        shiftCanvas(_xdiff, _ydiff);
    }

    this->repaint();
}

void Graph2D::mouseMoveEvent(QMouseEvent *e)
{
    lastMark = mark;
    mark=calcImage(fromWidget(e->pos()));
    m_mousePos = fromWidget(e->pos());

    if (!m_readonly && mode==Pan && ant != toViewport(e->pos()))
    {
        QPointF rel = toViewport(e->pos() - press);
        viewport.moveLeft(viewport.left() - rel.x());
        viewport.moveTop(viewport.top() - rel.y());
        setViewport(viewport);

        press = e->pos();





        QPoint relp = e->pos();

        if (lastpos == QPoint(0,0))
        {
            lastpos = relp;
        }
        else
        {
            int _xdiff = relp.x() - lastpos.x();
            int _ydiff = relp.y() - lastpos.y();
            lastpos = relp;
            shiftCanvas(_xdiff, _ydiff);
        }


    }
    else if(e->buttons()&Qt::RightButton)
    {
        last = e->pos();
    }
    else if(e->buttons()==0)
        sendStatus(QString("x=%1 y=%2").arg(mark.x(),3,'g',5).arg(mark.y(),3,'g',5));

    this->repaint();
}

void Graph2D::keyPressEvent(QKeyEvent * e)
{
    const double xstep=viewport.width()/12., ystep=viewport.height()/10.;

    switch(e->key())
    {
    case Qt::Key_Right:
        setViewport(userViewport.translated(xstep, 0));
        break;
    case Qt::Key_Left:
        setViewport(userViewport.translated(-xstep, 0));
        break;
    case Qt::Key_Down:
        setViewport(userViewport.translated(0, -ystep));
        break;
    case Qt::Key_Up:
        setViewport(userViewport.translated(0, ystep));
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    default:
        return;
    }
}

QPointF Graph2D::calcImage(const QPointF& ndp)
{





    if (!m_functionsModel) return QPointF();


    if (m_functionsModel->rowCount() == 0)
        return QPointF();

    if (!m_functionsModel) return QPointF();

    QPair<QPointF, QString> ret;
    ret.first=ndp;


    if (m_functionsModel->hasSelection())
    {
        Function & f = m_functionsModel->currentFunction();


        if (f.dimension() != 2) return QPointF();

        if(f.isShown())

        {
            ret = dynamic_cast<FunctionImpl2D*>(f.solver())->calc(ndp);
        }
    }

    return ret.first;


}

QLineF Graph2D::slope(const QPointF & dp) const
{

    if (!m_functionsModel) return QLineF();

    if (!m_functionsModel) return QLineF();

    QLineF ret;
    if(m_functionsModel->hasSelection())
    {
        const Function & f = m_functionsModel->currentFunction();
        if(f.isShown())
        {
            QPointF a = dp;
            QPointF b = lastMark;

            qreal s = (a.y()-b.y())/(a.x()-b.x());

            if((a.x()-b.x()) != 0)
            {
                ret = slopeToLine(s);
            }
            else
            {


            }

        }
    }

    return ret;
}

QLineF Graph2D::slopeNormal(const QPointF& dn) const
{
    if (!m_functionsModel) return QLineF();


    if (!m_functionsModel) return QLineF();

    QLineF ret;
    if(m_functionsModel->hasSelection())
    {
        const Function & f = m_functionsModel->currentFunction();
        if(f.isShown())
        {
            QPointF a = dn;
            QPointF b = lastMark;

            qreal s = (a.y()-b.y())/(a.x()-b.x());
            s = -1/s;

            ret = slopeToLine(s);

        }
    }

    return ret;


}

void Graph2D::unselect()
{


    m_functionsModel->unselect();
}

QLineF Graph2D::toWidget(const QLineF &f) const
{
    return QLineF(toWidget(f.p1()), toWidget(f.p2()));
}

QPointF Graph2D::toWidget(const QPointF& p) const
{
    double left=-viewport.left(), top=-viewport.top();
    return QPointF((left + p.x()) * rang_x,  (top + p.y()) * rang_y);
}

QPointF Graph2D::fromWidget(const QPoint& p) const
{
    double part_negativa_x = -viewport.left();
    double part_negativa_y = -viewport.top();
    return QPointF(p.x()/rang_x-part_negativa_x, p.y()/rang_y-part_negativa_y);
}

QPointF Graph2D::toViewport(const QPoint &mv) const
{
    return QPointF(mv.x()/rang_x, mv.y()/rang_y);
}

void Graph2D::setViewport(const QRectF& vp, bool repaint)
{
    userViewport = vp;
    Q_ASSERT(userViewport.top()>userViewport.bottom());
    Q_ASSERT(userViewport.right()>userViewport.left());

    sendStatus(QString("(%1, %2)-(%3, %4)")
               .arg(viewport.left()).arg(viewport.top()).arg(viewport.right()).arg(viewport.bottom()));
    updateScale(repaint);

    emit viewportChanged(userViewport);
}

void Graph2D::setXAxisLabel(const QString &label)
{
    m_axisXLabel = label;
    update();
}

void Graph2D::setYAxisLabel(const QString &label)
{
    m_axisYLabel = label;
    update();

}

void Graph2D::improvedResolutionForCurrentViewport()
{

    QRect _r = toBiggerRect(viewport);
    RealInterval::List sb;
    sb << RealInterval(_r.left(), _r.right()) << RealInterval(_r.bottom(), _r.top());


    for (int i = 0; i < m_functionsModel->rowCount(); i+=1)
    {

        FunctionImpl *tempsol = m_functionsModel->funclist.at(i).solver();

        if (!tempsol) continue;

        if (tempsol->dimension() == 3) continue;

        FunctionImpl2D *solver = (FunctionImpl2D*)(m_functionsModel->funclist.at(i).solver());

        if (!m_functionsModel->funclist.at(i).isShown()) continue;

        solver->solve(sb);
    }

    update();
}


void Graph2D::updateTickScale(QString tickScaleSymbol, qreal tickScaleSymbolValue,
                              int tickScaleNumerator,
                             int tickScaleDenominator)
{
    m_tickScaleSymbol = tickScaleSymbol;
    m_tickScaleSymbolValue = tickScaleSymbolValue;

    m_tickScaleNumerator = tickScaleNumerator;
    m_tickScaleDenominator = tickScaleDenominator;

    update();
}



void Graph2D::resizeEvent(QResizeEvent *)
{
    buffer = QPixmap(this->size());
    updateScale(true);
}

QRect Graph2D::toBiggerRect(const QRectF& ent)
{
    QRect ret;
    ret.setTop(static_cast<int>(ceil(ent.top())));
    ret.setBottom(static_cast<int>(floor(ent.bottom())));
    ret.setLeft(static_cast<int>(floor(ent.left())));
    ret.setRight(static_cast<int>(ceil(ent.right())));

    return ret;
}

bool Graph2D::toImage(const QString &path, Format fmt)
{
    bool b=false;

    if(!path.isEmpty() && path.endsWith(".svg"))
    {
        QFile f(path);
        QSvgGenerator gen;
        gen.setOutputDevice(&f);
        gen.setSize(this->size());
        drawFunctions(&gen);
        b=true;

        repaint();
    }
    else if(!path.isEmpty() && path.endsWith(".png"))
    {
        this->repaint();


        b=buffer.save(path, "PNG");
    }

    return b;
}

void Graph2D::updateScale(bool repaint)
{


    if (!m_functionsModel)
        return ;



    viewport=userViewport;
    rang_x= width()/viewport.width();
    rang_y= height()/viewport.height();

    if(m_keepRatio && rang_x!=rang_y)
    {
        rang_y=rang_x=qMax(fabs(rang_x), fabs(rang_y));
        if(rang_y>0.) rang_y=-rang_y;
        if(rang_x<0.) rang_x=-rang_x;

        double newW=width()/rang_x, newH=height()/rang_x;

        double mx=(userViewport.width()-newW)/2.;
        double my=(userViewport.height()-newH)/2.;

        viewport.setLeft(userViewport.left()+mx);
        viewport.setTop(userViewport.bottom()-my);
        viewport.setWidth(newW);
        viewport.setHeight(-newH);


    }

    if(repaint)
    {
        if (m_functionsModel) if(m_functionsModel->rowCount())
                updateFunctions(m_functionsModel->index(0,0), m_functionsModel->index(m_functionsModel->rowCount()-1,0));


        this->repaint();
    }
}

void Graph2D::zoomIn()
{
    if(userViewport.height() < -3. && userViewport.width() > 3.)
    {
        setViewport(QRect(userViewport.left() +1., userViewport.top() -1.,
                          userViewport.width() -2., userViewport.height() +2.));
    }
}

void Graph2D::zoomOut()
{


    setViewport(QRect(userViewport.left() -1., userViewport.top() +1.,
                      userViewport.width() +2., userViewport.height() -2.));
}

void Graph2D::updateFunctions(const QModelIndex & startIdx, const QModelIndex & endIdx)
{
    for (int i = 0; i < m_functionsModel->rowCount(); i+=1)
    {
        FunctionImpl *tempsol = m_functionsModel->funclist.at(i).solver();

        if (tempsol->dimension() == 3) continue;

        FunctionImpl2D *solver = (FunctionImpl2D*)(m_functionsModel->funclist.at(i).solver());

        if (!m_functionsModel->funclist.at(i).isShown()) continue;

        QRect _r = toBiggerRect(viewport);
        RealInterval::List sb;

        {
            sb << RealInterval(_r.left(), _r.right()) << RealInterval(_r.bottom(), _r.top());
        }

        solver->solve(sb);

    }

    repaint();

}

void Graph2D::addFuncs(const QModelIndex & parent, int start, int end)
{
    int nps = 0;

    for (int i = 0; i < m_functionsModel->rowCount(); i+=1)
    {
        FunctionImpl *tempsol = m_functionsModel->funclist.at(i).solver();

        if (tempsol->dimension() == 3) continue;

        FunctionImpl2D *solver = (FunctionImpl2D*)(m_functionsModel->funclist.at(i).solver());


        if (!m_functionsModel->funclist.at(i).isShown()) continue;

        QRect _r = toBiggerRect(viewport);
        RealInterval::List sb;

        {
            sb << RealInterval(_r.left(), _r.right()) << RealInterval(_r.bottom(), _r.top());
        }

        solver->solve(sb);


    }

    repaint();
}

void Graph2D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)


    repaint();
}

void Graph2D::setKeepAspectRatio(bool ar)
{
    m_keepRatio=ar;
    updateScale();
}

void Graph2D::setReadOnly(bool ro)
{
    m_readonly=ro;
    this->setCursor(ro ? Qt::ArrowCursor : Qt::CrossCursor);
    setMouseTracking(!ro);
}
void Graph2D::setFunctionsModel(FunctionsModel *fm)
{
    m_functionsModel = fm;

    connect(m_functionsModel, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )),
            this, SLOT(updateFunctions(const QModelIndex&, const QModelIndex)));
    connect(m_functionsModel, SIGNAL( rowsInserted ( const QModelIndex &, int, int ) ),
            this, SLOT(addFuncs(const QModelIndex&, int, int)));
    connect(m_functionsModel, SIGNAL( rowsRemoved ( const QModelIndex &, int, int ) ),
            this, SLOT(removeFuncs(const QModelIndex&, int, int)));
}

QRectF Graph2D::definedViewport() const
{
    return userViewport;
}

void Graph2D::snapshotToClipboard()
{

    QClipboard *cb = QApplication::clipboard();


    cb->setImage(buffer.toImage());
}

double Graph2D::millimetersToPixels(double width_mm, QPaintDevice *device) const
{

    return device->logicalDpiX() * (width_mm/25.4);
}
