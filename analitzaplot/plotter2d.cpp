/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "plotter2d.h"

#include "planecurve.h"

#include "plotsmodel.h"

#include <QPalette>
#include <QPen>
#include <QPainter>
#include <QApplication>
#include <cmath>
#include <KLocalizedString>
#include <KColorUtils>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

QColor const Plotter2D::m_axeColor(100,100,255);
QColor const Plotter2D::m_axe2Color(235,235,235);
QColor const Plotter2D::m_derivativeColor(90,90,160);

Plotter2D::Plotter2D(const QSizeF& size, QAbstractItemModel* model)
    : m_squares(true), m_keepRatio(true), m_dirty(true), m_size(size), m_model(model)
    , m_tickScaleSymbolValue(1)
    , m_tickScaleUseSymbols(true)
    , m_tickScaleNumerator(1)
    , m_tickScaleDenominator(1)
    , m_showHTicks(true)
    , m_showVTicks(true)
    , m_showHAxes(true)
    , m_showVAxes(true)
    , m_axisXLabel("x")
    , m_axisYLabel("y")
    , m_useCoordSys(1) //default cartesian coords style grid
    , m_gridColor(QColor(128,128,128))
{}

Plotter2D::~Plotter2D()
{}

void Plotter2D::drawAxes(QPainter *p, CoordinateSystem a)
{
    switch(a) {
    case Polar:
        drawPolarAxes(p);
        break;
    default:
        drawCartesianAxes(p);
    }

    //write coords
    QString rightBound=QString::number(viewport.right());
    int width=p->fontMetrics().width(rightBound);

    p->drawText(QPointF(3.+this->width()/2., 13.                 ), QString::number(viewport.top()));
    p->drawText(QPointF(3.+this->width()/2., this->height()-5.   ), QString::number(viewport.bottom()));
    p->drawText(QPointF(8.                 , this->height()/2.-5.), QString::number(viewport.left()));
    p->drawText(QPointF(this->width()-width, this->height()/2.-5.), rightBound);
    //EO write coords
}

