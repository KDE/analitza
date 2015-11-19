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

#ifndef MATRIXCOMMANDS_H
#define MATRIXCOMMANDS_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
}

/**
 * \class MatrixCommand
 * 
 * \brief Implements the \"matrix\" command.
 * 
 * MatrixCommand constructs a matrix by two ways:
 * 
 * The first way creates a matrix filled with a fixed value. 
 * \code matrix(3) \endcode 
 * constructs a square 3x3 matrix filled with zeros.
 * 
 * \code matrix(4,5) \endcode 
 * constructs a 4x5 matrix filled with zeros.
 * 
 * \code matrix(6,7, 0.5) \endcode 
 * constructs a 6x7 matrix filled with the value 0.5.
 * 
 * The second way creates a matrix by given vectors or matrixrow elements.
 * \code matrix(vector{1,3}, vector{4,5}) \endcode 
 * constructs a matrix with two column vectors:
 * [1 4]
 * [3 5]
 * 
 * \code matrix(matrixrow{1,3}, matrixrow{4,5}) \endcode 
 * constructs a matrix with two matrixrow elements:
 * [1 3]
 * [4 5]
 *
 */

class MatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;
    
    static const QString id;
    static const Analitza::ExpressionType type;
};

/**
 * \class IdentityMatrixCommand
 * 
 * \brief Implements the \"identitymatrix\" command.
 * 
 * IdentityMatrixCommand constructs a identitymatrix.
 * For example:
 * \code matrix(2) \endcode
 * \code matrix { matrixrow { 1, 0 }, matrixrow { 1, 0 } } \endcode
 * 
 */

class IdentityMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

/**
 * \class DiagonalMatrixCommand
 * 
 * \brief Implements the \"diag\" command.
 * 
 * DiagonalMatrixCommand can be used to construct a diagonal matrix and also 
 * to get the diagonal part of a matrix.
 * 
 * There are two ways to construct a diagonal matrix:
 *
 * The first way creates the diagonal part out of the parameters.
 * \code diag(1,2,3) \endcode 
 * constructs the 3x3 diagonal matrix:
 * [1 0 0]
 * [0 2 0]
 * [0 0 3]
 * 
 * The second way uses a vector as the diagonal part:
 * \code diag(vector{1,2,3}) \endcode 
 * is equivalent to 
 * \code diag(1,2,3) \endcode 
 *
 * There are two ways to get the diagonal part of a matrix, also both always 
 * return a vector that contains the requested diagonal part.
 * 
 * The first way always get the principal diagonal of the a given matrix.
 * \code diag(matrix{matrixrow{1,2}, matrixrow{3,4}}) \endcode 
 * returns 
 * \code vector{1,4} \endcode 
 * 
 * The second way returns a vector of the elements on the nth diagonal of 
 * a given matrix.
 * 
 * \code diag(A, n) \endcode 
 * where n>0 means is above the main diagonal of A and n<0 is below the main 
 * diagonal.
 * 
 * Examples:
 * 
 * \code diag(matrix{matrixrow{1,2}, matrixrow{3,4}}, 0) \endcode 
 * is equivalent to 
 * \code diag(matrix{matrixrow{1,2}, matrixrow{3,4}}) \endcode 
 * 
 * \code diag(matrix{matrixrow{1,2}, matrixrow{3,4}}, 1) \endcode 
 * returns 
 * \code vector{2} \endcode
 *
 * \code diag(matrix{matrixrow{1,2}, matrixrow{3,4}}, -1) \endcode 
 * returns 
 * \code vector{3} \endcode 
 * 
 */

class DiagonalMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

/**
 * \class TridiagonalMatrixCommand
 * 
 * \brief Implements the \"tridiag\" command.
 * 
 * TridiagonalMatrixCommand constructs a tridiagonal matrix.
 * 
 * \code tridiag(a, b, c, n) \endcode
 * where b is in the main diagonal, a is below the main diagonal, 
 * c is above the main diagonal and n is the size of the matrix.
 * 
 */

class TridiagonalMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

//TODO random matrix (default 0,1 ... but you can set min and max vals)

#endif // MATRIXCOMMANDS_H
