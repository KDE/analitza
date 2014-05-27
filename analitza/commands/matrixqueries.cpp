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
#include "operations.h"

using Analitza::Expression;
using Analitza::ExpressionType;

//BEGIN type utils

typedef QList<ExpressionType> ExpressionTypeList;

static const ExpressionType ValueType = ExpressionType(ExpressionType::Value);
static const ExpressionType VectorType = ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1);
static const ExpressionType MatrixType = ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1);
static const ExpressionTypeList VectorAndMatrixTypes = ExpressionTypeList() << VectorType << MatrixType;
static const ExpressionType VectorAndMatrixAlternatives = ExpressionType(ExpressionType::Many, VectorAndMatrixTypes);
static const ExpressionType IndefiniteArityType = ExpressionType(ExpressionType::Any);

static const ExpressionType functionType(const ExpressionTypeList &from, const ExpressionType &to)
{
	ExpressionType ret(ExpressionType::Lambda);
	
	foreach (const ExpressionType &type, from)
		ret.addParameter(type);
	
	ret.addParameter(to);
	
	return ret;
}

static const ExpressionType functionType(const ExpressionType &from, const ExpressionType &to)
{
	return functionType(ExpressionTypeList() << from, to);
}

static const ExpressionType variadicFunctionType(const ExpressionType &to)
{
	return functionType(IndefiniteArityType, to);
}

//END type utils

static const QString MATRIX_SIZE_ERROR_MESSAGE = QCoreApplication::tr("Matrix dimensions must be greater than zero");

// const QString VectorCommand::id = QString("range");
// // const ExpressionType VectorCommand::type = functionType(ExpressionTypeList() << ValueType << ValueType, VectorType);
// 
// Expression RangeCommand::operator()(const QList< Analitza::Expression >& args)
// {
// 
// }



//BEGIN IsZeroMatrix

const QString IsZeroMatrixCommand::id = QString("iszeromatrix");
const ExpressionType IsZeroMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsZeroMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsZeroMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isZero()));
}

//END IsZeroMatrix


const QString IsIdentityMatrixCommand::id = QString("isidentitymatrix");
const ExpressionType IsIdentityMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsIdentityMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsIdentityMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}

const QString IsDiagonalMatrixCommand::id = QString("isdiag");
const ExpressionType IsDiagonalMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsDiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsDiagonalMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}
