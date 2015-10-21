/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef ABSTRACTEXPRESSIONVISITOR_H
#define ABSTRACTEXPRESSIONVISITOR_H

#include "analitzaexport.h"

#include <QVariant>

namespace Analitza
{
class Object;
class None;
class Apply;
class Ci;
class Cn;
class CustomObject;
class Container;
class Operator;
class Vector;
class List;
class Matrix;
class MatrixRow;

/**
 * \class AbstractExpressionWriter
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This interface defines the generic expression writer contract.
 */

class ANALITZA_EXPORT AbstractExpressionVisitor
{
    public:
        virtual ~AbstractExpressionVisitor();
        
        virtual QVariant visit(const None* var) = 0;
        virtual QVariant visit(const Operator* var) = 0;
        virtual QVariant visit(const Ci* var) = 0;
        virtual QVariant visit(const Cn* var) = 0;
        virtual QVariant visit(const Container* var) = 0;
        virtual QVariant visit(const Vector* var) = 0;
        virtual QVariant visit(const List* l) = 0;
        virtual QVariant visit(const Apply* a) = 0;
        virtual QVariant visit(const CustomObject* c) = 0;
        virtual QVariant visit(const Matrix* c) = 0;
        virtual QVariant visit(const MatrixRow* c) = 0;
        
        virtual QVariant result() const = 0;
};

}

#endif
