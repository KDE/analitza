/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef LLVMIREXPRESSIONWRITER_H
#define LLVMIREXPRESSIONWRITER_H

#include "abstractexpressionvisitor.h"
#include "operator.h"

//TODO this class is just a copy of stringexpressionwriter ... it need to be modified 
namespace Analitza
{

/**
 * \class LLVMIRExpressionWriter
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This class represents the LLVM IR expression writer.//TODO better doc
 */

class LLVMIRExpressionWriter : public AbstractExpressionVisitor
{
	public:
		LLVMIRExpressionWriter(const Object* o);
		
		virtual QVariant visit(const None* var);
		virtual QVariant visit(const Ci* var);
		virtual QVariant visit(const Cn* var);
		virtual QVariant visit(const Container* var);
		virtual QVariant visit(const Operator* var);
		virtual QVariant visit(const Vector* var);
		virtual QVariant visit(const List* l);
		virtual QVariant visit(const Matrix* m);
		virtual QVariant visit(const MatrixRow* mr);
		virtual QVariant visit(const Apply* a);
		virtual QVariant visit(const CustomObject* c);
		
		QVariant result() const { return m_result; }
		
		static int weight(const Analitza::Operator* op, int size, int pos);
		static const QMap<Operator::OperatorType, QString> s_operators;
	private:
		template <class T>
			static QStringList allValues(T it, const T& itEnd, AbstractExpressionVisitor* writer);

		QVariant m_result;
};

}
#endif // LLVMIREXPRESSIONWRITER_H
