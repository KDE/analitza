/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef EXPRESSIONTYPECHECKER_H
#define EXPRESSIONTYPECHECKER_H

#include "abstractexpressionvisitor.h"
#include "analitzaexport.h"
#include "expressiontype.h"
#include <QStack>
#include <QStringList>
#include <QSet>

namespace Analitza
{
class Variables;
class Expression;

class ANALITZA_EXPORT ExpressionTypeChecker : public AbstractExpressionVisitor
{
    public:
        explicit ExpressionTypeChecker(Variables* v);
        
        ExpressionType check(const Expression& exp);
        
        virtual QVariant visit(const None* var) override;
        virtual QVariant visit(const Operator* var) override;
        virtual QVariant visit(const Ci* var) override;
        virtual QVariant visit(const Cn* var) override;
        virtual QVariant visit(const Container* var) override;
        virtual QVariant visit(const Vector* var) override;
        virtual QVariant visit(const List* l) override;
        virtual QVariant visit(const Matrix* c) override;
        virtual QVariant visit(const Analitza::MatrixRow* m) override;
        virtual QVariant visit(const Apply* a) override;
        virtual QVariant visit(const CustomObject* c) override;
        
        virtual QVariant result() const override { return QVariant(); }
        
        QStringList dependencies() const { return m_deps; }
        bool hasDependencies() const { return !m_deps.isEmpty(); }
        bool isCorrect() const { return m_err.isEmpty() && !current.isError(); }
        QStringList errors() const;
        
        void initializeVars(const QMap<QString, ExpressionType>& types);
        QMap<QString, ExpressionType> variablesTypes() const;
        
    private:
        ExpressionType tellTypeIdentity(const QString& name, const ExpressionType& type);
        ExpressionType solve(const Operator* o, const QVector<Object*>& parameters);
        bool inferType(const ExpressionType& c, const ExpressionType& targetType, QMap<QString, ExpressionType>* assumptions);
        QList<ExpressionType> computePairs(const QList<ExpressionType>& options, const ExpressionType& param);
        
        QMap<QString, ExpressionType> typeIs(const Object* o, const ExpressionType& type);
        template <class T>
            QMap<QString, ExpressionType> typeIs(T it, const T& itEnd, const ExpressionType& type);
            
        template <class T>
            QVariant visitListOrVector(const T* v, ExpressionType::Type t, int size);
        
        ExpressionType typeForVar(const QString& var);
        
        void addError(const QString& err);
        ExpressionType commonType(const QList<Object*>& values);
        bool isVariableDefined(const QString& id) const;
        
        uint m_stars;
        QList<QStringList> m_err;
        QStringList m_calculating;
        ExpressionType current;
        Variables* m_v;
        QMap<QString, ExpressionType> m_typeForBVar;
        QMap<QString, ExpressionType> m_vars;
        QSet<QString> m_lambdascope;
        QStack<const Object*> m_calls;
        QStringList m_deps;
};

}

#endif // EXPRESSIONTYPECHECKER_H
