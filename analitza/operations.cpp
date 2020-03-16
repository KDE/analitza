/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "operations.h"

#include <math.h>

#include <cmath>
#include <complex>
#include <QCoreApplication>

#include "value.h"
#include "vector.h"
#include "expression.h"
#include "list.h"
#include "expressiontypechecker.h"
#include "customobject.h"
#include "matrix.h"
#include "container.h"
#include "additionchains.h"

using namespace std;
using namespace Analitza;

Object* reduceRealReal(enum Operator::OperatorType op, Cn *oper, double a, double b, QString** correct)
{
    Object *ret = oper;
    switch(op) {
        case Operator::plus:
            oper->setValue(a+b);
            break;
        case Operator::times:
            oper->setValue(a*b);
            break;
        case Operator::divide:
            if(Q_LIKELY(b!=0.))
                oper->setValue(a / b);
            else {
                *correct=new QString(QCoreApplication::tr("Cannot divide by 0."));
                ret=new None();
            }
            break;
        case Operator::minus:
            oper->setValue(a - b);
            break;
        case Operator::power:
            if (b<1 && b>-1 && a<0) {
                oper->setValue(pow(complex<double>(a), complex<double>(b)));
            } else {
                oper->setValue(b==2 ? a*a : pow(a, b));
            }
            break;
        case Operator::rem:
            if(Q_LIKELY(floor(b)!=0.))
                oper->setValue(int(remainder(a, b)));
            else {
                *correct=new QString(QCoreApplication::tr("Cannot calculate the remainder on 0."));
                ret=new None();
            }
            break;
        case Operator::quotient:
            oper->setValue(int(floor(a / b)));
            break;
        case Operator::factorof: {
            int fb = int(floor(b));
            if(Q_LIKELY(fb!=0))
                oper->setValue((int(floor(a)) % fb)==0);
            else {
                *correct=new QString(QCoreApplication::tr("Cannot calculate the factor on 0."));
                ret=new None();
            }
        }    break;
        case Operator::min:
            oper->setValue(a < b? a : b);
            break;
        case Operator::max:
            oper->setValue(a > b? a : b);
            break;
        case Operator::gt:
            oper->setValue(a > b);
            break;
        case Operator::lt:
            oper->setValue(a < b);
            break;
        case Operator::eq:
            oper->setValue(a == b);
            break;
        case Operator::approx:
            oper->setValue(fabs(a-b)<0.001);
            break;
        case Operator::neq:
            oper->setValue(a != b);
            break;
        case Operator::geq:
            oper->setValue(a >= b);
            break;
        case Operator::leq:
            oper->setValue(a <= b);
            break;
        case Operator::_and:
            oper->setValue(a && b);
            break;
        case Operator::_or:
            oper->setValue(a || b);
            break;
        case Operator::_xor:
            oper->setValue((a || b) && !(a&&b));
            break;
        case Operator::implies:
            oper->setValue(a || !b);
            break;
        case Operator::gcd:  {
            //code by michael cane aka kiko :)
            int ia=floor(a), ib=floor(b), residu = 0;
            while (ib > 0) {
                residu = ia % ib;
                ia = ib;
                ib = residu;
            }
            oper->setValue(ia);
        }    break;
        case Operator::lcm:
            //code by michael cane aka kiko :)
            if(Q_UNLIKELY(floor(a)==0. || floor(b)==0.)) {
                *correct=new QString(QCoreApplication::tr("Cannot calculate the lcm of 0."));
                ret=new None();
            }
            else {
                int ia=floor(a), ib=floor(b);
                int ic=ia*ib, residu = 0;
                while (ib > 0) {
                    residu = ia % ib;
                    ia = ib;
                    ib = residu;
                }
                ia=ic/ia;
                oper->setValue(ia);
            }
            break;
        case Operator::root:
            oper->setValue(              b==2.0 ? sqrt(a)
            : (b>1 || b<-1) && a<0 ? pow(complex<double>(a), complex<double>(1./b)).real()
            : pow(a, 1.0/b));
            break;
        default:
            break;
    }
    return ret;
}

static bool operator<(complex<double> a, complex<double> b)
{ return a.real() < b.real() || (a.real() == b.real() && a.imag()<b.imag()); }

static bool operator>(complex<double> a, complex<double> b)
{ return a.real() > b.real() || (a.real() == b.real() && a.imag()>b.imag()); }

static bool operator<=(complex<double> a, complex<double> b)
{ return a.real() <= b.real() || (a.real() == b.real() && a.imag()<=b.imag()); }

