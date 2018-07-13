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

#include "matrixcommands.h"

#include <QCoreApplication>

#include "expression.h"
#include "value.h"
#include "matrix.h"
#include "operations.h"

using Analitza::Expression;
using Analitza::ExpressionType;

static const QString MATRIX_SIZE_ERROR_MESSAGE = QCoreApplication::tr("Matrix dimensions must be greater than zero");

//BEGIN FillMatrixConstructor

const QString MatrixCommand::id = QStringLiteral("matrix");
const ExpressionType MatrixCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression MatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    const int nargs = args.size();
    
    switch(nargs) {
        case 0: {
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(MatrixCommand::id));
            
            return ret;
        }    break;
        //BEGIN matrix(m,n,v)
        case 1: { // build square matrix filled with zeros
            if (args.at(0).tree()->type() == Analitza::Object::value) {
                const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
                
                if (nobj->isInteger() && nobj->value() > 0) {
                    ret.setTree(new Analitza::Matrix(nobj->intValue(), nobj->intValue(), new Analitza::Cn(0.)));
                } else
                    ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
                
                return ret;
            }
        }    break;
        case 2: { // build rectangular matrix filled with zeros
            if (args.at(0).tree()->type() == Analitza::Object::value && args.at(1).tree()->type() == Analitza::Object::value) {
                const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
                const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
                
                if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() > 0 && ncolsobj->value() > 0) {
                    ret.setTree(new Analitza::Matrix(nrowsobj->intValue(), ncolsobj->intValue(), new Analitza::Cn(0.)));
                } else
                    ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
                
                return ret;
            }
        }    break;
        case 3: { // build square matrix filled with a fixed value
            if (args.at(0).tree()->type() == Analitza::Object::value && 
                args.at(1).tree()->type() == Analitza::Object::value && 
                args.at(2).tree()->type() == Analitza::Object::value) {
                const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
                const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
                
                if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() > 0 && ncolsobj->value() > 0) {
                    ret.setTree(new Analitza::Matrix(nrowsobj->intValue(), ncolsobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())));
                } else
                    ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
                
                return ret;
            }
        }    break;
        //END matrix(m,n,v)
    }
    
    Q_ASSERT(nargs > 0);
    Q_ASSERT(ret.toString().isEmpty());
    Q_ASSERT(ret.isCorrect());
    
    //BEGIN matrix(vector{...}, ...) and matrix(matrixrow{...}, ...)
    const Analitza::Object::ObjectType firstArgType = args.first().tree()->type();
    
    if (firstArgType == Analitza::Object::vector || firstArgType == Analitza::Object::matrixrow) {
        const bool isVector = (firstArgType == Analitza::Object::vector);
        const Analitza::Vector *firstVector = static_cast<const Analitza::Vector*>(args.first().tree());
        
        if (firstVector->size() > 0) { // we will check this for all vectors later
            const int firstVectorSize = firstVector->size();
            
            bool iscorrect = true; // assumes all are rows
            Analitza::Matrix *matrix = new Analitza::Matrix();
            
            for (int i = 0; i < nargs && iscorrect; ++i) {
                if (args.at(i).tree()->type() == firstArgType)
                {
                    const Analitza::MatrixRow *row = static_cast<const Analitza::MatrixRow*>(args.at(i).tree());
                    
                    if (row->size() == firstVectorSize)
                        matrix->appendBranch(row->copy());
                    else {
                        iscorrect = false;
                        ret.addError(QCoreApplication::tr("All matrixrow elements must have the same size"));
                    }
                }
                else {
                    iscorrect = false;
                    ret.addError(QCoreApplication::tr("Not all are rows or vectors"));
                }
            }
            
            if (!ret.isCorrect())
                delete matrix;
            else if (iscorrect) {
                if (isVector) {
                    QString* error=nullptr;
                    ret.setTree(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, &error));
                    delete matrix;
                
                    Q_ASSERT(error == nullptr);
                } else
                    ret.setTree(matrix);
            } else {
                ret.addError(QCoreApplication::tr("Every argument must be a matrixrow element"));
                delete matrix;
            }
        } else
            ret.addError(QCoreApplication::tr("Do not want empty vectors/matrixrow elements"));
    } else
        ret.addError(QCoreApplication::tr("Matrix constructor needs vectors or matrixrow elements"));
    //END matrix(vector{...}, ...) and matrix(matrixrow{...}, ...)
    
    return ret;
}

//END FillMatrixConstructor


//BEGIN IdentityMatrixConstructor

const QString IdentityMatrixCommand::id = QStringLiteral("identitymatrix");
const ExpressionType IdentityMatrixCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

Expression IdentityMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.first().tree());
    const int n = nobj->value();
    
    if (nobj->isInteger() && n > 0) {
        ret.setTree(Analitza::Matrix::identity(n));
    } else
        ret.addError(MATRIX_SIZE_ERROR_MESSAGE);

    return ret;
}

//END IdentityMatrixConstructor


//BEGIN DiagonalMatrixConstructor

const QString DiagonalMatrixCommand::id = QStringLiteral("diag");
const ExpressionType DiagonalMatrixCommand::type  = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Many, QList<ExpressionType>() 
<< ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1) 
<< ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1)));

