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
#include "container.h"

using Analitza::Expression;
using Analitza::ExpressionType;

static const QString MATRIX_SIZE_ERROR_MESSAGE = QCoreApplication::tr("Matrix size must be some integer value greater or equal to zero");

const QString FillVectorConstructor::id = QString("fillvector");
const ExpressionType FillVectorConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression FillVectorConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *lengthobj = static_cast<const Analitza::Cn*>(args.first().tree());
	
	if (lengthobj->isInteger() && lengthobj->value() >= 0) {
		Analitza::Vector *vector = new Analitza::Vector(lengthobj->intValue());
		AnalitzaUtils::fillVector(vector, lengthobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())->value());
		ret.setTree(vector);
	}
	else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}


//BEGIN FillMatrixConstructor

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
	
	if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() >= 0 && ncolsobj->value() >= 0) {
		Analitza::Matrix *matrix = new Analitza::Matrix();
		AnalitzaUtils::fillMatrix(matrix, nrowsobj->intValue(), ncolsobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())->value());
		ret.setTree(matrix);
	}
	else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}

//END FillMatrixConstructor


//BEGIN ZeroMatrixConstructor

const QString ZeroMatrixConstructor::id = QString("zeromatrix");
const ExpressionType ZeroMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression ZeroMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	FillMatrixConstructor fillMatrix;
	
	return fillMatrix(QList<Analitza::Expression>(args) << Expression(new Analitza::Cn(0)));
}

//END ZeroMatrixConstructor


//BEGIN IdentityMatrixConstructor

const QString IdentityMatrixConstructor::id = QString("identitymatrix");
const ExpressionType IdentityMatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression IdentityMatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.first().tree());
	const int n = nobj->value();
	
	if (nobj->isInteger() && n > 0) {
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
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);

	return ret;
}

//END IdentityMatrixConstructor


//BEGIN DiagonalMatrixConstructor

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

//END DiagonalMatrixConstructor


//BEGIN TridiagonalMatrixConstructor

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
	
	if (nobj->isInteger() && n >= 0) {
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
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}

//END TridiagonalMatrixConstructor


//BEGIN GetNDiagonalOfMatrix

const QString GetNDiagonalOfMatrix::id = QString("getndiag");
const ExpressionType GetNDiagonalOfMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));

Expression GetNDiagonalOfMatrix::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
	
	if (nobj->isInteger()) {
		const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
		const int nrows = matrix->rowCount();
		const int ncols = matrix->columnCount();
		const int npos = nobj->value();
		const int absnpos = std::abs(npos);
		const int absnpos1 = absnpos + 1;
		const bool isneg = npos < 0;
		
		int n = 0; // or until/to
		int rowoffset = 0;
		int coloffset = 0;
		
		if (isneg) {
			if (absnpos1 > nrows) {
				ret.addError("The nth diagonal index must be less than the row count");
				return ret;
			}
			
			n = std::min(nrows - absnpos, ncols);
			rowoffset = absnpos;
		} else { // square matrix case too
			if (absnpos1 > ncols) {
				ret.addError("The nth diagonal index must be less than the column count");
				return ret;
			}
			
			n = std::min(nrows, ncols - absnpos);
			coloffset = absnpos;
		}
		
		Analitza::Vector *diagonal = new Analitza::Vector(n);
		
		for (int i = 0; i < n; ++i)
			diagonal->appendBranch(matrix->at(rowoffset + i, coloffset + i)->copy());
			
		ret.setTree(diagonal);
	}
	else
		ret.addError(QCoreApplication::tr("nth diagonal index must be integer number"));
	
	return ret;
}

//END GetNDiagonalOfMatrix


//BEGIN GetDiagonalOfMatrix

const QString GetDiagonalOfMatrix::id = QString("getdiag");
const ExpressionType GetDiagonalOfMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));

Expression GetDiagonalOfMatrix::operator()(const QList< Analitza::Expression >& args)
{
	GetNDiagonalOfMatrix getNDiagonalOfMatrix;
	
	return getNDiagonalOfMatrix(QList<Analitza::Expression>(args) << Expression(new Analitza::Cn(0)));
}

//END GetDiagonalOfMatrix


//BEGIN IsZeroMatrix

const QString IsZeroMatrix::id = QString("iszeromatrix");
const ExpressionType IsZeroMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value));

Expression IsZeroMatrix::operator()(const QList< Analitza::Expression >& args)
{
	return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isZero()));
}

//END IsZeroMatrix


const QString IsIdentityMatrix::id = QString("isidentitymatrix");
const ExpressionType IsIdentityMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value));

Expression IsIdentityMatrix::operator()(const QList< Analitza::Expression >& args)
{
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}

const QString IsDiagonalMatrix::id = QString("isdiagonalmatrix");
const ExpressionType IsDiagonalMatrix::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value));

Expression IsDiagonalMatrix::operator()(const QList< Analitza::Expression >& args)
{
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}
