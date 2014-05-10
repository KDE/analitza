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

#include <QCoreApplication>

#include "analitzautils.h"
#include "expression.h"
#include "value.h"
#include "matrix.h"
#include "list.h"

using Analitza::Expression;
using Analitza::ExpressionType;

static const QString MATRIX_EMPTY_ERROR_MESSAGE = QCoreApplication::tr("Matrix can't be empty");
static const QString MATRIX_SIZE_ERROR_MESSAGE = QCoreApplication::tr("Matrix size must be nonnegative integer");

const QString FillMatrixConstructor::id = QString("fillmatrix");
const ExpressionType FillMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression FillMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
	const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
	
	double val = args.size() == 2? 0 : static_cast<const Analitza::Cn*>(args.at(2).tree())->value();
	
	if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() > 0 && ncolsobj->value() > 0) {
		const unsigned int nrows = nrowsobj->value();
		const unsigned int ncols = ncolsobj->value();
		
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		AnalitzaUtils::fillMatrix(matrix, nrows, ncols, val);
		
		ret.setTree(matrix);
	}
	else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}

const QString ZeroMatrixConstructor::id = QString("zeromatrix");
const ExpressionType ZeroMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression ZeroMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	FillMatrixConstructor fillMatrix;
	
	return fillMatrix(args);
}

const QString IdentityMatrixConstructor::id = QString("identitymatrix");
const ExpressionType IdentityMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression IdentityMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.first().tree());
	const unsigned int n = nobj->value();
	
	if (nobj->isInteger() && n > 0) {
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (unsigned int row = 0; row < n; ++row) {
			Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
			
			for (unsigned int col= 0; col < n; ++col)
				if (row == col)
					rowobj->appendBranch(new Analitza::Cn(1));
				else
					rowobj->appendBranch(new Analitza::Cn(0));
			
			matrix->appendBranch(rowobj);
		}
		
		ret.setTree(matrix);
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);

	return ret;
}

const QString DiagonalMatrixConstructor::id = QString("diag");
const ExpressionType DiagonalMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression DiagonalMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;

	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	const Analitza::Vector *v =  static_cast<const Analitza::Vector*>(args.first().tree());
	const unsigned int n = v->size();
	
	for (unsigned int row = 0; row < n; ++row) {
		Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
		
		for (unsigned int col= 0; col < n; ++col)
			if (row == col)
				rowobj->appendBranch(v->at(col)->copy());
			else
				rowobj->appendBranch(new Analitza::Cn(0));
		
		matrix->appendBranch(rowobj);
	}
	
	ret.setTree(matrix);
	
	return ret;
}

/// bands

const QString TridiagonalMatrixConstructor::id = QString("tridiag");
const ExpressionType TridiagonalMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression TridiagonalMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;

	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
	const unsigned int n = nobj->value();
	
	if (nobj->isInteger() && n > 0) {
		if (!nobj->isInteger() && n > 0) {
			ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
			
			return ret;
		}
		
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (unsigned int row = 0; row < n; ++row) {
			Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
			
			for (unsigned int col= 0; col < n; ++col)
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
		
		ret.setTree(matrix);
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}

// utils

const QString GetNDiagonalOfMatrix::id = QString("getndiag");
const ExpressionType GetNDiagonalOfMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));

Expression GetNDiagonalOfMatrix::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
	
	if (nobj->isInteger())
	{
		const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
		
		if (matrix->size() > 0) {
			const int npos = nobj->value();
			const unsigned int absnpos = std::abs(npos);
			const unsigned int absnpos1 = absnpos + 1;
			const bool isneg = npos < 0;
			const unsigned int nrows = matrix->size();
			const unsigned int ncols = static_cast<const Analitza::MatrixRow*>(matrix->values().first())->size();
			
			if (isneg) {
				if (absnpos1 > nrows) {
					ret.addError("por filas max");
					return ret;
				}
			} else {
				if (absnpos1 > ncols) {
					ret.addError("por cols max");
					return ret;
				}
			}
			
			Analitza::Vector *diagonal = new Analitza::Vector(std::max(nrows, ncols));
			
			if (nrows == ncols) {
// 				for (int i = 0; i < n; ++i)
// 					diagonal->appendBranch(matrix->at(i, i)->copy());
			} else
				if (nrows > ncols) {
					
				} else {
					
				}
				
				

			
			const bool isfat = nrows < ncols;
// 			const bool issquare = 
			
			
			
			
			
			
			
			
			
			
			
			
			
/*
			
			unsigned int to = 0;
			unsigned int rowoffset = 0;
			unsigned int coloffset = 0;
			
			if (isneg) {
				if (absnpos1 > nrows) {
					ret.addError("por filas max");
					
					return ret;
				}
				
				to = std::min(ncols,absnpos1);
				rowoffset = absnpos;
				coloffset = 0;
			}
			else
			{
				if (absnpos1 > ncols)
				{
					ret.addError("por cols max");
					
					return ret;
				}
				
				to = std::min(nrows,absnpos1);
				rowoffset = 0;
				coloffset = absnpos;
			}
			qDebug() << "CHKR OSEA: " << ncols << absnpos << rowoffset << coloffset;

			for (int i = 0; i < to; ++i) 
				diagonal->appendBranch(matrix->at(rowoffset+i, coloffset+i)->copy());
				*/
// 			if (!negativenpos)
// 			{
// 			for (int i = -npos; i < n; ++i) 
// 				for (int j = 0; j < n+npos; ++j) 
// 					if (i==j-npos)
// 						diagonal->appendBranch(matrix->at(i, j)->copy());
// 					
// 				
// 			}
// 			else
// 			{
// 			for (int i = 0; i < n; ++i) 
// 				for (int j = npos; j < static_cast<const Analitza::MatrixRow*>(matrix->values().first())->size(); ++j) 
// 					if (i==j-npos)
// 						diagonal->appendBranch(matrix->at(i, j)->copy());
// 
// 				
// 			}

			ret.setTree(diagonal);
		} else {
			ret.addError(MATRIX_EMPTY_ERROR_MESSAGE);
		}
	}
	else
		ret.addError(QCoreApplication::tr("nth diagonal index must be integer number"));
	
	return ret;
}

const QString GetDiagonalOfMatrix::id = QString("getdiag");
const ExpressionType GetDiagonalOfMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));

Expression GetDiagonalOfMatrix::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Matrix *inputmatrix = static_cast<const Analitza::Matrix*>(args.first().tree());
	
	if (inputmatrix->size() > 0) {
		const int n = inputmatrix->size();
		bool issquare = true;
		Analitza::Matrix::const_iterator row;
		
		for (row = inputmatrix->constBegin(); row != inputmatrix->constEnd(); ++row)
			if (static_cast<const Analitza::MatrixRow*>(*row)->size() != n) {
				issquare = false;
				break;
			}
			
		if (issquare) {
			Analitza::Vector *diagonal = new Analitza::Vector(n);
			
			for (int i = 0; i < n; ++i)
				diagonal->appendBranch(inputmatrix->at(i, i)->copy());
			
			ret.setTree(diagonal);
		}
		else
			ret.addError("must be square matrix");
	}
	
	return ret;
}

