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
#include <private/utils/mathutils.h>

#include <QRectF>
#include <QLineF>
#include "analitza/value.h"
#include "analitza/variable.h"

using namespace Analitza;

class FunctionCartesian : public AbstractPlaneCurve
{
    public:
        FunctionCartesian(const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables)
            : AbstractPlaneCurve(functionExpression, variables) { }

        void update(const QRectF& viewport) override;
        QPair<QPointF, QString> image(const QPointF &mousepos) override;
        QLineF tangent(const QPointF &mousepos) override ;
        Analitza::Cn* arg() { return AbstractPlaneCurve::arg(parameters().at(0)); }

    protected:
        void initDerivative() {
            if(!analyzer->isCorrect())
                return;
            
            m_deriv = analyzer->derivative(parameters().at(0));
            if(!analyzer->isCorrect() || !m_deriv.isCorrect()) {
                m_deriv.clear();
                analyzer->flushErrors();
            }
        }
        void optimizeJump();
        void calculateValues(double, double);
        Analitza::Expression m_deriv;
};

///Functions where the x is bounding. like x->sin(x)
class FunctionY : public FunctionCartesian
{
public:
    FunctionY(const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables)
        : FunctionCartesian(functionExpression, variables) { initDerivative(); }
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Plane Curve F(y)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("x")))
    ICON_NAME(QStringLiteral("newfunction"))
    EXAMPLES(QStringList(QStringLiteral("x")) << QStringLiteral("x*x") << QStringLiteral("x+4"))
};

///Functions where the y is bounding. like y->sin(y). FunctionY mirrored
class FunctionX : public FunctionCartesian
{
public:
    FunctionX(const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables)
        : FunctionCartesian(functionExpression, variables) { initDerivative(); }
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Plane Curve F(x)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("y")))
    ICON_NAME(QStringLiteral("newfunction"))
    EXAMPLES(QStringList(QStringLiteral("y")) << QStringLiteral("y*y") << QStringLiteral("y+4"))
    
    void update(const QRectF& viewport) override;
    
    QPair<QPointF, QString> image(const QPointF &mousepos) override;
    QLineF tangent(const QPointF &mousepos) override;
    
    qreal resolution() const { return 5000; }
};

void FunctionCartesian::update(const QRectF& viewport)
{
    double l_lim=0, r_lim=0;
    
    if (!hasIntervals()) {
        l_lim=viewport.left();
        r_lim=viewport.right();
    } else {
        QPair< double, double> limits = interval(parameters().at(0));
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

QPair<QPointF, QString> FunctionCartesian::image(const QPointF &p)
{
    QPointF dp=p;
    QString pos;

    if (hasIntervals())
    {
        QPair<double, double> intervalx = interval(parameters().at(0));
    
        if (intervalx.first >=dp.x() || dp.x() >= intervalx.second)
            return QPair<QPointF, QString>(QPointF(), QString());
    }

    arg()->setValue(dp.x());
    Analitza::Expression r=analyzer->calculateLambda();

    if(!r.isReal())
        appendError(QCoreApplication::tr("We can only draw Real results."));

    dp.setY(r.toReal().value());
    pos = QStringLiteral("x=%1 y=%2").arg(dp.x(),3,'f',2).arg(dp.y(),3,'f',2);
    return QPair<QPointF, QString>(dp, pos);
}

QLineF FunctionCartesian::tangent(const QPointF &mousepos)
{
    Analitza::Analyzer a(analyzer->variables());
    double ret = 0;
    if(m_deriv.isCorrect()) {
        arg()->setValue(mousepos.x());
        QVector<Analitza::Object*> runStack;
        runStack += arg();

        a.setExpression(m_deriv);
        a.setStack(runStack);
        if(a.isCorrect())
            ret = a.calculateLambda().toReal().value();

        if(!a.isCorrect()) {
            qDebug() << "Derivative error: " <<  a.errors();
            ret = 0;
        }
    }

    if(ret==0) {
        QVector<Analitza::Object*> vars;
        vars.append(new Analitza::Cn(mousepos.x()));
        a.setExpression(analyzer->expression());
        ret=a.derivative(vars);
        qDeleteAll(vars);
    }
    return slopeToLine(ret);
}

void FunctionCartesian::optimizeJump()
{
    QPointF before = points.at(points.count()-2), after=points.last();
    qreal x1=before.x(), x2=after.x();
    qreal y1=before.y(), y2=after.y();
    int iterations=5;

//  qDebug() << "+++++++++" << before << after;
    for(; iterations>0; --iterations) {
        qreal dist = x2-x1;
        qreal x=x1+dist/2;

        arg()->setValue(x);
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

void FunctionCartesian::calculateValues(double l_lim, double r_lim)
{
    jumps.clear();
    points.clear();
    points.reserve(m_resolution);

    double step = double((-l_lim+r_lim)/m_resolution);
    bool jumping=true;
    for(double x=l_lim; x<r_lim-step; x+=step) {
        arg()->setValue(x);
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

QPair<QPointF, QString> FunctionX::image(const QPointF& p)
{
    QPointF dp=p;
    arg()->setValue(dp.y());
    Analitza::Expression r=analyzer->calculateLambda();

    if(!r.isReal())
        appendError(QCoreApplication::tr("We can only draw Real results."));
    
    dp.setX(r.toReal().value());
    return QPair<QPointF, QString>(dp, QCoreApplication::tr("x=%1 y=%2").arg(dp.x()).arg(dp.y()));
}

void FunctionX::update(const QRectF& viewport)
{
    double l_lim=0, r_lim=0;
    
    if (!hasIntervals()) {
        l_lim=viewport.left();
        r_lim=viewport.right();
    } else {
        QPair< double, double> limits = interval(parameters().at(0));
        l_lim = limits.first;
        r_lim = limits.second;
    }

    calculateValues(l_lim, r_lim);
    
    for(int i=0; i<points.size(); i++) {
        QPointF p=points[i];
        points[i]=QPointF(p.y(), p.x());
    }
}

QLineF FunctionX::tangent(const QPointF &p) 
{
    QPointF p1(p.y(), p.x());
    QLineF ret=FunctionCartesian::tangent(p1);
    return mirrorXY(ret);
}

REGISTER_PLANECURVE(FunctionY)
REGISTER_PLANECURVE(FunctionX)