void Plotter2D::drawPolarAxes(QPainter *painter)
{
    QPainter *w = painter;

    qreal until = qMax(viewport.height(), viewport.width());

    QPen ceixos;
    ceixos.setColor(m_gridColor); // m_axeColor
    w->setPen(ceixos);
    const QPointF center = toWidget(QPointF(0.,0.));

    ceixos.setColor(m_gridColor);
    ceixos.setStyle(Qt::SolidLine);
    w->setPen(ceixos);

    QString symbol =m_tickScaleSymbol;
    qreal symbolValue = m_tickScaleSymbolValue;
    bool symbolFormat = m_tickScaleUseSymbols; // cuando son numeros o cuando el usuario no kiere mostrar los simboles
    int numerator = m_tickScaleNumerator;
    int denominator =m_tickScaleDenominator;

    // prcesss
    qreal coef = (qreal)(numerator);
    coef /= (qreal)(denominator);
    bool isSymbol = !symbol.isEmpty();
    symbolFormat = isSymbol? symbolFormat : false; // force false if isSymbol is false too
    qreal tickValue = isSymbol? coef*symbolValue : coef;

    QString numStr = QString::number(numerator);
    QString denStr = QString::number(denominator);

    //END params of algotihm

    qreal inc = tickValue;

    int decimalPrecision = denominator == 1 && !isSymbol? 0 : denominator == 2? 1 : 2;

    //QString tickLabel = symbol;
    //QString tickCoef("");

    double xini=floor((viewport.left())/inc)*inc;
    double yini=(floor(viewport.bottom())/inc)*inc;

    double xend=ceil((viewport.right())/inc)*inc;
    double yend=(ceil(viewport.top())/inc)*inc;

    int correctScale = 1;// usado cuando el zoom es muy alto y se ve un vp muy grande

    if (viewport.width()>25)
    {
        int vpwi = ((int)viewport.width());
        int decs = 0; // cifras decimales
        while ((vpwi = vpwi/10) > 0) ++decs;

        correctScale = pow(10, decs)/2;
    }

    inc *= correctScale;
    xini = floor(xini/inc)*inc;
    yini = floor(yini/inc)*inc;

    QFontMetrics fm(w->font());

    if (m_squares)
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
    tickpen.setColor(QPalette().text().color());
    tickpen.setStyle(Qt::SolidLine);

    w->setFont(painter->font());

    if (m_showHTicks)
    {
        for(double x = -inc; x > xini; x -= inc, i+=1)
        {
            p = toWidget(QPointF(x, 0.));

            w->setPen(ceixos);
            w->setPen(tickpen);

            if (!isSymbol || !symbolFormat) {
                w->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = x <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                w->drawText(p.x(), p.y()+20, QString::number(sign*val*numerator)+symbol);

                if (denominator != 1 && (iCorrected % denominator != 0))
                {
                    w->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    w->drawText(p.x()-10+fm.width(symbol), p.y()+20+fm.height(), denStr);
                }
            }
        }

        i = 1;

        for(double x = inc; x < xend; x += inc, i+=1)
        {
            p = toWidget(QPointF(x, 0.));

            w->setPen(ceixos);
            w->setPen(tickpen);

            if (!isSymbol || !symbolFormat) {
                w->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = x <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                w->drawText(p.x(), p.y()+20, QString::number(sign*val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    w->drawText(p.x()-10+fm.width(symbol), p.y()+20+fm.height(), denStr);
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
            w->setPen(tickpen);

            if (!isSymbol || !symbolFormat) {
                w->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = y <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                QString text = QString::number(sign*val*numerator)+symbol;
                qreal correctxpos = fm.width(text)+6;

                w->drawText(-correctxpos + p.x(), p.y()+5, text);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    w->drawText(-correctxpos + p.x()-10+fm.width(symbol), p.y()+5+fm.height(), denStr);
                }
            }

        }

        i = 1;

        for(double y = inc; y < yend; y += inc, i+=1)
        {
            p = toWidget(QPointF(0., y));
            w->setPen(ceixos);
            w->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                w->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = y <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                QString text = QString::number(sign*val*numerator)+symbol;
                qreal correctxpos = fm.width(text)+6;

                w->drawText(-correctxpos + p.x(), p.y()+5, text);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    w->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    w->drawText(-correctxpos + p.x()-10+fm.width(symbol), p.y()+5+fm.height(), denStr);
                }
            }
        }
    }

    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);

    if (m_showHAxes)
    {
        //draw viewport axes
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

    // axis labels

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

void Plotter2D::drawCartesianAxes(QPainter *painter)
{
    QPainter *finestra = painter;

    QPen ceixos;
    QFont natfont = finestra->font();
    const QPointF center = toWidget(QPointF(0.,0.));
    QPointF p;

//     ceixos.setColor(KColorUtils::shade(m_gridColor, -0.5));//m_axe2Color
    ceixos.setColor(m_gridColor);
    ceixos.setStyle(Qt::SolidLine);
    finestra->setPen(ceixos);

    //BEGIN params of algotihm

    // pi/6

//    QString symbol = QString((QChar(0x03C0)));
//    qreal symbolValue = M_PI;
//    int numerator = 1;
//    int denominator = 6;


    // sqrt(2)

    QString symbol =m_tickScaleSymbol;
    qreal symbolValue = m_tickScaleSymbolValue;
    int numerator = m_tickScaleNumerator;
    int denominator =m_tickScaleDenominator;

    // prcesss
    qreal coef = qreal(numerator);
    coef /= qreal(denominator);
    bool isSymbol = !symbol.isEmpty();
    bool symbolFormat = isSymbol && m_tickScaleUseSymbols; // cuando son numeros o cuando el usuario no kiere mostrar los simboles

    QString numStr = QString::number(numerator);
    QString denStr = QString::number(denominator);

    //END params of algotihm

    qreal inc = isSymbol? coef*symbolValue : coef;

    int decimalPrecision = denominator == 1 && !isSymbol? 0 : denominator == 2? 1 : 2;

    //QString tickLabel = symbol;
    //QString tickCoef("");

    double xini=floor((viewport.left())/inc)*inc;
    double yini=(floor(viewport.bottom())/inc)*inc;

    double xend=ceil((viewport.right())/inc)*inc;
    double yend=(ceil(viewport.top())/inc)*inc;

    int correctScale = 1;// usado cuando el zoom es muy alto y se ve un vp muy grande

    if (viewport.width()>25)
    {
        int vpwi = ((int)viewport.width());
        int decs = 0; // cifras decimales
        while ((vpwi = vpwi/10) > 0) ++decs;

        correctScale = pow(10, decs)/2;
    }

    inc *= correctScale;
    xini = floor(xini/inc)*inc;
    yini = floor(yini/inc)*inc;

    // tick labels color = black
    QPen tickpen;
    //NOTE GSOC 2012 accessibility code :)
    tickpen.setColor(QPalette().text().color());
    tickpen.setStyle(Qt::SolidLine);

    QFont serifFont = painter->font();
    finestra->setFont(serifFont);

    //BEGIN Draw ticks X

    int i = 1;

    QFontMetrics fm(finestra->font());

    for(double x = -inc; x > xini; x -= inc, i+=1)
    {
        p = toWidget(QPointF(x, 0.));

        finestra->setPen(ceixos);

        if(m_squares)
            finestra->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            finestra->drawLine(p, p+QPointF(0.,-3.));

        if (m_showHTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat) {
                finestra->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = x <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                finestra->drawText(p.x(), p.y()+20, QString::number(sign*val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    finestra->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    finestra->drawText(p.x()-10+fm.width(symbol), p.y()+20+fm.height(), denStr);
                }
            }
        }
    }

    i = 1;

    for(double x = inc; x < xend; x += inc, i+=1)
    {
        p = toWidget(QPointF(x, 0.));

        finestra->setPen(ceixos);

        if(m_squares)
            finestra->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            finestra->drawLine(p, p+QPointF(0.,-3.));

        if (m_showHTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(p.x() - fm.width(QString::number(x, 'f', decimalPrecision))/2, p.y()+20, QString::number(x, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = x <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                finestra->drawText(p.x(), p.y()+20, QString::number(sign*val*numerator)+symbol);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    finestra->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    finestra->drawText(p.x()-10+fm.width(symbol), p.y()+20+fm.height(), denStr);
                }
            }
        }
    }

   //END Draw ticks X

   //BEGIN Draw ticks Y

    i = 1;

    for(double y = -inc; y > yini; y -= inc, i+=1)
    {
        p = toWidget(QPointF(0., y));

        finestra->setPen(ceixos);

        if(m_squares)
            finestra->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        else
            finestra->drawLine(p, p+QPointF(3.,0.));

        if (m_showVTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat) {
                finestra->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = y <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                QString text = QString::number(sign*val*numerator)+symbol;
                qreal correctxpos = fm.width(text)+6;

                finestra->drawText(-correctxpos + p.x(), p.y()+5, text);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    finestra->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                    -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    finestra->drawText(-correctxpos + p.x()-10+fm.width(symbol), p.y()+5+fm.height(), denStr);
                }
            }
        }
    }

    i = 1;

    for(double y = inc; y < yend; y += inc, i+=1)
    {
        p = toWidget(QPointF(0., y));

        finestra->setPen(ceixos);

        if(m_squares)
            finestra->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        else
            finestra->drawLine(p, p+QPointF(3.,0.));

        if (m_showVTicks)
        {
            finestra->setPen(tickpen);

            if (!isSymbol || !symbolFormat)
            {
                finestra->drawText(-20+p.x() - fm.width(QString::number(y, 'f', decimalPrecision))/2, p.y()+20, QString::number(y, 'f', decimalPrecision));
            } else {
                int iCorrected = i*correctScale;
                int sign = y <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                QString text = QString::number(sign*val*numerator)+symbol;
                qreal correctxpos = fm.width(text)+6;
                finestra->drawText(-correctxpos + p.x(), p.y()+5, text);

                if ((denominator != 1) && (iCorrected % denominator != 0))
                {
                    finestra->drawLine(-correctxpos + p.x()-5, p.y()+5,
                                    -correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    finestra->drawText(-correctxpos + p.x()-10+fm.width(symbol), p.y()+5+fm.height(), denStr);
                }
            }
        }
    }

   //END Draw ticks Y

    // restore values
    finestra->setFont(natfont);
    finestra->setPen(ceixos);

    ceixos.setStyle(Qt::SolidLine);

    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);

    if (m_showHAxes)
    {
        //draw viewport axes
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
    //EO draw viewport axes


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

    // axis labels

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

PlotItem* Plotter2D::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    //NOTE why this assert? a model contains plots (can have 2d or 3d plot) is responsibility of this class to not allow/get a 3d item
    //this assert forces to have a model for each type of plot, something that is not very suitable ... is better to have one big 
    //model than many models
    //     Q_ASSERT(plot->spaceDimension() == 2);
    if (plot->spaceDimension() != 2) // <= 2?
        return 0;

    return plot;
}

