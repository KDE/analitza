/*************************************************************************************
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

#include <QRect>
#include <qmath.h>
#include <QDebug>
#include <QBitArray>
#include <QLineF>

#include <analitza/value.h>
#include <analitza/expressiontype.h>
#include <analitza/variable.h>
#include <analitza/variables.h>
#include "private/utils/marchingsquares.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using namespace Analitza;

class FunctionImplicit : public AbstractPlaneCurve, public MarchingSquares
{
public:
    CONSTRUCTORS(FunctionImplicit)
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Implicit Curve"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("x")) << QStringLiteral("y"))
    ICON_NAME(QStringLiteral("newimplicit"))
    EXAMPLES(QStringList(QStringLiteral("x^3-y^2+2")) << QStringLiteral("y^2*(y^2-10)-x^2*(x^2-9)"))

    void update(const QRectF& viewport) override;

    QPair<QPointF, QString> image(const QPointF &mousepos) override;
    QLineF tangent(const QPointF &mousepos) override ;

    //
    virtual double evalScalarField(double x, double y) override;

private:
    double getFValue(double xValue, double yValue);
};

void FunctionImplicit::update(const QRectF& vp)
{
    points.clear();
    jumps.clear();

    double minx = vp.left();
    double maxx = vp.right();
    double miny = vp.top();
    double maxy = vp.bottom();
    
    if (hasIntervals())
    {
        QPair<double, double> intervalx = interval(QStringLiteral("x"));
        QPair<double, double> intervaly = interval(QStringLiteral("y"));

        minx = intervalx.first;
        maxx = intervalx.second;
        miny = intervaly.first;
        maxy = intervaly.second;
    }
    
    setWorld(minx, maxx, miny, maxy);
    buildGeometry();

    for (int i = 0;  i < _faces_.size(); ++i)
    {
        points << _faces_[i].first <<  _faces_[i].second;
        jumps.append(points.size());
    }

//     if (points.size() <= 2) // y aunque/PESE a que el viewport se corta con el dominio
//     {
//         appendError(QCoreApplication::translate("This function can't be represented as a curve. To draw implicit curve, the function has to satisfy the implicit function theorem.", "Implicit function undefined in the plane"));
//     }
}

//Own
QPair<QPointF, QString> FunctionImplicit::image(const QPointF &/*point*/)
{

    return qMakePair(QPointF(), QString());

    //TODO port

//     QVector<Analitza::Object*> vxStack;
//     vxStack.append(m_x);
//     QVector<Analitza::Object*> vyStack;
//     vyStack.append(m_y);
//
//     QString expLiteral = analyzer.expression().lambdaBody().toString();
//     expLiteral.replace("y", QString::number(point.y()));
//     expLiteral.prepend("x->");
//
//     Analitza::Analyzer f(analyzer.variables());
//     f.setExpression(Analitza::Expression(expLiteral, false));
//     f.setStack(vxStack);
//
//     Analitza::Analyzer df(analyzer.variables());
//     df.setExpression(f.derivative("x"));
//     df.setStack(vxStack);
//
//     const int MAX_I = 256;
//     const double E = 0.0001;
//     double x0 = point.x();
//     double x = x0;
//     double error = 1000.0;
//     int i = 0;
//     bool has_root_x = true;
//
//
//     if (!f.isCorrect() || !df.isCorrect())
//     {
//         return QPair<QPointF, QString>(QPointF(), QString());
//     }
//
//     while (true)
//     {
//         arg("x")->setValue(x0);
//
//         double r = f.calculateLambda().toReal().value();
//         double d = df.calculateLambda().toReal().value();
//
//         i++;
//         x = x0 - r/d;
//
//         if (error < E) break;
//         if (i > MAX_I)
//         {
//             has_root_x = false;
//             break;
//         }
//
//         error = fabs(x - x0);
//         x0 = x;
//     }
//
//
//     if (!has_root_x)
//     {
//         expLiteral = analyzer.expression().lambdaBody().toString();
//         expLiteral.replace("x", QString::number(point.x()));
//         expLiteral.prepend("y->");
//
//         Analitza::Analyzer f(analyzer.variables());
//         f.setExpression(Analitza::Expression(expLiteral, false));
//         f.setStack(vyStack);
//
//         Analitza::Analyzer df(analyzer.variables());
//         df.setExpression(f.derivative("y"));
//         df.setStack(vyStack);
//
//         double y0 = point.y();
//         double y = y0;
//         error = 1000.0;
//         i = 0;
//         bool has_root_y = true;
//
//         while (true)
//         {
//             arg("y")->setValue(y0);
//
//             double r = f.calculateLambda().toReal().value();
//             double d = df.calculateLambda().toReal().value();
//
//             i++;
//             y = y0 - r/d;
//
//             if (error < E) break;
//             if (i > MAX_I)
//             {
//                 has_root_y = false;
//                 break;
//             }
//
//             error = fabs(y - y0);
//             y0 = y;
//         }
//
//         if (has_root_y)
//             last_calc = QPointF(point.x(), y);
//         return QPair<QPointF, QString>(last_calc, QString());
//     }
//     else
//     {
//         last_calc = QPointF(x, point.y());
//
//         return QPair<QPointF, QString>(last_calc, QString());
//     }

}

QLineF FunctionImplicit::tangent(const QPointF &/*mousepos*/)
{
    return QLineF();
}

double FunctionImplicit::getFValue(double xValue, double yValue)
{
    arg(QStringLiteral("x"))->setValue(xValue);
    arg(QStringLiteral("y"))->setValue(yValue);

//     return analyzer->calculateLambda().toReal().value();
    
    Analitza::Expression r=analyzer->calculateLambda();

    if(r.isReal())
    {
        Analitza::Cn z = analyzer->calculateLambda().toReal();

        if(z.format()==Analitza::Cn::Real)
            return z.value();
    }
    
    return 0;
}

double FunctionImplicit::evalScalarField(double x, double y)
{
//     qDebug() << x << y;

    return getFValue(x,y);
}

REGISTER_PLANECURVE(FunctionImplicit)
