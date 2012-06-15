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

Function::Function()
: m_color(Qt::black), m_graphPrecision(AveragePrecision), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

Function::Function(const Function &f)
: m_id(f.m_id)
, m_isImplicit(f.m_isImplicit)
, m_name(f.m_name)
, m_iconName(f.m_iconName)
, m_color(f.m_color)
, m_examples(f.m_examples)
, m_argumentIntervals(f.m_argumentIntervals)
, m_type(f.m_type)
, m_graphDimension(f.m_graphDimension)
, m_coordinateSystem(f.m_coordinateSystem)
, m_graphPrecision(f.m_graphPrecision)
, m_plotStyle(f.m_plotStyle)
, m_graphVisible(f.m_graphVisible)
{
}

Function::Function (const QString &name, const QColor& col)
: m_name (name), m_color(col), m_graphPrecision(AveragePrecision), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

Function::~Function()
{
}



/////
Function2D::Function2D()
: Function()
{

}
Function2D::Function2D (const Function2D &f)
: Function (f)
{

}

Function2D::Function2D (const Analitza::Expression &functionExpression, const QString &name, const QColor &col, Analitza::Variables *v)
: Function (name, col)
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
    if(!Function2DFactory::self()->contains(bvars))
        m_errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        m_errors << a.errors();
    else {
        Analitza::ExpressionType expected=Function2DFactory::self()->type(bvars);
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
            m_function=Function2DFactory::self()->item(bvars, m_expression, v);
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
}

Function2D::~Function2D()
{
    delete m_function;
}

const Analitza::Expression &Function2D::expression() const
{
//     return m_function->
return Analitza::Expression();
}

bool Function2D::isImplicit() const
{
    //TODO gsoc

//     return m_function->
    return false;
}

QStringList Function2D::arguments() const
{
    QStringList ret;

    foreach (const Analitza::Ci *var, m_expression.parameters())
        ret.append(var->name());

    return ret;
}

void Function2D::setGraphPrecision (FunctionGraphPrecision precs)
{
    m_function->setGraphPrecision(precs);
}

QStringList Function2D::errors() const
{
return QStringList();
}


bool Function2D::isCorrect() const
{
    return false;
}


Function2D Function2D::operator= (const Function2D &f)
{
    if(&f!=this) {
        delete m_function;
        
        if(f.m_function) {
            m_function=dynamic_cast<FunctionImpl2D*>( f.m_function->copy() );
//          m_function=copy(f.m_function);
            Q_ASSERT(m_function);
        } else
            m_function=0;
        m_expression=f.m_expression;
        setGraphVisible(f.isGraphVisible());
        setColor(f.color());
        setName(f.name());
        setId(f.id());
        m_errors=f.m_errors;
    }
    return *this;
}

//functionimpl iface


const QVector< QPointF >& Function2D::points() const
{
return QVector< QPointF >();
}

QList< int > Function2D::jumps() const
{
return QList< int >();
}

QLineF Function2D::derivative (const QPointF &p) const
{
return QLineF();
}
QPair< QPointF, QString > Function2D::calc (const QPointF &dp)
{
return qMakePair(QPointF(), QString());
}




