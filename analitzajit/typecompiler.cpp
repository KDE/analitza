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

#include <QDebug>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

llvm::Type* TypeCompiler::mapExpressionType(const Analitza::ExpressionType& expressionType)
{
	llvm::Type *ret = 0;
	
// 	qDebug() << "PEPEPEPEPE " << expressionType.type();
	
	switch(expressionType.type()) {
		case Analitza::ExpressionType::Value: {
			//TODO get ineger bit width from host then decide if 32 or 64 bits
			//TODO handle llvm::Type::getInt32Ty(llvm::getGlobalContext())
			ret = llvm::Type::getDoubleTy(llvm::getGlobalContext());
		}	break;
		case Analitza::ExpressionType::Bool: {
			ret = llvm::Type::getInt1Ty(llvm::getGlobalContext()); //we need just one bit for bool
		}	break;
		//TODO
// 		case Analitza::ExpressionType::Char: {
// 			ret = llvm::Type::getInt8Ty(llvm::getGlobalContext());
// 		}	break;
		//TODO case Vector, List, Lambda, Matrix
	}
	
	Q_ASSERT(ret);
	
	return ret;
}

std::vector< llvm::Type* > TypeCompiler::mapExpressionTypes(const QList< Analitza::ExpressionType >& expressionTypes)
{
	const int n = expressionTypes.size();
	std::vector< llvm::Type* > ret(n);
	
	for (int i = 0; i < n; ++i)
		ret.push_back(mapExpressionType(expressionTypes.at(i)));
	
	return ret;
}
