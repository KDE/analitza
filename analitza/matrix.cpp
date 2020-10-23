/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "matrix.h"
#include "abstractexpressionvisitor.h"
#include "value.h"

using namespace Analitza;

Matrix::Matrix()
    : Object(matrix)
    , m_hasOnlyNumbers(true)
{}

Matrix::Matrix(int m, int n, const Cn* value)
    : Object(matrix)
    , m_hasOnlyNumbers(true)
{
    Q_ASSERT(m > 0);
    Q_ASSERT(n > 0);
    Q_ASSERT(value);
    
    for (int i = 0; i < m; ++i) {
        MatrixRow *row = new Analitza::MatrixRow(n);
        for (int j = 0; j < n; ++j) {
            row->appendBranch(value->copy());
        }
        appendBranch(row);
    }
}

Matrix::~Matrix()
{
    qDeleteAll(m_rows);
}

Matrix* Matrix::copy() const
{
    Matrix* nm = new Matrix;
    foreach(MatrixRow* r, m_rows) {
        nm->appendBranch(r->copy());
    }
    
    return nm;
}

bool Matrix::matches(const Object* exp, QMap< QString, const Object* >* found) const
{
    if(Object::matrix!=exp->type())
        return false;
    const Matrix* c=(const Matrix*) exp;
    if(m_rows.count()!=c->m_rows.count())
        return false;
    
    bool matching=true;
    Matrix::const_iterator it, it2, itEnd=m_rows.constEnd();
    for(it=m_rows.constBegin(), it2=c->m_rows.constBegin(); matching && it!=itEnd; ++it, ++it2) {
        matching &= (*it)->matches(*it2, found);
    }
    
    return matching;
}

QVariant Matrix::accept(AbstractExpressionVisitor* exp) const
{
    return exp->visit(this);
}

void Matrix::appendBranch(MatrixRow* o)
{
    Q_ASSERT(dynamic_cast<MatrixRow*>(o));
    Q_ASSERT(dynamic_cast<MatrixRow*>(o)->size()>0);
    Q_ASSERT(m_rows.isEmpty()?true:dynamic_cast<MatrixRow*>(o)->size() == dynamic_cast<MatrixRow*>(m_rows.last())->size()); // all rows are same size
    
    if (!o->hasOnlyNumbers() && m_hasOnlyNumbers)
        m_hasOnlyNumbers = false;
    
    m_rows.append(static_cast<MatrixRow*>(o));
}

bool Matrix::operator==(const Matrix& m) const
{
    bool eq = m.rowCount()==rowCount();
    
    for(int i=0; eq && i<m_rows.count(); ++i) {
        eq = eq && *static_cast<MatrixRow*>(m_rows[i])==*static_cast<MatrixRow*>(m.m_rows[i]);
    }
    
    return eq;
}

Object* Matrix::at(int i, int j) const
{
    return m_rows.at(i)->at(j);
}

int Matrix::columnCount() const
{
    return m_rows.isEmpty()? 0 : static_cast<const MatrixRow*>(m_rows.first())->size();
}

bool Matrix::isSquare() const
{
    return m_rows.isEmpty()? true : m_rows.size() == static_cast<const MatrixRow*>(m_rows.first())->size();
}

////// MatrixRow
MatrixRow::MatrixRow(int size)
    : Vector(Object::matrixrow, size)
{}

QVariant MatrixRow::accept(AbstractExpressionVisitor* e) const
{
    return e->visit(this);
}

MatrixRow* MatrixRow::copy() const
{
    MatrixRow *m=new MatrixRow(size());
    for(MatrixRow::const_iterator it=constBegin(); it!=constEnd(); ++it) {
        m->appendBranch((*it)->copy());
    }
    return m;
}

Matrix* Matrix::identity(int n)
{
    Q_ASSERT(n>0);
    
    Analitza::Matrix *ret = new Analitza::Matrix();
    
    for (int i = 0; i < n; ++i) {
        MatrixRow *row = new Analitza::MatrixRow(n);
        for (int j = 0; j < n; ++j) {
            if (i == j)
                row->appendBranch(new Analitza::Cn(1));
            else
                row->appendBranch(new Analitza::Cn(0));
        }
        ret->appendBranch(row);
    }
    
    return ret;
}

bool Matrix::isIdentity() const
{
    if (!hasOnlyNumbers())
        return false;
    
    const int nrows = m_rows.size();
    
    for (int row = 0; row < nrows; ++row)
        if (!m_rows.at(row)->isStandardBasisVector() || static_cast<const Cn*>(at(row, row))->value() != 1)
            return false;
    
    return true;
}

bool Matrix::isDiagonal() const
{
    if (!hasOnlyNumbers())
        return false;
    
    const int nrows = m_rows.size();
    
    for (int row = 0; row < nrows; ++row)
        if (!m_rows.at(row)->isDiagonalRow())
            return false;
    
    return true;
}

bool Matrix::isZero() const
{
    bool zero = false;
    foreach(const MatrixRow* o, m_rows) {
        zero |= o->isZero();
    }

    return zero;
}