static bool operator>=(complex<double> a, complex<double> b)
{ return a.real() >= b.real() || (a.real() == b.real() && a.imag()>=b.imag()); }

Cn* reduceComplexComplex(enum Operator::OperatorType op, Cn *oper, complex<double> a, complex<double> b, QString** correct)
{
    switch(op) {
        case Operator::plus:
            oper->setValue(a+b);
            break;
        case Operator::times:
            oper->setValue(a*b);
            break;
        case Operator::divide:
            if(Q_LIKELY(b.real()!=0. || b.imag()!=0.))
                oper->setValue(a / b);
            else
                *correct=new QString(QCoreApplication::tr("Cannot divide by 0."));
            break;
        case Operator::minus:
            oper->setValue(a - b);
            break;
        case Operator::power:
            oper->setValue(pow(a, b));
            break;
        case Operator::rem:
            if(Q_LIKELY(floor(b.real())!=0.))
                oper->setValue(int(remainder(a.real(), b.real())));
            else
                *correct=new QString(QCoreApplication::tr("Cannot calculate the remainder on 0."));
            break;
        case Operator::quotient:
            oper->setValue(int(floor((a / b).real())));
            break;
        case Operator::factorof: {
            int fb = int(floor(b.real()));
            if(Q_LIKELY(fb!=0))
                oper->setValue((int(floor(a.real())) % fb)==0);
            else
                *correct=new QString(QCoreApplication::tr("Cannot calculate the factor on 0."));
        }    break;
        case Operator::min:
            oper->setValue(a < b ? a : b);
            break;
        case Operator::max:
            oper->setValue(a > b? a : b);
            break;
        case Operator::gt:
            oper->setValue(a > b);
            break;
        case Operator::lt:
            oper->setValue(a < b);
            break;
        case Operator::eq:
            oper->setValue(a == b);
            break;
        case Operator::approx:
            oper->setValue(fabs(a.real()-b.real())+fabs(a.imag()-b.imag())<0.001);
            break;
        case Operator::neq:
            oper->setValue(a != b);
            break;
        case Operator::geq:
            oper->setValue(a >= b);
            break;
        case Operator::leq:
            oper->setValue(a <= b);
            break;
        case Operator::_and:
        case Operator::_or:
        case Operator::_xor:
        case Operator::implies:
            *correct = new QString(QCoreApplication::tr("Boolean operations on complex numbers not available"));
            break;
        case Operator::gcd:  {
            //code by michael cane aka kiko :)
            int ia=floor(a.real()), ib=floor(b.real()), residu = 0;
            while (ib > 0) {
                residu = ia % ib;
                ia = ib;
                ib = residu;
            }
            oper->setValue(ia);
        }    break;
        case Operator::lcm:
            //code by michael cane aka kiko :)
            if(Q_UNLIKELY(floor(a.real())==0. || floor(b.real())==0.))
                *correct=new QString(QCoreApplication::tr("Cannot calculate the lcm of 0."));
            else {
                int ia=floor(a.real()), ib=floor(b.real());
                int ic=ia*ib, residu = 0;
                while (ib > 0) {
                    residu = ia % ib;
                    ia = ib;
                    ib = residu;
                }
                ia=ic/ia;
                oper->setValue(ia);
            }
            break;
        case Operator::root:
            if(b.real()!=2.0 && b.imag()==0)
                *correct = new QString(QCoreApplication::tr("Only square root implemented for complex numbers"));
            else
                oper->setValue(sqrt(a));
            break;
        default:
            break;
    }
    return oper;
}

Object* Operations::reduceValueValue(enum Operator::OperatorType op, Cn *oper, const Cn *oper1, QString** correct)
{
    if(Q_UNLIKELY(oper->isComplex() || oper1->isComplex())) {
        const complex<double> a=oper->complexValue(), b=oper1->complexValue();
        return reduceComplexComplex(op, oper, a, b, correct);
    } else {
        const double a=oper->value(), b=oper1->value();
        return reduceRealReal(op, oper, a, b, correct);
    }
}

