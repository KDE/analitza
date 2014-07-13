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

#ifndef ANALITZAJIT_VALUECOMPILER_H
#define ANALITZAJIT_VALUECOMPILER_H

#include <QVector>

#include "abstractexpressionvisitor.h"

namespace Analitza {
class Variables;
}

namespace llvm {
class Module;
}

//TODO this class is just a copy of stringexpressionwriter ... it need to be modified 
namespace Analitza
{

/**
 * \class LLVMIRExpressionWriter
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This class represents the LLVM IR expression writer.//TODO better doc
 * this class will be used only when the expression is a valid lambda and will
 * generate the IR code when we have a call of the lambda.
 * 
 */

class TypeCompiler : public Analitza::AbstractExpressionVisitor
{
	public:
		TypeCompiler(const Analitza::Object* o, llvm::Module *mod, const QVector<Analitza::Object*>& stack, Analitza::Variables* v = 0);
		
		virtual QVariant visit(const Analitza::None* var);
		virtual QVariant visit(const Analitza::Ci* var);
		virtual QVariant visit(const Analitza::Cn* val);
		virtual QVariant visit(const Analitza::Container* c);
		virtual QVariant visit(const Analitza::Operator* var);
		virtual QVariant visit(const Analitza::Vector* var);
		virtual QVariant visit(const Analitza::List* l);
		virtual QVariant visit(const Analitza::Matrix* m);
		virtual QVariant visit(const Analitza::MatrixRow* mr);
		virtual QVariant visit(const Analitza::Apply* a);
		virtual QVariant visit(const Analitza::CustomObject* c);
		
		QVariant result() const { return m_result; }
	private:
		QVariant m_result;
		QVector<Analitza::Object*> m_runStack;
		llvm::Module *m_mod;
};

}
#endif // ANALITZAJIT_VALUECOMPILER_H
