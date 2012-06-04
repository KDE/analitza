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
#include "analitza/vector.h"


class ANALITZAPLOT_EXPORT ParametricCurve : public FunctionImpl2D
{
public:
    explicit ParametricCurve(const Analitza::Expression &expression, Analitza::Variables *variables);
    ParametricCurve(const ParametricCurve &parametricCurve);
    virtual ~ParametricCurve();

    static QStringList supportedBVars()
    {
        return QStringList("t");
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Vector,
                                            Analitza::ExpressionType(Analitza::ExpressionType::Value), 2));
    }
    static QStringList examples()
    {
        return QStringList() << "t->vector {sin(t), cos(t/0.2)}" << "t->vector {sin(t/2),t**3}";
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
        return new ParametricCurve(*this);
    }

    QPair<QPointF, QString> calc(const QPointF &dp);
    QLineF derivative(const QPointF &point);

    bool allDisconnected() const
    {
        return false;
    }

protected:
    Analitza::Cn* m_t;
};

ParametricCurve::ParametricCurve(const Analitza::Expression &expression, Analitza::Variables *variables)
    : FunctionImpl2D(expression, variables)
    , m_t(new Analitza::Cn)
{
    m_runStack.append(m_t);
    m_evaluator.setStack(m_runStack);
    m_type2D = 3;







}

ParametricCurve::ParametricCurve(const ParametricCurve &parametricCurve)
    : FunctionImpl2D(parametricCurve)
    , m_t(new Analitza::Cn)
{
    m_runStack.append(m_t);
    m_evaluator.setStack(m_runStack);
    m_type2D = 3;
}

ParametricCurve::~ParametricCurve()
{
    delete m_t;
}

void ParametricCurve::solve(const RealInterval::List &spaceBounds)
{

































































    QList<QPointF> points;

    qreal ulimit=domain()[0].upper();
    qreal dlimit=domain()[0].lower();

    for (qreal t = dlimit; t < ulimit; t += 0.01)
    {
        m_t->setValue(t);

        Analitza::Expression res = m_evaluator.calculateLambda();

        QPointF p(res.elementAt(0).toReal().value(),
                  res.elementAt(1).toReal().value());

        points.append(p);


    }


    QRectF viewport;
    viewport.setLeft(spaceBounds[0].lower());
    viewport.setRight(spaceBounds[0].upper());
    viewport.setBottom(spaceBounds[1].lower());
    viewport.setTop(spaceBounds[1].upper());

    buildPaths(viewport, points);
}

QPair<QPointF, QString> ParametricCurve::calc(const QPointF& point)
{

    m_t->setValue(0.);
    Analitza::Expression res=m_evaluator.calculateLambda();
    Analitza::Object* vo=res.tree();

    if(m_evaluator.isCorrect())
    {
        if(vo->type()!=Analitza::Object::vector)
        {
            m_errors += i18nc("if the specified function is not a vector",
                              "The parametric function does not return a vector");
        }
        else
        {
            Analitza::Vector* v=static_cast<Analitza::Vector*>(vo);
            if(v->size()!=2)
                m_errors += i18nc("If it is a vector but the wrong size. We work in R2 here",
                                  "A two-dimensional vector is needed");
            else if(v->at(0)->type()!=Analitza::Object::value || v->at(1)->type()!=Analitza::Object::value)
                m_errors += i18nc("The vector has to be composed by integer members",
                                  "The parametric function items should be scalars");
        }
    }

    if(m_evaluator.isCorrect() && m_evaluator.expression().lambdaBody().isVector() && m_errors.empty())
    {
        Analitza::Analyzer f(m_evaluator.variables());
        f.setExpression(Analitza::Expression("t->" + m_evaluator.expression().lambdaBody().elementAt(0).toString() + "+" + QString::number(-1.0*point.x()), false));




        f.setStack(m_runStack);

        Analitza::Analyzer df(m_evaluator.variables());
        df.setExpression(f.derivative("t"));
        df.setStack(m_runStack);










        const int MAX_I = 256;
        const double E = 0.0001;
        double t0 = -20.0;
        double t = t0;
        double error = 1000.0;
        int i = 0;

        while (true)
        {
            m_t->setValue(t0);



            double r = 0;
            double d = 0;





            if (f.isCorrect() && df.isCorrect())
            {
                Analitza::Expression expr = f.calculateLambda();
                Analitza::Expression expd = df.calculateLambda();

                r = expr.toReal().value();
                d = expd.toReal().value();

            }
            else
            {

            }


            i++;
            t = t0 - r/d;

            if ((error < E) || (i > MAX_I))
                break;

            error = fabs(t - t0);
            t0 = t;
        }



        if (std::isnan(t) || std::isinf(t))
        {
            return QPair<QPointF, QString>(point, QString());

        }

        m_t->setValue(t);
        if (m_t->format() != Analitza::Cn::Real)
        {
            return QPair<QPointF, QString>(point, QString());

        }


        return QPair<QPointF, QString>(QPointF(m_evaluator.calculateLambda().elementAt(0).toReal().value(),
                                               m_evaluator.calculateLambda().elementAt(1).toReal().value()), QString());

    }
    else
        return QPair<QPointF, QString>(point, QString());



    return QPair<QPointF, QString>(point, QString());


}

QLineF ParametricCurve::derivative(const QPointF& point)
{


    return QLineF();

}

REGISTER_FUNCTION_2D(ParametricCurve)



