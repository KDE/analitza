/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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
#include "private/utils/mathutils.h"

#include <cmath>
#include <QPalette>
#include <QPainter>
#include <QFontDatabase>
#include <QDebug>
#include <qnumeric.h>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool qIsInf(double x) { return !finite(x) && x==x; }
#endif

using namespace Analitza;

// #define DEBUG_GRAPH

class Analitza::Plotter2DPrivate : public QObject {
public:
    Plotter2DPrivate(Plotter2D* q) : q(q) {}

    QAbstractItemModel* m_model = nullptr;
    qreal m_dpr = 1.;
    Plotter2D* const q;

    void forceRepaint() { q->forceRepaint(); }
    void addFuncs(const QModelIndex& parent, int start, int end) { q->updateFunctions(parent, start, end); }
    void updateFuncs(const QModelIndex& start, const QModelIndex& end) { q->updateFunctions(QModelIndex(), start.row(), end.row()); }
    void setModel(QAbstractItemModel* f);
};

QColor const Plotter2D::m_axeColor(100,100,255); //TODO convert from const to param/attr and make setAxisColor(Qt::oriantation, qcolor)
QColor const Plotter2D::m_derivativeColor(90,90,160); //TODO remove derivative logic from plotter2d, move it to other module
QString const Plotter2D::PiSymbol(QChar(0x03C0));
QString const Plotter2D::DegreeSymbol(QChar(0x00B0));
QString const Plotter2D::GradianSymbol(QChar(0x1D4D));
double const Plotter2D::Pi6 = M_PI_2/3.0;
double const Plotter2D::Pi12 = Plotter2D::Pi6*0.5;
double const Plotter2D::Pi36 = Plotter2D::Pi6/6.0;
const double Plotter2D::ZoomInFactor = 0.97/2; // (regular mouse wheel forward value) / 2
const double Plotter2D::ZoomOutFactor = 2*1.03; // 2 x (regular mouse wheel backward value)

struct Plotter2D::GridInfo
{
    double inc, xini, yini, xend, yend;
    int incLabelSkip;
    // sub5 flag is used for draw sub 5 intervals instead of 4
    int subinc; // true if inc=5*pow(10,n) (so, in this case, we draw 5 sub intervals, not 4)
    // we need scale for ticks, and for labels, thicks can be minor, labels no: so 
    // this is necessary for minor ticks and for minir grid (but not for labels, so when draw labels we need )
    int nxiniticks, nyiniticks, nxendticks, nyendticks; // nxini = floor(viewport.left()/inc), so xini is nxini*inc ... and so on
    int nxinilabels, nyinilabels, nxendlabels, nyendlabels; // nxini = floor(viewport.left()/inc), so xini is nxini*inc ... and so on
};

Plotter2D::Plotter2D(const QSizeF& size)
    : m_showGrid(true)
    , m_showMinorGrid(false)
    , m_gridColor(QColor(Qt::lightGray).lighter(120))
    , m_backgroundColor(Qt::white)
    , m_autoGridStyle(true)
    , m_gridStyleHint(Squares)
    , rang_x(0)
    , rang_y(0)
    , m_keepRatio(true)
    , m_dirty(true)
    , m_size(size)
    , d(new Plotter2DPrivate(this))
    , m_angleMode(Radian)
    , m_scaleMode(Linear)
    , m_showTicks(Qt::Vertical|Qt::Horizontal)
    , m_showTickLabels(Qt::Vertical|Qt::Horizontal)
    , m_showMinorTicks(false)
    , m_showAxes(Qt::Vertical|Qt::Horizontal)
    , m_showPolarAxis(false)
    , m_showPolarAngles(false)
    , m_axisXLabel(QStringLiteral("x"))
    , m_axisYLabel(QStringLiteral("y"))
{}

Plotter2D::~Plotter2D()
{
    delete d;
}

void Plotter2D::setGridStyleHint(GridStyle suggestedgs)
{
    m_gridStyleHint = suggestedgs;
    
    forceRepaint();
}

