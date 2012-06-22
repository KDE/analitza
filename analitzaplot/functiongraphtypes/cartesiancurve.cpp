/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include <QRectF>
#include "analitza/value.h"
#include "analitza/variable.h"


class ANALITZAPLOT_EXPORT CartesianCurveY : public AbstractPlaneCurve
{
public:
    TYPE_NAME("CartesianCurveY")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    ARGUMENTS("x")
    ICON_NAME("noane")
    EXAMPLES("x,x*x,x+4")    
    
    
    CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables);
	~CartesianCurveY()
	{
	}


    //MappingGraph
    QStringList errors() const { return m_errors; }
    bool isCorrect() const { return false; }

    //Curve

    //Own
    void update(const QRect& viewport);
    
    QPair<QPointF, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

private:
    void optimizeJump();
    
    QStringList m_errors;
};

CartesianCurveY::CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:AbstractPlaneCurve(functionExpression, variables)
{
}

//FunctionGraph



//Curve


void CartesianCurveY::update(const QRect& viewport)
{
    double l_lim=viewport.left()-.1, r_lim=viewport.right()+.1;

    if(!points().isEmpty()
            && isSimilar(points().first().x(), l_lim)
            && isSimilar(points().last().x(), r_lim)) {
        return;
    }
    
    clearPoints();
    clearPoints();;
//     points.reserve(resolution());
 
    //TODO GSOC(pres)
    double step= 0.1;
    
    bool jumping=true;
    
    for(double x=l_lim; x<r_lim-step; x+=step) 
    {

        arg("x")->setValue(x);
        Analitza::Cn y = analyzer.calculateLambda().toReal();
        QPointF p(x, y.value());
        bool ch=addPoint(p);
        
        
        bool jj=jumping;
        jumping=false;
        if(ch && !jj) {
//          if(!m_jumps.isEmpty()) qDebug() << "popopo" << m_jumps.last() << points.count();
            double prevY=points()[points().count()-2].y();
            if(y.format()!=Analitza::Cn::Real && prevY!=y.value()) {
                setJump(points().count()-1);
                jumping=true;
            } else if(points().count()>3 && traverse(points()[points().count()-3].y(), prevY, y.value())) {
                optimizeJump();
                setJump(points().count()-1);
                jumping=true;
            }
        }
    }
    

//  qDebug() << "juuuumps" << m_jumps << resolution();
}



//Own
QPair<QPointF, QString> CartesianCurveY::calc(const QPointF &mousepos)
{
    return qMakePair(QPointF(), QString());
}

QLineF CartesianCurveY::derivative(const QPointF &mousepos) const
{
    return QLineF();
}

void CartesianCurveY::optimizeJump()
{
//     QPointF before = points.at(points.count()-2), after=points.last();
//     qreal x1=before.x(), x2=after.x();
//     qreal y1=before.y(), y2=after.y();
//     int iterations=5;
//     
// //  qDebug() << "+++++++++" << before << after;
//     for(; iterations>0; --iterations) {
//         qreal dist = x2-x1;
//         qreal x=x1+dist/2;
//         
//         vx->setValue(x);
//         qreal y = func.calculateLambda().toReal().value();
//         
//         if(fabs(y1-y)<fabs(y2-y)) {
//             before.setX(x);
//             before.setY(y);
//             x1=x;
//             y1=y;
//         } else {
//             after.setX(x);
//             after.setY(y);
//             x2=x;
//             y2=y;
//         }
//     }
// //  qDebug() << "---------" << before << after;
//     points[points.count()-2]=before;
//     points.last()=after;
}





REGISTER_PLANECURVE(CartesianCurveY)