void Plotter2D::drawFunctions(QPaintDevice *qpd)
{
    QPen pfunc(QColor(0,150,0), 2);

    QPainter p;
    p.begin(qpd);
//  finestra.initFrom(this);
    p.setPen(pfunc);

    int current=currentFunction();
    PlotItem* plot = itemAt(current);
    CoordinateSystem t = plot ? plot->coordinateSystem() : Cartesian;

// //     if (coordsysfromplot)
//         if (m_useCoordSys == 0) 
//             m_squares = false;
//         else
//         {
//             m_squares = true;
// 
//             if (m_useCoordSys == 2) 
//                 t = Polar;
// 
//             if (m_useCoordSys == 1) 
//                 t = Cartesian;
//         }

    p.setRenderHint(QPainter::Antialiasing, true);
    
    drawAxes(&p, t);

    if (!m_model || m_dirty)
        return;

    for (int k = 0; k < m_model->rowCount(); ++k )
    {
        PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(k));

        //NOTE GSOC POINTS=0
        //no siempre el backend va a generar puntos y si no lo hace no quiere decir que esta mal,
        //por ejemplo en el caso de parametric se hace un clip para ver si la curva esta dentro o no del viewport
        //entonces ademas de verificar que es sible si debe vericiar que existan puntos antes de pintar una funcion

        if (!curve || !curve->isVisible() || curve->points().isEmpty())
            continue;

        pfunc.setColor(curve->color());
        pfunc.setWidth((k==current)+1);
        p.setPen(pfunc);

        const QVector<QPointF> &vect=curve->points();
        QVector<int> jumps=curve->jumps();

        unsigned int pointsCount = vect.count();
        QPointF ultim(toWidget(vect[0]));

        int nextjump;
        nextjump = jumps.isEmpty() ? -1 : jumps.first();
        if (!jumps.isEmpty()) jumps.remove(0);

#ifdef DEBUG_GRAPH
        qDebug() << "---------" << jumps.count()+1;
#endif
        for(unsigned int j=0; j<pointsCount; j++)
        {
            QPointF act=toWidget(vect.at(j));

//          qDebug() << "xxxxx" << act << ultim << isnan(act.y()) << isnan(ultim.y());
            if(isinf(act.y()) && !isnan(act.y())) qDebug() << "trying to plot from a NaN value" << act << ultim;
            else if(isinf(act.y()) && isnan(act.y())) qDebug() << "trying to plot to a NaN value";

            bool bothinf=(isinf(ultim.y()) && isinf(act.y())) || (isinf(ultim.x()) && isinf(act.x()));
            if(!bothinf && !isnan(act.y()) && !isnan(ultim.y()) && nextjump!=int(j)) {
                if(isinf(ultim.y())) {
                    if(act.y()<0) ultim.setY(0);
                    if(act.y()>0) ultim.setY(qpd->height());
                }
//
                QPointF act2(act);
                if(isinf(act2.y())) {
                    if(ultim.y()<0) act2.setY(0);
                    if(ultim.y()>0) act2.setY(qpd->height());
                }

//              qDebug() << "xxxxx" << act2 << ultim << isnan(act2.y()) << isnan(ultim.y());
                p.drawLine(ultim, act2);

#ifdef DEBUG_GRAPH
                QPen p(Qt::red);
                p.setWidth(3);
                finestra.setPen(p);
                finestra.drawPoint(ultim);
                finestra.setPen(pfunc);
#endif
            } else if(nextjump==int(j)) {
                do {
                    if(nextjump!=int(j))
                        p.drawPoint(act);

//                     if(allJumps)
//                         nextjump += 2;
//                     else
                    nextjump = jumps.isEmpty() ? -1 : jumps.first();
                    if (!jumps.isEmpty()) jumps.remove(0);

                } while(!jumps.isEmpty() && jumps.first()==nextjump+1);

#ifdef DEBUG_GRAPH
                qDebug() << "jumpiiiiiing" << ultim << toWidget(vect.at(j));
                QPen p(Qt::blue);
                p.setWidth(2);
                finestra.setPen(p);
                finestra.drawLine(QLineF(QPointF(act.x(), height()/2-10), QPointF(act.x(), height()/2+10)));
                finestra.setPen(pfunc);
#endif
            }

            ultim=act;
        }
    }

    p.end();
}

