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
#include <plottingenums.h>

#include <QPair>
#include <QMap>

#define REGISTER_FUNCTIONGRAPH_DIM(dim, name) \
        static AbstractFunctionGraph * vcreate##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionGraphFactory::self()->registerFunctionGraph(dim, vcreate##name, \
        name ::TypeName(), name ::ExpressionType, name ::CoordSystem(), name ::Parameters(), \
        name ::IconName(), name ::Examples); }

        
#define REGISTER_PLANECURVE(...) REGISTER_FUNCTIONGRAPH_DIM(Dim2D, __VA_ARGS__)
#define REGISTER_SPACECURVE(...) REGISTER_FUNCTIONGRAPH_DIM(Dim3D, __VA_ARGS__)
#define REGISTER_SURFACE(...) REGISTER_FUNCTIONGRAPH_DIM(Dim3D, __VA_ARGS__)
        
class AbstractFunctionGraph;

namespace Analitza {
    class Variables;
    class Expression;
    class ExpressionType;
}

class FunctionGraphFactory
{
public:
    typedef AbstractFunctionGraph* (*BuilderFunctionWithVars)(const Analitza::Expression&, Analitza::Variables* );
    
    typedef Analitza::ExpressionType (*ExpressionTypeFunction)();
    typedef QStringList (*ExamplesFunction)();

    QString typeName(const QString& id) const;
    Analitza::ExpressionType expressionType(const QString& id) const;
    //swap args 
    Dimension spaceDimension(const QString& id) const;
    // falla cuando hay multiples cassos ...mult expressio
    Dimension spaceDimension(const Analitza::ExpressionType& ftype, const QStringList &bvars) const;
    CoordinateSystem coordinateSystem(const QString& id) const;
    QString iconName(const QString& id) const;
    QStringList examples(const QString& id) const;
    
    static FunctionGraphFactory* self();

    bool registerFunctionGraph(Dimension dim, BuilderFunctionWithVars builderFunctionWithVars,
                  const QString& typeNameFunction, ExpressionTypeFunction expressionTypeFunction, 
                         CoordinateSystem coordinateSystemFunction, const QStringList& argumentsFunction,
                         const QString& iconNameFunction, ExamplesFunction examplesFunction);
    QString trait(const Analitza::Expression& expr, const Analitza::ExpressionType& t, Dimension dim) const;
    bool contains(const QString &id) const;
    
    AbstractFunctionGraph * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;

    QMap< QString, QPair< QStringList, Analitza::ExpressionType > > registeredFunctionGraphs() const;

private:
    QMap<QString, QString> typeNameFunctions;
    QMap<QString, ExpressionTypeFunction> expressionTypeFunctions;
    QMap<QString, Dimension> spaceDimensions; //internal use (without a "getter")
    QMap<QString, CoordinateSystem> coordinateSystemFunctions;
    QMap<QString, QStringList> argumentsFunctions; //internal use (without a "getter")
    QMap<QString, QString> iconNameFunctions;
    QMap<QString, ExamplesFunction> examplesFunctions;

    static FunctionGraphFactory* m_self;
    FunctionGraphFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunctionWithVars> builderFunctionsWithVars;
};


#endif // ABSTRACTFUNCTIONGRAPHFACTORY_H
