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

#ifndef ANALITZAJIT_TYPECOMPILER_H
#define ANALITZAJIT_TYPECOMPILER_H

#include "analitzajitexport.h"

#include "analitza/expressiontype.h"

namespace llvm
{
class Type;
}

//TODO better documentation
/**
 * \class LLVMIRExpressionWriter
 * 
 * \ingroup AnalitzaJITModule
 * 
 * \brief In the future we could have a type system based on hierarchies,
 * so we can use here a visitor to convert the Analitza type into a valid LLVM IR type.
 * For now, this class will take some ExpressionType and will convert it into a IR type
 * using simple and direct methods.
 * 
 */

class ANALITZAJIT_EXPORT TypeCompiler //TODO : public Analitza::AbstractExpressionTypeVisitor
{
	public:
		//TODO ExpressionType::Value is not complete, it can't discriminate/differentiate/handle complex 
		// and int types properly, currently any Value (real, integer or complex) will be mapped into 
		// a double by default (so for now we only support real and integer scalars)
		// The Analitza type system needs to offer a Value type that is complete according to 
		// basic math scalars (e.g. real, integer and complex)
		//NOTE ... perhaps we could rename Value to Real and add entries for Int and Complex into ExpressionType.
		static llvm::Type *mapExpressionType(const Analitza::ExpressionType &expressionType);
		
		//convenience method we will use mapExpressionType over each element of expressionType
		static std::vector<llvm::Type*> mapExpressionTypes(const QList<Analitza::ExpressionType>& expressionTypes);
};

#endif // ANALITZAJIT_TYPECOMPILER_H
