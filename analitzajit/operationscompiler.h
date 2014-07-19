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

#ifndef OPERATIONSCOMPILER_H
#define OPERATIONSCOMPILER_H

#include "operator.h"

namespace llvm {
class Value;
}

namespace Analitza
{

	//TODO Better docs
	//In case the input arer not valid the compilation will fail and we will return 0 or null poitr
	//in case of valid input code then it will generate IR code
class OperationsCompiler
{
	public:
		typedef llvm::Value* (*UnaryOp)(Operator::OperatorType op, llvm::Value* val, QString& error);
		typedef llvm::Value* (*BinaryOp)(Operator::OperatorType op, llvm::Value* val1, llvm::Value* val2, QString& error);
		
		static llvm::Value* compileUnaryOperation(Operator::OperatorType op, llvm::Value* val, QString& error);
		static llvm::Value* compileBinaryOperation(Operator::OperatorType op,llvm::Value* val1, llvm::Value* val2, QString& error);
		
	private:
		static UnaryOp opsUnary[Object::custom+1];
		static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
		
		static llvm::Value* compileValueNoneOperation(Operator::OperatorType op, llvm::Value* val, llvm::Value* none, QString &error);
		static llvm::Value* compileNoneValueOperation(Operator::OperatorType op, llvm::Value* none, llvm::Value* val, QString &error);
		static llvm::Value* compileValueValueOperation(Operator::OperatorType op, llvm::Value* val1, const llvm::Value* val2, QString &error);
		static llvm::Value* compileUnaryValueOperation(Operator::OperatorType op, llvm::Value* val, QString &error);
		
		static llvm::Value* compileValueVectorOperation(Operator::OperatorType op, llvm::Value* val, llvm::Value* vec, QString &error);
		static llvm::Value* compileVectorValueOperation(Operator::OperatorType op, llvm::Value* vec, llvm::Value* val, QString &error);
		static llvm::Value* compileVectorVectorOperation(Operator::OperatorType op, llvm::Value* vec1, llvm::Value* vec2, QString &error);
		static llvm::Value* compileMatrixVectorOperation(Operator::OperatorType op, llvm::Value* matrix, llvm::Value* vector, QString &error);
		static llvm::Value* compileUnaryVectorOperation(Operator::OperatorType op, llvm::Value* c, QString &error);
		
		static llvm::Value* compileValueListOperation(Operator::OperatorType op, llvm::Value* , llvm::Value* , QString &error);
		static llvm::Value* compileListListOperation(Operator::OperatorType op, llvm::Value* l1, llvm::Value* l2, QString &error);
		static llvm::Value* compileUnaryListOperation(Operator::OperatorType op, llvm::Value* , QString &error);
		
		static llvm::Value* compileValueMatrixOperation(Analitza::Operator::OperatorType op, llvm::Value* v, llvm::Value* m1, QString &error);
		static llvm::Value* compileMatrixValueOperation(Analitza::Operator::OperatorType op, llvm::Value* m1, llvm::Value* v, QString &error);
		static llvm::Value* compileVectorMatrixOperation(Operator::OperatorType op, llvm::Value* vector, llvm::Value* matrix, QString &error);
		static llvm::Value* compileMatrixMatrixOperation(Operator::OperatorType op, llvm::Value* m1, llvm::Value* m2, QString &error);
		static llvm::Value* compileUnaryMatrixOperation(Analitza::Operator::OperatorType op, llvm::Value* m, QString &error);
		static llvm::Value* compileMatrixNoneOperation(Operator::OperatorType op, llvm::Value* m, llvm::Value* cntr, QString &error);
		static llvm::Value* compileNoneMatrixOperation(Operator::OperatorType op, llvm::Value* cntr, llvm::Value* m, QString &error);
		
		static llvm::Value* compileCustomCustomOperation(Operator::OperatorType op, llvm::Value* v1, llvm::Value* v2, QString &error);
		
		static llvm::Value* errorCase(const QString& err, QString &error);
};

}
#endif // OPERATIONSCOMPILER_H
