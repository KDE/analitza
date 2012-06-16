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


class ANALITZAPLOT_EXPORT PolarCurve : public AbstractCurve
{
public:
    explicit PolarCurve(const Analitza::Expression &expression, Analitza::Variables *variables);
    PolarCurve(const PolarCurve &polarCurve);
    virtual ~PolarCurve();

    static QStringList supportedBVars()
    {
        return QStringList("q");
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static QStringList examples()
    {
        return QStringList("q->3*sin(q/0.142)");
    }

    QStringList arguments() const
    {
        return supportedBVars();
    }
    FunctionGraph::Axe axeType() const
    {
        return FunctionGraph::Polar;
    }
    void solve(const RealInterval::List &spaceBounds = RealInterval::List());
    AbstractMappingGraph * copy()
    {
        return new PolarCurve(*this);
    }

    QPair<QPointF, QString> calc(const QPointF &dp);
    QLineF derivative(const QPointF &point);

    bool allDisconnected() const
    {
        return false;
    }

    QPointF fromPolar(double r, double th)
    {
        return QPointF(r*cos(th), r*sin(th));
    }

protected:
    Analitza::Cn* m_th;
};

PolarCurve::PolarCurve(const Analitza::Expression &expression, Analitza::Variables *variables)
    : AbstractCurve(expression, variables)
    , m_th(new Analitza::Cn)
{
    m_runStack.append(m_th);
    m_evaluator.setStack(m_runStack);
    m_type2D = 4;
}

PolarCurve::PolarCurve(const PolarCurve &polarCurve)
    : AbstractCurve(polarCurve)
    , m_th(new Analitza::Cn)
{
    m_runStack.append(m_th);
    m_evaluator.setStack(m_runStack);
    m_type2D = 4;
}

PolarCurve::~PolarCurve()
{
    delete m_th;
}

void PolarCurve::solve(const RealInterval::List &spaceBounds)
{
    QList<QPointF> points;

    qreal ulimit=m_domain[0].upper();
    qreal dlimit=m_domain[0].lower();

    for(qreal th=dlimit; th<ulimit; th+=0.01)
    {
        m_th->setValue(th);
        qreal r = m_evaluator.calculateLambda().toReal().value();


        points.append(fromPolar(r, th));
    }


    QRectF viewport;
    viewport.setLeft(spaceBounds[0].lower());
    viewport.setRight(spaceBounds[0].upper());
    viewport.setBottom(spaceBounds[1].lower());
    viewport.setTop(spaceBounds[1].upper());

    buildPaths(viewport, points);

}

QPair<QPointF, QString> PolarCurve::calc(const QPointF& p)
{

    QPointF dp=p;

    if(p==QPointF(0., 0.))
        return QPair<QPointF, QString>(dp, i18n("center"));
    double th=atan(p.y()/ p.x()), r=1., d, d2;
    if(p.x()<0.)	th += PI;
    else if(th<0.)	th += 2.*PI;

    th=qMax(th, m_domain.at(0).lower());
    th=qMin(th, m_domain.at(0).upper());

    m_evaluator.setStack(m_runStack);
    QPointF dist;
    m_th->setValue(th);


    int imax = 0;

    do
    {
        m_th->setValue(th);
        r = m_evaluator.calculateLambda().toReal().value();
        dp = fromPolar(r, th);
        dist = (dp-p);
        d = sqrt(dist.x()*dist.x() + dist.y()*dist.y());

        m_th->setValue(th+2.*PI);
        r = m_evaluator.calculateLambda().toReal().value();
        dp = fromPolar(r, th);
        dist = (dp-p);
        d2 = sqrt(dist.x()*dist.x() + dist.y()*dist.y());

        th += 2.*PI;

        ++imax;
    }
    while(d>d2 && imax < 128);
    th -= 2.*PI;

    m_th->setValue(th);
    Analitza::Expression res=m_evaluator.calculateLambda();

    if(!res.isReal())
        m_errors += i18n("We can only draw Real results.");
    r = res.toReal().value();
    dp = fromPolar(r, th);




    return QPair<QPointF, QString>(dp, QString());

}

QLineF PolarCurve::derivative(const QPointF& point)
{
    return QLineF();

}

REGISTER_FUNCTION_2D(PolarCurve)



