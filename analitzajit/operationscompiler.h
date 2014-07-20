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
#include <llvm/IR/IRBuilder.h>
namespace llvm {
class Value;
class Module;
}



namespace Analitza
{

//WARNING
//NOTE  if we need use same module across compilers then we can get an error
//Referencing function in another module!
//also if we don't use the current block where we are inserting some instruction then we will get an error
//Instruction does not dominate all uses! 
//(the istructuion was not inserted into a right block context)

	//TODO Better docs
	//In case the input arer not valid the compilation will fail and we will return 0 or null poitr
	//in case of valid input code then it will generate IR code

//TODO this class insert a analitza operation (i.e. an llvm instruction) into a current block
//e.g. Expression("... sin(4) ...") will insert the LLVM IR instruction sin(4) into the current block (that may be a function body)
	
//TODO check if we want to use 'const' for each value pointr
class OperationsCompiler
{
	public:
		typedef llvm::Value* (*UnaryOp)(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val, QString& error);
		typedef llvm::Value* (*BinaryOp)(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val1, llvm::Value* val2, QString& error);
		
		static llvm::Value* compileUnaryOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object::ObjectType type, llvm::Value* val, QString& error);
		static llvm::Value* compileBinaryOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, Object::ObjectType type1, Object::ObjectType type2, llvm::Value* val1, llvm::Value* val2, QString& error);
		
	private:
		static UnaryOp opsUnary[Object::custom+1];
		static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
		
		static llvm::Value* compileValueNoneOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val, llvm::Value* none, QString &error);
		static llvm::Value* compileNoneValueOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* none, llvm::Value* val, QString &error);
		static llvm::Value* compileValueValueOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val1, llvm::Value* val2, QString &error);
		static llvm::Value* compileUnaryValueOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val, QString &error);
		
		static llvm::Value* compileValueVectorOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* val, llvm::Value* vec, QString &error);
		static llvm::Value* compileVectorValueOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* vec, llvm::Value* val, QString &error);
		static llvm::Value* compileVectorVectorOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* vec1, llvm::Value* vec2, QString &error);
		static llvm::Value* compileMatrixVectorOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* matrix, llvm::Value* vector, QString &error);
		static llvm::Value* compileUnaryVectorOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* c, QString &error);
		
		static llvm::Value* compileValueListOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* , llvm::Value* , QString &error);
		static llvm::Value* compileListListOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* l1, llvm::Value* l2, QString &error);
		static llvm::Value* compileUnaryListOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* , QString &error);
		
		static llvm::Value* compileValueMatrixOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, llvm::Value* v, llvm::Value* m1, QString &error);
		static llvm::Value* compileMatrixValueOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, llvm::Value* m1, llvm::Value* v, QString &error);
		static llvm::Value* compileVectorMatrixOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* vector, llvm::Value* matrix, QString &error);
		static llvm::Value* compileMatrixMatrixOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* m1, llvm::Value* m2, QString &error);
		static llvm::Value* compileUnaryMatrixOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Analitza::Operator::OperatorType op, llvm::Value* m, QString &error);
		static llvm::Value* compileMatrixNoneOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* m, llvm::Value* cntr, QString &error);
		static llvm::Value* compileNoneMatrixOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* cntr, llvm::Value* m, QString &error);
		
		static llvm::Value* compileCustomCustomOperation(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* v1, llvm::Value* v2, QString &error);
		
		static llvm::Value* errorCase(const QString& err, QString &error);
};

}
#endif // OPERATIONSCOMPILER_H
