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

#include <QPalette>
#include <QPen>
#include <QPainter>
#include <QApplication>
#include <cmath>
#include <QDebug>
#include <QStack>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

using namespace Analitza;

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

QColor const Plotter2D::m_axeColor(100,100,255);
QColor const Plotter2D::m_derivativeColor(90,90,160);

namespace Analitza {
    struct GridInfo
    {
        qreal inc, xini, yini, xend, yend;
        // sub5 flag is used for draw sub 5 intervals instead of 4
        bool sub5; // true if inc=5*pow(10,n) (so draw 5 sub intervals)
    };
}

Plotter2D::Plotter2D(const QSizeF& size)
    : m_showGrid(true)
    , m_gridColor(Qt::lightGray)
    , m_backgroundColor(Qt::white)
    , m_autoGridStyle(true)
    , m_gridStyleHint(Cartesian)
    , m_keepRatio(true), m_dirty(true), m_size(size), m_model(0)
    , m_tickScaleSymbolValue(1)
    , m_ticksShown(Qt::Vertical|Qt::Horizontal)
    , m_axesShown(Qt::Vertical|Qt::Horizontal)
    , m_axisXLabel("x")
    , m_axisYLabel("y")
    , m_ticksFormat(Number)
{}

Plotter2D::~Plotter2D()
{}

void Plotter2D::setGridStyleHint(CoordinateSystem suggestedgs)
{
    Q_ASSERT(suggestedgs == Analitza::Cartesian || suggestedgs == Analitza::Polar);
    
    m_gridStyleHint = suggestedgs;
    
    forceRepaint();
}

const QColor Plotter2D::computeSubGridColor() const
{
    QColor col = m_gridColor;
    
    if (m_backgroundColor.value() < 200)
    {
        if (m_gridColor.value() < 40)
            col.setHsv(col.hsvHue(), col.hsvSaturation(), 80);
        else
            col.setHsv(col.hsvHue(), col.hsvSaturation(), 25);
    }
    else // e.g: background color white and grid color gray
        col.setHsv(col.hsvHue(), col.hsvSaturation(), 245);
    
    return col;
}

const GridInfo Plotter2D::getGridInfo() const
{
    GridInfo ret;
    
    const double currvpsize = viewport.width();
    
    static double oldvpsize = currvpsize;
    static double inc = m_tickScaleSymbolValue;
    static double zoomoutacum = 0.0;
    static int zoomcount = -1;
    static bool sub5 = false;
    
    float zoomfactor = 2;
    
    if (oldvpsize < currvpsize) // zoom-out
    {
        zoomoutacum += currvpsize - oldvpsize;
        
        if (2*zoomfactor*inc <= zoomoutacum)
        {
            zoomoutacum = 0.0;
            
            if ((zoomcount) % 3 == 0)
                    sub5 = true;
                else
                    sub5 = false;

            if (zoomcount % 3 == 0)
                inc *= 2.5; // 5*h/2
            else
                inc *= 2; //2*h
            
            ++zoomcount;
        }
    }
    else
        if (oldvpsize > currvpsize) // zoom-in
        {
            if (currvpsize < 2.0)
                zoomfactor = 1;
        
            if (currvpsize <= 2*inc*zoomfactor)
            {
                --zoomcount;
                
                if ((zoomcount - 1) % 3 == 0)
                    sub5 = true;
                else
                    sub5 = false;
                
                if (zoomcount % 3 == 0)
                    inc *= 0.4; // 2*h/5
                else
                    inc *= 0.5; // h/2
            }
        }
    
    oldvpsize = currvpsize;

    ret.inc = inc;
    ret.sub5 = sub5;
    ret.xini=floor((viewport.left())/ret.inc)*ret.inc;
    ret.yini=floor((viewport.bottom())/ret.inc)*ret.inc;
    ret.xend=ceil((viewport.right())/ret.inc)*ret.inc;
    ret.yend=ceil((viewport.top())/ret.inc)*ret.inc;
    
    return ret; 
}

void Plotter2D::drawAxes(QPainter* painter, CoordinateSystem coordsys) const
{
    GridInfo grid = getGridInfo();
    
    switch (coordsys) 
    {
        case Polar: drawPolarGrid(painter, grid); break;
        default: drawCartesianGrid(painter, grid);
    }
    
    drawMainAxes(painter);
    //NOTE always draw the ticks at the end: avoid the grid lines overlap the ticks text
    drawTicks(painter, grid);
}

