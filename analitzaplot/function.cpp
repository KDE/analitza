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


#include "function.h"

#include <KDE/KLocalizedString>
#include <QPen>
#include <QUuid>

#include "analitza/analyzer.h"
#include <analitza/value.h>
#include <analitza/vector.h>
#include <analitza/variable.h>

#include "functiongraph.h"
#include "functiongraphfactory.h"

MappingGraph::MappingGraph()
: m_color(Qt::black), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

MappingGraph::MappingGraph(const MappingGraph &f)
: m_id(f.m_id)
, m_name(f.m_name)
, m_iconName(f.m_iconName)
, m_color(f.m_color)
, m_examples(f.m_examples)
, m_coordinateSystem(f.m_coordinateSystem)
, m_plotStyle(f.m_plotStyle)
, m_graphVisible(f.m_graphVisible)
{
}

MappingGraph::MappingGraph(const QString &name, const QColor& col)
: m_name (name), m_color(col), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

MappingGraph::~MappingGraph()
{
}



/////

template<typename VectorType>
Curve<VectorType>::Curve()
: FunctionGraph()
{

}
template<typename VectorType>
Curve<VectorType>::Curve(const Curve &f)
: FunctionGraph(f)
{

}

template<typename VectorType>
Curve<VectorType>::Curve(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
: FunctionGraph(name, col)
{
    if(!functionExpression.isCorrect()) {
        m_errors << i18n("The expression is not correct");
        return;
    }

    Analitza::Analyzer a(v);
    a.setExpression(functionExpression);

    m_expression = a.dependenciesToLambda();
    a.setExpression(m_expression);

    QStringList bvars = m_expression.bvarList();

    //TODO: turn into assertion
    if(!CurveFactory::self()->contains(bvars))
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=CurveFactory::self()->type(bvars);
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
            m_curve=CurveFactory::self()->item(bvars, m_expression, v);
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
}

template<typename VectorType>
Curve<VectorType>::~Curve()
{
    delete m_curve;
}

template<typename VectorType>
const Analitza::Expression & Curve<VectorType>::expression() const
{
//     return m_curve->
return Analitza::Expression();
}

template<typename VectorType>
QStringList Curve<VectorType>::arguments() const
{
    QStringList ret;

    foreach (const Analitza::Ci *var, m_expression.parameters())
        ret.append(var->name());

    return ret;
}

template<typename VectorType>
DrawingPrecision Curve<VectorType>::drawingPrecision()
{
    return m_curve->drawingPrecision();
}

template<typename VectorType>
void Curve<VectorType>::setDrawingPrecision(DrawingPrecision precision)
{
    m_curve->setDrawingPrecision(precision);
}

template<typename VectorType>
QStringList Curve<VectorType>::errors() const
{
    QStringList err(m_errors);
    if(m_curve) {
        err += m_curve->errors();
    }
    return err;
}

template<typename VectorType>
bool Curve<VectorType>::isCorrect() const
{
    return m_curve && m_errors.isEmpty() && m_curve->isCorrect();
}


template<typename VectorType>
const QVector< VectorType > & Curve<VectorType>::points() const
{
    Q_ASSERT(m_curve);
    Q_ASSERT(m_curve->points().size()>1);
    return m_curve->points();
}
template<typename VectorType>
QList< int > Curve<VectorType>::jumps() const
{
return m_curve->jumps();
}
template<typename VectorType>
LineSegment<VectorType> Curve<VectorType>::derivative(const VectorType &p) const
{
    Q_ASSERT(m_curve);
    return m_curve->derivative(p);
}
template<typename VectorType>
QPair< VectorType, QString > Curve<VectorType>::calc(const VectorType &dp)
{
    Q_ASSERT(m_curve);
    return m_curve->calc(dp);
}
template<typename VectorType>
Curve<VectorType> Curve<VectorType>::operator= (const Curve<VectorType>& f)
{
    if(&f!=this) {
        delete m_curve;
        
        if(f.m_curve) {
            m_curve=dynamic_cast<AbstractCurve*>( f.m_curve->copy() );
//          m_curve=copy(f.m_curve);
            Q_ASSERT(m_curve);
        } else
            m_curve=0;
        m_expression=f.m_expression;
        setGraphVisible(f.isGraphVisible());
        setColor(f.color());
        setName(f.name());
        setId(f.id());
        m_errors=f.m_errors;
    }
    return *this;
}



