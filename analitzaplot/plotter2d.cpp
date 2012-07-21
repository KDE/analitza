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
#include "plotsproxymodel.h"

#include <QPalette>
#include <QPen>
#include <QPainter>
#include <QApplication>
#include <cmath>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

using namespace std;

// #define DEBUG_GRAPH

QColor const Plotter2D::m_axeColor(100,100,255);
QColor const Plotter2D::m_axe2Color(235,235,235);
QColor const Plotter2D::m_derivativeColor(90,90,160);



Plotter2D::Plotter2D(const QSizeF& size, PlotsProxyModel* model)
    : m_squares(true), m_keepRatio(true), m_size(size), m_model(model), m_dirty(true), m_axis(true)
{}

Plotter2D::~Plotter2D()
{}

void Plotter2D::drawAxes(QPainter *p, CoordinateSystem a)
{
    p->setRenderHint(QPainter::Antialiasing, false);
    
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

void Plotter2D::drawPolarAxes(QPainter *p)
{
    QPen ceixos;
    ceixos.setColor(m_axeColor);
    p->setPen(ceixos);
    const QPointF center = toWidget(QPointF(0.,0.));
    bool zero= center.x()>0. && center.y()>0. && center.x()<width() && center.y()<height();
    double thmin = zero ? 0. : fmin(floor(fmin(viewport.left(), viewport.bottom())), ceil(fmax(viewport.right(), viewport.top())));
    double thmax = ceil(fmax(
                sqrt(pow(viewport.topRight().x(), 2.) + pow(viewport.topRight().y(), 2.)),
                sqrt(pow(viewport.bottomLeft().x(), 2.)+ pow(viewport.bottomLeft().y(), 2.))
                )
            );
        
    ceixos.setColor(m_axe2Color);
    ceixos.setStyle(Qt::SolidLine);
    p->setPen(ceixos);
    
    p->setRenderHint(QPainter::Antialiasing, true);
    for(double i=thmin; i<thmax; i++) { //i is +
        QPointF p1(toWidget(QPointF(i,i)));
        QPointF p2(toWidget(QPointF(-i,-i)));
        p->drawEllipse(QRectF(p1.x(),p1.y(), p2.x()-p1.x(),p2.y()-p1.y()));
    }
    p->setRenderHint(QPainter::Antialiasing, false);
    
    ceixos.setColor(m_axeColor);
    ceixos.setStyle(Qt::SolidLine);
    p->setPen(ceixos);
    p->drawLine(QPointF(0., center.y()), QPointF(this->width(), center.y()));
    p->drawLine(QPointF(center.x(), 0.), QPointF(center.x(),this->height()));
}

void Plotter2D::drawCartesianAxes(QPainter *painter)
{
    
    QPen ceixos;
    const QPointF center = toWidget(QPointF(0.,0.));
    
    ceixos.setColor(m_axe2Color);
    ceixos.setStyle(Qt::SolidLine);
    painter->setPen(ceixos);
    
    double xini=ceil(viewport.left()), inc=1.;
    double yini=ceil(viewport.top());
    
    if(viewport.width()>100.) { //Draw less lines on large viewports
        inc=10.;
        xini=floor(xini/10.)*10.;
        yini=floor(yini/10.)*10.;
    }
    
    for(double x=xini; x<=viewport.right(); x+=inc) {   // ticks X
        QPointF p = toWidget(QPointF(x, 0.));
        if(m_squares)
            painter->drawLine(QPointF(p.x(), this->height()), QPointF(p.x(), 0.));
        else
            painter->drawLine(p, p+QPointF(0.,-3.));
    }
    
    for(double y=yini; y>=viewport.bottom(); y-=inc) {      // ticks y
        QPointF p = toWidget(QPointF(0., y));
        if(m_squares)
            painter->drawLine(QPointF(0., p.y()), QPointF(width(), p.y()));
        else
            painter->drawLine(p, p+QPointF(3.,0.));
    }
    
    ceixos.setColor(m_axeColor);
    ceixos.setStyle(Qt::SolidLine);
    painter->setPen(ceixos);
    
    QPointF Xright(this->width(), center.y());
    QPointF Ytop(center.x(), 0.);
    
    //draw viewport axes
    painter->drawLine(QPointF(0., center.y()), Xright);
    painter->drawLine(Ytop, QPointF(center.x(),this->height()));
    //EO draw viewport axes
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(m_axeColor);
    
    const double width=15., height=4.;
    QPointF dpx(width, height);
    QPointF dpy(height, width);
    QRectF rectX(Xright+dpx, Xright-dpx);
    QRectF rectY(Ytop+dpy, Ytop-dpy);
    
    int startAngleX = 150*16;
    int startAngleY = 240*16;
    int spanAngle = 60*16;
    painter->drawPie(rectX, startAngleX, spanAngle);
    painter->drawPie(rectY, startAngleY, spanAngle);
}

PlotItem* Plotter2D::fromProxy(int proxy_row) const
{
    QModelIndex pi = m_model->mapToSource(m_model->index(proxy_row, 0));
    
//     qDebug() << "de" << m_model->rowCount();
    
    if (pi.isValid())
        return qobject_cast<PlotsModel *>(m_model->sourceModel())->item(pi.row());
    
    return 0;
}

void Plotter2D::drawFunctions(QPaintDevice *qpd)
{
    QPen pfunc(QColor(0,150,0), 2);
    
    QPainter p;
    p.begin(qpd);
//  finestra.initFrom(this);
    p.setPen(pfunc);
    
    int current=currentFunction();
    CoordinateSystem t=Cartesian;
//     if(current>=0)
        //los cast producto de los itemroles son demaciado largos ... nada elegantes
//         t=(CoordinateSystem)m_model->data(m_model->index(current), FunctionGraphModel::CoordinateSystemRole).toInt(); // editFunction(current)->axeType();
    //comparado con esto que es mucho mejor
    

        if (!m_model || current == -1) t = Cartesian; 
        else
        {
            if (fromProxy(current))
                t=fromProxy(current)->coordinateSystem();
            else
                t = Cartesian;

        }
    
    if (m_axis)
        drawAxes(&p, t);
    
    p.setRenderHint(QPainter::Antialiasing, true);
    
    if (!m_model && m_dirty) return; // guard // si tenemos el model y ademas las funciones estan actualizadas pasamos este if y pintamos

    int k=0;
//     PlaneCurveModel::const_iterator it=m_model->constBegin(), itEnd=m_model->constEnd();
//     for (; it!=itEnd; ++it, ++k ) {

    //funtiongraph
    PlaneCurve *curve = 0;
    
    for (int k = 0; k < m_model->rowCount(); ++k )
    {
        curve = dynamic_cast<PlaneCurve *>(fromProxy(k));
        
        //NOTE GSOC POINTS=0
        //no siempre el backend va a generar puntos y si no lo hace no quiere decir que esta mal,
        //por ejemplo en el caso de parametric se hace un clip para ver si la curva esta dentro o no del viewport
        //entonces ademas de verificar que es sible si debe vericiar que existan puntos antes de pintar una funcion
        
        if (!curve->isVisible() || curve->points().isEmpty())
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

void Plotter2D::updateFunctions(const QModelIndex & parent, int start, int end) // indices del proxy
{
    if (!m_model) return; // guard

//     Q_ASSERT(startIdx.isValid() && endIdx.isValid());
//     int start=startIdx.row(), end=endIdx.row();
    
    PlaneCurve *curve = 0;
    for(int i=start; i<=end; i++) 
    {
        curve = dynamic_cast<PlaneCurve *>(fromProxy(i));

        if (!curve) continue;
        
        if (!curve->isVisible()) continue;
        
        QRectF viewport_fixed = viewport;
        viewport_fixed.setTopLeft(viewport.bottomLeft());
        viewport_fixed.setHeight(fabs(viewport.height()));
        //NOTE GSOC 
//         qDebug() << "ORI" << viewport << viewport.center() <<  "CH" << viewport_fixed<< viewport_fixed.center();
        //it works con este parche se le pasa a las curvas la informacion adecuada basada en centro y size
        // y se conserva la semantica de qrectf la esquina superior izquierda es el origen
        
        curve->update(viewport_fixed);
    }

    m_dirty = false;
    
    forceRepaint();
}

QPointF Plotter2D::calcImage(const QPointF& ndp) const
{
    if (!m_model || currentFunction() == -1) return QPointF(); // guard
    
    //DEPRECATED if (m_model->data(model()->index(currentFunction()), FunctionGraphModel::VisibleRole).toBool())
    if (fromProxy(currentFunction()))
        if (fromProxy(currentFunction())->isVisible())
            return dynamic_cast<PlaneCurve*>(fromProxy(currentFunction()))->image(ndp).first;

//         return m_model->curveImage(currentFunction(), ndp).first;

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
//         if(m_model && m_model->rowCount()>0)
//             updateFunctions(QModelIndex(), 0, m_model->rowCount()-1);
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
    if (!m_model || currentFunction() == -1) return QLineF(); // guard

    if (fromProxy(currentFunction()))
        if (fromProxy(currentFunction())->isVisible())
        {
            QLineF ret = dynamic_cast<PlaneCurve*>(fromProxy(currentFunction()))->tangent(dp);
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

void Plotter2D::setModel(PlotsProxyModel* f)
{
    //esto proxy debe ser del plotsmodel ... no de otro modelo
    Q_ASSERT(qobject_cast< PlotsModel* >(f->sourceModel()));
    Q_ASSERT(f->filterSpaceDimension() == 2); // este widget renderiza solo plots que esten en un spacio 2D
    
    m_model=f;
    modelChanged();
    forceRepaint();
}

void Plotter2D::setPaintedSize(const QSize& size)
{
    m_size=size;
    updateScale(true);
}
