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

#include "functiongraph.h"

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>

#include "private/abstractplanecurve.h"
#include "functiongraphfactory.h"



///

///




FunctionGraph::FunctionGraph(const Analitza::Expression &functionExpression, int spaceDimension, const QString &name, const QColor &col, Analitza::Variables *variables)
    : PlotItem(name, col), m_functionGraph(0)
{
    m_errors.clear();

    
    //NOTE GSOC see functionExpression.isLambda ask for
    //se comenta || !functionExpression.isLambda() pues ahora hay expresiones que son ecuaciones: como las superficies implicitas
    if(!functionExpression.isCorrect() )
    {
        m_errors << i18n("The expression is not correct");
//         return false;
    }
    
    Analitza::Analyzer *a = 0;
    
    if (variables)
        a = new Analitza::Analyzer(variables);
    else
        a = new Analitza::Analyzer;


    a->setExpression(functionExpression);
    if (functionExpression.isEquation())
    {
        a->setExpression(a->expression().equationToFunction());
        a->setExpression(a->dependenciesToLambda());
    }
    else
        if (functionExpression.isLambda())
            a->setExpression(a->dependenciesToLambda());
        else
        {
            //pues no es ni ecuacion ni lambda
                    m_errors << i18n("The expression is not correct");

                    
                    
                    
        }
        
    Q_ASSERT(m_errors.isEmpty());
    
    
    
    
    
    QStringList bvars = a->expression().bvarList();

    
    if (functionExpression.isEquation())
        a->setExpression(functionExpression);
    
    
    //TODO: turn into assertion
    QString id = FunctionGraphFactory::self()->trait(a->expression(), spaceDimension);

    
    
    if(!FunctionGraphFactory::self()->contains(id))
        m_errors << i18n("Function type not recognized");
    else if(!a->isCorrect())
        m_errors << a->errors();
    else {
        Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(id);
        Analitza::ExpressionType actual=a->type();

        if(actual.canReduceTo(expected)) {

            delete m_functionGraph;

            m_functionGraph=static_cast<AbstractFunctionGraph*>(FunctionGraphFactory::self()->build(id,a->expression()));
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
    
    
    delete a;
    
//     return m_errors.empty();
    
}

FunctionGraph::~FunctionGraph()
{
    delete m_functionGraph;
}


Analitza::Variables* FunctionGraph::variables() const
{
    return m_functionGraph->variables();
}

void FunctionGraph::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    m_functionGraph->setVariables(variables);
}

const QString FunctionGraph::typeName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->typeName();
}

const Analitza::Expression & FunctionGraph::expression() const
{
    Q_ASSERT(m_functionGraph);
    
//     return m_functionGraph->
    return m_functionGraph->expression();
}

QString FunctionGraph::iconName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->iconName();
}

QStringList FunctionGraph::examples() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->examples();
}

int FunctionGraph::spaceDimension() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->spaceDimension();
}

CoordinateSystem FunctionGraph::coordinateSystem() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->coordinateSystem();
}

QStringList FunctionGraph::errors() const
{
    Q_ASSERT(m_functionGraph);
    
    QStringList err(m_errors);
    if(m_functionGraph) {
        err += m_functionGraph->errors();
    }
    return err;
}

bool FunctionGraph::isCorrect() const
{
    Q_ASSERT(m_functionGraph);

    return m_errors.isEmpty() && m_functionGraph->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> FunctionGraph::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->interval(argname, evaluate);
}

bool FunctionGraph::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_functionGraph);
    
    bool ret = m_functionGraph->setInterval(argname, min, max);
    
    if (ret)
        emitDataChanged();
    
    return ret;
}

QPair<double, double> FunctionGraph::interval(const QString &argname) const
{
    Q_ASSERT(m_functionGraph);

    return m_functionGraph->interval(argname);
}

bool FunctionGraph::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_functionGraph);
    
    bool ret = m_functionGraph->setInterval(argname, min, max);

    if (ret)
        emitDataChanged();
    
    return ret;
}