const QColor Plotter2D::computeSubGridColor() const
{
    //impl. details: since any kde user can create any palette style, we need this hard/magic numbres
    // because there is no way to guess, however, this code covers almost any case, 
    // it was tested with more then 35 color styles, and all give good results.
    
    QColor col = m_gridColor;
    
    if (m_backgroundColor.value() < 200)
    {
        if (m_gridColor.value() < 40)
            col.setHsv(col.hsvHue(), col.hsvSaturation(), m_gridColor.value()-15);
        else
            col.setHsv(col.hsvHue(), col.hsvSaturation(), m_gridColor.value()>=255?120:m_gridColor.value()-10);
    }
    else // e.g: background color white and grid color black
        if (m_backgroundColor.value() < 245)
            col.setHsv(col.hsvHue(), col.hsvSaturation(), m_backgroundColor.value()-(m_backgroundColor.value()-200)/3);
        else
            col.setHsv(col.hsvHue(), col.hsvSaturation(), m_backgroundColor.value()-(m_backgroundColor.value()-200)/8);            
    
    return col;
}

const Plotter2D::GridInfo Plotter2D::getGridInfo() const
{
    GridInfo ret;
    
    if (m_scaleMode == Linear) {
        const double val = log10(qMax(viewport.width(), -viewport.height()));
        const double diff = val-floor(val);
        const double magnitude = pow(10, floor(val)-1);

        ret.inc = magnitude;
        ret.incLabelSkip = diff < 0.5 ? 1 : 3;
    } else {
        ret.inc = M_PI;
        ret.incLabelSkip = 1;
    }

    ret.subinc = 4;
    
    ret.nxinilabels = std::floor(viewport.left()/ret.inc);
    ret.nyinilabels = std::floor(viewport.bottom()/ret.inc);
    ret.nxendlabels = std::ceil(viewport.right()/ret.inc);
    ret.nyendlabels = std::ceil(viewport.top()/ret.inc);
    
    ret.xini = ret.nxinilabels*ret.inc;
    ret.yini = ret.nyinilabels*ret.inc;
    ret.xend = ret.nxendlabels*ret.inc;
    ret.yend = ret.nyendlabels*ret.inc;

    const bool drawminor = m_showMinorGrid || m_showMinorTicks;
    const double nfactor = drawminor ? ret.subinc : 1;
    
    ret.nxiniticks = nfactor*ret.nxinilabels;
    ret.nyiniticks = nfactor*ret.nyinilabels;
    ret.nxendticks = nfactor*ret.nxendlabels;
    ret.nyendticks = nfactor*ret.nyendlabels;
    
    return ret; 
}

void Plotter2D::drawGrid(QPaintDevice *qpd)
{
    QPainter p;
    p.begin(qpd);
    
    int current=currentFunction();
    PlotItem* plot = itemAt(current);
    
    GridStyle t = Squares; // default for Cartesian coord. sys.
    
    if (plot && plot->coordinateSystem() == Polar)
        t = Circles;
    
    if (!m_autoGridStyle)
        t = m_gridStyleHint;
    
    drawAxes(&p, t);
}

void Plotter2D::drawAxes(QPainter* painter, GridStyle gridStyle) const
{
    GridInfo grid = getGridInfo();
    
    switch (gridStyle) 
    {
        case Circles: drawCircles(painter, grid, gridStyle); break;
        default: drawSquares(painter, grid, gridStyle); break;
    }
    
    drawMainAxes(painter);
    //NOTE always draw the ticks at the end: avoid the grid lines overlap the ticks text
    drawGridTickLabels(painter, grid, gridStyle);
}

void Plotter2D::drawMainAxes(QPainter* painter) const
{
    const QFontMetrics fm(painter->font());
    const QPen axesPen(m_axeColor, 1, Qt::SolidLine);
    const QPointF center = toWidget(QPointF(0.,0.));
    
    painter->setPen(axesPen);
    painter->setBrush(axesPen.color());
    
    const int startAngleX = 150*16;
    const int startAngleY = 240*16;
    const int spanAngle = 60*16;
    const QPointF Xright(this->width(), center.y());
    const QPointF Ytop(center.x(), 0.);
    const double arrowWidth=15., arrowHeight=4.;
    const QPointF dpx(arrowWidth, arrowHeight);
    const QPointF dpy(arrowHeight, arrowWidth);
    const QRectF rectX(Xright+dpx, Xright-dpx);
    const QRectF rectY(Ytop+dpy, Ytop-dpy);

    if (m_showAxes&Qt::Horizontal)
    {
        painter->drawLine(QPointF(0., center.y()), Xright);
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectX, startAngleX, spanAngle);
    }

    if (m_showAxes&Qt::Vertical)
    {
        painter->drawLine(Ytop, QPointF(center.x(), this->height()));
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectY, startAngleY, spanAngle);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    // axis labels
    QFont labelsfont = painter->font();
    labelsfont.setBold(true);

    painter->drawText(Xright.x() - fm.boundingRect(m_axisXLabel).width() - 5, center.y() - 20, m_axisXLabel);
    painter->drawText(center.x() + 5, Ytop.y() + fm.height() + 15, m_axisYLabel);

    //write coords
    QString rightBound=QString::number(viewport.right());
    int width=painter->fontMetrics().boundingRect(rightBound).width();

    painter->drawText(QPointF(3.+this->width()/2., 13.                 ), QString::number(viewport.top()));
    painter->drawText(QPointF(3.+this->width()/2., this->height()-5.   ), QString::number(viewport.bottom()));
    painter->drawText(QPointF(8.                 , this->height()/2.-5.), QString::number(viewport.left()));
    painter->drawText(QPointF(this->width()-width, this->height()/2.-5.), rightBound);
    //EO write coords
}

