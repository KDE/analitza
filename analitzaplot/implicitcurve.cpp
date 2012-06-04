/*************************************************************************************
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

#include <QRect>
#include <QBitArray>

#include "analitza/value.h"

class ANALITZAPLOT_EXPORT ImplicitCurve : public FunctionImpl2D
{
public:
    explicit ImplicitCurve(const Analitza::Expression &expression, Analitza::Variables *variables);
    ImplicitCurve(const ImplicitCurve &implicitCurve);
    virtual ~ImplicitCurve();

    static QStringList supportedBVars()
    {
        return QStringList() << "x" << "y";
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static QStringList examples()
    {
        QStringList ret;
        ret << "x*x+y*y-10" << "(2*x+y)*(x^2+y^2)^4+2*y*(5*x^4+10*x^2*y^2-3*y^4)-2*x+y" <<
            "sin(x)-cos(y)" << "(x^2+y^2-16)+x^2*y^2" << "(x^2+y^2)*(y^2+x*(x+4))-4*4*x*y^2" <<
            "(x^2+y^2)^2-19*x*(x^2-y^2)" << "x*(x^2-3*y^2)-9*(x^2+y^2)" <<
            "(x^2+y^2)*(x^2+y^2-16)^2-4*16*(x^2+y^2-4*x)^2" << "y^2*(y^2-10)-x^2*(x^2-9)" <<
            "x^702+y^702-9000000";

        return ret;
    }

    QStringList arguments() const
    {
        return supportedBVars();
    }
    Function::Axe axeType() const
    {
        return Function::Cartesian;
    }
    void solve(const RealInterval::List &spaceBounds = RealInterval::List());
    FunctionImpl * copy()
    {
        return new ImplicitCurve(*this);
    }

    QPair<QPointF, QString> calc(const QPointF &dp);
    QLineF derivative(const QPointF &point);

    bool allDisconnected() const
    {
        return true;
    }
    double getFValue(double xValue, double yValue);

protected:
    Analitza::Cn* m_x;
    Analitza::Cn* m_y;

    RealInterval::List m_oldDomain;
    bool m_resolutionWasImproved;









    QPointF last_calc;


};

ImplicitCurve::ImplicitCurve(const Analitza::Expression &expression, Analitza::Variables *variables)
    : FunctionImpl2D(expression, variables)
    , m_x(new Analitza::Cn)
    , m_y(new Analitza::Cn)
    , m_resolutionWasImproved(false)
{
    m_runStack.append(m_x);
    m_runStack.append(m_y);
    m_type2D = 2;

    m_evaluator.setStack(m_runStack);
}

ImplicitCurve::ImplicitCurve(const ImplicitCurve &implicitCurve)
    : FunctionImpl2D(implicitCurve)
    , m_x(new Analitza::Cn)
    , m_y(new Analitza::Cn)
    , m_resolutionWasImproved(implicitCurve.m_resolutionWasImproved)
{
    m_runStack.append(m_x);
    m_runStack.append(m_y);
    m_type2D = 2;

    m_evaluator.setStack(m_runStack);
}

ImplicitCurve::~ImplicitCurve()
{
    delete m_x;
    delete m_y;
}

void ImplicitCurve::solve(const RealInterval::List &spaceBounds)
{






























    if ((m_oldDomain != m_domain) || m_oldDomain.isEmpty())
    {





        QList<QPointF> points;

        points.clear();



        int resolutionForImplicitCurves = 20*m_resolution;

        qreal w = m_domain.at(0).upper() - m_domain.at(0).lower();
        qreal h = m_domain.at(1).upper() - m_domain.at(1).lower();

        unsigned int widthFlags = resolutionForImplicitCurves;
        unsigned int heightFlags = resolutionForImplicitCurves;

        qreal dw = w/resolutionForImplicitCurves;
        qreal dh = h/resolutionForImplicitCurves;

        QBitArray lastRow(widthFlags);
        QBitArray curRow(widthFlags);
        QBitArray nextRow(widthFlags);

        qreal xZero = m_domain.at(0).lower();
        qreal yZero = m_domain.at(1).lower();
        qreal yOne = m_domain.at(1).lower() + dh;

        m_resolutionWasImproved = false;























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







                points.append(QPointF(_xRelative, yNext));

            }

            QBitArray temp = lastRow;
            lastRow = curRow;
            curRow = nextRow;
            nextRow = temp;
        }








        QRectF viewport;
        viewport.setLeft(spaceBounds[0].lower());
        viewport.setRight(spaceBounds[0].upper());
        viewport.setBottom(spaceBounds[1].lower());
        viewport.setTop(spaceBounds[1].upper());





















        for (int i = 1; i < points.size(); i++)
        {
            if (!std::isnan(points.at(i).x()) && !std::isnan(points.at(i).y()) &&
                    !std::isnan(points.at(i).x()) && !std::isnan(points.at(i).y()))
            {
                if (viewport.contains(points.at(i-1)) && viewport.contains(points.at(i)))
                {





                    m_paths.append(QLineF(points.at(i), points.at(i) +QPointF(.001, 0.001)));
                }

            }
        }









    }




















    m_oldDomain = m_domain;
}

QPair<QPointF, QString> ImplicitCurve::calc(const QPointF& point)
{

    QVector<Analitza::Object*> vxStack;
    vxStack.append(m_x);
    QVector<Analitza::Object*> vyStack;
    vyStack.append(m_y);

    QString expLiteral = m_evaluator.expression().lambdaBody().toString();
    expLiteral.replace("y", QString::number(point.y()));
    expLiteral.prepend("x->");

    Analitza::Analyzer f(m_evaluator.variables());
    f.setExpression(Analitza::Expression(expLiteral, false));
    f.setStack(vxStack);

    Analitza::Analyzer df(m_evaluator.variables());
    df.setExpression(f.derivative("x"));
    df.setStack(vxStack);

    const int MAX_I = 256;
    const double E = 0.0001;
    double x0 = point.x();
    double x = x0;
    double error = 1000.0;
    int i = 0;
    bool has_root_x = true;


    if (!f.isCorrect() || !df.isCorrect())
    {
        return QPair<QPointF, QString>(QPointF(), QString());
    }

    while (true)
    {
        m_x->setValue(x0);

        double r = f.calculateLambda().toReal().value();
        double d = df.calculateLambda().toReal().value();

        i++;
        x = x0 - r/d;

        if (error < E) break;
        if (i > MAX_I)
        {
            has_root_x = false;
            break;
        }

        error = fabs(x - x0);
        x0 = x;
    }


    if (!has_root_x)
    {
        expLiteral = m_evaluator.expression().lambdaBody().toString();
        expLiteral.replace("x", QString::number(point.x()));
        expLiteral.prepend("y->");

        Analitza::Analyzer f(m_evaluator.variables());
        f.setExpression(Analitza::Expression(expLiteral, false));
        f.setStack(vyStack);

        Analitza::Analyzer df(m_evaluator.variables());
        df.setExpression(f.derivative("y"));
        df.setStack(vyStack);

        double y0 = point.y();
        double y = y0;
        error = 1000.0;
        i = 0;
        bool has_root_y = true;

        while (true)
        {
            m_y->setValue(y0);

            double r = f.calculateLambda().toReal().value();
            double d = df.calculateLambda().toReal().value();

            i++;
            y = y0 - r/d;

            if (error < E) break;
            if (i > MAX_I)
            {
                has_root_y = false;
                break;
            }

            error = fabs(y - y0);
            y0 = y;
        }

        if (has_root_y)
            last_calc = QPointF(point.x(), y);
        return QPair<QPointF, QString>(last_calc, QString());
    }
    else
    {
        last_calc = QPointF(x, point.y());

        return QPair<QPointF, QString>(last_calc, QString());
    }


}

QLineF ImplicitCurve::derivative(const QPointF& p)
{






    return QLineF();
}




double ImplicitCurve::getFValue(double xValue, double yValue)
{
    m_x->setValue(xValue);
    m_y->setValue(yValue);

    return m_evaluator.calculateLambda().toReal().value();
}


REGISTER_FUNCTION_2D(ImplicitCurve)

