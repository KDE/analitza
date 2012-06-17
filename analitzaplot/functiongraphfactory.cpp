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
                              IconNameFunction iconNameFunction, ExamplesFunction examplesFunction,
                              IsImplicitFunction isImplicitFunction, IsParametricFunction isParametricFunction)
{
    Q_ASSERT(!contains(argumentsFunction()));

    QString arguments = argumentsFunction().join("|");
    
    m_builderFunctions[arguments] = builderFunction;

    m_typeNameFunctions[arguments] = typeNameFunction;
    m_expressionTypeFunctions[arguments] = expressionTypeFunction;
    m_spaceDimensionFunctions[arguments] = spaceDimensionFunction;
    m_coordinateSystemFunctions[arguments] = coordinateSystemFunction;
    m_iconNameFunctions[arguments] = iconNameFunction;
    m_examplesFunctions[arguments] = examplesFunction;
    m_IsImplicitFunctions[arguments] = isImplicitFunction;
    m_IsParametricFunctions[arguments] = isParametricFunction;

    return true;
}

bool PlaneCurveFactory::contains(const QStringList& arguments) const
{
    return m_builderFunctions.contains(arguments.join("|"));
}

        AbstractPlaneCurve* PlaneCurveFactory::build(const QStringList& arguments, const Analitza::Expression& exp, Analitza::Variables* v) const
        {
           return m_builderFunctions[arguments.join("|")](exp, v);
        }
        
        QString PlaneCurveFactory::typeName(const QStringList& arguments) const
        {
            return m_typeNameFunctions[arguments.join("|")]();
        }
        
        Analitza::ExpressionType PlaneCurveFactory::expressionType(const QStringList& arguments) const
        {
            return m_expressionTypeFunctions[arguments.join("|")]();
        }
        
        int PlaneCurveFactory::spaceDimension(const QStringList& arguments) const
        {
            return m_spaceDimensionFunctions[arguments.join("|")]();
        }
        
        CoordinateSystem PlaneCurveFactory::coordinateSystem(const QStringList& arguments) const
        {
            return m_coordinateSystemFunctions[arguments.join("|")]();
        }
        
        QString PlaneCurveFactory::iconName(const QStringList& arguments) const
        {
            return m_iconNameFunctions[arguments.join("|")]();
        }
        
        QStringList PlaneCurveFactory::examples(const QStringList& arguments) const
        {
            return m_examplesFunctions[arguments.join("|")]();
        }
        
        bool PlaneCurveFactory::isImplicit(const QStringList& arguments) const
        {
            return m_IsImplicitFunctions[arguments.join("|")]();
        }
        
        bool PlaneCurveFactory::isParametric(const QStringList& arguments) const
        {
            return m_IsParametricFunctions[arguments.join("|")]();
        }
        



