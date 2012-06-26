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


#include "analitza/expressiontype.h"

///
#include "planecurvefactory.h"


PlaneCurveFactory* PlaneCurveFactory::m_self=0;

PlaneCurveFactory* PlaneCurveFactory::self()
{
    if(!m_self)
        m_self=new PlaneCurveFactory;
    return m_self;
}

bool PlaneCurveFactory::registerPlaneCurve(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, 
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));

    QString id = registerFunctionGraphDefs(typeNameFunction,expressionTypeFunction, 2,
                                           coordinateSystemFunction, argumentsFunction, iconNameFunction, examplesFunction);

    builderFunctions[id] = builderFunction;

    return true;
}

QString PlaneCurveFactory::id(const QStringList& args) const
{
    QString key;
    
    bool found = false;
    
    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
        if (argumentsFunctions.values()[i]() == args)
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);
            found = true;
            break;
        }

        
    if (found)
        return QString("2|")+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");

    return QString();    
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