const QString Plotter2D::computeAngleLabelByFrac(unsigned int n, unsigned int d) const
{
    QString s;
    
    switch (m_angleMode)
    {
        case Radian:
        {
            s = (n==1) ? QString() : QString::number(n);
            s += PiSymbol;
            s += (d==1) ? QString() : '/'+QString::number(d);
        }
        break;
        case Degree: s = QString::number(radiansToDegrees(n*M_PI/d))+DegreeSymbol; break;
        case Gradian:  s = QString::number(radiansToGradians(n*M_PI/d))+GradianSymbol; break;
    }
    
    return s;
}

const QString Plotter2D::computeAngleLabelByStep(unsigned int k, unsigned int step) const
{
    QString s;
    
    switch (m_angleMode)
    {
        case Radian:
        {
            s = (k==1) ? ((step==1) ? QLatin1String("") : QString::number(step)) : QString::number(k*step);
            s += PiSymbol;
        }
        break;
        case Degree: s = QString::number(radiansToDegrees(k*step*M_PI))+DegreeSymbol; break;
        case Gradian:  s = QString::number(radiansToGradians(k*step*M_PI))+GradianSymbol; break;
    }
    
    return s;
}

void Plotter2D::drawCartesianTickLabels(QPainter* painter, const Plotter2D::GridInfo& gridinfo, CartesianAxis axis) const
{
    Q_ASSERT(axis == XAxis || axis == Analitza::YAxis);
    
    const bool isxaxis = (axis == XAxis);
    const double fontHeight = painter->fontMetrics().height();
    
    const bool incbig = (M_PI <= gridinfo.inc);
    const unsigned int bigstep = std::floor(gridinfo.inc/M_PI);
    const unsigned int step = std::ceil(M_PI/gridinfo.inc);
    
    QString s;

    const int from = (axis == Analitza::XAxis) ? gridinfo.nxinilabels : gridinfo.nyinilabels;
    const int to = (axis == Analitza::XAxis) ? gridinfo.nxendlabels : gridinfo.nyendlabels;
    
    painter->save();
    QFont tickFont = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont);
    painter->setFont(tickFont);
    painter->setPen(QPen(QPalette().text().color()));
    for (int i = from; i <= to; ++i)
    {
        if (i == 0 || i%gridinfo.incLabelSkip!=0) continue;
        
        double newval = i*gridinfo.inc;
        
        const QPointF p = isxaxis
            ? toWidget(QPointF(newval, 0.0))
            : toWidget(QPointF(0.0, newval));
        
        switch (m_scaleMode)
        {
            case Linear: 
                s = QString::number(newval); 
                break;
            case Trigonometric:
            {
                s = (i < 0) ? QString(QLatin1Char('-')) : QString();
                
                if (incbig)
                    s += computeAngleLabelByStep(qAbs(i), bigstep);
                else
                {
                    const QPair<unsigned int, unsigned int> frac = simplifyFraction(qAbs(i), step);
                    
                    s += computeAngleLabelByFrac(frac.first, frac.second);
                }
            }
            break;
        }
        
        const int swidth = painter->fontMetrics().boundingRect(s).width();
        if (isxaxis)
            painter->drawText(p.x() - swidth/2, p.y()+fontHeight, s);
        else
            painter->drawText(p.x() - swidth - fontHeight/2, p.y()+fontHeight/2, s);
    }
    painter->restore();
}

