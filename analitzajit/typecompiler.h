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

#include "object.h"
#include "analitzajitexport.h"

namespace Analitza
{
class Object;
class Container;
class Variables;
class Expression;
class Apply;
class List;
class Operator;
class Vector;
class Matrix;
}
//TODO in the future we could have a type system based in hierarchy, so we can 
//apply here a visitor to convert the Analitza type into a valid IR type

//TODO convert ExpressionType into a valid LLVM IR Type
class TypeCompiler //TODO : public Analitza::AbstractExpressionTypeVisitor
{
	public:
		TypeCompiler();
};

#endif // ANALITZAJIT_TYPECOMPILER_H
