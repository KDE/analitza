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

#include "analitza/expressiontype.h"

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

QString AbstractFunctionGraphFactory::registerFunctionGraphDefs(TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));

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

    return id;
}

///

PlaneCurveFactory* PlaneCurveFactory::m_self=0;

PlaneCurveFactory* PlaneCurveFactory::self()
{
    if(!m_self)
        m_self=new PlaneCurveFactory;
    return m_self;
}

bool PlaneCurveFactory::registerPlaneCurve(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));

    QString id = registerFunctionGraphDefs(typeNameFunction,expressionTypeFunction, spaceDimensionFunction,
                                           coordinateSystemFunction, argumentsFunction, iconNameFunction, examplesFunction);

    builderFunctions[id] = builderFunction;

    return true;
}

QString PlaneCurveFactory::id(const QStringList& args) const
{
    QString key;

    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
        if (argumentsFunctions.values()[i]() == args)
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);

            break;
        }

    return QString("2|")+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");
}

bool PlaneCurveFactory::contains(const QString& id) const
{
    return builderFunctions.contains(id);
}

AbstractPlaneCurve* PlaneCurveFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctions[id](exp, v);
}





///


SurfaceFactory* SurfaceFactory::m_self=0;

SurfaceFactory* SurfaceFactory::self()
{
    if(!m_self)
        m_self=new SurfaceFactory;
    return m_self;
}

bool SurfaceFactory::registerSurface(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
                                     ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
                                     CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                                     IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//             Q_ASSERT(!contains(argumentsFunction()));

    QString id = registerFunctionGraphDefs(typeNameFunction,expressionTypeFunction, spaceDimensionFunction,
                                           coordinateSystemFunction, argumentsFunction, iconNameFunction, examplesFunction);

    builderFunctions[id] = builderFunction;

    return true;

}


QString SurfaceFactory::id(const QStringList& args, CoordinateSystem coordsys) const
{
    QString key;

    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
        if ((argumentsFunctions.values()[i]() == args) &&
                (coordinateSystemFunctions.values()[i]() == coordsys))
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);

            break;
        }

    return QString("3|")+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");
}

bool SurfaceFactory::contains(const QString& id) const
{
    return builderFunctions.contains(id);
}

AbstractSurface* SurfaceFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctions[id](exp, coordinateSystem(id) ,v);
}



