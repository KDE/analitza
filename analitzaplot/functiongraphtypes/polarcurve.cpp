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


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include <QRectF>
#include "analitza/value.h"

#include "analitza/localize.h"

class ANALITZAPLOT_EXPORT FunctionPolar : public AbstractPlaneCurve
{
public:
    TYPE_NAME("FunctionPolarFunctionPolar")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Polar)
    ARGUMENTS("q")
    ICON_NAME("newpolar")
    EXAMPLES("q->3*sin(q/0.142),q->q+3")    
    
    
    FunctionPolar(const Analitza::Expression &functionExpression, Analitza::Variables *variables);
    ~FunctionPolar()
    {
    }

    void update(const QRect& viewport);
    
    QPair<QPointF, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;
    
    //
    

private:
    inline QPointF fromPolar(double r, double th) { return QPointF(r*std::cos(th), r*std::sin(th)); }

};

FunctionPolar::FunctionPolar(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:AbstractPlaneCurve(functionExpression, variables)
{
}



void FunctionPolar::update(const QRect& viewport)
{
//     Q_ASSERT(analyzer.expression().isCorrect());
//     if(int(resolution())==points.capacity())
//         return;
    
    
    //TODO CACHE en intervalvalues!!!
    static QPair<double, double> c_limits = intervalValues("q");
    
//     if ()
//     
//     static QPair<double, double> o_limits = c_limits;
    
    
    double ulimit=c_limits.second;
    double dlimit=c_limits.first;
    
    points.clear();
    //TODO port
//     points.reserve(resolution());
    
//     analyzer.setStack(m_runStack);
//     double inv_res= double((ulimit-dlimit)/resolution()); TODO
       double inv_res = 0.1;
    double final=ulimit-inv_res;
    for(double th=dlimit; th<final; th+=inv_res) {
        arg("q")->setValue(th);
        double r = analyzer.calculateLambda().toReal().value();
        
        addPoint(fromPolar(r, th));
    }
}


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
static const double pi=M_PI;
//Own
QPair<QPointF, QString> FunctionPolar::calc(const QPointF &p)
{
    QPointF dp=p;
    QString pos;
    if(p==QPointF(0., 0.))
        return QPair<QPointF, QString>(dp, i18n("center"));
    double th=atan(p.y()/ p.x()), r=1., d, d2;
    if(p.x()<0.)    th += pi;
    else if(th<0.)  th += 2.*pi;
    
    
        //TODO CACHE en intervalvalues!!!
    static QPair<double, double> c_limits = intervalValues("q");
    
//     if ()
//     
//     static QPair<double, double> o_limits = c_limits;
    
    
    double ulimit=c_limits.second;
    double dlimit=c_limits.first;
    
    th=qMax(th, dlimit);
    th=qMin(th, ulimit);
    
//     analyzer.setStack(m_runStack);
    QPointF dist;
    arg("q")->setValue(th);
    do {
        arg("q")->setValue(th);
        r = analyzer.calculateLambda().toReal().value();
        dp = fromPolar(r, th);
        dist = (dp-p);
        d = sqrt(dist.x()*dist.x() + dist.y()*dist.y());
        
        arg("q")->setValue(th+2.*pi);
        r = analyzer.calculateLambda().toReal().value();
        dp = fromPolar(r, th);
        dist = (dp-p);
        d2 = sqrt(dist.x()*dist.x() + dist.y()*dist.y());
        
        th += 2.*pi;
    } while(d>d2);
    th -= 2.*pi;
    
    arg("q")->setValue(th);
    Analitza::Expression res=analyzer.calculateLambda();
    
    if(!res.isReal())
       appendError(i18n("We can only draw Real results."));
    r = res.toReal().value();
    dp = fromPolar(r, th);
    
    pos = QString("r=%1 th=%2").arg(r,3,'f',2).arg(th,3,'f',2);
    return QPair<QPointF, QString>(dp, pos);
}

QLineF FunctionPolar::derivative(const QPointF &mousepos) const
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



REGISTER_PLANECURVE(FunctionPolar)



