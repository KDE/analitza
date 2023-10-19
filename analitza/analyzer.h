/*************************************************************************************
 *  Copyright (C) 2007-2010 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef ANALYZER_H
#define ANALYZER_H

#include <QStringList>
#include <QSharedPointer>
#include <QStack>

#include "expression.h"
#include "analitzaexport.h"
#include "expressiontype.h"
#include "builtinmethods.h"
#include <analitza/analitzautils.h>

namespace Analitza
{
class Polynomial;
class Apply;
class BoundingIterator;
class BuiltinMethods;
class Object;
class Variables;
class Container;
class Operator;
class Ci;

/**
 * \class Analyzer
 * 
 * \ingroup AnalitzaModule
 *
 * \brief The parser: Evaluates and calculates math expressions.
 *
 * Is the base Math class, Analyzer can evaluate simple expressions, expressions 
 * that contains any number of variables and lambda expressions.
 * 
 * If Analyzer is created with a custom Variables module, then calculate() or evaluate() 
 * will be aware of the variables inside the module.
 */

class ANALITZA_EXPORT Analyzer
{
    public:
        /** Constructor. Creates an empty Analyzer module with a Variables module. */
        Analyzer();
        
        /** Constructor. Creates an empty Analyzer module. 
            @param v: Sets a custom variables module. This module will _not_ be deleted along with Analyzer
        */
        explicit Analyzer(Variables* v);
        explicit Analyzer(const QSharedPointer<Variables> &v);
        
        /** Copy constructor. Creates a copy of the @p a Analyzer instance. Inherits its Variable structure. */
        Analyzer(const Analyzer& a);
        
        /** Destructor. */
        ~Analyzer();

        /** sets the @p v Variables tuple we are going to use */
        void setVariables(const QSharedPointer<Variables> &v);
        
        /** Sets an expression to calculate. */
        void setExpression(const Expression &e);
        
        /** Returns the expression in display. */
        const Expression& expression() const { return m_exp; }
        
        /** Calculates the expression and returns a value alone. */
        Expression calculate();
        
        /**
         * Calculates the expression and returns a value alone.
         * The parameters need to be set by passing a stack instance
         */
        Expression calculateLambda();
        
        /** Evaluates an expression, like calculate() but returns a tree. */
        Expression evaluate();
        
        /** Evaluates the derivative of an expression expression, like expression() but sorrounded with a diff(). */
        Expression derivative(const QString& var);
        
        /** Evaluates the derivative of an expression expression. */
        double derivative(const QList<Object *> &values);

        /** Returns whether there has been a problem in the last calculation. */
        bool isCorrect() const { return m_err.isEmpty() && m_exp.isCorrect(); }
        
        /** Empties the error list. */
        void flushErrors() { m_err.clear(); }
        
        /** simplifies the expression. */
        void simplify(); //FIXME: Should return an Expression
        
        /** @returns Return an error list. */
        QStringList errors() const { return m_exp.error() + m_err; }
        
        /** @returns Returns a way to query variables. */
        QSharedPointer<Variables> variables() const { return m_vars; }
        
        /**
            Adds a variable entry. It is the proper way to do it because tracks some possible errors.
            May change the error in case we're trying to represent something wrong.
            @returns Returns if it was actually inserted.
        */
        bool insertVariable(const QString& name, const Expression& value);
        
        /**
            Adds a variable entry. It is the proper way to do it because tracks some possible errors.
            May change the error in case we're trying to represent something wrong.
            @returns Returns if it was actually inserted.
        */
        bool insertVariable(const QString& name, const Object* value);
        
        /**
            Adds a variable entry named @p name with @p value value.
            @returns Returns the added object
        */
        Cn* insertValueVariable(const QString& name, double value);
        
        /** Returns whether the current expression has all data it needs to be calculated.*/
        bool hasDependencies() const { return m_hasdeps; }
        
        /** This method is useful if you want to work programatically on functions with undefined variables.
            @returns the same expression set but with explicit dependencies.
            
            e.g. x+2 would return x->x+2
        */
        Expression dependenciesToLambda() const;
        
        /** This method lets you retrieve the current type in use.
         @returns the type of the current expression.
         */
        ExpressionType type() const { return m_currentType; }

        void setStack(const QList<Object *> &stack) { m_runStack = stack; }

        QList<Object *> runStack() const { return m_runStack; }

        BuiltinMethods* builtinMethods();
        
        /** Makes it possible to easily enter a bunch of code to execute it */
        void importScript(QTextStream* stream);
        
        /** @returns the type for any variable that depends on the last executed procedure */
        QMap<QString, ExpressionType> variableTypes() const { return m_variablesTypes; }
    private:
        typedef Object* (Analyzer::*funcContainer)(const Container*);
        static funcContainer operateContainer[];
        
        Expression m_exp;
        QSharedPointer<Variables> m_vars;
        QStringList m_err;
        QList<Object *> m_runStack;
        int m_runStackTop;
        BuiltinMethods m_builtin;
        
        bool m_hasdeps;
        ExpressionType m_currentType;
        QMap<QString, ExpressionType> m_variablesTypes;
        
        void registerBuiltinMethods(); //util to be called in each ctr
        
        Object* calc(const Object* e);
        Object* operate(const Container*);
        Object* operate(const Apply*);
        Object* eval(const Object* e, bool vars, const QSet<QString>& unscoped);
        
        Object* sum(const Apply& c);
        Object* product(const Apply& c);
        Object* exists(const Apply& c);
        Object* forall(const Apply& c);
        Object* func(const Apply& c);
        Object* calcDiff(const Apply* c);
        Object* calcMap(const Apply* c);
        Object* calcFilter(const Apply* c);
        
        Object* calcPiecewise(const Container* c);
        Object* calcDeclare(const Container* c);
        Object* calcMath(const Container* c);
        Object* calcLambda(const Container* c);
        Object *calcCallFunction(Analitza::Container *function,
                                 const QList<Analitza::Object *> &args,
                                 const Analitza::Object *op);

        Object* simp(Object* root);
        Object* simpPolynomials(Apply* c);
        Object* simpSum(Apply* c);
        Object* simpApply(Apply* c);
        Object* simpPiecewise(Container* c);
        
        QList<Object*> findRoots(const QString& dep, const Analitza::Object* o);
        QList<Object*> findRootsApply(const QString& dep, const Analitza::Apply* a);
        
        Object* derivative(const QString &var, const Object*);
        Object* boundedOperation(const Apply & n, const Operator & t, Object* initial);
        
        BoundingIterator* initializeBVars(const Apply* n, int base);
        BoundingIterator* initBVarsContainer(const Apply* n, int base, Object* domain);
        BoundingIterator* initBVarsRange(const Apply* n, int base, Object* dlimit, Object* ulimit);
        
        template <class T, class Tcontained = Object>
        void iterateAndSimp(T* v);
        
        Object* variableValue(Ci* var);
        Object* testResult(const Analitza::Object* o, const QString& var, const Analitza::Object* val);
        
        template <class T, class Tcontained = Object>
        void alphaConversion(T* o, int min);
        void alphaConversion(Apply* a, int min);
        void alphaConversion(Container* a, int min);
        Object* applyAlpha(Analitza::Object* o, int min);
        
        template<class T, class Tcontained = Object>
        Object* calcElements(const Analitza::Object* root, T* nv);

        template<class T, class Tcontained = Object>
        Object* evalElements(const Analitza::Object* root, T* nv, bool resolve, const QSet<QString>& unscoped);
};

}
#endif
