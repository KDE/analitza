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



#ifndef ABSTRACTFUNCTIONGRAPHFACTORY_H
#define ABSTRACTFUNCTIONGRAPHFACTORY_H

#include "analitzaplotexport.h"

#include "utils/mathutils.h"

#include "analitza/expressiontype.h"

#define REGISTER_FUNCTIONGRAPH(name) \
        static AbstractFunctionGraph * vcreate##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionGraphFactory::self()->registerFunctionGraph(vcreate##name, \
        name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
        name ::IconName, name ::Examples); }

        
#define REGISTER_PLANECURVE REGISTER_FUNCTIONGRAPH
#define REGISTER_SPACECURVE REGISTER_FUNCTIONGRAPH
#define REGISTER_SURFACE REGISTER_FUNCTIONGRAPH
        
class AbstractFunctionGraph;

class FunctionGraphFactory
{
public:
    typedef AbstractFunctionGraph* (*BuilderFunctionWithVars)(const Analitza::Expression&, Analitza::Variables* );
    
    typedef QString (*TypeNameFunction)();
    typedef Analitza::ExpressionType (*ExpressionTypeFunction)();
    typedef CoordinateSystem (*CoordinateSystemFunction)();
    typedef QStringList (*ArgumentsFunction)();
    typedef QString (*IconNameFunction)();
    typedef QStringList (*ExamplesFunction)();

    QString typeName(const QString& id) const;
    Analitza::ExpressionType expressionType(const QString& id) const;
    //swap args 
    int spaceDimension(const Analitza::ExpressionType& ftype, const QStringList &bvars) const;
    CoordinateSystem coordinateSystem(const QString& id) const;
    QString iconName(const QString& id) const;
    QStringList examples(const QString& id) const;
    
    static FunctionGraphFactory* self();

    bool registerFunctionGraph(BuilderFunctionWithVars builderFunctionWithVars,
                  TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, 
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
    QString trait(const Analitza::Expression &expr, int dim) const;
    bool contains(const QString &id) const;
    
    AbstractFunctionGraph * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;

    QMap< QString, QPair< QStringList, Analitza::ExpressionType > > registeredFunctionGraphs() const;

private:
    QMap<QString, TypeNameFunction> typeNameFunctions;
    QMap<QString, ExpressionTypeFunction> expressionTypeFunctions;
    QMap<QString, int> spaceDimensions; //internal use (without a "getter")
    QMap<QString, CoordinateSystemFunction> coordinateSystemFunctions;
    QMap<QString, ArgumentsFunction> argumentsFunctions; //internal use (without a "getter")
    QMap<QString, IconNameFunction> iconNameFunctions;
    QMap<QString, ExamplesFunction> examplesFunctions;

    static FunctionGraphFactory* m_self;
    FunctionGraphFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunctionWithVars> builderFunctionsWithVars;
};


#endif // ABSTRACTFUNCTIONGRAPHFACTORY_H
