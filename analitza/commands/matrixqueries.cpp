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

#include "matrixqueries.h"

#include <QCoreApplication>

#include "expression.h"
#include "value.h"
#include "matrix.h"

using Analitza::Expression;
using Analitza::ExpressionType;

//BEGIN IsZeroMatrix

const QString IsZeroMatrixCommand::id = QStringLiteral("iszeromatrix");
const ExpressionType IsZeroMatrixCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
.addParameter(ExpressionType(ExpressionType::Value));

Expression IsZeroMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isZero()));
}

//END IsZeroMatrix

const QString IsIdentityMatrixCommand::id = QStringLiteral("isidentitymatrix");
const ExpressionType IsIdentityMatrixCommand::type = IsZeroMatrixCommand::type;

Expression IsIdentityMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isIdentity()));
}

const QString IsDiagonalMatrixCommand::id = QStringLiteral("isdiag");
const ExpressionType IsDiagonalMatrixCommand::type = IsZeroMatrixCommand::type;

Expression IsDiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isDiagonal()));
}
