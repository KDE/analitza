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

#ifndef ANALITZAJIT_OPERATIONSCOMPILER_H
#define ANALITZAJIT_OPERATIONSCOMPILER_H

#include "operator.h"

namespace llvm {
class Value;
class BasicBlock;
};

namespace Analitza
{
class None;
class List;
class Cn;
class Vector;
class Matrix;
class CustomObject;

/**
 * \class OperationsCompiler
 * 
 * \ingroup AnalitzaJITModule
 * 
 * \brief Compiles analitza operation into a valid LLVM instruction.
 * 
 * OperationsCompiler converts any Analitza operation into LLVM instruction and inserts it 
 * into a current LLVM basic block.
 * 
 * Every basic block needs to exist inside a LLVM function and every LLVM function 
 * needs to belong the the same LLVM module.
 * 
 * Finally, in case the input is invalid or could not perform the compilation, then 
 * every method will return a null pointer.
 * 
 * \internal Implementation details: If we don't use the use same module across compilers 
 * and methods then we can get this error:
 * 'Referencing function in another module!'
 * also if we don't use the current basic block (where we are inserting the instruction) then 
 * we will get this error:
 * 'Instruction does not dominate all uses!'
 * that means the instruction was not inserted into a right basic block context.
 */

class OperationsCompiler
{
	public:
// 		OperationsCompiler(llvm::BasicBlock *currentBlock);
		
// 		llvm::BasicBlock *currentBlock() const { return m_ current }
		
		typedef llvm::Value* (*UnaryOp)(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object* refval, llvm::Value* val, QString& error);
		typedef llvm::Value* (*BinaryOp)(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object* refval1, Object* refval2, llvm::Value* val1, llvm::Value* val2, QString& error);
		
		static llvm::Value* compileUnaryOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object* refval, llvm::Value* val, QString& error);
		static llvm::Value* compileBinaryOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object* refval1, Object* refval2, llvm::Value* val1, llvm::Value* val2, QString& error);
		
	private:
		//TODO we need to choice based on llvm::Valye::type instead of Analitza type 
		//this we could avoid use analitza::type1 for a llvm::value that is not (avoid inconsistence)
		//currently we¿ll cast all types to double (analitza value)
		static UnaryOp opsUnary[Object::custom+1];
		static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
		
		static llvm::Value* compileValueNoneOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Cn* refval1, None* refval2, llvm::Value* val, llvm::Value* none, QString &error);
		static llvm::Value* compileNoneValueOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, None* refval1, Cn* refval2, llvm::Value* none, llvm::Value* val, QString &error);
		static llvm::Value* compileValueValueOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Cn* refval1, Cn* refval2, llvm::Value* val1, llvm::Value* val2, QString &error);
		static llvm::Value* compileUnaryValueOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Cn* refval, llvm::Value* val, QString &error);
		
		static llvm::Value* compileValueVectorOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Cn* refval1, Vector* refval2, llvm::Value* val, llvm::Value* vec, QString &error);
		static llvm::Value* compileVectorValueOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Vector* refval1, Cn* refval2, llvm::Value* vec, llvm::Value* val, QString &error);
		static llvm::Value* compileVectorVectorOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Vector* refval1, Vector* refval2, llvm::Value* vec1, llvm::Value* vec2, QString &error);
		static llvm::Value* compileMatrixVectorOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Matrix* refval1, Vector* refval2, llvm::Value* matrix, llvm::Value* vector, QString &error);
		static llvm::Value* compileUnaryVectorOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Vector* refval, llvm::Value* vec, QString &error);
		
		static llvm::Value* compileValueListOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Cn* refval1, List* refval2, llvm::Value* , llvm::Value* , QString &error);
		static llvm::Value* compileListListOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, List* refval1, List* refval2, llvm::Value* l1, llvm::Value* l2, QString &error);
		static llvm::Value* compileUnaryListOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, List* refval, llvm::Value* , QString &error);
		
		static llvm::Value* compileValueMatrixOperation(llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, Cn* refval1, Matrix* refval2, llvm::Value* v, llvm::Value* m1, QString &error);
		static llvm::Value* compileMatrixValueOperation(llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, Matrix* refval1, Cn* refval2, llvm::Value* m1, llvm::Value* v, QString &error);
		static llvm::Value* compileVectorMatrixOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Vector* refval1, Matrix* refval2, llvm::Value* vector, llvm::Value* matrix, QString &error);
		static llvm::Value* compileMatrixMatrixOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Matrix* refval1, Matrix* refval2, llvm::Value* m1, llvm::Value* m2, QString &error);
		static llvm::Value* compileUnaryMatrixOperation(llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, Matrix* refval, llvm::Value* m, QString &error);
		static llvm::Value* compileMatrixNoneOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Matrix* refval1, None* refval2, llvm::Value* m, llvm::Value* cntr, QString &error);
		static llvm::Value* compileNoneMatrixOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, None* refval1, Matrix* refval2, llvm::Value* cntr, llvm::Value* m, QString &error);
		
		static llvm::Value* compileCustomCustomOperation(llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object* refval1, Object* refval2, llvm::Value* v1, llvm::Value* v2, QString &error);
		
		static llvm::Value* errorCase(const QString& err, QString &error);
};

}

#endif // ANALITZAJIT_OPERATIONSCOMPILER_H
