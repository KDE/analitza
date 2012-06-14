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

FunctionImpl::FunctionImpl(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
             Analitza::Variables *variables, bool isImplicit)
{
    m_isImplicit = isImplicit;
    m_analyzer.setExpression(functionExpression);
    m_analyzer.simplify();
    m_analyzer.flushErrors();

    for (int i = 0; i < m_analyzer.expression().parameters().size(); ++i)
        m_runStack.append(new Analitza::Cn);

    m_analyzer.setStack (m_runStack);
}

FunctionImpl::FunctionImpl(const FunctionImpl& fi)
    : m_analyzer(fi.m_analyzer.variables()), m_isImplicit(fi.m_isImplicit)
{
//  Q_ASSERT(fi.isCorrect());
    m_analyzer.setExpression(fi.m_analyzer.expression());
}

FunctionImpl::~FunctionImpl()
{
//    delete data();
}
