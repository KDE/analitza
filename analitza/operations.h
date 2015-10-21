/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "operator.h"
#include <expressiontype.h>

namespace Analitza
{

class None;

class List;
class Cn;
class Vector;
class Matrix;
class CustomObject;
class ExpressionType;

//WARNING this class can lead to some memory leaks ... we need to review all the methods and see if everything goes ok
class Operations
{
    public:
        typedef Object* (*UnaryOp)(Operator::OperatorType, Object*, QString**);
        typedef Object * (*BinaryOp)(Operator::OperatorType op, Object *, Object *, QString** correct);
        
        static Object* reduce(Operator::OperatorType op, Object* oper, Object* oper1, QString** correct);
        static Object* reduceUnary(Operator::OperatorType op, Object* oper, QString** correct);
        
        static QList<ExpressionType> infer(Analitza::Operator::OperatorType op);
        static QList<ExpressionType> inferUnary(Operator::OperatorType op);
        
    private:
        static UnaryOp opsUnary[Object::custom+1];
        static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
        
        static Object* reduceValueNone(Operator::OperatorType op, Cn* oper, None *cntr, QString** correct);
        static Object* reduceNoneValue(Operator::OperatorType op, None *cntr, Cn* oper, QString** correct);
        static Object* reduceValueValue(Operator::OperatorType op, Cn *oper, const Cn* oper1, QString** correct);
        static Object* reduceUnaryValue(Operator::OperatorType op, Cn *oper, QString** correct);
        
        static Object* reduceValueVector(Operator::OperatorType op, Cn *oper, Vector* vector, QString** correct);
        static Object* reduceVectorValue(Operator::OperatorType op, Vector* vector, Cn *oper, QString** correct);
        static Object* reduceVectorVector(Operator::OperatorType op, Vector* v1, Vector* v2, QString** correct);
        static Object* reduceMatrixVector(Operator::OperatorType op, Matrix* matrix, Vector* vector, QString** correct);
        static Object* reduceUnaryVector(Operator::OperatorType op, Vector* c, QString** correct);
        
        static Object* reduceValueList(Operator::OperatorType op, Cn *oper, List* vector, QString** correct);
        static Object* reduceListList(Operator::OperatorType op, List* l1, List* l2, QString** correct);
        static Object* reduceUnaryList(Operator::OperatorType op, List* l, QString** correct);
        
        static Object* reduceValueMatrix(Analitza::Operator::OperatorType op, Analitza::Cn* v, Analitza::Matrix* m1, QString** correct);
        static Object* reduceMatrixValue(Analitza::Operator::OperatorType op, Analitza::Matrix* m1, Analitza::Cn* v, QString** correct);
        static Object* reduceVectorMatrix(Operator::OperatorType op, Vector* vector, Matrix* matrix, QString** correct);
        static Object* reduceMatrixMatrix(Operator::OperatorType op, Matrix* m1, Matrix* m2, QString** correct);
        static Object* reduceUnaryMatrix(Analitza::Operator::OperatorType op, Analitza::Matrix* m, QString** correct);
        static Object* reduceMatrixNone(Operator::OperatorType op, Matrix* m, None *cntr, QString** correct);
        static Object* reduceNoneMatrix(Operator::OperatorType op, None *cntr, Matrix* m, QString** correct);
        
        static Object* reduceCustomCustom(Operator::OperatorType op, CustomObject* v1, CustomObject* v2, QString** correct);
        
        static Object* errorCase(const QString& error, QString** correct); // errorcntr is none None
};

}
#endif
