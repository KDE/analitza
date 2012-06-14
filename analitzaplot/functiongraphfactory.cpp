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

#include "functiongraphfactory.h"

FunctionFactory *FunctionFactory::m_self = 0;

bool FunctionFactory::registerFunction2D (registerFunction2D_fn functionGraphConstructor,
        const Analitza::ExpressionType &expressionType, const QStringList &varnames,
        CoordinateSystem coordinateSystem,
        bool hasImplicitExpression, const QStringList &fexamples, const QString &iconName)
{
    //Q_ASSERT(!contains(bvars));


    constructors.append(functionGraphConstructor);
    expressionTypes.append(expressionType);
    argumentNames.append(varnames);
    coordinateSystems.append (coordinateSystem);
    implicitFlags.append (hasImplicitExpression);
    examples.append (fexamples);
    iconNames.append(iconName);

    return true;
}

FunctionImpl2D * FunctionFactory::createFunction2D (int functionGraphIndex, const Analitza::Expression &functionExpression, Analitza::Variables *variables) const
{
    return constructors[functionGraphIndex](functionExpression, variables);
}

FunctionFactory *FunctionFactory::self()
{
	if(!m_self)
		m_self=new FunctionFactory;
	return m_self;
}
