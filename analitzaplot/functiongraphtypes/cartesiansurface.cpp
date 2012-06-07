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

#include "analitza/value.h"
#include <analitza/vector.h>

class ANALITZAPLOT_EXPORT CartesianSurface : public FunctionImpl3D
{
public:
    explicit CartesianSurface(const Analitza::Expression &expression, Analitza::Variables *variables);
    CartesianSurface(const CartesianSurface &cartesianSurface);
    virtual ~CartesianSurface();

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

    const Analitza::Expression & lambda() const;

    QStringList arguments() const
    {
        return supportedBVars();
    }
    Function::Axe axeType() const
    {
        return Function::Cartesian;
    }
    void solve(const RealInterval::List &spaceBounds);
    FunctionImpl * copy()
    {
        return new CartesianSurface(*this);
    }

    QVector3D evalSurface(qreal u, qreal v);

    void loadParametricEvaluator();

protected:
    Analitza::Cn* m_u;
    Analitza::Cn* m_v;

    Analitza::Expression m_originalCartesianLambda;
};

CartesianSurface::CartesianSurface(const Analitza::Expression &expression, Analitza::Variables *variables)
    : FunctionImpl3D(expression, variables)
    , m_u(new Analitza::Cn)
    , m_v(new Analitza::Cn)
{



    loadParametricEvaluator();

    m_evaluator.setStack(m_runStack);

    if(m_evaluator.isCorrect())
    {






        m_evaluator.flushErrors();
    }
}

CartesianSurface::CartesianSurface(const CartesianSurface &cartesianSurface)
    : FunctionImpl3D(cartesianSurface)
    , m_u(new Analitza::Cn)
    , m_v(new Analitza::Cn)
{
    loadParametricEvaluator();
}

CartesianSurface::~CartesianSurface()
{
    delete m_u;
    delete m_v;
}

const Analitza::Expression & CartesianSurface::lambda() const
{
    return m_originalCartesianLambda;
}

void CartesianSurface::solve(const RealInterval::List &spaceBounds)
{


}

QVector3D CartesianSurface::evalSurface(qreal u, qreal v)
{
    m_u->setValue(u);
    m_v->setValue(v);

    Analitza::Expression res = m_evaluator.calculateLambda();

    Analitza::Object* vo = res.tree();
    Analitza::Vector* vec = dynamic_cast<Analitza::Vector*>(vo);


    if (vec)
    {
        Analitza::Cn* x = static_cast<Analitza::Cn*>(vec->at(0));
        Analitza::Cn* y = static_cast<Analitza::Cn*>(vec->at(1));
        Analitza::Cn* z = static_cast<Analitza::Cn*>(vec->at(2));

        return QVector3D(x->value(), y->value(), z->value());
    }

    return QVector3D();
}

void CartesianSurface::loadParametricEvaluator()

{
    m_originalCartesianLambda = Analitza::Expression(m_evaluator.expression());

    QString oldExp = m_evaluator.expression().lambdaBody().toString();
    oldExp.replace(QString("x"), QString("u"));
    oldExp.replace(QString("y"), QString("v"));

    QString c1 = "u";
    QString c2 = "v";
    QString c3 = oldExp;

    QString newExpLiteral = QString("(u, v)->vector{" + c1 + " , " + c2 + " , " + c3 + "}");

    m_evaluator.setExpression(Analitza::Expression(newExpLiteral, false));

    m_runStack.append(m_u);
    m_runStack.append(m_v);
    m_evaluator.setStack(m_runStack);

}

REGISTER_FUNCTION_3D(CartesianSurface)

