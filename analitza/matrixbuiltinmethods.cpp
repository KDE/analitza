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

#include "matrixbuiltinmethods.h"

#include "analitzautils.h"
#include "expression.h"
#include "value.h"
#include "operations.h"
#include "list.h"
#include "matrix.h"
#include "container.h"
#include "customobject.h"

#include <QDebug>

using Analitza::Expression;
using Analitza::ExpressionType;

const QString MatrixConstructor::id = QString("matrix");
const ExpressionType MatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any), -2), -1))
.addParameter(ExpressionType(ExpressionType::Many));

Expression MatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret("matrix{}");
	
	if (args.isEmpty())
		return ret;
	
	// if using sizes as input, then construct a mxn matrix filled with zeros
	if (args.size() == 2 && args.at(0).isReal() && args.at(1).isReal())
	{
		const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
		const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
		
		if (nrowsobj->isInteger() && ncolsobj->isInteger())
		{
			const int nrows = nrowsobj->value();
			const int ncols = ncolsobj->value();
			
			Analitza::Container *container = new Analitza::Container(Analitza::Container::math);
			Analitza::Matrix *matrix = new Analitza::Matrix();
			
			AnalitzaUtils::fillMatrix(matrix, nrows, ncols, 0);
			
			container->appendBranch(matrix);
			
			ret.setTree(container);
		}
	}
	else //using vectors (rows or cols) as input
	{
		int oldsize = -1;
		QString oldVectorTypeInfo;
		const Analitza::CustomObject *info; // to know if we have are rows or cols
		
		for (int i = 0; i < args.size(); ++i)
		{
			if (!args.at(i).isList())
			{
				ret.addError("i list, no es list");
				
				return ret;
			}
			
			const Analitza::List *list = static_cast<const Analitza::List*>(args.at(i).tree());
			
			if (list->size() <= 1)
			{
				ret.addError("i list, es vacia o no tiene suf elements");
				
				return ret;
			}
			
			if (list->at(0)->type() != Analitza::Object::custom)
			{
				ret.addError("i list, no parece have venido de col o row: first must be custom");
				
				return ret;
			}
			if (i == 0) oldsize = list->size(); //TODO this kind of validation is missing in matrix {...}
			
			if (list->size() != oldsize)
			{
				ret.addError("i list, debe ser de tamanio uniforme con respecto al resto");
				
				return ret;
			}
			
			info = static_cast<const Analitza::CustomObject*>(list->at(0));
			
			if (info->value() != MatrixRowConstructor::id && info->value() != MatrixColConstructor::id)
			{
				ret.addError("i list, debe venir de col o row");
				
				return ret;
			}
			
			if (i == 0) oldVectorTypeInfo = info->value().toString();
			
			if (info->value() != oldVectorTypeInfo)
			{
				ret.addError("i list, debe formarsolo de rows o cols nada de hibridos");
				
				return ret;
			}
		}
		
		//NOTE at this point everything is ok
		
		Analitza::Container *container = new Analitza::Container(Analitza::Container::math);
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int row = 0; row < args.size(); ++row)
		{
			const Analitza::List *list = static_cast<const Analitza::List*>(args.at(row).tree());
			Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(list->size()-1); // -1: ignore customobjt
			
			for (int i = 1; i < list->size(); ++i) // ignore customobjt
				rowobj->appendBranch(list->at(i)->copy()); //NOTE you must copy to avoid invalid references
			
			matrix->appendBranch(rowobj);
		}
		
		if (info->value() == MatrixRowConstructor::id) // if we have rows as args
			container->appendBranch(matrix);
		else // we have cols as args
			container->appendBranch(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, 0));
		
		ret.setTree(container);
	}
	
	return ret;
}

//BEGIN IdentityMatrixConstructor

const QString IdentityMatrixConstructor::id = QString("identitymatrix");
const ExpressionType IdentityMatrixConstructor::type = MatrixConstructor::type;

Expression IdentityMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret("matrix{}");
	
	if (args.isEmpty())
		return ret;
	
	if (args.size() == 1 && args.at(0).isReal())
	{
		const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
		
		if (nobj->isInteger())
		{
			const int n = nobj->value();
			
			Analitza::Container *container = new Analitza::Container(Analitza::Container::math);
			Analitza::Matrix *matrix = new Analitza::Matrix();
			
			for (int row = 0; row < n; ++row)
			{
				Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
				
				for (int col= 0; col < n; ++col)
					if (row == col)
						rowobj->appendBranch(new Analitza::Cn(1));
					else
						rowobj->appendBranch(new Analitza::Cn(0));
				
				matrix->appendBranch(rowobj);
			}
			
			container->appendBranch(matrix);
			
			ret.setTree(container);
		}
	}
	
	return ret;
}

//END IdentityMatrixConstructor

//BEGIN DiagonalMatrixConstructor

const QString DiagonalMatrixConstructor::id = QString("diag");
const ExpressionType DiagonalMatrixConstructor::type = MatrixConstructor::type;

Expression DiagonalMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret("matrix{}");
	
	if (args.isEmpty())
		return ret;
	
	Analitza::Container *container = new Analitza::Container(Analitza::Container::math);
	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	const int n = args.size();
	
	for (int row = 0; row < n; ++row)
	{
		Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
		
		for (int col= 0; col < n; ++col)
			if (row == col)
				rowobj->appendBranch(args.at(col).tree()->copy());
			else
				rowobj->appendBranch(new Analitza::Cn(0));
		
		matrix->appendBranch(rowobj);
	}
	
	container->appendBranch(matrix);
			
	ret.setTree(container);
	
	return ret;
}


//END DiagonalMatrixConstructor
