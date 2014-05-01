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
#include "customobject.h"

#include <QDebug> //TODO remove this debug header

//TODO better errs mgs and use i18n calls

using Analitza::Expression;
using Analitza::ExpressionType;



//BEGIN Normal/Standard Matrix

const QString MatrixConstructor::id = QString("matrix");
const ExpressionType MatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any),100), 100));

Expression MatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
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
			
			Analitza::Matrix *matrix = new Analitza::Matrix();
			
			AnalitzaUtils::fillMatrix(matrix, nrows, ncols, 0);
			
			ret.setTree(matrix);
		}
		else
			ret.addError("size of matrix must be integers, bad format ...");
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
			ret.setTree(matrix);
		else // we have cols as args
			ret.setTree(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, 0));
	}
	
	return ret;
}

//END Normal/Standard Matrix

//BEGIN IdentityMatrixConstructor

const QString IdentityMatrixConstructor::id = QString("identitymatrix");
const ExpressionType IdentityMatrixConstructor::type = MatrixConstructor::type;

Expression IdentityMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	if (args.isEmpty())
		return ret;
	
	if (args.size() == 1)
	{
		if (!args.first().toString().isEmpty() && args.first().isCorrect())
		{
			if (args.first().isReal())
			{
				const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.first().tree());
				
				if (!nobj->isInteger() && nobj->value() > 0)
				{
					const int n = nobj->value();
					
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
					
					ret.setTree(matrix);
				}
				else
					ret.addError("bad format of arg:must be positve integer");
			}
		}
		else
			ret.addError("incorrect input:empty or bad expression");
	}
	else
		ret.addError("bad numbers of args");
	
	return ret;
}

//END IdentityMatrixConstructor

//BEGIN DiagonalMatrixConstructor

const QString DiagonalMatrixConstructor::id = QString("diag");
const ExpressionType DiagonalMatrixConstructor::type = MatrixConstructor::type;

Expression DiagonalMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	if (args.isEmpty())
		return ret;
	
	// check if we can return the diagonal of a matrix as a vector
	if (args.size() == 1)
	{
		if (!args.first().toString().isEmpty() && args.first().isCorrect())
		{
			if (args.first().isMatrix())
			{
				const Analitza::Matrix *inputmatrix = static_cast<const Analitza::Matrix*>(args.first().tree());
				
				if (inputmatrix->size() > 0)
				{
					const int n = inputmatrix->size();
					bool issquare = true;
					Analitza::Matrix::const_iterator row;
					
					for (row = inputmatrix->constBegin(); row != inputmatrix->constEnd(); ++row)
					{
		// 				qDebug() << "es: " << (*row)->type();
						if (static_cast<const Analitza::MatrixRow*>(*row)->size() != n)
						{
							issquare = false;
							break;
						}
					}
						
					if (issquare)
					{
						Analitza::Vector *diagonal = new Analitza::Vector(n);
						
						for (int i = 0; i < n; ++i)
							diagonal->appendBranch(inputmatrix->at(i, i)->copy());
						
						ret.setTree(diagonal);
					}
					else
						ret.addError("must be square matrix");
				}
			}
		}
		else
			ret.addError("empty string o bad strin");
	}
	else // construct a diagonal matrix from args
	{
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
		
		ret.setTree(matrix);
	}
	
	return ret;
}

/// bands

//TODO statc in this file static:supportedrettypes and supportedargstypes
static const ExpressionType variasmatrices()
{
	QList<ExpressionType> alternatives;
	
	//NOTE this constraint must be admissible since Analitza is not dynamically typed
	const int max_rows_supported = 10;
	const int max_cols_supported = 10;
	
	for (int row = 1; row <= max_rows_supported; ++row)
		for (int col = 1; col <= max_cols_supported; ++col)
			alternatives.append(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), col), row));
	
// 	alternatives.append(Analitza::ExpressionType(Analitza::ExpressionType::List, Analitza::ExpressionType(Analitza::ExpressionType::Any)));
	
	return ExpressionType(ExpressionType::Many, alternatives);
}

const QString TridiagonalMatrixConstructor::id = QString("tridiag");
const ExpressionType TridiagonalMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(variasmatrices());

Expression TridiagonalMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	if (args.isEmpty())
		return ret;
	
	if (args.size() != 4)
	{
		ret.addError("bad number of args meust be a,b,c,n");
		
		return ret;
	}

	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
	
	const int n = nobj->value();
	
// 	qDebug() << "CALCLATE 1 " << n;
	
	if (!nobj->isInteger() && n > 0)
	{
		ret.addError("the last arg (n) must be positve integer");
		
		return ret;
	}
		
	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	for (int row = 0; row < n; ++row)
	{
		Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
		
		for (int col= 0; col < n; ++col)
			if (row == col + 1) // a
				rowobj->appendBranch(args.at(0).tree()->copy());
			else
				if (row == col) // b
					rowobj->appendBranch(args.at(1).tree()->copy());
				else
					if (row == col - 1) // c
						rowobj->appendBranch(args.at(2).tree()->copy());
					else
						rowobj->appendBranch(new Analitza::Cn(0));
		
		matrix->appendBranch(rowobj);
	}
	
// 	Analitza::List *list= new Analitza::List();
// 	list->appendBranch(new Analitza::CustomObject(QVariant(QString("ismatrix")), 0));
// 	list->appendBranch(matrix);
// 	
// 	ret.setTree(list); ///NO WAY, to much impact on many parts of the lib
	
	Q_ASSERT(matrix->size() > 0); //sizes (in types) goes from 1 to .. n not from 0 to ... n
	
	ret.setTree(matrix);
	
	return ret;
}


//END DiagonalMatrixConstructor
