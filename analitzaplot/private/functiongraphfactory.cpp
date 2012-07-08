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

FunctionGraphFactory* FunctionGraphFactory::m_self=0;

QString FunctionGraphFactory::typeName(const QString& id) const
{
    return typeNameFunctions[id]();
}

Analitza::ExpressionType FunctionGraphFactory::expressionType(const QString& id) const
{
    return expressionTypeFunctions[id]();
}

int FunctionGraphFactory::spaceDimension(const Analitza::ExpressionType& etype) const
{
    Analitza::ExpressionType ftype = etype;
    
    int dim = -1;

    if (ftype.type() == Analitza::ExpressionType::Many)
    {
        //buscar solo las funciones de variable/parametro real
        //evito los casos "(<num,-1> -> <num,-1> -> <num,-1>...)" y solo acepto casos
        //"(num -> num -> num...)" 
        foreach (const Analitza::ExpressionType &exptype, ftype.alternatives())
            if (exptype.parameters().first().type() == Analitza::ExpressionType::Value)
            {
                ftype = exptype;
                break;
            }
    }

    switch (ftype.returnValue().type())
    {
        // implicit function
        case Analitza::ExpressionType::Bool: //last bool means = operator is used (not <,>,...)
        {
            switch (ftype.parameters().size())
            {
                case 3: dim = 2; break; // implicit curve
                case 4: dim = 3; break; // implicit surf
            }

            break;
        }
        
        // vector valued function
        case Analitza::ExpressionType::Vector:
        {
            switch (ftype.parameters().last().anyValue())
            {
                case 2: dim = 2; break; // param curve
                case 3: dim = 3; break; // param surf
            }
            
            break;
        }
        
        // real valued function
        case Analitza::ExpressionType::Value:
        {
            switch (ftype.parameters().size())
            {
                case 2: dim = 2; break; // f(x)
                case 3: dim = 3; break; // f(x,y)
            }

            break;
        }
    }

    return dim;
}

CoordinateSystem FunctionGraphFactory::coordinateSystem(const QString& id) const
{
    return coordinateSystemFunctions[id]();
}

QString FunctionGraphFactory::iconName(const QString& id) const
{
    return iconNameFunctions[id]();
}

QStringList FunctionGraphFactory::examples(const QString& id) const
{
    return examplesFunctions[id]();
}

FunctionGraphFactory* FunctionGraphFactory::self()
{
    if(!m_self)
        m_self=new FunctionGraphFactory;
    return m_self;
}

bool FunctionGraphFactory::registerFunctionGraph(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, 
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
    int dim = spaceDimension(expressionTypeFunction());

//     Q_ASSERT(!contains(id(argumentsFunction(), dim)));
    Q_ASSERT(expressionTypeFunction().type() == Analitza::ExpressionType::Lambda);
    
    QString id = QString::number(dim)+"|"+
                 QString::number((int)coordinateSystemFunction())+"|"+
                 argumentsFunction().join(",");

    typeNameFunctions[id] = typeNameFunction;
    expressionTypeFunctions[id] = expressionTypeFunction;
    spaceDimensions[id] = dim;
    coordinateSystemFunctions[id] = coordinateSystemFunction;
    argumentsFunctions[id] = argumentsFunction;
    iconNameFunctions[id] = iconNameFunction;
    examplesFunctions[id] = examplesFunction;

    builderFunctionsWithVars[id] = builderFunctionWithVars;
    builderFunctionsWithoutVars[id] = builderFunctionWithoutVars;

    return true;
}

QString FunctionGraphFactory::trait(const Analitza::Expression &expression, int dim) const
{
    Analitza::Analyzer a;
    a.setExpression(expression);
    
    QStringList args = expression.bvarList();

    QString key;
    
    bool found = false;
    
    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
    {
        if (argumentsFunctions.values()[i]() == args && 
            dim == spaceDimensions.values()[i])
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);

            found = true;
            
            break;
        }
    }

    if (found)
        return QString::number(spaceDimensions[key])+"|"+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");

    return QString();    
}

bool FunctionGraphFactory::contains(const QString& id) const
{
    return builderFunctionsWithVars.contains(id);
}

AbstractFunctionGraph* FunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctionsWithVars[id](exp, v);
}

AbstractFunctionGraph* FunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp) const
{
    return builderFunctionsWithoutVars[id](exp);
}