Cn* reduceUnaryComplex(Operator::OperatorType op, Cn* oper, QString** correct)
{
    const complex<double> a=oper->complexValue();

    switch(op) {
        case Operator::minus:
                 oper->setValue(-a);
            break;
        case Operator::sin:
                 oper->setValue(sin(a));
            break;
        case Operator::cos:
                 oper->setValue(cos(a));
            break;
        case Operator::tan:
                 oper->setValue(tan(a));
            break;
        case Operator::sinh:
                 oper->setValue(sinh(a));
            break;
        case Operator::cosh:
                 oper->setValue(cosh(a));
            break;
        case Operator::tanh:
                 oper->setValue(tanh(a));
            break;
        case Operator::coth:
                 oper->setValue(cosh(a)/sinh(a));
            break;
        case Operator::exp:
                 oper->setValue(exp(a));
            break;
        case Operator::ln:
                 oper->setValue(log(a));
            break;
        case Operator::log:
                 oper->setValue(log10(a));
            break;
        case Operator::abs:
                 oper->setValue(std::abs(a));
            break;
        case Operator::conjugate:
                 oper->setValue(std::conj(a));
            break;
            case Operator::arg:
                    oper->setValue(std::arg(a));
            break;
            case Operator::real:
                    oper->setValue(a.real());
            break;
            case Operator::imaginary:
                    oper->setValue(a.imag());
            break;
        default:
            *correct=new QString(QCoreApplication::tr("Could not calculate a value %1").arg(Operator(op).toString()));
    }
    return oper;
}

Cn* reduceUnaryReal(Operator::OperatorType op, Cn* oper, QString** correct)
{
    const double a=oper->value();
    
    switch(op) {
        case Operator::minus:
                 oper->setValue(-a);
            break;
        case Operator::factorial: {
            //Use gamma from math.h?
            uint res=1;
            for(int i=a; i>1.; i--) {
                res*=floor(i);
            }
                 oper->setValue(res);
        }    break;
        case Operator::sin:
                 oper->setValue(sin(a));
            break;
        case Operator::cos:
                 oper->setValue(cos(a));
            break;
        case Operator::tan:
                 oper->setValue(tan(a));
            break;
        case Operator::sec:
                 oper->setValue(1./cos(a));
            break;
        case Operator::csc:
                 oper->setValue(1./sin(a));
            break;
        case Operator::cot:
                 oper->setValue(1./tan(a));
            break;
        case Operator::sinh:
                 oper->setValue(sinh(a));
            break;
        case Operator::cosh:
                 oper->setValue(cosh(a));
            break;
        case Operator::tanh:
                 oper->setValue(tanh(a));
            break;
        case Operator::sech:
                 oper->setValue(1.0/cosh(a));
            break;
        case Operator::csch:
                 oper->setValue(1.0/sinh(a));
            break;
        case Operator::coth:
                 oper->setValue(cosh(a)/sinh(a));
            break;
        case Operator::arcsin:
                 oper->setValue(asin(a));
            break;
        case Operator::arccos:
                 oper->setValue(acos(a));
            break;
        case Operator::arctan:
                 oper->setValue(atan(a));
            break;
        case Operator::arccot:
                 oper->setValue(log(a+pow(a*a+1., 0.5)));
            break;
        case Operator::arcsinh: //see https://en.wikipedia.org/wiki/Inverse_hyperbolic_function
                 oper->setValue(asinh(a));
            break;
        case Operator::arccosh:
                 oper->setValue(acosh(a));
            break;
        case Operator::arccsc:
                 oper->setValue(1/asin(a));
            break;
        case Operator::arccsch:
                 oper->setValue(log(1/a+sqrt(1/(a*a)+1)));
            break;
        case Operator::arcsec:
                 oper->setValue(1/(acos(a)));
            break;
        case Operator::arcsech:
                 oper->setValue(log(1/a+sqrt(1/a+1)*sqrt(1/a-1)));
            break;
        case Operator::arctanh:
                 oper->setValue(atanh(a));
            break;
        case Operator::exp:
                 oper->setValue(exp(a));
            break;
        case Operator::ln:
                 oper->setValue(log(a));
            break;
        case Operator::log:
                 oper->setValue(log10(a));
            break;
        case Operator::abs:
                 oper->setValue(a>=0. ? a : -a);
            break;
        case Operator::floor:
                 oper->setValue(floor(a));
            break;
        case Operator::ceiling:
                 oper->setValue(ceil(a));
            break;
        case Operator::_not:
                 oper->setValue(!a);
            break;
        default:
            *correct=new QString(QCoreApplication::tr("Could not calculate a value %1").arg(Operator(op).toString()));
            break;
    }
    return oper;
}

Object* Operations::reduceUnaryValue(Operator::OperatorType op, Cn* oper, QString** correct)
{
    if(Q_UNLIKELY(oper->isComplex()))
        return reduceUnaryComplex(op, oper, correct);
    else
        return reduceUnaryReal(op, oper, correct);
}