void Plotter2D::updateFunctions(const QModelIndex & parent, int start, int end)
{
    if (!m_model || parent.isValid())
        return;

    for(int i=start; i<=end; i++) {
        PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(i));

        if (!curve || !curve->isVisible())
            continue;

        QRectF viewport_fixed = viewport;
        viewport_fixed.setTopLeft(viewport.bottomLeft());
        viewport_fixed.setHeight(fabs(viewport.height()));
        curve->update(viewport_fixed);
    }

    m_dirty = false;

    forceRepaint();
}

QPointF Plotter2D::calcImage(const QPointF& ndp) const
{
    if (!m_model || currentFunction() == -1) return QPointF(); // guard

    //DEPRECATED if (m_model->data(model()->index(currentFunction()), FunctionGraphModel::VisibleRole).toBool())
    PlaneCurve* curve = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    if (curve && curve->isVisible())
        return curve->image(ndp).first;

    return QPointF();
}

void Plotter2D::updateScale(bool repaint)
{
    viewport=userViewport;
    rang_x= width()/viewport.width();
    rang_y= height()/viewport.height();

    if(m_keepRatio && rang_x!=rang_y)
    {
        rang_y=rang_x=qMin(std::fabs(rang_x), std::fabs(rang_y));
        if(rang_y>0.) rang_y=-rang_y;
        if(rang_x<0.) rang_x=-rang_x;

        double newW=width()/rang_x, newH=height()/rang_x;

        double mx=(userViewport.width()-newW)/2.;
        double my=(userViewport.height()-newH)/2.;

        viewport.setLeft(userViewport.left()+mx);
        viewport.setTop(userViewport.bottom()-my);
        viewport.setWidth(newW);
        viewport.setHeight(-newH); //NOTE WARNING POR QUE DISTANCIA NEGATIVA???
        //Commented because precision could make the program crash
//      Q_ASSERT(userViewport.center() == viewport.center());
    }

    if(repaint) {
        //WARNING estas 2 lineas cuasan que se actulicen los plots cuando no es necesario hacerlo
        //tener en cuenta que el costo de actualizarlo es alto pues alli es donde se poligoniza la curva
        if(m_model && m_model->rowCount()>0)
        {
//             updateFunctions(QModelIndex(), 0, m_model->rowCount()-1);
            for(int i=0; i<m_model->rowCount(); i++)  //<=??
            {
                PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(i));

                if (!curve) continue;

                //NOTE si no es visble o si no debe actualizarce segun el viewport (solo segun los intervalos)
                if (!curve->isVisible() || !curve->isAutoUpdate()) continue;

                QRectF viewport_fixed = viewport;
                viewport_fixed.setTopLeft(viewport.bottomLeft());
                viewport_fixed.setHeight(fabs(viewport.height()));
                //NOTE GSOC
                //         qDebug() << "ORI" << viewport << viewport.center() <<  "CH" << viewport_fixed<< viewport_fixed.center();
                //it works con este parche se le pasa a las curvas la informacion adecuada basada en centro y size
                // y se conserva la semantica de qrectf la esquina superior izquierda es el origen
                curve->update(viewport_fixed);
            }
        }

        forceRepaint();
    }
}