Expression DiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    int nargs = args.size();
    bool byvector = false;
    
    switch(nargs) {
        case 0: {
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(DiagonalMatrixCommand::id));
            
            return ret;
        }    break;
        //BEGIN diag(matrix{...}, diagindex)
        case 1: {
            if (args.first().tree()->type() == Analitza::Object::matrix) {
                const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
                const int n = std::min(matrix->rowCount(), matrix->columnCount());
                
                Analitza::Vector *diagonal = new Analitza::Vector(n);
                
                for (int i = 0; i < n; ++i)
                    diagonal->appendBranch(matrix->at(i, i)->copy());
                
                ret.setTree(diagonal);
                
                return ret;
            } else if (args.first().tree()->type() == Analitza::Object::vector)
                byvector = true;
        }    break;
        case 2: {
            if (args.first().tree()->type() == Analitza::Object::matrix && args.last().tree()->type() == Analitza::Object::value) {
                const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
                
                if (nobj->isInteger()) {
                    const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
                    const int nrows = matrix->rowCount();
                    const int ncols = matrix->columnCount();
                    const int npos = nobj->value();
                    const int absnpos = std::abs(npos);
                    const int absnpos1 = absnpos + 1;
                    const bool isneg = npos < 0;
                    
                    int n = 0; // or until/to
                    int rowoffset = 0;
                    int coloffset = 0;
                    
                    if (isneg) {
                        if (absnpos1 > nrows) {
                            ret.addError(QCoreApplication::tr("The nth diagonal index must be less than the row count"));
                            return ret;
                        }
                        
                        n = std::min(nrows - absnpos, ncols);
                        rowoffset = absnpos;
                    } else { // square matrix case too
                        if (absnpos1 > ncols) {
                            ret.addError(QCoreApplication::tr("The nth diagonal index must be less than the column count"));
                            return ret;
                        }
                        
                        n = std::min(nrows, ncols - absnpos);
                        coloffset = absnpos;
                    }
                    
                    Analitza::Vector *diagonal = new Analitza::Vector(n);
                    
                    for (int i = 0; i < n; ++i)
                        diagonal->appendBranch(matrix->at(rowoffset + i, coloffset + i)->copy());
                        
                    ret.setTree(diagonal);
                }
                else
                    ret.addError(QCoreApplication::tr("nth diagonal index must be integer number"));
                
                return ret;
            } else if (args.last().tree()->type() != Analitza::Object::value && args.last().tree()->type() != Analitza::Object::matrix) {
                ret.addError(QCoreApplication::tr("nth diagonal index must be a positive integer number"));
                
                return ret;
            }
        }    break;
        //END diag(matrix{...}, diagindex)
    }
    
    Q_ASSERT(nargs > 0);
    Q_ASSERT(ret.toString().isEmpty());
    Q_ASSERT(ret.isCorrect());
    
    //BEGIN diag(a,b, ...) or diag(vector{a,b, ...})
    const Analitza::Vector *v = byvector? static_cast<const Analitza::Vector*>(args.first().tree()) : nullptr;
    
    if (byvector) nargs = v->size();
    
    for (int k = 0; k < nargs; ++k)
        if ((byvector? v->at(k)->type() : args.at(k).tree()->type()) == Analitza::Object::none) {
            ret.addError(QCoreApplication::tr("the arg %1 is invalid or is error").arg(k+1));
            return ret;
        }
    
    Analitza::Matrix *matrix = new Analitza::Matrix();
    
    for (int i = 0; i < nargs; ++i) {
        Analitza::MatrixRow *row = new Analitza::MatrixRow(nargs);
        
        for (int j = 0; j < nargs; ++j)
            if (i == j)
                row->appendBranch(byvector? v->at(j)->copy() : args.at(j).tree()->copy());
            else
                row->appendBranch(new Analitza::Cn(0));
        
        matrix->appendBranch(row);
    }
    
    ret.setTree(matrix);
    //END diag(a,b, ...) or diag(vector{a,b, ...})
    
    return ret;
}

//END DiagonalMatrixConstructor


//BEGIN TridiagonalMatrixConstructor

const QString TridiagonalMatrixCommand::id = QStringLiteral("tridiag");
const ExpressionType TridiagonalMatrixCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Value))
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));

//TODO create rectangular matrices too 
Expression TridiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
    const int n = nobj->value();
    
    if (nobj->isInteger() && n > 0) {
        Analitza::Matrix *matrix = new Analitza::Matrix();
        
        for (int row = 0; row < n; ++row) {
            Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
            
            for (int col= 0; col < n; ++col)
                if (row == col + 1) // a
                    rowobj->appendBranch(args.at(0).tree()->copy());
                else
                    if (row == col) // b
                        rowobj->appendBranch(args.at(1).tree()->copy());
                    else
                        if (row == col - 1) // c
                            rowobj->appendBranch(args.at(2).tree()->copy());
                        else
                            rowobj->appendBranch(new Analitza::Cn(0));
            
            matrix->appendBranch(rowobj);
        }
        
        ret.setTree(matrix);
    } else
        ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
    
    return ret;
}

//END TridiagonalMatrixConstructor
