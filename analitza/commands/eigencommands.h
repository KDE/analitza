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

#ifndef EIGENCOMMANDS_H
#define EIGENCOMMANDS_H

#include "builtinmethods.h"
#include <config-analitza.h>

namespace Analitza {
class Expression;
}

class EigenvaluesCommand: public Analitza::FunctionDefinition
{
public:
    Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};


class EigenvectorsCommand: public Analitza::FunctionDefinition
{
public:
    Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

#endif // EIGENCOMMANDS_H
