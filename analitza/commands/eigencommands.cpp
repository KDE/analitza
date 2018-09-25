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

#include "eigencommands.h"

#include <QCoreApplication>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include "expression.h"
#include "value.h"
#include "list.h"
#include "matrix.h"

using Analitza::Expression;
using Analitza::ExpressionType;

static const Eigen::MatrixXcd executeEigenSolver(const QList< Analitza::Expression >& args, bool computeEigenvectors, QStringList &errors)
{
    const int nargs = args.size();
    
    if (nargs != 1) {
        errors.append(QCoreApplication::tr("Invalid parameter count for '%1'. Should have 1 parameter").arg(EigenvaluesCommand::id));
        
        return Eigen::MatrixXcd();
    }
    
    const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
    
    if (!matrix->isSquare()) {
        errors.append(QCoreApplication::tr("To use '%1' command the input matrix must be square").arg(EigenvaluesCommand::id));
        
        return Eigen::MatrixXcd();
    }
    
    const int m = matrix->rowCount();
    const int n = matrix->columnCount();
    
    Eigen::MatrixXd realmatrix(m, n);
    
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix->at(i,j)->type() == Analitza::Object::value) {
                const Analitza::Cn *entry = static_cast<const Analitza::Cn*>(matrix->at(i,j));
                const Analitza::Cn::ValueFormat entryformat = entry->format();
                
                //Don't allow complex numbers
                if (entryformat == Analitza::Cn::Char || entryformat == Analitza::Cn::Real || 
                    entryformat == Analitza::Cn::Integer || entryformat == Analitza::Cn::Boolean) {
                        realmatrix(i,j) = entry->value();
                } else {
                    errors.append(QCoreApplication::tr("Invalid parameter type in matrix entry (%1,%2) for '%3', it must be a number value")
                                 .arg(i).arg(j).arg(EigenvaluesCommand::id));
                    
                    return Eigen::MatrixXcd();
                }
            } else {
                errors.append(QCoreApplication::tr("Invalid parameter type in matrix entry (%1,%2) for '%3', it must be a number value")
                .arg(i).arg(j).arg(EigenvaluesCommand::id));
                
                return Eigen::MatrixXcd();
            }
        }
    }
    
    Q_ASSERT(nargs > 0);
    
    Eigen::EigenSolver<Eigen::MatrixXd> eigensolver;
    eigensolver.compute(realmatrix, computeEigenvectors);
    
    Eigen::MatrixXcd ret;
    
    if (computeEigenvectors)
        ret = eigensolver.eigenvectors();
    else
        ret = eigensolver.eigenvalues();
    
    return ret;
}

const QString EigenvaluesCommand::id = QStringLiteral("eigenvalues");
const ExpressionType EigenvaluesCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any, ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1)))
.addParameter(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Value)));

//TODO complex values as matrix entries too
//TODO support endomorphisms over Rn too
Expression EigenvaluesCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    QStringList errors;
    const Eigen::MatrixXcd eigeninfo = executeEigenSolver(args, false, errors);
        
    if (!errors.isEmpty()) {
        ret.addError(errors.first());
        
        return ret;
    }
    const int neigenvalues = eigeninfo.rows();
    
    Analitza::List *list = new Analitza::List;
    
    for (int i = 0; i < neigenvalues; ++i) {
        const std::complex<double> eigenvalue = eigeninfo(i);
        const double realpart = eigenvalue.real();
        const double imagpart = eigenvalue.imag();
        
        if (std::isnan(realpart) || std::isnan(imagpart)) {
            ret.addError(QCoreApplication::tr("Returned eigenvalue is NaN", "NaN means Not a Number, is an invalid float number"));
            
            delete list;
            
            return ret;
        } else if (std::isinf(realpart) || std::isinf(imagpart)) {
            ret.addError(QCoreApplication::tr("Returned eigenvalue is too big"));
            
            delete list;
            
            return ret;
        } else {
            bool isonlyreal = true;
            
            if (std::isnormal(imagpart)) {
                isonlyreal = false;
            }
            
            Analitza::Cn * eigenvalueobj = nullptr;
            
            if (isonlyreal) {
                eigenvalueobj = new Analitza::Cn(realpart);
            } else {
                eigenvalueobj = new Analitza::Cn(realpart, imagpart);
            }
            
            list->appendBranch(eigenvalueobj);
        }
    }
    
    ret.setTree(list);
    
    return ret;
}


const QString EigenvectorsCommand::id = QStringLiteral("eigenvectors");
const ExpressionType EigenvectorsCommand::type = EigenvaluesCommand::type;

//TODO complex values as matrix entries too
//TODO support endomorphisms over Rn too
Expression EigenvectorsCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    QStringList errors;
    const Eigen::MatrixXcd eigeninfo = executeEigenSolver(args, true, errors);
    
    if (!errors.isEmpty()) {
        ret.addError(errors.first());
        
        return ret;
    }
    const int neigenvectors = eigeninfo.rows();
    
    QScopedPointer<Analitza::List> list(new Analitza::List);
    
    for (int j = 0; j < neigenvectors; ++j) {
        const Eigen::VectorXcd col = eigeninfo.col(j);
        QScopedPointer<Analitza::Vector> eigenvector(new Analitza::Vector(neigenvectors));
        
        for (int i = 0; i < neigenvectors; ++i) {
            const std::complex<double> eigenvalue = col(i);
            const double realpart = eigenvalue.real();
            const double imagpart = eigenvalue.imag();
            
            if (std::isnan(realpart) || std::isnan(imagpart)) {
                ret.addError(QCoreApplication::tr("Returned eigenvalue is NaN", "NaN means Not a Number, is an invalid float number"));
                return ret;
            } else if (std::isinf(realpart) || std::isinf(imagpart)) {
                ret.addError(QCoreApplication::tr("Returned eigenvalue is too big"));
                return ret;
            } else {
                auto eigenvalueobj = !std::isnormal(imagpart) ?
                                            new Analitza::Cn(realpart)
                                        : new Analitza::Cn(realpart, imagpart);
                
                eigenvector->appendBranch(eigenvalueobj);
            }
        }
        
        list->appendBranch(eigenvector.take());
    }
    
    ret.setTree(list.take());
    
    return ret;
}
