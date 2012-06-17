/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#ifndef ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
#define ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H

#include <QVector>
#include <QString>
#include <QMap>
#include <QStringList>
#include "analitza/analyzer.h"

#include "functionutils.h"
extern Analitza::Analyzer a;
namespace Analitza
{
class Expression;
class ExpressionType;
class Variables;
}

#define REGISTER_PLANECURVE(name) \
        static AbstractPlaneCurve * create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=PlaneCurveFactory::self()->registerPlaneCurve(create##name, \
        name ::TypeName, name ::ExpressionType, name ::SpaceDimension, name ::CoordSystem, name ::Arguments, \
        name ::IconName, name ::Examples, name ::IsImplicit, name ::IsParametric); }


class AbstractPlaneCurve;




// plugins must expose this static members

// QString TypeName 
// Analitza::ExpressionType ExpressionType
// int SpaceDimension
// CoordinateSystem CoordinateSystem
// QStringList Arguments
// QString IconName
// QStringList Examples
// bool IsImplicit()
// bool IsParametric()

class ANALITZAPLOT_EXPORT PlaneCurveFactory
{
    public:
        typedef AbstractPlaneCurve* (*BuilderFunction)(const Analitza::Expression&, Analitza::Variables* );
        
        typedef QString (*TypeNameFunction)();
        typedef Analitza::ExpressionType (*ExpressionTypeFunction)();
        typedef int (*SpaceDimensionFunction)();
        typedef CoordinateSystem (*CoordinateSystemFunction)();
        typedef QStringList (*ArgumentsFunction)();
        typedef QString (*IconNameFunction)();
        typedef QStringList (*ExamplesFunction)();
        typedef bool (*IsImplicitFunction)();
        typedef bool (*IsParametricFunction)();

        static PlaneCurveFactory* self();
        
        bool registerPlaneCurve(BuilderFunction builderFunction, TypeNameFunction typeNameFunction, 
                              ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
                              CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                              IconNameFunction iconNameFunction, ExamplesFunction examplesFunction,
                              IsImplicitFunction isImplicitFunction, IsParametricFunction isParametricFunction);
        
        bool contains(const QStringList& arguments) const;

        AbstractPlaneCurve * build(const QStringList& arguments, const Analitza::Expression& exp, Analitza::Variables* v) const;
        
        QString typeName(const QStringList& arguments) const;
        Analitza::ExpressionType expressionType(const QStringList& arguments) const;
        int spaceDimension(const QStringList& arguments) const;
        CoordinateSystem coordinateSystem(const QStringList& arguments) const;
        QString iconName(const QStringList& arguments) const;
        QStringList examples(const QStringList& arguments) const;
        bool isImplicit(const QStringList& arguments) const;
        bool isParametric(const QStringList& arguments) const;

    private:
        static PlaneCurveFactory* m_self;
        PlaneCurveFactory() { Q_ASSERT(!m_self); m_self = this; }
        
        QMap<QString, BuilderFunction> m_builderFunctions;

        QMap<QString, TypeNameFunction> m_typeNameFunctions;
        QMap<QString, ExpressionTypeFunction> m_expressionTypeFunctions;
        QMap<QString, SpaceDimensionFunction> m_spaceDimensionFunctions;
        QMap<QString, CoordinateSystemFunction> m_coordinateSystemFunctions;
        QMap<QString, IconNameFunction> m_iconNameFunctions;
        QMap<QString, ExamplesFunction> m_examplesFunctions;
        QMap<QString, IsImplicitFunction> m_IsImplicitFunctions;
        QMap<QString, IsParametricFunction> m_IsParametricFunctions;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
