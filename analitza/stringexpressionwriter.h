/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef STRINGEXPRESSIONWRITER_H
#define STRINGEXPRESSIONWRITER_H

#include "abstractexpressionvisitor.h"
#include "operator.h"

namespace Analitza
{

/**
 * \class StringExpressionWriter
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This class represents the string expression writer.
 */

class StringExpressionWriter : public AbstractExpressionVisitor
{
    public:
        explicit StringExpressionWriter(const Object* o);
        
        virtual QVariant visit(const None* var) override;
        virtual QVariant visit(const Ci* var) override;
        virtual QVariant visit(const Cn* var) override;
        virtual QVariant visit(const Container* var) override;
        virtual QVariant visit(const Operator* var) override;
        virtual QVariant visit(const Vector* var) override;
        virtual QVariant visit(const List* l) override;
        virtual QVariant visit(const Matrix* m) override;
        virtual QVariant visit(const MatrixRow* mr) override;
        virtual QVariant visit(const Apply* a) override;
        virtual QVariant visit(const CustomObject* c) override;
        
        QVariant result() const override { return m_result; }
        
        static int weight(const Analitza::Operator* op, int size, int pos);
        static const QMap<Operator::OperatorType, QString> s_operators;
    private:
        template <class T>
            static QStringList allValues(T it, const T& itEnd, AbstractExpressionVisitor* writer);

        QVariant m_result;
        static const double MIN_PRINTABLE_VALUE;
};

}
#endif
