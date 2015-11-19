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

#ifndef VECTORCOMMANDS_H
#define VECTORCOMMANDS_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
}


/**
 * \class VectorCommand
 * 
 * \brief Implements the \"vector\" command.
 * 
 * VectorCommand constructs a vector by two ways:
 * 
 * The first way creates a vector from a list, with the same list elements.
 * \code vector(range(5)) \endcode 
 * constructs 
 * \code vector { 1, 2, 3, 4, 5 } \endcode
 * 
 * The second way creates a vector of size \"n\" filled with a value \"a\":
 * \code vector(n, a) \endcode
 * constructs 
 * \code vector { a, a, ... , a } \endcode
 * 
 */

class VectorCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;
    
    static const QString id;
    static const Analitza::ExpressionType type;
};

#endif // VECTORCOMMANDS_H