void Plotter2D::drawPolarTickLabels(QPainter* painter, const Plotter2D::GridInfo& gridinfo) const
{
    unsigned int k = 1;

    painter->setPen(m_gridColor);
    
    //TODO if minor
    const double newinc = gridinfo.inc/(gridinfo.subinc); // inc with sub intervals
    
    //x
    // we assume 0 belongs to interval [gridinfo.xini, gridinfo.xend]
    double maxh = qMax(std::abs(gridinfo.xini), std::abs(gridinfo.xend));
    int i = std::ceil(maxh/newinc)/2;
    double x = i*newinc;
    
    if (std::abs(gridinfo.xend) <= std::abs(gridinfo.xini))
        x *= -1.0;
    
    // if 0 doesn't belongs to interval [gridinfo.xini, gridinfo.xend]
    if (((gridinfo.xend < 0.0) && (gridinfo.xini < 0.0)) || 
        ((gridinfo.xend > 0.0) && (gridinfo.xini > 0.0)))
    {
        maxh = gridinfo.xend - gridinfo.xini;
        i = std::ceil(maxh/newinc)/2;
        
        x = gridinfo.xini + i*newinc;
    }
    
    //y
    // we assume 0 belongs to interval [gridinfo.yini, gridinfo.yend]
    maxh = qMax(std::abs(gridinfo.yini), std::abs(gridinfo.yend));
    i = std::ceil(maxh/newinc)/2;
    double y = i*newinc;
    
    if (std::abs(gridinfo.yend) <= std::abs(gridinfo.yini))
        y *= -1.0;
    
    // if 0 doesn't belongs to interval [gridinfo.xini, gridinfo.xend]
    if (((gridinfo.yend < 0.0) && (gridinfo.yini < 0.0)) || 
        ((gridinfo.yend > 0.0) && (gridinfo.yini > 0.0)))
    {
        maxh = gridinfo.yend - gridinfo.yini;
        i = std::ceil(maxh/newinc)/2;
        
        y = gridinfo.yini + i*newinc;
    }
    
    const double r = qMax(std::abs(x), std::abs(y)); // radius
    const unsigned short axisxseparation = 16; // distance between tick text and x-axis 
    const float axispolarseparation = axisxseparation*0.5;
    
    double h = Pi6; // pi/6, then 12 rounds
    double t = 0.0;
    
    unsigned int turns = 12; // 12 turns in [0, 2pi], since h=pi/6
    
    // if we are far from origin then sudivide angles by pi/12
    if (!viewport.contains(QPointF(0.0, 0.0)))
    {
        h = Pi12;
        turns = 24; // 24 turns in [0, 2pi], since h=pi/12
    }
    
    k = 0; 
    
    const unsigned int drawoverxaxiscount = turns/2;
    const unsigned int drawnextyaxiscount = turns/4;
    const unsigned int halfturns = drawoverxaxiscount; // or turns/2;
    
    for (uint j = 0; j < turns; ++j, ++k, t += h) // Pi6 then 24 turns
    {
        const QPair<unsigned int, unsigned int> frac = simplifyFraction(k, halfturns);
        
        QString s = (k != 0) ? computeAngleLabelByFrac(frac.first, frac.second) : QStringLiteral("0");
        QPointF p(r*std::cos(t), r*std::sin(t));
        
        if (viewport.contains(p))
        {

            if (k % drawoverxaxiscount == 0) // draw 0, pi over x
                painter->drawText(toWidget(p)+QPointF(0.0, -axispolarseparation), s);
            else 
                if (k % drawnextyaxiscount == 0) // draw pi/2, 3pi/2 next to y
                    painter->drawText(toWidget(p)+QPointF(axispolarseparation, 0.0), s);
                else
                    painter->drawText(toWidget(p), s);
        }
    }
}