void Plotter2D::drawMainAxes(QPainter* painter) const
{
    const QFontMetrics fm(painter->font());
    const QPen axesPen(m_axeColor, 1, Qt::SolidLine);
    const QPointF center = toWidget(QPointF(0.,0.));
    
    painter->setPen(axesPen);
    painter->setBrush(axesPen.color());
    
    int startAngleX = 150*16;
    int startAngleY = 240*16;
    int spanAngle = 60*16;
    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);
    const double arrowWidth=15., arrowHeight=4.;
    QPointF dpx(arrowWidth, arrowHeight);
    QPointF dpy(arrowHeight, arrowWidth);
    QRectF rectX(Xright+dpx, Xright-dpx);
    QRectF rectY(Ytop+dpy, Ytop-dpy);

    if (m_axesShown&Qt::Horizontal)
    {
        painter->drawLine(QPointF(0., center.y()), Xright);
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectX, startAngleX, spanAngle);
    }

    if (m_axesShown&Qt::Vertical)
    {
        painter->drawLine(Ytop, QPointF(center.x(), this->height()));
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectY, startAngleY, spanAngle);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    // axis labels
    QFont labelsfont = painter->font();
    labelsfont.setBold(true);

    painter->drawText(Xright.x() - fm.width(m_axisXLabel) - 5, center.y() - 20, m_axisXLabel);
    painter->drawText(center.x() + 5, Ytop.y() + fm.height() + 15, m_axisYLabel);

    //write coords
    QString rightBound=QString::number(viewport.right());
    int width=painter->fontMetrics().width(rightBound);

    painter->drawText(QPointF(3.+this->width()/2., 13.                 ), QString::number(viewport.top()));
    painter->drawText(QPointF(3.+this->width()/2., this->height()-5.   ), QString::number(viewport.bottom()));
    painter->drawText(QPointF(8.                 , this->height()/2.-5.), QString::number(viewport.left()));
    painter->drawText(QPointF(this->width()-width, this->height()/2.-5.), rightBound);
    //EO write coords
}

void Plotter2D::drawTicks(QPainter* painter, const GridInfo& gridinfo) const
{
    const double inc = gridinfo.inc;
    const unsigned short axisxseparation = 16; // distance between tick text and x-axis 
    const unsigned short axisyseparation = 4; // distance between tick text and y-axis
    const unsigned short textposcorrection = 2;
    
    painter->setRenderHint(QPainter::Antialiasing, true);    
    painter->setPen(QPen(QPalette().text().color()));
    
    QString s;
    QPointF p;
    
    if (m_ticksShown & Qt::Horizontal)
    {
        for(double x = inc; x < viewport.right(); x += inc)
        {
            p = toWidget(QPointF(x, 0.));
            s = QString::number(x);
            int swidth = painter->fontMetrics().width(s);
            
            painter->drawText(p.x()-swidth/2+textposcorrection, p.y()+axisxseparation, s);
        }
        
        for(double x = inc; x < -viewport.left(); x += inc)
        {
            p = toWidget(QPointF(-x, 0.));
            s = QString::number(-x);
            int swidth = painter->fontMetrics().width(s);
            
            painter->drawText(p.x()-swidth/2, p.y()+axisxseparation, s);
        }
    }
    
    if (m_ticksShown & Qt::Vertical)
    {
        for(double y = inc; y < viewport.top(); y += inc)
        {
            p = toWidget(QPointF(0., y));
            s = QString::number(y);
            int swidth = painter->fontMetrics().width(s);
            
            painter->drawText(p.x()-swidth-axisyseparation, p.y()+painter->fontMetrics().height()/2-textposcorrection, s);
        }

        for(double y = inc; y < -viewport.bottom(); y += inc)
        {
            p = toWidget(QPointF(0., -y));
            s = QString::number(-y);
            int swidth = painter->fontMetrics().width(s);
            
            painter->drawText(p.x()-swidth-axisyseparation, p.y()+painter->fontMetrics().height()/2, s);
        }
    }
}

void Plotter2D::drawPolarGrid(QPainter* painter, const GridInfo& grid) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPen gridPen(m_gridColor);
    painter->setPen(gridPen);
    
    const unsigned short nsubinc = grid.sub5? 5:4; // count for draw sub intervals
    const double inc = grid.inc/nsubinc; // inc with sub intervals
    
    QPen subGridPen = QPen(computeSubGridColor());
    
    if (m_showGrid) 
    {
        qreal until = qMax(qMax(qAbs(viewport.left()), qAbs(viewport.right())), qMax(qAbs(viewport.top()), qAbs(viewport.bottom())));
        until *= std::sqrt(2);
        
        int k = 1;
        
        for (qreal i = inc; i < until; i += inc, ++k)
        {
            QPointF p(toWidget(QPointF(i,i)));
            QPointF p2(toWidget(QPointF(-i,-i)));
            QRectF er(p.x(),p.y(), p2.x()-p.x(),p2.y()-p.y());
            
            if (k % nsubinc == 0)
                painter->setPen(gridPen);
            else // sub intervals
                painter->setPen(subGridPen);
            
            painter->drawEllipse(er);
        }
    }
    else
    {
        QPointF p;
        int i = 0;
        
        for (double x =grid.xini; x <grid.xend; x += inc, ++i)
        {
            if (x == 0.0) continue;
            
            p = toWidget(QPointF(x, 0.));

            painter->drawLine(p, p+QPointF(0.,-3.));
        }
        
        i = 0;
        
        for(double y = grid.yini; y < grid.yend; y += inc, ++i)
        {
            if (y == 0.0) continue;
            
            p = toWidget(QPointF(0., y));

            painter->drawLine(p, p+QPointF(3.,0.));
        }
    }
}

