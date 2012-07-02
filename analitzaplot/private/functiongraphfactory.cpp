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


#include "functiongraphfactory.h"

AbstractFunctionGraphFactory* AbstractFunctionGraphFactory::m_self=0;

QString AbstractFunctionGraphFactory::typeName(const QString& id) const
{
    return typeNameFunctions[id]();
}

Analitza::ExpressionType AbstractFunctionGraphFactory::expressionType(const QString& id) const
{
    return expressionTypeFunctions[id]();
}

int AbstractFunctionGraphFactory::spaceDimension(const QString& id) const
{
    return spaceDimensionFunctions[id]();
}

CoordinateSystem AbstractFunctionGraphFactory::coordinateSystem(const QString& id) const
{
    return coordinateSystemFunctions[id]();
}

QStringList AbstractFunctionGraphFactory::arguments(const QString& id)
{
    return argumentsFunctions[id]();
}

QString AbstractFunctionGraphFactory::iconName(const QString& id) const
{
    return iconNameFunctions[id]();
}

QStringList AbstractFunctionGraphFactory::examples(const QString& id) const
{
    return examplesFunctions[id]();
}

AbstractFunctionGraphFactory* AbstractFunctionGraphFactory::self()
{
    if(!m_self)
        m_self=new AbstractFunctionGraphFactory;
    return m_self;
}

bool AbstractFunctionGraphFactory::registerFunctionGraph(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));
    
    Q_ASSERT(expressionTypeFunction().type() == Analitza::ExpressionType::Lambda);
    
    QString id = QString::number(spaceDimensionFunction())+"|"+
                 QString::number((int)coordinateSystemFunction())+"|"+
                 argumentsFunction().join(",");

    typeNameFunctions[id] = typeNameFunction;
    expressionTypeFunctions[id] = expressionTypeFunction;
    spaceDimensionFunctions[id] = spaceDimensionFunction;
    coordinateSystemFunctions[id] = coordinateSystemFunction;
    argumentsFunctions[id] = argumentsFunction;
    iconNameFunctions[id] = iconNameFunction;
    examplesFunctions[id] = examplesFunction;

    builderFunctionsWithVars[id] = builderFunctionWithVars;
    builderFunctionsWithoutVars[id] = builderFunctionWithoutVars;

    return true;
}

QString AbstractFunctionGraphFactory::id(const QStringList& args, int spaceDimension) const
{
    QString key;
    
    bool found = false;
    
    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
    {
        if (argumentsFunctions.values()[i]() == args && 
            spaceDimension == spaceDimensionFunctions.values()[i]())
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);

            found = true;
            
            break;
        }
    }

    if (found)
        return QString::number(spaceDimensionFunctions[key]())+"|"+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");

    return QString();    
}

bool AbstractFunctionGraphFactory::contains(const QString& id) const
{
    return builderFunctionsWithVars.contains(id);
}

AbstractFunctionGraph* AbstractFunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctionsWithVars[id](exp, v);
}

AbstractFunctionGraph* AbstractFunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp) const
{
    return builderFunctionsWithoutVars[id](exp);
}