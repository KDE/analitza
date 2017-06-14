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
#include <analitza/value.h>
#include <analitza/vector.h>

using namespace Analitza;

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

class FunctionParametric : public AbstractPlaneCurve
{
public:
    FunctionParametric(const Expression& e, const QSharedPointer<Analitza::Variables>& v);
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Parametric Curve 2D"))
    EXPRESSION_TYPE(
        Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
            .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
            .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Vector, Analitza::ExpressionType(Analitza::ExpressionType::Value), 2)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("t")))
    ICON_NAME(QStringLiteral("newparametric"))
    EXAMPLES(QStringList(QStringLiteral("t->vector {t,t**2}")))
    
    void update(const QRectF& viewport) override;
    
    QPair<QPointF, QString> image(const QPointF &mousepos) override;
    QLineF tangent(const QPointF &mousepos) override ;
    
    Analitza::Cn *t;

private:
    Cn findTValueForPoint(const QPointF& p);
    int resolution() { return 5000; }
};

FunctionParametric::FunctionParametric(const Expression& e, const QSharedPointer<Analitza::Variables>& v): AbstractPlaneCurve(e, v)
{
    t = arg(QStringLiteral("t"));
}

void FunctionParametric::update(const QRectF& viewport)
{
    QPair< double, double > theInterval;
    if(hasIntervals())
         theInterval = interval(QStringLiteral("t"));
    else
        theInterval = qMakePair(-3.1415*5, 3.1415*5);
    double dlimit=theInterval.first;
    double ulimit=theInterval.second;
    
    points.clear();
    jumps.clear();
    points.reserve(resolution());
    
    double inv_res= double((ulimit-dlimit)/resolution());
    QRectF vp(viewport);
    
    vp.setTop(viewport.top() - 2);
    vp.setBottom(viewport.bottom() + 2);
    vp.setLeft(viewport.left() + 2);
    vp.setRight(viewport.right() - 2);
    
    int i = 0;
    
    for(double t=dlimit; t<ulimit; t+=inv_res, ++i) {
        arg(QStringLiteral("t"))->setValue(t);
        Expression res=analyzer->calculateLambda();
        
        Cn x=res.elementAt(0).toReal();
        Cn y=res.elementAt(1).toReal();
        
        QPointF curp = QPointF(x.value(), y.value());
        addPoint(curp);
        Q_ASSERT(res.isVector());
    }
}

QPair<QPointF, QString> FunctionParametric::image(const QPointF &point)
{
    
    arg(QStringLiteral("t"))->setValue(findTValueForPoint(point).value());
    
    Expression res=analyzer->calculateLambda();
    Analitza::Cn comp1 = res.elementAt(0).toReal();
    Analitza::Cn comp2 = res.elementAt(1).toReal();
    
    return QPair<QPointF, QString>(QPointF(comp1.value(), comp2.value()), QString());

}

QLineF FunctionParametric::tangent(const QPointF &/*mousepos*/)
{
//    if(m_deriv)
//     {
//         double theT = findTValueForPoint(point).value();
//         
//         Analitza::Analyzer a;
//         a.setExpression(*m_deriv);
//         a.setStack(m_runStack);
//         arg("t")->setValue(theT);
//         Expression exp = a.calculateLambda();
//         
//         Analitza::Cn comp1 = exp.elementAt(0).toReal();
//         Analitza::Cn comp2 = exp.elementAt(1).toReal();
// 
//         double m = comp2.value()/comp1.value();
// 
//         return FunctionUtils::slopeToLine(m);
//     }
//     else
//         return QLineF();
    return QLineF();
}

Cn FunctionParametric::findTValueForPoint(const QPointF& /*p*/)
{
    //TODO doesn't work well
    
//     Q_ASSERT(analyzer.isCorrect() && analyzer.expression().lambdaBody().isVector());
//     
//     Analitza::Analyzer f(analyzer.variables());
//     f.setExpression(analyzer.expression().lambdaBody().elementAt(0));
//     f.setExpression(f.dependenciesToLambda());
//     f.setStack(m_runStack);
// 
//     Analitza::Analyzer df(analyzer.variables());
//     df.setExpression(f.derivative("t"));
//     df.setStack(m_runStack);
// 
// //TODO
// //    Analitza::Analyzer g(analyzer.variables());
// //    g.setExpression(Analitza::Expression("t->" + analyzer.expression().lambdaBody().elementAt(1).toString() + "+" + QString::number(-1.0*point.y()), false));
// //    g.refExpression()->parameters()[0]->value() = vx;
// //
// //    Analitza::Analyzer dg(analyzer.variables());
// //    dg.setExpression(g.derivative("t"));
// //    dg.refExpression()->parameters()[0]->value() = vx;
// 
//     const int MAX_I = 256;
//     const double E = 0.0001;
//     double t0 = 1.0;
//     double t = t0;
//     double error = 1000.0;
//     int i = 0;
// 
//     while (true)
//     {
//         arg("t")->setValue(t0);
// 
//         double r = f.calculateLambda().toReal().value()-p.x();
//         double d = df.calculateLambda().toReal().value();
// 
//         i++;
//         t = t0 - r/d;
// 
//         if ((error < E) || (i > MAX_I))
//             break;
// 
//         error = fabs(t - t0);
//         t0 = t;
//     }
// 
// //TODO
// //    t0 = 1.0;
// //    t = t0;
// //    error = 1000.0;
// //    i = 0;
// //
// //    while (true)
// //    {
// //        arg("t")->setValue(t0);
// //
// //        double r = g.calculateLambda().toReal().value();
// //        double d = dg.calculateLambda().toReal().value();
// //
// //        i++;
// //        t = t0 - r/d;
// //
// //        if ((error < E) || (i > MAX_I))
// //            break;
// //
// //        error = fabs(t - t0);
// //        t0 = t;
// //    }
//     return Cn(t);

    return Cn(0.0);
}

REGISTER_PLANECURVE(FunctionParametric)