Object* Operations::reduceNoneValue(Operator::OperatorType op, None*, Cn*, QString** correct)
{
    return errorCase(QCoreApplication::tr("Cannot calculate %1 between a value and an error type").arg(Operator(op).name()), correct);
}

Object* Operations::reduceValueNone(Operator::OperatorType op, Cn* oper, None* cntr, QString** correct)
{
    return reduceNoneValue(op, cntr, oper, correct);
}

Object * Operations::reduceValueVector(Operator::OperatorType op, Cn * oper, Vector * v1, QString** correct)
{
    switch(op) {
        case Operator::selector: {
            int select=oper->intValue();
            delete oper;
            Object* ret=nullptr;
            if(select<1 || (select-1) >= v1->size()) {
                *correct=new QString(QCoreApplication::tr("Invalid index for a container"));
                ret=new None();
            } else {
                ret=v1->at(select-1)->copy();
            }
            return ret;
        }    break;
        default: {
            Vector *ret = v1->copy();
            for(Vector::iterator it=ret->begin(); it!=ret->end(); ++it)
            {
                *it=reduce(op, new Cn(*oper), *it, correct);
            }
            
            delete oper;
            return ret;
        }
    }
}

Object * Operations::reduceVectorValue(Operator::OperatorType op, Vector * v1, Cn * oper, QString** correct)
{
    for(Vector::iterator it=v1->begin(); it!=v1->end(); ++it)
    {
        *it=reduce(op, *it, new Cn(*oper), correct);
    }
    return v1;
}

Object * Operations::reduceVectorVector(Operator::OperatorType op, Vector * v1, Vector * v2, QString** correct)
{
    if(v1->size()!=v2->size()) { //FIXME: unneeded? ... aucahuasi: I think is needed ...
        *correct=new QString(QCoreApplication::tr("Cannot operate '%1' on different sized vectors.").arg(Operator(op).name()));
        return new None();
    }
    
    if(op==Operator::scalarproduct)
        op=Operator::times;
    Vector::iterator it2=v2->begin();
    for(Vector::iterator it1=v1->begin(); it1!=v1->end(); ++it1, ++it2)
    {
        *it1=reduce(op, *it1, *it2, correct);
    }
    return v1;
}

Object* Operations::reduceMatrixVector(Operator::OperatorType op, Matrix* matrix, Vector* vector, QString** correct)
{
    Object* ret = nullptr;
    if (op == Operator::times) {
        if (matrix->hasOnlyNumbers() && vector->hasOnlyNumbers()) {
            const int maxk = matrix->columnCount();
            
            if (maxk == vector->size()) {
                const int m = matrix->rowCount();
                
                Vector *newvec = new Vector(m);
                
                for (int i = 0; i < m; ++i) {
                    std::complex<double> sum = 0.;
                    for (int k = 0; k < maxk; ++k)
                        sum += static_cast<const Cn*>(reduceValueValue(op, (Cn*)matrix->at(i,k), (Cn*)vector->at(k), correct))->complexValue();
                    
                    if (sum.imag() == 0)
                        newvec->appendBranch(new Cn(sum.real()));
                    else
                        newvec->appendBranch(new Cn(sum.real(), sum.imag()));
                }
                
                ret = newvec;
            } else {
                *correct=new QString(QCoreApplication::tr("Multiplication between a matrix and a vector is allowed provided that the number of columns of the matrix equals the size of the vector"));
                ret=new None();
            }
        } else {
            *correct=new QString(QCoreApplication::tr("Matrix and vector entries must be numbers"));
            ret=new None();
        }
    }
    
    return ret;
}

Object* Operations::reduceUnaryVector(Operator::OperatorType op, Vector* c, QString** correct)
{
    Object *ret=nullptr;
    switch(op) {
        case Operator::card:
            ret=new Cn(c->size());
            break;
        case Operator::transpose: {
            const int n = c->size();
            
            Matrix* mret = new Matrix;
            MatrixRow* row = new MatrixRow(n);
            for(int j=0; j<n; ++j) {
                row->appendBranch(c->at(j)->copy());
            }
            
            mret->appendBranch(row);
            ret = mret;
        }    break;
        default:
            //Should be dealt by typechecker. not necessary
            *correct=new QString(QCoreApplication::tr("Could not calculate a vector's %1").arg(Operator(op).toString()));
            ret=new None();
            break;
    }
    delete c;
    return ret;
}

