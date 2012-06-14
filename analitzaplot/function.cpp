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
, m_expression(f.m_expression)
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

Function::Function (const Analitza::Expression &functionExpression, const QString &name, const QColor& col)
: m_expression(functionExpression), m_name (name), m_color(col), m_graphPrecision(AveragePrecision), m_plotStyle(Solid)
{
    m_id = QUuid::createUuid().toString();
}

Function::~Function()
{
}

QStringList Function::arguments() const
{
    QStringList ret;

    foreach (const Analitza::Ci *var, m_expression.parameters())
        ret.append(var->name());

    return ret;
}


/////
Function2D::Function2D()
{

}
Function2D::Function2D (const Function2D &f) : Function (f)
{

}

Function2D::Function2D (const Analitza::Expression &functionExpression, const QString &name, const QColor &col, Analitza::Variables *variables) : Function (functionExpression, name, col)
{

}


Function2D::~Function2D()
{

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
//     if(&f!=this) {
//         delete m_function;
//
//         if(f.m_function) {
//             m_function=f.m_function->copy();
// //          m_function=copy(f.m_function);
//             Q_ASSERT(m_function);
//         } else
//             m_function=0;
//         m_expression=f.m_expression;
//         m_show=f.m_show;
//         m_pen=f.m_pen;
//         m_name=f.m_name;
//         m_err=f.m_err;
//     }
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




