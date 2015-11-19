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

#ifndef MATRIXQUERIES_H
#define MATRIXQUERIES_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
}

class IsZeroMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

class IsIdentityMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

class IsDiagonalMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

#endif // MATRIXQUERIES_H
