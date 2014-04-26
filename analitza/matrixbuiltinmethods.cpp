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

#include "expression.h"
#include <QDebug>

using Analitza::Expression;
using Analitza::ExpressionType;

const QString MatrixRowConstructor::id = QString("row");
const ExpressionType MatrixRowConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any)))
.addParameter(ExpressionType(ExpressionType::Many));

Expression MatrixRowConstructor::operator()(const QList< Expression >& args)
{
	//check paent context, must be insdide a matrix command call
	qDebug() << "begcomm";
	qDebug() << args.at(0).toString();
	qDebug() << args.at(1).toString();
	qDebug() << args.at(2).toString();
	qDebug() << "endcomm";
	return Expression("list{233}");
}

/*

const QString Analitza::IdentityMatrix::id = QString("matrix");
// Analitza::ExpressionType ExpressionType() { return Analitza::ExpressionType(name); }
Expression Analitza::IdentityMatrix::operator()(const QList< Expression >& args)
{
		Q_UNUSED(args)
		return Expression("2");
}*/


