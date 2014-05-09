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
#include "matrix.h"

#include <QDebug> //TODO remove this debug header

//TODO better errs mgs and use i18n calls

using Analitza::Expression;
using Analitza::ExpressionType;

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
		const int nrows = nrowsobj->value();
		const int ncols = ncolsobj->value();
		
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		AnalitzaUtils::fillMatrix(matrix, nrows, ncols, val);
		
		ret.setTree(matrix);
	}
	else
		ret.addError("size of matrix must be positve integers, bad format ...");
	
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
	
	if (nobj->isInteger() && nobj->value() > 0) {
		const int n = nobj->value();
		
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int row = 0; row < n; ++row) {
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
	const int n = v->size();
	
	for (int row = 0; row < n; ++row) {
		Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
		
		for (int col= 0; col < n; ++col)
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
	
	const int n = nobj->value();
	
	if (!nobj->isInteger() && n > 0) {
		ret.addError("the last arg (n) must be positve integer");
		
		return ret;
	}
	
	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	for (int row = 0; row < n; ++row) {
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
	
	ret.setTree(matrix);
	
	return ret;
}

// utils

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

