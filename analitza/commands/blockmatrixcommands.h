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

#ifndef BLOCKMATRIXCOMMANDS_H
#define BLOCKMATRIXCOMMANDS_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
}

/**
 * \class BlockMatrixCommand
 * 
 * \brief Implements the \"blockmatrix\" command.
 * 
 * BlockMatrixCommand constructs a block matrix by two ways:
 * 
 * The first way creates a block matrix based on rows.
 * \code blockmatrix(matrixrow{A1,B1,..., Z1}, ..., matrixrow{AN,BN,..., ZN}) \endcode 
 * Here A1,B1, ..., ZN are blocks (i.e. matrices) 
 * The number of elements in each matrixrow element must be the same.
 * 
 * The first way creates a block matrix based on vectors (i.e. column vectors)
 * \code blockmatrix(matrixrow{A1,B1,..., Z1}, ..., matrixrow{AN,BN,..., ZN}) \endcode 
 * Here A1,B1, ..., ZN are blocks (i.e. matrices) 
 * The number of elements in each vector must be the same.
 * 
 * Also, in general, the size of A1 is equals to the size of A2 and 
 * equals to the size of A3 and so on (the same it holds for B1,B2,...,BN, 
 * ..., Z1, Z2, ZN) 
 *
 */

class BlockMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;
    
    static const QString id;
    static const Analitza::ExpressionType type;
};

/**
 * \class BlockDiagonalMatrixCommand
 * 
 * \brief Implements the \"blockdiag\" command.
 * 
 * BlockDiagonalMatrixCommand constructs a block diagonal matrix.
 * 
 * \code blockdiag(A, B, ..., Z) \endcode 
 * A, B, ... and Z are the blocks, so they are matrices.
 * 
 */

class BlockDiagonalMatrixCommand: public Analitza::FunctionDefinition
{
public:
    virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args) override;

    static const QString id;
    static const Analitza::ExpressionType type;
};

//TODO block tridiagonal matrices

#endif // BLOCKMATRIXCOMMANDS_H