void Plotter2D::setViewport(const QRectF& vp, bool repaint)
{
    userViewport = vp;
    Q_ASSERT(userViewport.top()>userViewport.bottom());
    Q_ASSERT(userViewport.right()>userViewport.left());

    updateScale(repaint);

    viewportChanged();
}

QLineF Plotter2D::slope(const QPointF& dp) const
{
    if (!m_model || currentFunction() == -1)
        return QLineF(); // guard

    PlaneCurve* plot = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    if (plot && plot->isVisible())
    {
        QLineF ret = plot->tangent(dp);
        if(ret.isNull() && currentFunction()>=0) {
            QPointF a = calcImage(dp-QPointF(.1,.1));
            QPointF b = calcImage(dp+QPointF(.1,.1));

            ret = slopeToLine((a.y()-b.y())/(a.x()-b.x()));
        }
        return ret;
    }

    return QLineF();
}

QLineF Plotter2D::toWidget(const QLineF &f) const
{
    return QLineF(toWidget(f.p1()), toWidget(f.p2()));
}

QPointF Plotter2D::toWidget(const QPointF& p) const
{
    double left=-viewport.left(), top=-viewport.top();
    return QPointF((left + p.x()) * rang_x,  (top + p.y()) * rang_y);
}

QPointF Plotter2D::fromWidget(const QPoint& p) const
{
    double part_negativa_x = -viewport.left();
    double part_negativa_y = -viewport.top();
    return QPointF(p.x()/rang_x-part_negativa_x, p.y()/rang_y-part_negativa_y);
}