Object* Operations::reduceListList(Operator::OperatorType op, List* l1, List* l2, QString** correct)
{
    Object* ret=nullptr;
    switch(op) {
        case Operator::_union: {
            List::iterator itEnd=l2->end();
            for(List::iterator it=l2->begin(); it!=itEnd; ++it) {
                l1->appendBranch((*it)->copy());
            }
            
            ret=l1;
        }    break;
        default:
            //Should be dealt by typechecker. not necessary
            *correct=new QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
            delete l1;
            ret=new None();
            break;
    }
    return ret;
}

Object* Operations::reduceUnaryList(Operator::OperatorType op, List* l, QString** correct)
{
    Object *ret=nullptr;
    switch(op) {
        case Operator::card:
            ret=new Cn(l->size());
            break;
        default:
            *correct=new QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
            ret=new None();
            break;
    }
    delete l;
    return ret;
}

Object* Operations::reduceValueList(Operator::OperatorType op, Cn* oper, List* v1, QString** correct)
{
    switch(op) {
        case Operator::selector: {
            int select=oper->intValue();
            Object* ret=nullptr;
            if(select<1 || (select-1) >= v1->size()) {
                *correct=new QString(QCoreApplication::tr("Invalid index for a container"));
                ret=new None();
            } else {
                ret=v1->at(select-1);
                v1->setAt(select-1, nullptr);
            }
            delete oper;
            return ret;
        }    break;
        default:
            break;
    }
    return nullptr;
}

Object* Operations::reduceCustomCustom(Operator::OperatorType op, CustomObject* v1, CustomObject* v2, QString** )
{
    switch(op) {
        case Operator::neq:
            return new Cn(v1->value()!=v2->value());
        case Operator::eq:
            return new Cn(v1->value()==v2->value());
        default:
            break;
    }
    
    Q_ASSERT(false && "not implemented, please report");
    return nullptr;
}

Object* Operations::reduceVectorMatrix(Operator::OperatorType op, Vector* vector, Matrix* matrix, QString** correct)
{
    Object* ret = nullptr;
    if (op == Operator::times) {
        if (vector->hasOnlyNumbers() && matrix->hasOnlyNumbers()) {
            if (1 == matrix->rowCount()) {
                const int m = vector->size();
                const int n = matrix->columnCount();
                
                Matrix *newmat = new Matrix();
                
                for (int i = 0; i < m; ++i) {
                    MatrixRow *row = new MatrixRow(n);
                    for (int j = 0; j < n; ++j)
                        row->appendBranch(reduceValueValue(op, (Cn*)vector->at(i), (Cn*)matrix->at(0,j), correct)->copy());
                    
                    newmat->appendBranch(row);
                }
                
                ret = newmat;
            } else {
                *correct=new QString(QCoreApplication::tr("Multiplication between a vector and a matrix is allowed provided that the matrix has only one matrixrow element"));
                ret=new None();
            }
        } else {
            *correct=new QString(QCoreApplication::tr("Matrix and vector entries must be numbers"));
            ret=new None();
        }
    }
    
    return ret;
}

Object* Operations::reduceMatrixMatrix(Operator::OperatorType op, Matrix* m1, Matrix* m2, QString** correct)
{
    Object* ret = nullptr;
    switch(op) {
        //TODO see if we can use here and or xor for matrix too
        case Operator::plus:
        case Operator::minus: {
            if(m1->rowCount() == m2->rowCount() && m1->columnCount() == m2->columnCount()) {
                Matrix::iterator it2=m2->begin();
                for(Matrix::iterator it1=m1->begin(); it1!=m1->end(); ++it1, ++it2)
                {
                    *it1 = static_cast<MatrixRow*>(reduceVectorVector(op, *it1, *it2, correct));
                }
                ret = m1;
            } else {
                *correct=new QString(QCoreApplication::tr("Addition of two matrices is allowed provided that both matrices have the same number of rows and the same number of columns"));
                ret=new None();
            }
        }    break;
        case Operator::times: {
            if (m1->hasOnlyNumbers() && m2->hasOnlyNumbers()) {
                const int maxk = m1->columnCount();
                if (maxk == m2->rowCount()) {
                    const int m = m1->rowCount();
                    const int n = m2->columnCount();
                    
                    Matrix *matrix = new Matrix();
                    
                    for (int i = 0; i < m; ++i) {
                        MatrixRow *row = new MatrixRow(n);
                        for (int j = 0; j < n; ++j) {
                            std::complex<double> sum = 0;
                            for (int k = 0; k < maxk; ++k) {
                                sum += static_cast<const Cn*>(reduceValueValue(op, (Cn*)m1->at(i,k)->copy(), (Cn*)m2->at(k,j)->copy(), correct))->complexValue();
                            }
                            
                            if (sum.imag() == 0)
                                row->appendBranch(new Cn(sum.real()));
                            else
                                row->appendBranch(new Cn(sum.real(), sum.imag()));
                        }
                        
                        matrix->appendBranch(row);
                    }
                    
                    ret = matrix;
                } else {
                    *correct=new QString(QCoreApplication::tr("Multiplication of two matrices is allowed provided that the number of columns of the first matrix equals the number of rows of the second matrix"));
                    ret=new None();
                }
            } else {
                *correct=new QString(QCoreApplication::tr("Matrix entries must be numbers"));
                ret=new None();
            }
        }    break;
        default:
            break;
    }
    
    return ret;
}