void Plotter2D::drawGridTickLabels(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const
{
    if (m_showTickLabels & Qt::Horizontal)
        drawCartesianTickLabels(painter, gridinfo, XAxis);
    
    if (m_showTickLabels & Qt::Vertical)
        drawCartesianTickLabels(painter, gridinfo, YAxis);
    
    if ((gridStyle == Circles) && m_showPolarAngles && m_showGrid) // draw labels for angles (polar axis)
        drawPolarTickLabels(painter, gridinfo);
}

void Plotter2D::drawCircles(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const
{
    Q_ASSERT(gridStyle == Analitza::Circles);
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    const QPen textPen = QPen(QPalette().text().color());
    const QPen gridPen(m_gridColor);
    const QPen subGridPen(computeSubGridColor());

    const unsigned short nsubinc = gridinfo.subinc; // count for draw sub intervals
    const bool drawminor = m_showMinorGrid || m_showMinorTicks;
    const double inc = drawminor ? gridinfo.inc/nsubinc : gridinfo.inc; // if show minor, then inc with sub intervals
    
    if (m_showGrid) 
    {
        const qreal until = qMax(qMax(qAbs(viewport.left()), qAbs(viewport.right())), qMax(qAbs(viewport.top()), qAbs(viewport.bottom())))*M_SQRT2;
        
        int k = 1;
        
        painter->setPen(gridPen);
        
        for (double i = inc; i < until; i += inc, ++k)
        {
            if (i == 0.0) continue;
            
            QPointF p(toWidget(QPointF(i,i)));
            QPointF p2(toWidget(QPointF(-i,-i)));
            QRectF er(p.x(),p.y(), p2.x()-p.x(), p2.y()-p.y());
            
            if ((k % nsubinc == 0) || !drawminor) // intervals
            {
                painter->setPen(gridPen);
                painter->drawEllipse(er);
            }
            else if (m_showMinorGrid)// sub intervals
            {
                painter->setPen(subGridPen);
                painter->drawEllipse(er);
            }
        }
        
        if (m_showPolarAxis) // draw polar rays
        {
            double h = Pi12; // 2pi/(Pi/12) = 24 steps/turns
            const double r = std::abs(until); // radius
            const QPointF origin = toWidget(QPointF(0,0));
            
            int k = 1;
            unsigned int alternatesubgridcount = 2;
            unsigned int dontdrawataxiscount = 5;
            unsigned int steps = 24; // or turns
            
            // if we are far from origin then sudivide angles by 5 degrees
            if (!viewport.contains(QPointF(0.0, 0.0)))
            {
                h = Pi36; // 2pi/(Pi/36) = 72 steps
                steps = 72; // or turns
                alternatesubgridcount = 3;
                dontdrawataxiscount = 17;
            }
            
            double t = h;
            for (uint j = 1; j <= steps; ++j, ++k, t += h)
            {
                if (j % alternatesubgridcount == 0)
                    painter->setPen(gridPen);
                else if (m_showMinorGrid)
                {
                    painter->setPen(subGridPen);

                    QPointF p = toWidget(QPointF(r*std::cos(t), r*std::sin(t)));
                    painter->drawLine(origin, p);

                    if (k % dontdrawataxiscount == 0) // avoid draw the ray at 0,pi/2,pi,3pi/2
                    {
                        t += h;
                        ++j;
                    }
                }
            }
        }
    }
    
    //BEGIN draw ticks
    if (m_showTickLabels & Qt::Horizontal)
    {
        painter->setPen(textPen);
        const QPointF yoffset(0.,-3.);
        
        for (int j = gridinfo.nxiniticks, i = 0; j < gridinfo.nxendticks; ++j, ++i)
        {
            if (j == 0) continue;
            
            const double x = j*inc;
            const QPointF p = toWidget(QPointF(x, 0.));

            if ((i % nsubinc == 0) || !drawminor || m_showMinorTicks)
                painter->drawLine(p, p+yoffset);
        }
    }
    
    if (m_showTickLabels & Qt::Vertical)
    {
        painter->setPen(textPen);
        const QPointF xoffset(3.,0.);
        
        for (int j = gridinfo.nyiniticks, i = 0; j < gridinfo.nyendticks; ++j, ++i)
        {
            if (j == 0) continue;
            
            const double y = j*inc;
            
            const QPointF p = toWidget(QPointF(0., y));
            
            if ((i % nsubinc == 0) || !(m_showMinorGrid || m_showMinorTicks) || m_showMinorTicks)
                painter->drawLine(p, p+xoffset);
        }
    }
    //END draw ticks
}

void Plotter2D::drawSquares(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const
{
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    const QPen gridPen(m_gridColor);
    const QPen subGridPen(computeSubGridColor());
    const QPen gridPenBold(gridPen.brush(), 2);
    const QPen subGridPenBold(subGridPen.brush(), gridPenBold.widthF());
    
    const unsigned short nsubinc = gridinfo.subinc; // count for draw sub intervals
    const bool drawminor = m_showMinorGrid || m_showMinorTicks;
    const double inc = drawminor? gridinfo.inc/nsubinc : gridinfo.inc; // if show minor, then inc with sub intervals
    
    for (int n = gridinfo.nxiniticks, i = 0; n < gridinfo.nxendticks; ++n, ++i)
    {
        if (n == 0) continue;
        
        const double x = n*inc;
        const QPointF p = toWidget(QPointF(x, 0.));
        
        if (m_showGrid && gridStyle == Crosses)
        {
            const double crossside = 5;
            const double centx = p.x();

            for (int j = gridinfo.nyiniticks, k = 0; j < gridinfo.nyendticks; ++j, ++k)
            {
                if (j == 0) continue;

                const double y = j*inc;
                const QPointF py = toWidget(QPointF(0., y));
                const double centy = py.y();
                
                if (((i % nsubinc == 0) && (k % nsubinc == 0)) || !drawminor) // intervals
                {
                    painter->setPen(gridPenBold);
                    painter->drawLine(QPointF(centx-crossside, centy), QPointF(centx+crossside, centy)); // horizontal
                    painter->drawLine(QPointF(centx, centy-crossside), QPointF(centx, centy+crossside)); // vertical
                }
                else if (m_showMinorGrid)// sub intervals
                {
                    painter->setPen(subGridPenBold);
                    painter->drawLine(QPointF(centx-crossside, centy), QPointF(centx+crossside, centy)); // horizontal
                    painter->drawLine(QPointF(centx, centy-crossside), QPointF(centx, centy+crossside)); // vertical
                }
            }
        }
        
        if ((i % nsubinc == 0) || !drawminor) // intervals
        {
            if (m_showGrid && (gridStyle == Squares || gridStyle == VerticalLines))
            {
                painter->setPen(gridPen);
                painter->drawLine(QPointF(p.x(), height()), QPointF(p.x(), 0.));
            }
            
            if ((m_showTicks & Qt::Horizontal))
            {
                painter->setPen(QPen(QPalette().text().color()));
                painter->drawLine(p, p+QPointF(0.,-3.));
            }
        }
        else // sub intervals
        {
            if (m_showGrid && m_showMinorGrid && (gridStyle == Squares || gridStyle == VerticalLines))
            {
                painter->setPen(subGridPen);
                painter->drawLine(QPointF(p.x(), height()), QPointF(p.x(), 0.));
            }
            
            if ((m_showTicks & Qt::Horizontal) && m_showMinorTicks)
            {
                painter->setPen(QPen(QPalette().text().color()));
                painter->drawLine(p, p+QPointF(0.,-3.));
            }
        }
    }
    
    for (int j = gridinfo.nyiniticks, i = 0; j < gridinfo.nyendticks; ++j, ++i)
    {
        if (j == 0) continue;
        
        const double y = j*inc;
        const QPointF p = toWidget(QPointF(0., y));
        
        if ((i % nsubinc == 0) || !drawminor) // intervals
        {
            if (m_showGrid && (gridStyle == Squares || gridStyle == HorizontalLines))
            {
                painter->setPen(gridPen);
                painter->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
            }
            
            if ((m_showTicks & Qt::Horizontal))
            {
                painter->setPen(QPen(QPalette().text().color()));
                painter->drawLine(p, p+QPointF(3.,0.));
            }
        }
        else // sub intervals
        {
            if (m_showGrid && m_showMinorGrid && (gridStyle == Squares || gridStyle == HorizontalLines))
            {
                painter->setPen(subGridPen);
                painter->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
            }
            
            if ((m_showTicks & Qt::Horizontal) && m_showMinorTicks)
            {
                painter->setPen(QPen(QPalette().text().color()));
                painter->drawLine(p, p+QPointF(3.,0.));
            }
        }
    }
}

PlotItem* Plotter2D::itemAt(int row) const
{
    if (!d->m_model)
        return nullptr;
    
    QModelIndex pi = d->m_model->index(row, 0);

    if (!pi.isValid())
        return nullptr;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim2D)
        return nullptr;

    return plot;
}

void Plotter2D::drawFunctions(QPaintDevice *qpd)
{
    drawGrid(qpd);
    QPen pfunc(QColor(0,150,0), 2);
    
    QPainter p;
    p.begin(qpd);
    p.setPen(pfunc);

    if (!d->m_model || m_dirty)
        return;
    
    p.setRenderHint(QPainter::Antialiasing, true);
    
    const int current=currentFunction();
    const int dpr = qMax<int>(1, qRound(qpd->logicalDpiX()/100.)) << 1;

    for (int k = 0; k < d->m_model->rowCount(); ++k )
    {
        PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(k));

        //NOTE from GSOC: not all valid plots always has points (so, we need to check if points().isEmpty() too)
        if (!curve || !curve->isVisible() || curve->points().isEmpty())
            continue;

        pfunc.setColor(curve->color());
        pfunc.setWidth((k==current)+dpr);
        pfunc.setStyle(Qt::SolidLine);
        p.setPen(pfunc);

        const QVector<QPointF> &vect=curve->points();
        QVector<int> jumps=curve->jumps();

        unsigned int pointsCount = vect.count();
        QPointF ultim = toWidget(vect.at(0));

        int nextjump;
        nextjump = jumps.isEmpty() ? -1 : jumps.first();
        if (!jumps.isEmpty()) jumps.remove(0);
// #define DEBUG_GRAPH 1
#ifdef DEBUG_GRAPH
        qDebug() << "---------" << jumps.count()+1;
#endif
        for(unsigned int j=0; j<pointsCount; ++j)
        {
            const QPointF act = toWidget(vect.at(j));

//          qDebug() << "xxxxx" << act << ultim << qIsNaN(act.y()) << qIsNaN(ultim.y());
            if(qIsInf(act.y()) && !qIsNaN(act.y())) qDebug() << "trying to plot from a NaN value" << act << ultim;
            else if(qIsInf(act.y()) && qIsNaN(act.y())) qDebug() << "trying to plot to a NaN value";

            const bool bothinf=(qIsInf(ultim.y()) && qIsInf(act.y())) || (qIsInf(ultim.x()) && qIsInf(act.x()));
            if(!bothinf && !qIsNaN(act.y()) && !qIsNaN(ultim.y()) && nextjump!=int(j)) {
                if(qIsInf(ultim.y())) {
                    if(act.y()<0) ultim.setY(0);
                    if(act.y()>0) ultim.setY(qpd->height());
                }
//
                QPointF act2(act);
                if(qIsInf(act2.y())) {
                    if(ultim.y()<0) act2.setY(0);
                    if(ultim.y()>0) act2.setY(qpd->height());
                }

//              qDebug() << "xxxxx" << act2 << ultim << qIsNaN(act2.y()) << qIsNaN(ultim.y());

                p.drawLine(ultim, act2);

#ifdef DEBUG_GRAPH
                QPen dpen(Qt::red);
                dpen.setWidth(3);
                p.setPen(dpen);
                p.drawPoint(ultim);
                p.setPen(pfunc);
#endif
            } else if(nextjump==int(j)) {
                do {
                    if(nextjump!=int(j))
                        p.drawPoint(act);
                    
                    nextjump = jumps.isEmpty() ? -1 : jumps.first();
                    if (!jumps.isEmpty()) jumps.remove(0);

                } while(!jumps.isEmpty() && jumps.first()==nextjump+1);

#ifdef DEBUG_GRAPH
                qDebug() << "jumpiiiiiing" << ultim << toWidget(vect.at(j));
                QPen dpen(Qt::blue);
                dpen.setWidth(2);
                p.setPen(dpen);
                p.drawLine(QLineF(QPointF(act.x(), height()/2-10), QPointF(act.x(), height()/2+10)));
                p.setPen(pfunc);
#endif
            }

            ultim=act;
        }
    }

    p.end();
}

void Plotter2D::updateFunctions(const QModelIndex & parent, int start, int end)
{
    if (!d->m_model || parent.isValid())
        return;

    QRectF viewportFixed = viewport;
    viewportFixed.setTopLeft(viewport.bottomLeft());
    viewportFixed.setHeight(std::fabs(viewport.height()));
    
    for(int i=start; i<=end; i++)
    {
        PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(i));

        if (!curve || !curve->isVisible())
            continue;
        
        curve->update(viewportFixed);
    }
    
    m_dirty = false; // m_dirty = false means that the we will not recalculate functions points
    
    forceRepaint();
}

