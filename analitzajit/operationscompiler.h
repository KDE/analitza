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
#include <expressiontype.h>

namespace llvm {
class Value;
}

namespace Analitza
{

class None;

class List;
class Cn;
class Vector;
class Matrix;
class CustomObject;
class ExpressionType;

class OperationsCompiler
{
	public:
		typedef llvm::Value* (*UnaryOp)(Operator::OperatorType op, Object* val, QString& error);
		typedef llvm::Value* (*BinaryOp)(Operator::OperatorType op, Object* val1, Object* val2, QString& error);
		
		static llvm::Value* reduce(Operator::OperatorType op, Object* oper, Object* oper1, QString& error);
		static llvm::Value* reduceUnary(Operator::OperatorType op, Object* oper, QString& error);
		
	private:
		static UnaryOp opsUnary[Object::custom+1];
		static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
		
		static llvm::Value* reduceValueNone(Operator::OperatorType op, Cn* oper, None *cntr, QString &error);
		static llvm::Value* reduceNoneValue(Operator::OperatorType op, None *cntr, Cn* oper, QString &error);
		static llvm::Value* reduceValueValue(Operator::OperatorType op, Cn *oper, const Cn* oper1, QString &error);
		static llvm::Value* reduceUnaryValue(Operator::OperatorType op, Cn *oper, QString &error);
		
		static llvm::Value* reduceValueVector(Operator::OperatorType op, Cn *oper, Vector* vector, QString &error);
		static llvm::Value* reduceVectorValue(Operator::OperatorType op, Vector* vector, Cn *oper, QString &error);
		static llvm::Value* reduceVectorVector(Operator::OperatorType op, Vector* v1, Vector* v2, QString &error);
		static llvm::Value* reduceMatrixVector(Operator::OperatorType op, Matrix* matrix, Vector* vector, QString &error);
		static llvm::Value* reduceUnaryVector(Operator::OperatorType op, Vector* c, QString &error);
		
		static llvm::Value* reduceValueList(Operator::OperatorType op, Cn *oper, List* vector, QString &error);
		static llvm::Value* reduceListList(Operator::OperatorType op, List* l1, List* l2, QString &error);
		static llvm::Value* reduceUnaryList(Operator::OperatorType op, List* l, QString &error);
		
		static llvm::Value* reduceValueMatrix(Analitza::Operator::OperatorType op, Analitza::Cn* v, Analitza::Matrix* m1, QString &error);
		static llvm::Value* reduceMatrixValue(Analitza::Operator::OperatorType op, Analitza::Matrix* m1, Analitza::Cn* v, QString &error);
		static llvm::Value* reduceVectorMatrix(Operator::OperatorType op, Vector* vector, Matrix* matrix, QString &error);
		static llvm::Value* reduceMatrixMatrix(Operator::OperatorType op, Matrix* m1, Matrix* m2, QString &error);
		static llvm::Value* reduceUnaryMatrix(Analitza::Operator::OperatorType op, Analitza::Matrix* m, QString &error);
		static llvm::Value* reduceMatrixNone(Operator::OperatorType op, Matrix* m, None *cntr, QString &error);
		static llvm::Value* reduceNoneMatrix(Operator::OperatorType op, None *cntr, Matrix* m, QString &error);
		
		static llvm::Value* reduceCustomCustom(Operator::OperatorType op, CustomObject* v1, CustomObject* v2, QString &error);
		
		static llvm::Value* errorCase(const QString& err, QString &error);
};

}
#endif // OPERATIONSCOMPILER_H