QStringList FunctionGraph::parameters() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->parameters();
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression, int spaceDimension)
{
//     QStringList errors;
// 
//     //NOTE GSOC see functionExpression.isLambda ask for
//     if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
//         errors << i18n("The expression is not correct");
//         return false;
//     }
// 
// 
//     Analitza::Analyzer a;
//     a.setExpression(functionExpression);
// //     a.setExpression(a.dependenciesToLambda());
// 
// 
//     
//     a.setExpression(functionExpression);
//     if (functionExpression.isEquation())
//     {
//         a.setExpression(a.expression().equationToFunction());
//         a.setExpression(a.dependenciesToLambda());
//     }
//     else
//         a.setExpression(a.dependenciesToLambda());
// 
//     QStringList bvars = a.expression().bvarList();
// 
//     
//     if (functionExpression.isEquation())
//         a.setExpression(functionExpression);
//     
//             
//             
//     //TODO: turn into assertion
//     if(!FunctionGraphFactory::self()->contains(FunctionGraphFactory::self()->trait(a.expression(), spaceDimension)))
//     {
//         
//         errors << i18n("Function type not recognized");
//     }
//     else if(!a.isCorrect())
//         errors << a.errors();
//     else {
//         Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(FunctionGraphFactory::self()->trait(a.expression(), spaceDimension));
//         Analitza::ExpressionType actual=a.type();
// 
//         if (actual.type() == Analitza::ExpressionType::Many)
//         {
//             //buscar solo las funciones de variable/parametro real
//             //evito los casos "(<num,-1> -> <num,-1> -> <num,-1>...)" y solo acepto casos
//             //"(num -> num -> num...)" 
//             foreach (const Analitza::ExpressionType &exptype, actual.alternatives())
//                 if (exptype.parameters().first().type() == Analitza::ExpressionType::Value)
//                 {
//                     actual = exptype;
//                     break;
//                 }
//         }
// 
//         if(actual.canReduceTo(expected) || actual.returnValue() == expected.returnValue()) {
// //             delete m_functionGraph;
// //             m_functionGraph=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
// 
// //                 qDebug() << FunctionGraphFactory::self()->typeName(FunctionGraphFactory::self()->trait(a.expression(), spaceDimension));
// 
//         } else
//             errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
//         
//     }
//     
// //     qDebug() << errors;
//     
//     return errors.empty();





    QStringList errors;

    
    //NOTE GSOC see functionExpression.isLambda ask for
    //se comenta || !functionExpression.isLambda() pues ahora hay expresiones que son ecuaciones: como las superficies implicitas
    if(!functionExpression.isCorrect() )
    {
        errors << i18n("The expression is not correct");
        return false;
    }
    
    Analitza::Analyzer *a = 0;
 
        a = new Analitza::Analyzer;

    a->setExpression(functionExpression);
    if (functionExpression.isEquation())
    {
        a->setExpression(a->expression().equationToFunction());
        a->setExpression(a->dependenciesToLambda());
    }
    else
        if (functionExpression.isLambda())
            a->setExpression(a->dependenciesToLambda());
        else
        {
            //pues no es ni ecuacion ni lambda
                    errors << i18n("The expression is not correct");
                    return false;

        }

    QStringList bvars = a->expression().bvarList();

    
    if (functionExpression.isEquation())
        a->setExpression(functionExpression);
    
    
    //TODO: turn into assertion
    QString id = FunctionGraphFactory::self()->trait(a->expression(), spaceDimension);

    
    
    if(!FunctionGraphFactory::self()->contains(id))
        errors << i18n("Function type not recognized");
    else if(!a->isCorrect())
        errors << a->errors();
    else {
        Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(id);
        Analitza::ExpressionType actual=a->type();

        if(actual.canReduceTo(expected)) {

//             delete m_functionGraph;
// 
//             m_functionGraph=static_cast<AbstractFunctionGraph*>(FunctionGraphFactory::self()->build(id,a->expression()));
            return true;
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
    
    
    delete a;
    
    return errors.isEmpty();
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression, int spaceDimension, QStringList &errors)
{
    Q_ASSERT(errors.isEmpty()); // el usuario deberia ingresar un lista vacia 
    
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a;
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    

    
    //TODO: turn into assertion
    if(!FunctionGraphFactory::self()->contains(FunctionGraphFactory::self()->trait(a.expression(), spaceDimension)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(FunctionGraphFactory::self()->trait(a.expression(), spaceDimension));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_functionGraph;
//             m_functionGraph=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool FunctionGraph::reset(const Analitza::Expression& functionExpression, int spaceDimension)
{
    m_errors.clear();

    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() || !functionExpression.isLambda())
    {
        m_errors << i18n("The expression is not correct");
        return false;
    }
    
    Analitza::Analyzer *a;
        a = new Analitza::Analyzer(m_functionGraph->variables());

    a->setExpression(functionExpression);
    a->setExpression(a->dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a->expression().parameters())
        bvars.append(arg->name());

    
    //TODO: turn into assertion
    if(!FunctionGraphFactory::self()->contains(FunctionGraphFactory::self()->trait(a->expression(), spaceDimension)))
        m_errors << i18n("Function type not recognized");
    else if(!a->isCorrect())
        m_errors << a->errors();
    else {
        
        
        QString id = FunctionGraphFactory::self()->trait(a->expression(), spaceDimension);
        Analitza::ExpressionType expected=FunctionGraphFactory::self()->expressionType(id);
        Analitza::ExpressionType actual=a->type();

        if(actual.canReduceTo(expected)) {

            delete m_functionGraph;
//TODO if vars
                m_functionGraph= static_cast<AbstractFunctionGraph*>(FunctionGraphFactory::self()->build(id,a->expression(), m_functionGraph->variables()));
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
    
    delete a;
    
    return m_errors.empty();
}
