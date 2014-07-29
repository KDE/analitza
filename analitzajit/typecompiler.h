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

namespace Analitza
{

/**
 * \class TypeCompiler
 * 
 * \ingroup AnalitzaJITModule
 * 
 * \brief Compiles analitza type into a valid LLVM type.
 * 
 * TypeCompiler converts Analitza::ExpressionType into a LLVM IR type.
 * 
 * \internal ExpressionType::Value is not complete: it can't discriminate/differentiate/handle 
 * complex and int types properly, currently any Value (real, integer or complex) will be mapped into 
 * double by default (so for now we only support real and integer scalars) The Analitza type 
 * system needs to offer a Value type that is complete according to basic math 
 * scalars (e.g. real, integer and complex) perhaps we could rename Value to Real and add entries 
 * for Int and Complex into ExpressionType. Also, in the future we could have a type system based 
 * on hierarchies, so we can use here a visitor to convert the Analitza type to a valid 
 * LLVM IR type. Finally, not all types are implemented (e.g. matrix, list, 
 * vector and complex numbers)
 */

class ANALITZAJIT_EXPORT TypeCompiler //TODO : public Analitza::AbstractExpressionTypeVisitor
{
	public:
		/** @returns Returns LLVM type for a valid or supported ExpressionType, otherwise will return a null pointer. */
		static llvm::Type *compileType(const Analitza::ExpressionType &expressionType);
		
		/** Convenience method that applies TypeCompiler::compileType over each element of @p expressionTypes */
		static QVector<llvm::Type*> compileTypes(const QList<Analitza::ExpressionType>& expressionTypes);
		
		/** 
		 * Sometimes the user manages associations between variable names and its types, so this is a convenience method 
		 * that applies TypeCompiler::compileType for each variable type contained inside of @p expressionTypes
		 */
		static QMap<QString, llvm::Type*> compileTypes(const QMap<QString, Analitza::ExpressionType>& expressionTypes);
};

}

#endif // ANALITZAJIT_TYPECOMPILER_H
