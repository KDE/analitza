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

#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variable.h"

FunctionImpl::FunctionImpl(const Analitza::Expression& e, Analitza::Variables* v)
{
    analyzer.setExpression(e);
    analyzer.simplify();
    analyzer.flushErrors();

    foreach (const Analitza::Ci *var, analyzer.expression().parameters())
        m_argumentValues[var->name()] = new Analitza::Cn;

    analyzer.setStack(m_argumentValues.values().toVector());
}

FunctionImpl::FunctionImpl(const FunctionImpl& fi)
    : analyzer(fi.analyzer.variables())
{
    analyzer.setExpression(fi.analyzer.expression());
    m_graphPrecision = fi.m_graphPrecision;
}

FunctionImpl::~FunctionImpl()
{
    qDeleteAll(m_argumentValues.begin(), m_argumentValues.end());
    m_argumentValues.clear();
}

///

FunctionImpl2D::FunctionImpl2D(const Analitza::Expression& e, Analitza::Variables* v)
: FunctionImpl(e, v)
{

}

FunctionImpl2D::FunctionImpl2D(const FunctionImpl2D& fi)
: FunctionImpl(fi)
{

}

FunctionImpl2D::~FunctionImpl2D()
{

}

QStringList FunctionImpl2D::errors() const
{
    return QStringList();
}

FunctionImpl *FunctionImpl2D::copy()
{
    return 0;
}





