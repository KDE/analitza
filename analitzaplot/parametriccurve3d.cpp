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


#include "functionimpl.h"
#include "functionfactory.h"

#include "analitza/value.h"
#include <analitza/vector.h>


class ANALITZAPLOT_EXPORT ParametricCurve3D : public FunctionImpl3D
{
public:
    explicit ParametricCurve3D(const Analitza::Expression &expression, Analitza::Variables *variables);
    ParametricCurve3D(const ParametricCurve3D &ParametricCurve3D);
    virtual ~ParametricCurve3D();

    static QStringList supportedBVars()
    {
        return QStringList() << "t";
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Vector,
                                            Analitza::ExpressionType(Analitza::ExpressionType::Value), 3));




    }
    static QStringList examples()
    {
        QStringList ret;


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
    void solve(const RealInterval::List &spaceBounds);
    FunctionImpl * copy()
    {
        return new ParametricCurve3D(*this);
    }
    QVector3D evalCurve(qreal t);

    QVector3D evalSurface(qreal u, qreal v)
    {
        return QVector3D();
    }

    void loadParametricEvaluator();
    virtual bool isCurve() const
    {
        return true;
    }

protected:
    Analitza::Cn* m_t;

};

ParametricCurve3D::ParametricCurve3D(const Analitza::Expression &expression, Analitza::Variables *variables)
    : FunctionImpl3D(expression, variables)
    , m_t(new Analitza::Cn)
{





    loadParametricEvaluator();




    if(m_evaluator.isCorrect())
    {






        m_evaluator.flushErrors();
    }
}

ParametricCurve3D::ParametricCurve3D(const ParametricCurve3D &ParametricCurve3D)
    : FunctionImpl3D(ParametricCurve3D)
    , m_t(new Analitza::Cn)
{

    loadParametricEvaluator();





}

void ParametricCurve3D::loadParametricEvaluator()

{
    m_runStack.append(m_t);
    m_evaluator.setStack(m_runStack);
}

ParametricCurve3D::~ParametricCurve3D()
{
    delete m_t;
}




void ParametricCurve3D::solve(const RealInterval::List &spaceBounds)
{


}

QVector3D ParametricCurve3D::evalCurve(qreal t)
{
    m_t->setValue(t);

    Analitza::Expression res = m_evaluator.calculateLambda();


    return QVector3D(res.elementAt(0).toReal().value(),
                     res.elementAt(1).toReal().value(),
                     res.elementAt(2).toReal().value());

}

REGISTER_FUNCTION_3D(ParametricCurve3D)



