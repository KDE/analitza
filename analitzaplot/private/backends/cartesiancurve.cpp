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


#include "private/abstractplanecurve.h"
#include "private/functiongraphfactory.h"

#include <QRectF>
#include <QLineF>
#include "analitza/value.h"
#include "analitza/variable.h"
#include "analitza/localize.h"

///Functions where the x is bounding. like x->sin(x)
class FunctionY : public AbstractPlaneCurve
{
public:
    CONSTRUCTORS(FunctionY)
    TYPE_NAME("Plane Curve F(y)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x")
    ICON_NAME("newfunction")
    EXAMPLES("x,x*x,x+4")    

    void update(const QRectF& viewport);
    
    QPair<QPointF, QString> image(const QPointF &mousepos);
    QLineF tangent(const QPointF &mousepos) ;
    
    //
    

private:
    void optimizeJump();
    void calculateValues(double, double);
    
};

void FunctionY::update(const QRectF& viewport)
{
//     double l_lim=viewport.left()-.1, r_lim=viewport.right()+.1;


    double l_lim=0, r_lim=0;
    
    if (isAutoUpdate())
    {
        l_lim=viewport.left();
        r_lim=viewport.right();
    }
    else // obey intervals
    {
        QPair< double, double> limits = interval("x");
        l_lim = limits.first;
        r_lim = limits.second;
    }

    
    if(!points.isEmpty()
            && isSimilar(points.first().x(), l_lim)
            && isSimilar(points.last().x(), r_lim)) {
        return;
    }
    
    calculateValues(l_lim, r_lim);
//  qDebug() << "end." << jumps;
}

//Own
QPair<QPointF, QString> FunctionY::image(const QPointF &p)
{
    QPointF dp=p;
    QString pos;

    if (!isAutoUpdate())
    {
        QPair<double, double> intervalx = interval("x");
    
        if (intervalx.first >=dp.x() || dp.x() >= intervalx.second)
            return QPair<QPointF, QString>(QPointF(), QString());
    }

    arg("x")->setValue(dp.x());
    Analitza::Expression r=analyzer->calculateLambda();

    if(!r.isReal())
        appendError(i18n("We can only draw Real results."));
    
    dp.setY(r.toReal().value());
    pos = QString("x=%1 y=%2").arg(dp.x(),3,'f',2).arg(dp.y(),3,'f',2);
    return QPair<QPointF, QString>(dp, pos);
}

QLineF FunctionY::tangent(const QPointF &mousepos) 
{
    //TODO port
//     Analitza::Analyzer a(analyzer.variables());
//     double ret;
//     
//     if(m_deriv) {
//         arg("x")->setValue(p.x());
//         a.setExpression(*m_deriv);
//         
//         a.setStack(m_runStack);
//         if(a.isCorrect())
//             ret = a.calculateLambda().toReal().value();
//         
//         if(!a.isCorrect()) {
//             qDebug() << "Derivative error: " <<  a.errors();
//             return QLineF();
//         }
//     } else {
//         QVector<Analitza::Object*> vars;
//         vars.append(new Cn(p.x()));
//         a.setExpression(analyzer.expression());
//         ret=a.derivative(vars);
//         qDeleteAll(vars);
//     }
//     
//     return FunctionUtils::slopeToLine(ret);

    return QLineF();
}


void FunctionY::optimizeJump()
{
    QPointF before = points.at(points.count()-2), after=points.last();
    qreal x1=before.x(), x2=after.x();
    qreal y1=before.y(), y2=after.y();
    int iterations=5;
    
//  qDebug() << "+++++++++" << before << after;
    for(; iterations>0; --iterations) {
        qreal dist = x2-x1;
        qreal x=x1+dist/2;
        
        arg("x")->setValue(x);
        qreal y = analyzer->calculateLambda().toReal().value();
        
        if(fabs(y1-y)<fabs(y2-y)) {
            before.setX(x);
            before.setY(y);
            x1=x;
            y1=y;
        } else {
            after.setX(x);
            after.setY(y);
            x2=x;
            y2=y;
        }
    }
//  qDebug() << "---------" << before << after;
    points[points.count()-2]=before;
    points.last()=after;
}

void FunctionY::calculateValues(double l_lim, double r_lim)
{
    jumps.clear();
    points.clear();
//     points.reserve(resolution());
    
//     double step= double((-l_lim+r_lim)/resolution());
    double step = double((-l_lim+r_lim)/500);
    
    bool jumping=true;
    for(double x=l_lim; x<r_lim-step; x+=step) {
        arg("x")->setValue(x);
        Analitza::Cn y = analyzer->calculateLambda().toReal();
        QPointF p(x, y.value());
        bool ch=addPoint(p);
        
        bool jj=jumping;
        jumping=false;
        if(ch && !jj) {
//          if(!jumps.isEmpty()) qDebug() << "popopo" << jumps.last() << points.count();
            double prevY=points[points.count()-2].y();
            if(y.format()!=Analitza::Cn::Real && prevY!=y.value()) {
                jumps.append(points.count()-1);
                jumping=true;
            } else if(points.count()>3 && traverse(points[points.count()-3].y(), prevY, y.value())) {
                optimizeJump();
                jumps.append(points.count()-1);
                jumping=true;
            }
        }
    }
//  qDebug() << "juuuumps" << m_jumps << resolution();
}

