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

#include "eigencommands.h"

#include <QCoreApplication>

#include <Eigen/Dense>

#include "expression.h"
#include "value.h"
#include "matrix.h"

using Analitza::Expression;
using Analitza::ExpressionType;

const QString EigenTestCommand::id = QString("eigentest");
const ExpressionType EigenTestCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any, 
							 ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1)))
.addParameter(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Value)));

Expression EigenTestCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const int nargs = args.size();
	
	if (nargs != 1) {
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'. Should have 1 parameter.").arg(EigenTestCommand::id));
		
		return ret;
	}
	
	const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
	const int m = matrix->rowCount();
	const int n = matrix->columnCount();
	
	Eigen::MatrixXd scalarmatrix(m, n);
	
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j) 
			if (matrix->at(i,j)->type() == Analitza::Object::value) {
				const Analitza::Cn *entry = static_cast<const Analitza::Cn*>(matrix->at(i,j));
				const Analitza::Cn::ValueFormat entryformat = entry->format();
				
				//Don't allow complex numbers
				if (entryformat == Analitza::Cn::Char || entryformat == Analitza::Cn::Real || 
					entryformat == Analitza::Cn::Integer || entryformat == Analitza::Cn::Boolean) {
					scalarmatrix(i,j) = entry->value();
				} else {
					ret.addError(QCoreApplication::tr("Invalid parameter type in matrix entry (%1,%2) for '%3', it must be a number value.")
								 .arg(i).arg(j).arg(EigenTestCommand::id));
					
					return ret;
				}
			} else {
				ret.addError(QCoreApplication::tr("Invalid parameter type in matrix entry (%1,%2) for '%3', it must be a number value.")
				.arg(i).arg(j).arg(EigenTestCommand::id));
				
				return ret;
			}
	
	Q_ASSERT(nargs > 0);
	Q_ASSERT(ret.toString().isEmpty());
	Q_ASSERT(ret.isCorrect());
	
	Eigen::EigenSolver<Eigen::MatrixXd> eigensolver;
	eigensolver.compute(scalarmatrix, /* computeEigenvectors = */ false);
	
	
	
	return ret;
}

//E