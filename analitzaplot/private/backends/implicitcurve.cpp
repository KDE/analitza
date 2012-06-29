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


#include <QRect>
#include <QBitArray>

#include "analitza/value.h"


#include <QVector2D>
#include <qmath.h>
#include <QDebug>
#include <analitza/localize.h>

#include "analitza/value.h"
#include "analitza/expressiontype.h"
#include "analitza/variable.h"
#include "analitza/variables.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
static const double pi=M_PI;

using Analitza::Expression;
using Analitza::ExpressionType;
using Analitza::Variables;
using Analitza::Object;
using Analitza::Cn;

class ANALITZAPLOT_EXPORT FunctionImplicit : public AbstractPlaneCurve
{
public:
    CONSTRUCTORS(FunctionImplicit)
    TYPE_NAME("FunctionImplicit implicit curve")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,y")
    ICON_NAME("newimplicit")
    EXAMPLES("x^3-y^2+2,y^2*(y^2-10)-x^2*(x^2-9)")    
    
    void update(const QRect& viewport);
    
    QPair<QPointF, QString> image(const QPointF &mousepos);
    QLineF tangent(const QPointF &mousepos) ;
    
    //
    

private:
    double getFValue(double xValue, double yValue);
};

void FunctionImplicit::update(const QRect& vp)
{
    QRectF viewport(vp);
    
    QList<QPointF> temppts;

    temppts.clear();

//     int resolutionForImplicitCurves = 20*m_resolution;
    int resolutionForImplicitCurves = 256;

    //TODO CACHE en intervalvalues!!!
    static QPair<double, double> intervalx = interval("x");
    static QPair<double, double> intervaly = interval("y");
    
    
    qreal w = intervalx.second - intervalx.first;
    qreal h = intervaly.second - intervaly.first;

    unsigned int widthFlags = resolutionForImplicitCurves;
    unsigned int heightFlags = resolutionForImplicitCurves;

    qreal dw = w/resolutionForImplicitCurves;
    qreal dh = h/resolutionForImplicitCurves;

    QBitArray lastRow(widthFlags);
    QBitArray curRow(widthFlags);
    QBitArray nextRow(widthFlags);

    qreal xZero = intervalx.first;
    qreal yZero = intervaly.first;
    qreal yOne = intervaly.first + dh;

//     m_resolutionWasImproved = false;

    for (int xAbsolute = 0; xAbsolute < widthFlags; xAbsolute+=1)
    {
        qreal _xRelative = xZero + xAbsolute*dw;
        qreal resZero = getFValue(_xRelative, yZero);
        lastRow.setBit(xAbsolute, resZero >= 0.0);
        double resOne = getFValue(_xRelative, yOne);
        curRow.setBit(xAbsolute, resOne >= 0.0);
    }

    for (int yAbsolute = 1; yAbsolute < heightFlags - 1; yAbsolute++)
    {
        double yNext = yOne + yAbsolute*dh;

        for (int xAbsolute = 0; xAbsolute < widthFlags; xAbsolute+=1)
        {
            qreal _xRelative = xZero + xAbsolute*dw;
            qreal resNext = getFValue(_xRelative, yNext);
            bool south = resNext >= 0.0;
            nextRow.setBit(xAbsolute, south);

            if ((xAbsolute == 0) || (xAbsolute == widthFlags - 1))
                continue;

            bool north = lastRow.at(xAbsolute);
            bool east = curRow.at(xAbsolute - 1);
            bool west = curRow.at(xAbsolute + 1);
            bool cen = curRow.at(xAbsolute);
            int nPos = south ? 1 : 0;
            nPos += (north ? 1 : 0);
            nPos += (east ? 1 : 0);
            nPos += (west ? 1 : 0);

            if (((!cen) || (nPos >= 4)) && ((cen) || (nPos <= 0) )) continue;

            temppts.append(QPointF(_xRelative, yNext));
            
        }

        QBitArray temp = lastRow;
        lastRow = curRow;
        curRow = nextRow;
        nextRow = temp;
    }

    for (int i = 1; i < temppts.size(); i++)
    {
        if (!std::isnan(temppts.at(i).x()) && !std::isnan(temppts.at(i).y()) &&
                !std::isnan(temppts.at(i).x()) && !std::isnan(temppts.at(i).y()))
        {
            if (viewport.contains(temppts.at(i-1)) && viewport.contains(temppts.at(i)))
            {
                addPoint(temppts.at(i));
                jumps.append(temppts.size());
                addPoint(temppts.at(i) +QPointF(.001, 0.001));
                jumps.append(temppts.size());
            }
        }
    }

    if (points.size() <= 2)
    {
        appendError(i18nc("This function can't be represented as a curve. To draw implicit curve, the function has to satisfy the implicit function theorem.", "Implicit function undefined in the plane"));
    }
}

//Own
QPair<QPointF, QString> FunctionImplicit::image(const QPointF &point)
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

QLineF FunctionImplicit::tangent(const QPointF &mousepos) 
{


return QLineF();
    
}

double FunctionImplicit::getFValue(double xValue, double yValue)
{
    arg("x")->setValue(xValue);
    arg("y")->setValue(yValue);

    return analyzer->calculateLambda().toReal().value();
}

REGISTER_PLANECURVE(FunctionImplicit)