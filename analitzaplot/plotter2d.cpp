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
#include "private/utils/mathutils.h"

#include <QPalette>
#include <QPen>
#include <QPainter>
#include <QApplication>
#include <cmath>
#include <QDebug>
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
QColor const Plotter2D::m_derivativeColor(90,90,160);

struct GridInfo
{
	qreal inc, xini, yini, xend, yend;
};

Plotter2D::Plotter2D(const QSizeF& size)
    : m_squares(true), m_keepRatio(true), m_dirty(true), m_size(size), m_model(0)
    , m_tickScaleSymbolValue(1)
    , m_tickScaleNumerator(1)
    , m_tickScaleDenominator(1)
    , m_ticksShown(Qt::Vertical|Qt::Horizontal)
    , m_axesShown(Qt::Vertical|Qt::Horizontal)
    , m_axisXLabel("x")
    , m_axisYLabel("y")
    , m_gridColor(QColor(230,230,230))
{}

Plotter2D::~Plotter2D()
{}

void Plotter2D::drawAxes(QPainter* painter, CoordinateSystem a) const
{
	GridInfo grid = drawTicks(painter);
    switch(a) {
    case Polar:
        drawPolarGrid(painter, grid);
        break;
    default:
        drawCartesianGrid(painter, grid);
    }
    drawMainAxes(painter);
}

void Plotter2D::drawMainAxes(QPainter* painter) const
{
	painter->setRenderHint(QPainter::Antialiasing, false);

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

    if (m_axesShown&Qt::Horizontal) {
        painter->drawLine(QPointF(0., center.y()), Xright);
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectX, startAngleX, spanAngle);
        painter->setRenderHint(QPainter::Antialiasing, false);
    }

    if (m_axesShown&Qt::Vertical) {
        painter->drawLine(Ytop, QPointF(center.x(), this->height()));
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPie(rectY, startAngleY, spanAngle);
        painter->setRenderHint(QPainter::Antialiasing, false);
    }

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

GridInfo Plotter2D::drawTicks(QPainter* painter) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QPen gridPen(m_gridColor);
	const QPen tickPen(QPalette().text().color());

    const QString symbol =m_tickScaleSymbol;
	const bool symbolFormat = !symbol.isEmpty();
    const int numerator = m_tickScaleNumerator;
    const int denominator =m_tickScaleDenominator;

	GridInfo ret;
    // prcesss
    qreal coef = qreal(numerator) / qreal(denominator);
    ret.inc = symbolFormat? coef*m_tickScaleSymbolValue : coef;

    QString numStr = QString::number(numerator);
    QString denStr = QString::number(denominator);

    //END params of algotihm

    int decimalPrecision = denominator == 1 && !symbolFormat? 0 : denominator == 2? 1 : 2;

    //QString tickLabel = symbol;
    //QString tickCoef("");

    int correctScale = 1;
	double decs = std::floor(std::log10(qMax(-viewport.height(), viewport.width())));
	if(decs>1)
		correctScale = pow(10, decs)/2;

    ret.inc *= correctScale;
    ret.xini=floor((viewport.left())/ret.inc)*ret.inc;
    ret.yini=floor((viewport.bottom())/ret.inc)*ret.inc;
    ret.xend=ceil((viewport.right())/ret.inc)*ret.inc;
    ret.yend=ceil((viewport.top())/ret.inc)*ret.inc;

    int i = 1;

    QPointF p;

	QFontMetrics fm(painter->font());
    if (m_ticksShown & Qt::Horizontal)
    {
        for(double x =ret.xini; x <ret.xend; x +=ret.inc, i+=1)
        {
			if(x==0)
				continue;
            p = toWidget(QPointF(x, 0.));

            painter->setPen(tickPen);

            if (!symbolFormat || !symbolFormat) {
                QString s = QString::number(x, 'f', decimalPrecision);
                painter->drawText(p.x() + fm.width(s)/2, p.y()+20, s);
            } else {
                int iCorrected = i*correctScale;
                int sign = x <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                painter->drawText(p.x(), p.y()+20, QString::number(sign*val*numerator)+symbol);

                if (denominator != 1 && (iCorrected % denominator != 0))
                {
                    painter->drawLine(p.x()-5, p.y()+22, p.x()+fm.width(numStr+symbol), p.y()+22);
                    painter->drawText(p.x()-10+fm.width(symbol), p.y()+20+fm.height(), denStr);
                }
            }
        }
    }

    if (m_ticksShown & Qt::Vertical)
    {
        i = 1;

        for(double y = ret.yini; y < ret.yend; y +=ret.inc, i+=1)
        {
			if(y==0)
				continue;
            p = toWidget(QPointF(0., y));
            painter->setPen(tickPen);

            if (!symbolFormat || !symbolFormat) {
                QString s = QString::number(y, 'f', decimalPrecision);
                painter->drawText(-20+p.x() - fm.width(s)/2, p.y()+20, s);
            } else {
                int iCorrected = i*correctScale;
                int sign = y <= 0. ? -1 : 1;
                int val = iCorrected % denominator == 0? iCorrected/denominator : iCorrected;

                QString text = QString::number(sign*val*numerator)+symbol;
                qreal correctxpos = fm.width(text)+6;
                painter->drawText(-correctxpos + p.x(), p.y()+5, text);

                if (denominator != 1 && (iCorrected % denominator != 0))
                {
                    painter->drawLine(-correctxpos + p.x()-5, p.y()+5,-correctxpos + p.x()+fm.width(numStr+symbol), p.y()+5);
                    painter->drawText(-correctxpos + p.x()-10+fm.width(symbol), p.y()+5+fm.height(), denStr);
                }
            }

        }
    }
    return ret;
}

void Plotter2D::drawPolarGrid(QPainter* painter, const GridInfo& grid) const
{
	const QPen gridPen(m_gridColor);
	painter->setPen(gridPen);
    if (m_squares) {
		qreal until = qMax(qMax(qAbs(viewport.left()), qAbs(viewport.right())), qMax(qAbs(viewport.top()), qAbs(viewport.bottom())));
		until *= std::sqrt(2);
        for (qreal i = grid.inc; i < until; i +=grid.inc )
        {
            QPointF p(toWidget(QPointF(i,i)));
            QPointF p2(toWidget(QPointF(-i,-i)));
            QRectF er(p.x(),p.y(), p2.x()-p.x(),p2.y()-p.y());
            painter->drawEllipse(er);
        }
	}
}

void Plotter2D::drawCartesianGrid(QPainter* f, const GridInfo& grid) const
{
    QPointF p;
	const QPen gridPen(m_gridColor);
	int i = 1;
	f->setPen(gridPen);
	
    for(double x =grid.xini; x <grid.xend; x +=grid.inc, i+=1)
    {
        p = toWidget(QPointF(x, 0.));

        if(m_squares)
            f->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            f->drawLine(p, p+QPointF(0.,-3.));
    }

    for(double y = grid.yini; y < grid.yend; y +=grid.inc, i+=1)
    {
        p = toWidget(QPointF(0., y));

        if(m_squares)
            f->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
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
//  finestra.initFrom(this);
    p.setPen(pfunc);

    int current=currentFunction();
    PlotItem* plot = itemAt(current);
    CoordinateSystem t = plot ? plot->coordinateSystem() : Cartesian;
    
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

QPointF Plotter2D::calcImage(const QPointF& ndp) const
{
    if (!m_model || currentFunction() == -1)
        return QPointF();

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

void Plotter2D::updateTickScale(const QString& tickScaleSymbol, qreal tickScaleSymbolValue,
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
