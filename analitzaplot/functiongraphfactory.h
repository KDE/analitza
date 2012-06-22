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

class ANALITZAPLOT_EXPORT AbstractFunctionGraphFactory
{
public:
    typedef QString (*TypeNameFunction)();
    typedef Analitza::ExpressionType (*ExpressionTypeFunction)();
    typedef CoordinateSystem (*CoordinateSystemFunction)();
    typedef QStringList (*ArgumentsFunction)();
    typedef QString (*IconNameFunction)();
    typedef QStringList (*ExamplesFunction)();

    QString typeName(const QString& id) const;
    Analitza::ExpressionType expressionType(const QString& id) const;
    int spaceDimension(const QString& id) const;
    CoordinateSystem coordinateSystem(const QString& id) const;
    QStringList arguments(const QString& id);
    QString iconName(const QString& id) const;
    QStringList examples(const QString& id) const;
    
    virtual bool contains(const QString &id) const = 0;

protected:
    QString registerFunctionGraphDefs(TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, int spaceDimension,
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);    
    
    
    QMap<QString, TypeNameFunction> typeNameFunctions;
    QMap<QString, ExpressionTypeFunction> expressionTypeFunctions;
    QMap<QString, int> spaceDimensions;
    QMap<QString, CoordinateSystemFunction> coordinateSystemFunctions;
    QMap<QString, ArgumentsFunction> argumentsFunctions;
    QMap<QString, IconNameFunction> iconNameFunctions;
    QMap<QString, ExamplesFunction> examplesFunctions;
};


#define REGISTER_PLANECURVE(name) \
        static AbstractPlaneCurve * create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=PlaneCurveFactory::self()->registerPlaneCurve(create##name, \
        name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Arguments, \
        name ::IconName, name ::Examples); }

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


class ANALITZAPLOT_EXPORT PlaneCurveFactory : public AbstractFunctionGraphFactory
{
public:
    typedef AbstractPlaneCurve* (*BuilderFunction)(const Analitza::Expression&, Analitza::Variables* );


    static PlaneCurveFactory* self();

    bool registerPlaneCurve(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, 
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
    QString id(const QStringList& args) const;
    bool contains(const QString &id) const;
    AbstractPlaneCurve * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;

private:
    static PlaneCurveFactory* m_self;
    PlaneCurveFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunction> builderFunctions;
};


///

#define REGISTER_SURFACE(name) \
        static AbstractSurface * create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=SurfaceFactory::self()->registerSurface(create##name, \
        name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Arguments, \
        name ::IconName, name ::Examples); }

class AbstractSurface;

class ANALITZAPLOT_EXPORT SurfaceFactory : public AbstractFunctionGraphFactory
{
public:
    typedef AbstractSurface* (*BuilderFunction)(const Analitza::Expression&, Analitza::Variables* );

    static SurfaceFactory* self();

    bool registerSurface(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, 
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
    QString id(const QStringList& args, CoordinateSystem coordsys) const;
    bool contains(const QString &id) const;
    AbstractSurface * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;

private:
    static SurfaceFactory* m_self;
    SurfaceFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunction> builderFunctions;
};


#endif // ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