QPair<QPointF, QString> Plotter2D::calcImage(const QPointF& ndp) const
{
    if (!d->m_model || currentFunction() == -1)
        return QPair<QPointF, QString>();

    PlaneCurve* curve = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    
    if (curve && curve->isVisible())
        return curve->image(ndp);

    return QPair<QPointF, QString>();
}

QRectF Plotter2D::normalizeUserViewport(const QRectF &uvp)
{
    QRectF normalizeduvp = uvp;
    rang_x = width()/normalizeduvp.width();
    rang_y = height()/normalizeduvp.height();
    
    if (m_keepRatio && rang_x != rang_y)
    {
        rang_y=rang_x=qMin(std::fabs(rang_x), std::fabs(rang_y));

        if(rang_y>0.) rang_y=-rang_y;
        if(rang_x<0.) rang_x=-rang_x;

        double newW=width()/rang_x, newH=height()/rang_x;

        double mx=(uvp.width()-newW)/2.;
        double my=(uvp.height()-newH)/2.;

        normalizeduvp.setLeft(uvp.left()+mx);
        normalizeduvp.setTop(uvp.bottom()-my);
        normalizeduvp.setWidth(newW);
        normalizeduvp.setHeight(-newH); //WARNING why negative distance?

        //Commented because precision could make the program crash
//      Q_ASSERT(uvp.center() == viewport.center());
    }

    return normalizeduvp;
}