Object* Operations::reduceValueMatrix(Operator::OperatorType op, Cn* v, Matrix* m1, QString** correct)
{
    Object* ret = nullptr;
    switch(op) {
        case Operator::selector: {
            int select=v->intValue();
            if(select<1 || (select-1) >= m1->rowCount()) {
                *correct=new QString(QCoreApplication::tr("Invalid index for a container"));
                ret=new None();
            } else {
                MatrixRow* row = static_cast<MatrixRow*>(m1->rows().at(select-1));
                Vector* nv = new Vector(row->size());
                for(Vector::iterator it=row->begin(); it!=row->end(); ++it) {
                    nv->appendBranch((*it));
                    *it = nullptr;
                }
                ret = nv;
            }
            delete v;
        }    break;
        case Operator::times: {
            if (m1->hasOnlyNumbers()) {
                Matrix *nm = new Matrix();
                for(Matrix::iterator it=m1->begin(); it!=m1->end(); ++it)
                    nm->appendBranch(static_cast<MatrixRow*>(reduceValueVector(op, static_cast<Cn*>(v->copy()), static_cast<MatrixRow*>(*it), correct)));
                ret = nm;
            } else {
                *correct=new QString(QCoreApplication::tr("Matrix entries must be numbers"));
                ret=new None();
            }
        }    break;
        default:
            break;
    }
    return ret;
}

Object* Operations::reduceMatrixValue(Operator::OperatorType op, Matrix* matrix, Cn* value, QString** correct)
{
    Object* ret = nullptr;
    switch(op) {
        case Operator::power: {
            if (matrix->hasOnlyNumbers()) {
                if (matrix->isSquare()) {
                    if (value->isInteger()) {
                        const int exp = qAbs(value->intValue());
                        
                        switch(exp) {
                            case 0: ret = Matrix::identity(matrix->rowCount()); break;
                            case 1: ret = matrix; break;
                            default: {
                                //base = value->intValue() < 0? invert(matrix) : matrix;//TODO negative exponents
                                Matrix *base = matrix;
                                
                                if (exp <= MAX_EXPONENT) { // then: use Addition-chain exponentiation
                                    const int len = additionChains[exp][0];
                                    int i, j, k;
                                    QVector<Matrix*> products(len+1);
                                    products[0] = base;
                                    
                                    if (exp>1)
                                        products[1] = static_cast<Matrix*>(reduceMatrixMatrix(Operator::times, matrix, matrix, correct));
                                    
                                    //NOTE see https://rosettacode.org/wiki/Addition-chain_exponentiation#C for more details
                                    for (i = 2; i <= len; ++i) 
                                        for (j = i - 1; j; --j) 
                                            for (k = j; k >= 0; --k) 
                                            {
                                                if (additionChains[exp][k+1] + additionChains[exp][j+1] < additionChains[exp][i+1]) break;
                                                if (additionChains[exp][k+1] + additionChains[exp][j+1] > additionChains[exp][i+1]) continue;
                                                products[i] = static_cast<Matrix*>(reduceMatrixMatrix(Operator::times, products[j], products[k], correct));
                                                j = 1;
                                                break;
                                            }
                                    ret = products[len];
                                    
                                    //NOTE free the memory, except products[len]
                                    for (i = 0; i < len; ++i)
                                        delete products[i];
                                } else { // else: use Exponentiation by squaring
                                    Matrix *product = Matrix::identity(base->rowCount());
                                    Matrix *newbase = base->copy();
                                    int n = exp;
                                    
                                    while (n != 0) {
                                        if (n % 2 != 0) {
                                            Matrix *oldproduct = product;
                                            product = static_cast<Matrix*>(reduceMatrixMatrix(Operator::times, product, newbase, correct));
                                            delete oldproduct;
                                            --n;
                                        }
                                        Matrix *oldbase = newbase;
                                        newbase = static_cast<Matrix*>(reduceMatrixMatrix(Operator::times, newbase, newbase, correct));
                                        delete oldbase;
                                        n /= 2;
                                    }
                                    
                                    ret = product;
                                }
                            }    break;
                        }
                    } else {
                        *correct=new QString(QCoreApplication::tr("The exponent of 'power' must be some integer number"));
                        ret=new None();
                    }
                } else {
                    *correct=new QString(QCoreApplication::tr("Cannot compute 'power' for non square matrix"));
                    ret=new None();
                }
            } else {
                *correct=new QString(QCoreApplication::tr("Matrix entries must be numbers"));
                ret=new None();
            }
        }    break;
        //TODO root
        default:
            break;
    }
    return ret;
}

