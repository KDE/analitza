/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta\gmail.com>      *
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

#ifndef LISTCOMMANDS_H
#define LISTCOMMANDS_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
}

/**
 * \class RangeCommand
 * 
 * \brief Implements the \"range\" command.
 * 
 * RangeCommand constructs a list that contains a sequence.
 * There are three ways to call this command:
 * 
 * The first way creates a sequence from 1, with increment 1, that ends in 
 * the number \"b\".
 * \code range(b) \endcode 
 * constructs 
 * \code list { 1, 2, 3, ... , b } \endcode
 * 
 * The second way creates a sequence from the number \"a\", with increment 1, that 
 * ends in the number \"b\".
 * \code range(a, b) \endcode
 * constructs 
 * \code list { a, a+1, a+2, ... , b } \endcode
 * 
 * The second way creates a sequence from the number \"a\", with 
 * increment \"h\" (numeric value), that ends in the number \"b\".
 * \code range(a, b, h) \endcode
 * constructs 
 * \code list { a, a+h, a+h, ... , b } \endcode
 * 
 * The numbers can be reals too. For example:
 * \code range(0.0, 1.0, 0.2) \endcode
 * constructs 
 * \code list { 0, 0.2, 0.4, 0.6, 0.8, 1 } \endcode
 * 
 */

class RangeCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;
    
    static const QString id;
    static const Analitza::ExpressionType type;
};

#endif // LISTCOMMANDS_H
