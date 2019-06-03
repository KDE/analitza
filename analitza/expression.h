/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QStringList>
#include <QSharedDataPointer>

#include "analitzaexport.h"
#include "object.h"

namespace Analitza
{
class Ci;
class Cn;

/**
 * \class Expression
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Represents a mathematical expression.
 *
 * Expression let to convert it to string, MathML and make some little queries 
 * to it, without calculating anything.
 */

class ANALITZA_EXPORT Expression
{
    Q_GADGET
    public:
        typedef void (*CustomObjectDestructor)(const QVariant&);
        /**
         *    Constructs an empty Expression.
         */
        Expression();
        
        /**
         *    Copy constructor, copies the whole object to the constructed one.
         */
        Expression(const Expression& e);
        
        /**
         *    Creates an expression from a value
         */
        explicit Expression(const Cn& e);
        
        explicit Expression(Object* o);
        
        /**
         *    Constructor. Parses an expression and creates the object.
         *    @param exp expression to be assigned
         *    @param mathml format of the expression
         */
        explicit Expression(const QString& exp, bool mathml=false);
        
        /** Destructor */
        ~Expression();
        
        /**
         *    Sets an expression @p exp which is not in MathML format. Returns whether it was correctly assigned or not.
         */
        bool setText(const QString &exp);
        
        /**
         *    Sets an expression @p exp which is in MathML format. Returns whether it was correctly assigned or not.
         */
        bool setMathML(const QString &exp);
        
        /**
         *    Returns the list of errors that had experienced while building the expression.
         */
        QStringList error() const;
        
        /** Adds a new error @p error to the expression, to be reported afterwards. */
        void addError(const QString& error);
        
        /**
         *    Returns whether this is a correct expression.
         */
        bool isCorrect() const;
        
        /**
         *    Returns whether the @p e is equal.
         */
        bool operator==(const Expression& e) const;
        
        bool operator!=(const Expression& e) const;
        
        /**
         *    Copy assignment. Copies the @p e expression here.
         */
        Expression operator=(const Expression& e);
        
        /**
         *    Returns whether it is a lambda-expression.
         */
        bool isLambda() const;
        
        /**
         *    Returns the expression of the lambda body (without resolving the dependencies)
         */
        Expression lambdaBody() const /*Q_REQUIRED_RESULT*/;
        
        /**
         *    Returns whether it is a vector expression.
         */
        bool isVector() const;
        
        /**
         *    Returns whether it is a matrix expression.
         */
        bool isMatrix() const;
        
        /**
         *    Returns whether it is a list expression.
         */
        bool isList() const;
        
        /**
         *    Returns whether it is a string expression (a list of chars).
         */
        bool isString() const;
        
        /**
         *    Returns the element at @p position in a vector
         */
        Expression elementAt(int position) const;
        
        /**
         *    sets an expression value @p value to a @p position
         */
        void setElementAt(int position, const Analitza::Expression& exp);
        
        /**
         *    Returns the tree associated to this object.
         */
        const Object* tree() const;
        
        /**
         *    Returns the tree associated to this object.
         */
        Object* tree();

        /**
         *    Returns the tree associated to this object and clears the object,
         *    so the ownership of the tree is acquired by the caller.
         */
        Object* takeTree();
        
        void setTree(Object* o);
        /**
         *    Converts the expression to a string expression.
         */
        Q_SCRIPTABLE QString toString() const;
        
        /**
         *    Converts the expression to MathML.
         */
        QString toMathML() const;
        
        /**
         *    Exports the expression to HTML.
         */
        QString toHtml() const;
        
        /**
         *    Converts the expression to MathML Presentation Markup.
         */
        QString toMathMLPresentation() const;
        
        /** @returns the contained string value */
        QString stringValue() const;
        
        /**
         * Invalidates the data of the expression.
         */
        void clear();
        
        /**
         * @returns Lists the global bounded variables in the expression
         */
        QStringList bvarList() const;
        
        /** @returns Value representation of the expression. */
        Cn toReal() const;
        
        /** @returns true if the expression is a value, false otherwise. */
        bool isReal() const;
        
        /** @returns true if the expression is a custom object, false otherwise. */
        bool isCustomObject() const;
        
        /** @returns a list of the parameters in case this expression represents
            a lambda construction. */
        QList<Ci*> parameters() const;
        
        /** In case it was a vector or list, it returns a list of each expression on the vector. */
        QList<Expression> toExpressionList() const;
        
        /** In case it contains a custom object it returns its value. */
        QVariant customObjectValue() const;
        
        /** renames the @p depth -th variable into @p newName */
        void renameArgument(int depth, const QString& newName);
        
        /** @returns whether it's an equation */
        bool isEquation() const;
        
        /** @returns whether it's a declaration */
        bool isDeclaration() const;
        
        /** @returns the name of the expression. If it's not a declaration
         * then an empty string it's returned.
         */
        QString name() const;
        
        /** @returns the value of the declaration. If it's not a declaration
         * then an expression it's returned.
         */
        Expression declarationValue() const;
        
        /** @returns the expression that evaluates the current equation to equal 0 */
        Expression equationToFunction() const;
        
        QStringList comments() const;
        
        /**
         *    Converts a @p tag to an object type.
         */
        static enum Object::ObjectType whatType(const QString& tag);
        
        /**
         *    @returns whether @p s is MathML or not. Very simple.
         */
        static bool isMathML(const QString& s) { return !s.isEmpty() && s[0]=='<'; }
        
        static void computeDepth(Object* o);
        
        /**
         * @returns an expression containing a list of every expression passed on @p exps on the form:
         *    list { exps[0], exps[1], ... }
         */
        static Expression constructList(const QList<Expression> & exps);
        
        /** creates an expression filled with just a custom object */
        static Expression constructCustomObject(const QVariant& custom, Analitza::Expression::CustomObjectDestructor d);
        
        /** creates an expression filled with just a string */
        static Expression constructString(const QString& str);
        
        /** @returns if a non-mathml expression is fully introduced (e.g. has closed all parentheses).
            @param exp the expression to analyze
            @param justempty tells if it's an expression with just spaces and comments
         */
        static bool isCompleteExpression(const QString& exp, bool justempty=false);
    private:
        class ExpressionPrivate;
        QSharedDataPointer<ExpressionPrivate> d;
        QStringList m_comments;
};

}

Q_DECLARE_METATYPE(Analitza::Expression)

#endif