Object* Operations::reduceUnaryMatrix(Operator::OperatorType op, Matrix* m, QString** )
{
    Object* ret = nullptr;
    switch(op) {
        case Operator::transpose: {
            int sizeA = m->rowCount(), sizeB = static_cast<MatrixRow*>(*m->constBegin())->size();
            Matrix* mret = new Matrix;
            for(int i=0; i<sizeB; ++i) {
                MatrixRow* row = new MatrixRow(sizeA);
                for(int j=0; j<sizeA; ++j) {
                    row->appendBranch(m->at(j, i)->copy());
                }
                mret->appendBranch(row);
            }
            ret = mret;
        }    break;
        default:
            break;
    }
    return ret;
}

Object* Operations::reduceMatrixNone(Operator::OperatorType op, Matrix*, None*, QString** correct)
{
    return errorCase(QCoreApplication::tr("Cannot calculate %1 between a matrix and an error type").arg(Operator(op).name()), correct);
}

Object* Operations::reduceNoneMatrix(Operator::OperatorType op, None* cntr, Matrix* m, QString** correct)
{
    return reduceMatrixNone(op, m, cntr, correct);
}

ExpressionType TypeTriplet(const ExpressionType& a,const ExpressionType& b,const ExpressionType& c) { return ExpressionType(ExpressionType::Lambda).addParameter(a).addParameter(b).addParameter(c); }

//TODO: test that there's one output per input
QList<ExpressionType> Operations::infer(Operator::OperatorType op)
{
    QList<ExpressionType> ret;
    
    switch(op) {
        case Operator::plus:
        case Operator::minus:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
            break;
        case Operator::divide:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            break;
        case Operator::times:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), 1),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
            break;
        case Operator::eq:
        case Operator::neq:
            ret << TypeTriplet(ExpressionType(ExpressionType::Any, 1),
                               ExpressionType(ExpressionType::Any, 1),
                               ExpressionType(ExpressionType::Bool));
            break;
        case Operator::scalarproduct:
            ret << TypeTriplet(
                                ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                                ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                                ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
            break;
        case Operator::power:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypeTriplet(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1), -1),
                               ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1), -1));
            break;
        case Operator::rem:
        case Operator::quotient:
        case Operator::factorof:
        case Operator::min:
        case Operator::max:
        case Operator::gcd:
        case Operator::lcm:
        case Operator::root:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            break;
        case Operator::gt:
        case Operator::lt:
        case Operator::approx:
        case Operator::geq:
        case Operator::leq:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Bool));
            break;
        case Operator::_and:
        case Operator::_or:
        case Operator::_xor:
        case Operator::implies:
            ret << TypeTriplet(ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool));
            break;
        case Operator::selector:
            ret << TypeTriplet(ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1),
                               ExpressionType(ExpressionType::Any, 1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
                               ExpressionType(ExpressionType::Any, 1));
            ret << TypeTriplet(ExpressionType(ExpressionType::Value),
                               ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), -2),
                               ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
            break;
        case Operator::_union:
            ret << TypeTriplet(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)));
            break;
        case Operator::map:
            ret << TypeTriplet(ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Any, 1)).addParameter(ExpressionType(ExpressionType::Any, 2)),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 2)));
            break;
        case Operator::filter:
            ret << TypeTriplet(ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Any, 1)).addParameter(ExpressionType::Bool),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
                               ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)));
            break;
        default:
            break;
    }
    return ret;
}