void Plotter2D::drawCartesianGrid(QPainter* f, const GridInfo& grid) const
{
    f->setRenderHint(QPainter::Antialiasing, false);
    const QPen gridPen(m_gridColor);
    f->setPen(gridPen);
    
    const unsigned short nsubinc = grid.sub5? 5:4; // count for draw sub intervals
    const double inc = grid.inc/nsubinc; // inc with sub intervals
    
    QPen subGridPen = QPen(computeSubGridColor());
    
    QPointF p;
    int i = 0;
    
    for (double x =grid.xini; x <grid.xend; x += inc, ++i)
    {
        if (x == 0.0) continue;
        
        p = toWidget(QPointF(x, 0.));

        if(m_showGrid)
        {
            if (i % nsubinc == 0)
                f->setPen(gridPen);
            else // sub intervals
                f->setPen(subGridPen);
            
            f->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        }
        else
            f->drawLine(p, p+QPointF(0.,-3.));
    }
    
    i = 0;
    
    for (double y = grid.yini; y < grid.yend; y += inc, ++i)
    {
        if (y == 0.0) continue;
        
        p = toWidget(QPointF(0., y));

        if(m_showGrid)
        {
            if (i % nsubinc == 0)
                f->setPen(gridPen);
            else // sub intervals
                f->setPen(subGridPen);
            
            f->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        }
        else
            f->drawLine(p, p+QPointF(3.,0.));
    }
}

PlotItem* Plotter2D::itemAt(int row) const
{
    if(!m_model)
        return 0;
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim2D)
        return 0;

    return plot;
}

void Plotter2D::drawFunctions(QPaintDevice *qpd)
{
    QPen pfunc(QColor(0,150,0), 2);
    
    QPainter p;
    p.begin(qpd);
    p.setPen(pfunc);
    
    int current=currentFunction();
    PlotItem* plot = itemAt(current);
    
    CoordinateSystem t = plot ? plot->coordinateSystem() : Cartesian;
    
    if (!m_autoGridStyle)
        t = m_gridStyleHint;
    
    drawAxes(&p, t);

    if (!m_model || m_dirty)
        return;
    
    p.setRenderHint(QPainter::Antialiasing, true);
    
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
//         pfunc.setStyle(Qt::SolidLine);
        p.setPen(pfunc);

        const QVector<QPointF> &vect=curve->points();
        QVector<int> jumps=curve->jumps();

        unsigned int pointsCount = vect.count();
        QPointF ultim(toWidget(vect[0]));

        int nextjump;
        nextjump = jumps.isEmpty() ? -1 : jumps.first();
        if (!jumps.isEmpty()) jumps.remove(0);
// #define DEBUG_GRAPH 1
#ifdef DEBUG_GRAPH
        qDebug() << "---------" << jumps.count()+1;
#endif
        for(unsigned int j=0; j<pointsCount; ++j)
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

//                     if(allJumps)
//                         nextjump += 2;
//                     else
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
    if (!m_model || parent.isValid())
        return;

    QRectF viewportFixed = viewport;
    viewportFixed.setTopLeft(viewport.bottomLeft());
    viewportFixed.setHeight(fabs(viewport.height()));
    for(int i=start; i<=end; i++) {
        PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(i));

        if (!curve || !curve->isVisible())
            continue;
        
        curve->update(viewportFixed);
    }

    m_dirty = false;

    forceRepaint();
}

QPair<QPointF, QString> Plotter2D::calcImage(const QPointF& ndp) const
{
    if (!m_model || currentFunction() == -1)
        return QPair<QPointF, QString>();

    //DEPRECATED if (m_model->data(model()->index(currentFunction()), FunctionGraphModel::VisibleRole).toBool())
    PlaneCurve* curve = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    if (curve && curve->isVisible())
        return curve->image(ndp);

    return QPair<QPointF, QString>();
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
        if(m_model && m_model->rowCount()>0)
            updateFunctions(QModelIndex(), 0, m_model->rowCount()-1);
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
    if (!m_model || currentFunction() == -1)
        return QLineF();

    PlaneCurve* plot = dynamic_cast<PlaneCurve*>(itemAt(currentFunction()));
    if (plot && plot->isVisible())
    {
        QLineF ret = plot->tangent(dp);
        if(ret.isNull() && currentFunction()>=0) {
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
	if(m_model == f)
		return;
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

void Plotter2D::setTicksFormat(TicksFormat tsfmt)
{
    m_ticksFormat = tsfmt;

    forceRepaint();
}