///

///Functions where the y is bounding. like y->sin(y). FunctionY mirrored
class FunctionX : public AbstractPlaneCurve
{
public:
    CONSTRUCTORS(FunctionX)
    TYPE_NAME("Plane Curve F(x)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("y")
    ICON_NAME("newfunction")
    EXAMPLES("y,y*y,y+4")  
    
    void update(const QRectF& viewport);
    
    QPair<QPointF, QString> image(const QPointF &mousepos);
    QLineF tangent(const QPointF &mousepos);
    
    //
    

private:
    void optimizeJump();
    void calculateValues(double, double);
};

QPair<QPointF, QString> FunctionX::image(const QPointF& p)
{
    QPointF dp=p;
    arg("y")->setValue(dp.y());
    Analitza::Expression r=analyzer->calculateLambda();
    
    if(!r.isReal())
        appendError(i18n("We can only draw Real results."));
    
    dp.setX(r.toReal().value());
    QString pos = QString("x=%1 y=%2").arg(dp.x(),3,'f',2).arg(dp.y(),3,'f',2);
    return QPair<QPointF, QString>(dp, pos);
}

void FunctionX::update(const QRectF& viewport)
{
    double l_lim=0, r_lim=0;
    
    if (isAutoUpdate())
    {
        l_lim=viewport.top();
        r_lim=viewport.bottom();
    }
    else // obey intervals
    {
        QPair< double, double> limits = interval("y");
        l_lim = limits.first;
        r_lim = limits.second;
    }    
    
//     qDebug() << l_lim << r_lim << isAutoUpdate();
    
    calculateValues(l_lim, r_lim);
    for(int i=0; i<points.size(); i++) {
        QPointF p=points[i];
        points[i]=QPointF(p.y(), p.x());
    }

// //BUG he movido algo debo verificar que el cidogo de arriva genera puntos ... por mientras algo mas simple que se ve igual :p
// //TODO
//     for (double eps = -10.0; eps <= 10.0; eps += 0.1)
//     {
//         arg("y")->setValue(eps);
//         double fy = analyzer->calculateLambda().toReal().value();
//         points.append(QPointF(fy, eps));
//     }
}

QLineF FunctionX::tangent(const QPointF &p) 
{
//     QPointF p1(p.y(), p.x());
//     QLineF ret=FunctionY::derivative(p1);
//     return FunctionUtils::mirrorXY(ret);

return QLineF();
}

void FunctionX::optimizeJump()
{
    QPointF before = points.at(points.count()-2), after=points.last();
    qreal x1=before.x(), x2=after.x();
    qreal y1=before.y(), y2=after.y();
    int iterations=5;
    
//  qDebug() << "+++++++++" << before << after;
    for(; iterations>0; --iterations) {
        qreal dist = x2-x1;
        qreal x=x1+dist/2;
        
        arg("y")->setValue(x);
        qreal y = analyzer->calculateLambda().toReal().value();
        
        if(fabs(y1-y)<fabs(y2-y)) {
            before.setX(x);
            before.setY(y);
            x1=x;
            y1=y;
        } else {
            after.setX(x);
            after.setY(y);
            x2=x;
            y2=y;
        }
    }
//  qDebug() << "---------" << before << after;
    points[points.count()-2]=before;
    points.last()=after;
}

void FunctionX::calculateValues(double l_lim, double r_lim)
{
    jumps.clear();
    points.clear();
//     points.reserve(resolution()); //TODO port
    
//     double step= double((-l_lim+r_lim)/resolution()); //TODO port
    double step = 0.1;
    
    bool jumping=true;
    for(double x=l_lim; x<r_lim-step; x+=step) {
        arg("y")->setValue(x);
        Analitza::Cn y = analyzer->calculateLambda().toReal();
        QPointF p(x, y.value());
        bool ch=addPoint(p);
        
        bool jj=jumping;
        jumping=false;
        if(ch && !jj) {
//          if(!jumps.isEmpty()) qDebug() << "popopo" << jumps.last() << points.count();
            double prevY=points[points.count()-2].y();
            if(y.format()!=Analitza::Cn::Real && prevY!=y.value()) {
                jumps.append(points.count()-1);
                jumping=true;
            } else if(points.count()>3 && traverse(points[points.count()-3].y(), prevY, y.value())) {
                optimizeJump();
                jumps.append(points.count()-1);
                jumping=true;
            }
        }
    }
//  qDebug() << "juuuumps" << jumps << resolution();
}



REGISTER_PLANECURVE(FunctionY)
REGISTER_PLANECURVE(FunctionX)