ExpressionType TypePair(const ExpressionType& a, const ExpressionType& b) { return ExpressionType(ExpressionType::Lambda).addParameter(a).addParameter(b); }

QList<ExpressionType> Operations::inferUnary(Operator::OperatorType op)
{
    QList<ExpressionType> ret;
    switch(op) {
        case Operator::minus:
            ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
            break;
        case Operator::card:
            ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Value));
            ret << TypePair(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)), ExpressionType(ExpressionType::Value));
            break;
        case Operator::sum:
        case Operator::product:
            ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
            break;
        case Operator::transpose:
            ret << TypePair(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
                            ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1), -2));
            ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
                            ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1), 1));
            break;
        case Operator::factorial:
        case Operator::sin:
        case Operator::cos:
        case Operator::tan:
        case Operator::sec:
        case Operator::csc:
        case Operator::cot:
        case Operator::sinh:
        case Operator::cosh:
        case Operator::tanh:
        case Operator::sech:
        case Operator::csch:
        case Operator::coth:
        case Operator::arcsin:
        case Operator::arccos:
        case Operator::arctan:
        case Operator::arccot:
        case Operator::arcsinh:
        case Operator::arccosh:
        case Operator::arccsc:
        case Operator::arccsch:
        case Operator::arcsec:
        case Operator::arcsech:
        case Operator::arctanh:
        case Operator::exp:
        case Operator::ln:
        case Operator::log:
        case Operator::abs:
        case Operator::conjugate:
        case Operator::arg:
        case Operator::real:
        case Operator::imaginary:
        case Operator::floor:
        case Operator::ceiling:
            ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
            break;
        case Operator::_not:
        case Operator::exists:
        case Operator::forall:
            ret << TypePair(ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool));
            break;
        default:
            break;
    }
    return ret;
}

Operations::BinaryOp Operations::opsBinary[Object::custom+1][Object::custom+1] = {
    {nullptr,(Operations::BinaryOp) reduceNoneValue,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,(Operations::BinaryOp) reduceNoneMatrix,nullptr,nullptr},
    {(Operations::BinaryOp) reduceValueNone, (Operations::BinaryOp) reduceValueValue, nullptr, (Operations::BinaryOp) reduceValueVector, (Operations::BinaryOp) reduceValueList,nullptr,nullptr,nullptr,(Operations::BinaryOp) reduceValueMatrix,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
    {nullptr, (Operations::BinaryOp) reduceVectorValue, nullptr, (Operations::BinaryOp) reduceVectorVector, nullptr,nullptr,nullptr,nullptr,(Operations::BinaryOp) reduceVectorMatrix,nullptr,nullptr},
    {nullptr, nullptr, nullptr,nullptr, (Operations::BinaryOp) reduceListList, nullptr,nullptr,nullptr,nullptr,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
    {(Operations::BinaryOp) reduceMatrixNone, (Operations::BinaryOp) reduceMatrixValue,nullptr, (Operations::BinaryOp) reduceMatrixVector,nullptr,nullptr,nullptr,nullptr, (Operations::BinaryOp) reduceMatrixMatrix,nullptr,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
    {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,(Operations::BinaryOp) reduceCustomCustom}
};

Object * Operations::reduce(Operator::OperatorType op, Object * val1, Object * val2, QString** correct)
{
    Object::ObjectType t1=val1->type(), t2=val2->type();
    
    BinaryOp f=opsBinary[t1][t2];
    Q_ASSERT(f);
    return f(op, val1, val2, correct);
}

Operations::UnaryOp Operations::opsUnary[] = {
    nullptr,
    (Operations::UnaryOp) Operations::reduceUnaryValue,
    nullptr, //variable
    (Operations::UnaryOp) Operations::reduceUnaryVector,
    (Operations::UnaryOp) Operations::reduceUnaryList,
    nullptr, //apply
    nullptr, //oper
    nullptr, //container
    (Operations::UnaryOp) Operations::reduceUnaryMatrix
};

Object * Operations::reduceUnary(Operator::OperatorType op, Object * val, QString** correct)
{
    Q_ASSERT(val->type()<=Operator::matrix);
    UnaryOp f=opsUnary[val->type()];
    
    Q_ASSERT(f && "using reduceUnary in a wrong way");
    return f(op, val, correct);
}

Object* Operations::errorCase(const QString& error, QString** correct)
{
    *correct = new QString(error);
    return new None;
}
