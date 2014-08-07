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

#include "typecompiler.h"

#include <QVector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

static llvm::LLVMContext &llvmcontext = llvm::getGlobalContext();

using namespace Analitza;
// using namespace AnalitzaUtils;

llvm::Type* TypeCompiler::compileType(const Analitza::ExpressionType& expressionType, bool containerAsPointer)
{
	llvm::Type *ret = 0;
	
	switch(expressionType.type()) {
		//TODO Value: Analitza must implementa way to deal with integer and complex scalars (not only reals)
		case Analitza::ExpressionType::Value: ret = llvm::Type::getDoubleTy(llvmcontext); break;
		case Analitza::ExpressionType::Bool: ret = llvm::Type::getInt1Ty(llvmcontext); break;
		case Analitza::ExpressionType::Char: ret = llvm::Type::getInt8Ty(llvmcontext); break;
		case Analitza::ExpressionType::Vector: {
			if (expressionType.size() > 0) {
				//NOTE LLVM::VectorType can have issues with being platform independent ...we'll use array until that
				//ret = llvm::VectorType::get(compileType(expressionType.contained()), expressionType.size());
				//ret = llvm::ArrayType::get(compileType(expressionType.contained()), expressionType.size());
				if (containerAsPointer) {
					ret = llvm::Type::getDoubleTy(llvm::getGlobalContext())->getPointerTo();
				} else {
					ret = llvm::ArrayType::get(compileType(expressionType.contained()), expressionType.size());
				}
			} else {
				//TODO error user needs to specify the size of the vector
			}
		}	break;
		case Analitza::ExpressionType::Matrix: {
			if (expressionType.size() > 0) {
				//NOTE LLVM::VectorType can have issues with being platform independent ...we'll use array until that
				//ret = llvm::VectorType::get(compileType(expressionType.contained()), expressionType.size());
				//ret = llvm::ArrayType::get(compileType(expressionType.contained()), expressionType.size());
				if (containerAsPointer) {
					ret = llvm::Type::getDoubleTy(llvm::getGlobalContext())->getPointerTo();
					ret = ret->getPointerTo();
				} else {
					ret = llvm::ArrayType::get(compileType(expressionType.contained()), expressionType.contained().size());
					ret = llvm::ArrayType::get(ret, expressionType.size());;
				}
			} else {
				//TODO error user needs to specify the size of the matrix
			}
		}	break;
		//TODO case List, Lambda
	}
	
	Q_ASSERT(ret);
	
	return ret;
}

QVector< llvm::Type* > TypeCompiler::compileTypes(const QList< Analitza::ExpressionType >& expressionTypes, bool containerAsPointer)
{
	const int n = expressionTypes.size();
	QVector< llvm::Type* > ret(n);
	
	for (int i = 0; i < n; ++i)
		ret.append(compileType(expressionTypes.at(i), containerAsPointer));
	
	return ret;
}

QMap<QString, llvm::Type*> TypeCompiler::compileTypes(const QMap<QString, Analitza::ExpressionType>& expressionTypes, bool containerAsPointer)
{
	QMap<QString, llvm::Type*> ret;
	
	foreach (const QString &var, expressionTypes.keys()) {
		ret[var] = compileType(expressionTypes[var], containerAsPointer);
	}
	
	return ret;
}
