/*************************************************************************************
 *  Copyright (C) 2022 Aleix Pol Gonzalez <aleixpol@kde.org>                         *
 *  Copyright (C) 2022 Stephen Swanson <stephen.swanson@mailbox.org>                 *
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

#include "combinatronics.h"

#include <QCoreApplication>

#include "expression.h"
#include "list.h"
#include "value.h"

using namespace Analitza;

const QString CombinationCommand::id = QStringLiteral("comb");
const ExpressionType CombinationCommand::type = ExpressionType(ExpressionType::Lambda)
                                        .addParameter(ExpressionType::Value)
                                        .addParameter(ExpressionType::Value)
                                        .addParameter(ExpressionType::Value);

Expression CombinationCommand::operator()(const QList<Expression>& args)
{
    const auto a = args.constFirst().toReal().intValue();
    const auto b = args.constLast().toReal().intValue();

    // a=n b=r
    uint n_factorial=1;
    for(int i=a; i>1.; i--) {
        n_factorial*=floor(i);
    }
    uint r_factorial=1;
    for(int i=b; i>1.; i--) {
        r_factorial*=floor(i);
    }
    uint rn = a - b;
    uint rn_factorial=1;
    for(int i=rn; i>1.; i--) {
        rn_factorial*=floor(i);
    }
    uint res = n_factorial / (r_factorial * rn_factorial);
    return Expression(Cn(res));
}

const QString PermutationCommand::id = QStringLiteral("perm");
const ExpressionType PermutationCommand::type = ExpressionType(ExpressionType::Lambda)
                                        .addParameter(ExpressionType::Value)
                                        .addParameter(ExpressionType::Value)
                                        .addParameter(ExpressionType::Value);

Expression PermutationCommand::operator()(const QList<Expression>& args)
{
    const auto a = args.constFirst().toReal().value();
    const auto b = args.constLast().toReal().intValue();

    // a=n b=r
    if (b > a) {
        return Expression(Cn(0));
    } else {
        uint n_factorial=1;
        for(int i=a; i>1.; i--) {
            n_factorial*=floor(i);
        }
        uint rn = a - b;
        uint rn_factorial=1;
        for(int i=rn; i>1.; i--) {
            rn_factorial*=floor(i);
        }
        uint res = n_factorial / rn_factorial;
        return Expression(Cn(res));
    }
}