QPointF Plotter2D::toViewport(const QPoint &mv) const
{
    return QPointF(mv.x()/rang_x, mv.y()/rang_y);
}

void Plotter2D::moveViewport(const QPoint& delta)
{
    QPointF rel = toViewport(delta);
    QRectF viewport = lastViewport();
    viewport.moveLeft(viewport.left() - rel.x());
    viewport.moveTop(viewport.top() - rel.y());
    setViewport(viewport);
}

void Plotter2D::scaleViewport(qreal s, const QPoint& center)
{
    QPointF p = fromWidget(center);
    QSizeF ns = viewport.size()*s;

    QRectF nv(viewport.topLeft(), ns);
    setViewport(nv, false);

    QPointF p2 = p-fromWidget(center);
    nv.translate(p2);
    setViewport(nv);
}

void Plotter2D::setKeepAspectRatio(bool ar)
{
    m_keepRatio=ar;
    updateScale(true);
}

void Plotter2D::setModel(QAbstractItemModel* f)
{
    m_model=f;
    modelChanged();
    forceRepaint();
}

void Plotter2D::setPaintedSize(const QSize& size)
{
    m_size=size;
    updateScale(true);
}

void Plotter2D::setXAxisLabel(const QString &label)
{
    m_axisXLabel = label;
    forceRepaint();
}

void Plotter2D::setYAxisLabel(const QString &label)
{
    m_axisYLabel = label;
    forceRepaint();

}

void Plotter2D::updateTickScale(QString tickScaleSymbol, qreal tickScaleSymbolValue,
        /*bool tickScaleUseSymbols,*/ int tickScaleNumerator,
        int tickScaleDenominator)
{
    m_tickScaleSymbol = tickScaleSymbol;
    m_tickScaleSymbolValue = tickScaleSymbolValue;
    //m_tickScaleUseSymbols = tickScaleUseSymbols;
    m_tickScaleNumerator = tickScaleNumerator;
    m_tickScaleDenominator = tickScaleDenominator;

    forceRepaint();
}
