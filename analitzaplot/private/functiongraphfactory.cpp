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
#include "abstractfunctiongraph.h"

#include <QStringList>
#include <QCoreApplication>
#include <analitza/analyzer.h>

using namespace Analitza;

FunctionGraphFactory* FunctionGraphFactory::m_self=nullptr;

QString FunctionGraphFactory::typeName(const QString& id) const
{
    return QCoreApplication::tr(typeNameFunctions[id]);
}

Analitza::ExpressionType FunctionGraphFactory::expressionType(const QString& id) const
{
    return expressionTypeFunctions[id]();
}

Dimension FunctionGraphFactory::spaceDimension(const QString& id) const
{
    return spaceDimensions[id];
}

CoordinateSystem FunctionGraphFactory::coordinateSystem(const QString& id) const
{
    return coordinateSystemFunctions[id];
}

QString FunctionGraphFactory::iconName(const QString& id) const
{
    return iconNameFunctions[id];
}

QStringList FunctionGraphFactory::examples(const QString& id) const
{
    return examplesFunctions[id]();
}

QStringList FunctionGraphFactory::examples(Dimension dim) const
{
    QStringList ret;
    QStringList ids = spaceDimensions.keys(dim);
    foreach(const QString& id, ids) {
        ret += examplesFunctions[id]();
    }
    return ret;
}

FunctionGraphFactory* FunctionGraphFactory::self()
{
    if(!m_self)
        m_self=new FunctionGraphFactory;
    return m_self;
}

bool FunctionGraphFactory::registerFunctionGraph(Dimension dim, PlotItemConstuctor constructor,
                                                 BuilderFunctionWithVars builderFunctionWithVars, const char* typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, 
        CoordinateSystem coordinateSystemFunction, const QStringList& _arguments,
        const QString& iconNameFunction, ExamplesFunction examplesFunction)
{
    QStringList arguments(_arguments);
    std::sort(arguments.begin(), arguments.end());

    //TODO: turn this id into an internal struct
    QString id = QString::number((int)dim)+'|'+
                 QString::number((int)coordinateSystemFunction)+'|'+
                 arguments.join(QStringLiteral(","));
    Q_ASSERT(!contains(id)); // verificar que no se registren los mismos tipos

    typeNameFunctions[id] = typeNameFunction;
    expressionTypeFunctions[id] = expressionTypeFunction;
    spaceDimensions[id] = dim;
    coordinateSystemFunctions[id] = coordinateSystemFunction;
    argumentsFunctions[id] = arguments;
    iconNameFunctions[id] = iconNameFunction;
    examplesFunctions[id] = examplesFunction;
    builderFunctionsWithVars[id] = builderFunctionWithVars;
    plotConstructor[id] = constructor;

    return true;
}

QString FunctionGraphFactory::trait(const Analitza::Expression& expr, const Analitza::ExpressionType& t, Dimension dim) const
{
    Q_ASSERT(!expr.isEquation());
    QStringList args = expr.bvarList();
    std::sort(args.begin(), args.end());

    QString key;
    for (int i = 0; i < argumentsFunctions.values().size() && key.isEmpty(); ++i) {
//         qDebug() << "---" << args << dim << t.toString() << " || " << argumentsFunctions.values()[i] << spaceDimensions.values()[i] << expressionTypeFunctions.values()[i]().toString();
        if (args == argumentsFunctions.values()[i]
            && dim == spaceDimensions.values()[i]
            && t.canReduceTo(expressionTypeFunctions.values()[i]()))
        {
            key = typeNameFunctions.key(typeNameFunctions.values()[i]);
        }
    }
    
    return key;
}

bool FunctionGraphFactory::contains(const QString& id) const
{
    return builderFunctionsWithVars.contains(id);
}

AbstractFunctionGraph* FunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp, const QSharedPointer<Analitza::Variables>& v) const
{
    Q_ASSERT(builderFunctionsWithVars.contains(id));
    AbstractFunctionGraph* ret = builderFunctionsWithVars.value(id)(exp, v);
    Q_ASSERT(ret);
    ret->setInternalId(id);
    return ret;
}

FunctionGraph* FunctionGraphFactory::buildItem(const QString& id, const Analitza::Expression& exp, const QSharedPointer<Analitza::Variables>& v) const
{
    return plotConstructor[id](build(id, exp, v));
}

QMap< QString, QPair< QStringList, Analitza::ExpressionType > > FunctionGraphFactory::registeredFunctionGraphs() const
{
    QMap< QString, QPair< QStringList, Analitza::ExpressionType > > ret;
    
    for (int i = 0; i < typeNameFunctions.values().size(); ++i)
        ret[typeNameFunctions.values()[i]] = qMakePair( argumentsFunctions.values()[i],
            expressionTypeFunctions.values()[i]()); 

    return ret;
}
