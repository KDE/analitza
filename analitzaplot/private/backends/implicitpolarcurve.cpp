/*************************************************************************************
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

#include "private/abstractplanecurve.h"
#include "private/functiongraphfactory.h"
#include <private/utils/marchingsquares.h>
#include <private/utils/mathutils.h>

#include <QRectF>
#include <analitza/value.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using namespace Analitza;

class ImplicitPolar : public AbstractPlaneCurve, MarchingSquares
{
public:
    ImplicitPolar(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v = {});
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Polar implicit Curve 0=F(r: Radial, p: Polar)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Polar)
    PARAMETERS(QStringList(QStringLiteral("r")) << QStringLiteral("p")) //r:radial - t:theta
    ICON_NAME(QStringLiteral("newpolar"))
    EXAMPLES(QStringList(QStringLiteral("r+p=0")))

    void update(const QRectF& viewport) override;
    
    QPair<QPointF, QString> image(const QPointF &mousepos) override;
    QLineF tangent(const QPointF &mousepos) override ;
    
    virtual double evalScalarField(double x, double y) override;
    
    Analitza::Cn *r;
    Analitza::Cn *p;
};

ImplicitPolar::ImplicitPolar(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v)
    : AbstractPlaneCurve(e, v)
{
    r = arg(QStringLiteral("r"));
    p = arg(QStringLiteral("p"));
}

double ImplicitPolar::evalScalarField(double x, double y)
{
    double radial = 0;
    double polar = 0;

    cartesianToPolar(x,y, radial, polar);

    p->setValue(polar);
    r->setValue(radial);
    
    return analyzer->calculateLambda().toReal().value();
}

void ImplicitPolar::update(const QRectF& vp)
{
    points.clear();
    jumps.clear();

    double minx = 0;
    double maxx = 0;
    double miny = 0;
    double maxy = 0;
    
    if(!hasIntervals())
    {
        minx = vp.left();
        maxx = vp.right();
        miny = vp.top();
        maxy = vp.bottom();
    }

    setWorld(minx, maxx, miny, maxy);
    buildGeometry();

    for (int i = 0;  i < _faces_.size(); ++i)
    {
        points << _faces_[i].first <<  _faces_[i].second;
        jumps.append(points.size());
    }

    //TODO
//     if (points.size() <= 2) // y aunque/PESE a que el viewport se corta con el dominio
//     {
//         appendError(QCoreApplication::translate("This function can't be represented as a curve. To draw implicit curve, the function has to satisfy the implicit function theorem.", "Implicit function undefined in the plane"));
//     }
}

QPair<QPointF, QString> ImplicitPolar::image(const QPointF &p)
{
    QPointF dp=p;
    QString pos;
//     if(p==QPointF(0., 0.))
//         return QPair<QPointF, QString>(dp, QCoreApplication::translate("", "center"));
//     double th=atan(p.y()/ p.x()), r=1., d, d2;
//     if(p.x()<0.)    th += pi;
//     else if(th<0.)  th += 2.*pi;
//     
//     
//         //TODO CACHE en intervalvalues!!!
//     static QPair<double, double> c_limits = interval("p");
//     
// //     if ()
// //     
// //     static QPair<double, double> o_limits = c_limits;
//     
//     
//     double ulimit=c_limits.second;
//     double dlimit=c_limits.first;
//     
//     th=qMax(th, dlimit);
//     th=qMin(th, ulimit);
//     
// //     analyzer.setStack(m_runStack);
//     QPointF dist;
//     arg("p")->setValue(th);
//     do {
//         arg("p")->setValue(th);
//         r = analyzer->calculateLambda().toReal().value();
//         
//         dp = polarToCartesian(r,th);
//         dist = (dp-p);
//         d = sqrt(dist.x()*dist.x() + dist.y()*dist.y());
//         
//         arg("p")->setValue(th+2.*pi);
//         r = analyzer->calculateLambda().toReal().value();
// 
//         dp = polarToCartesian(r,th);
//         dist = (dp-p);
//         d2 = sqrt(dist.x()*dist.x() + dist.y()*dist.y());
//         
//         th += 2.*pi;
//     } while(d>d2);
//     th -= 2.*pi;
//     
//     arg("p")->setValue(th);
//     Analitza::Expression res=analyzer->calculateLambda();
//     
//     if(!res.isReal())
//        appendError(QCoreApplication::translate("", "We can only draw Real results."));
//     r = res.toReal().value();
//     
//     
//         
//     dp = polarToCartesian(r,th);
//     
//     pos = QStringLiteral("r=%1 th=%2").arg(r,3,'f',2).arg(th,3,'f',2);
    return QPair<QPointF, QString>(dp, pos);
}

QLineF ImplicitPolar::tangent(const QPointF &/*mousepos*/)
{
//     //TODO review calc and this method
// 
//     QString rt = analyzer.expression().lambdaBody().toString();
//     rt.replace("q", "t");
// 
//     QString comp1str = rt + "*cos(t)";
//     QString comp2str = rt + "*sin(t)";
// 
//     QString polart;
//     polart = "t->vector{" + comp1str + ", " + comp2str + "}";
// 
//     Analitza::Analyzer newfunc(analyzer.variables());
//     newanalyzer.setExpression(Analitza::Expression(polart, false));
// 
//     Q_ASSERT(newanalyzer.isCorrect() && newanalyzer.expression().lambdaBody().isVector());
//     
//     Analitza::Analyzer f(newanalyzer.variables());
//     f.setStack(m_runStack);
//     f.setExpression(Analitza::Expression("t->" + newanalyzer.expression().lambdaBody().elementAt(0).toString() + "+" + QString::number(-1.0*point.x()), false));
// 
//     Analitza::Analyzer df(newanalyzer.variables());
//     df.setStack(m_runStack);
//     df.setExpression(f.derivative("t"));
// 
//     if (!df.isCorrect()) return QLineF();
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
//     double t0 = atan(point.y()/ point.x());
// 
//     if(point.x()<0.)    t0 += pi;
//     else if(t0<0.)  t0 += 2.*pi;
// 
//     t0=qBound(downlimit(), t0, uplimit());
// 
//     double t = t0;
//     double error = 1000.0;
//     int i = 0;
// 
//     while (true)
//     {
//         m_th->setValue(t0);
// 
//         double r = f.calculateLambda().toReal().value();
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
// //        m_th->setValue(t0);
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
// 
//     Analitza::Analyzer dfunc(newanalyzer.variables());
//     danalyzer.setExpression(newanalyzer.derivative("t"));
//     danalyzer.setStack(m_runStack);
// 
//     m_th->setValue(t);
// 
//     Expression res = danalyzer.calculateLambda();
//     Cn comp1 = res.elementAt(0).toReal();
//     Cn comp2 = res.elementAt(1).toReal();
// 
//     double m = comp2.value()/comp1.value();
// 
//     return FunctionUtils::slopeToLine(m);

return QLineF();
    
}

REGISTER_PLANECURVE(ImplicitPolar)