void Plotter2D::updateScale(bool repaint)
{
    viewport = normalizeUserViewport(userViewport);

    if (repaint) {
        if (d->m_model && d->m_model->rowCount()>0)
            updateFunctions(QModelIndex(), 0, d->m_model->rowCount()-1);
        else
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
    if (!d->m_model || currentFunction() == -1)
        return QLineF();

    PlaneCurve* plot = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    
    if (plot && plot->isVisible())
    {
        QLineF ret = plot->tangent(dp);
        
        if (ret.isNull() && currentFunction()>=0)
        {
            QPointF a = calcImage(dp-QPointF(.1,.1)).first;
            QPointF b = calcImage(dp+QPointF(.1,.1)).first;

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
    return QPointF((left + p.x()) * rang_x / d->m_dpr,  (top + p.y()) * rang_y / d->m_dpr);
}

QPointF Plotter2D::fromWidget(const QPoint& p) const
{
    double negativePartX = -viewport.left();
    double negativePartY = -viewport.top();
    
    return QPointF(p.x()/(rang_x*d->m_dpr)-negativePartX, p.y()/(rang_y*d->m_dpr)-negativePartY);
}

QPointF Plotter2D::toViewport(const QPoint &mv) const
{
    return QPointF(mv.x()/rang_x, mv.y()/rang_y);
}

void Plotter2D::moveViewport(const QPoint& delta)
{
    QPointF rel = toViewport(delta);
    QRectF viewport = currentViewport();
    
    viewport.moveLeft(viewport.left() - rel.x());
    viewport.moveTop(viewport.top() - rel.y());
    setViewport(viewport);
}

void Plotter2D::scaleViewport(qreal scale, const QPoint& center, bool repaint)
{
    QPointF p = fromWidget(center);
    QSizeF ns = viewport.size()*scale;
    QRectF nv(viewport.topLeft(), ns);
    
    setViewport(nv, false); //NOTE here isn't necessary to repaint, thus false

    QPointF p2 = p-fromWidget(center);
    nv.translate(p2);
    setViewport(nv, repaint);
}

void Plotter2D::setKeepAspectRatio(bool ar)
{
    m_keepRatio=ar;
    updateScale(true);
}

void Analitza::Plotter2D::setModel(QAbstractItemModel* f)
{
    d->setModel(f);
}

void Plotter2DPrivate::setModel(QAbstractItemModel* f)
{
    if(m_model == f)
        return;
    

    if (m_model) {
        disconnect(m_model, &QAbstractItemModel::dataChanged, this, &Plotter2DPrivate::updateFuncs);
        disconnect(m_model, &QAbstractItemModel::rowsInserted, this, &Plotter2DPrivate::addFuncs);
        disconnect(m_model, &QAbstractItemModel::rowsRemoved, this, &Plotter2DPrivate::forceRepaint);
    }

    m_model = f;

    if (m_model) {
        connect(m_model, &QAbstractItemModel::dataChanged, this, &Plotter2DPrivate::updateFuncs);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &Plotter2DPrivate::addFuncs);
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, &Plotter2DPrivate::forceRepaint);

        q->updateFunctions({}, 0, m_model->rowCount());
    } else {
        q->forceRepaint();
    }
}

void Analitza::Plotter2D::setDevicePixelRatio(qreal dpr)
{
    d->m_dpr = dpr;
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

void Plotter2D::zoomIn(bool repaint)
{
    scaleViewport(ZoomInFactor, QPoint(m_size.width()*0.5, m_size.height()*0.5), repaint);
}

void Plotter2D::zoomOut(bool repaint)
{
    scaleViewport(ZoomOutFactor, QPoint(m_size.width()*0.5, m_size.height()*0.5), repaint);
}

void Plotter2D::setShowMinorGrid(bool mt)
{
    m_showMinorGrid=mt;
    forceRepaint();
}

void Plotter2D::setShowGrid(bool show)
{
    if (m_showGrid != show) {
        m_showGrid=show;
        forceRepaint();

        showGridChanged();
    }
}

QAbstractItemModel* Plotter2D::model() const
{
    return d->m_model;
}
