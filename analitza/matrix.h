/*************************************************************************************
 *  Copyright (C) 2013 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef MATRIX_H
#define MATRIX_H

#include "object.h"
#include "vector.h"

namespace Analitza {

class Cn;

class Vector;

class ANALITZA_EXPORT MatrixRow : public Vector
{
    public:
        explicit MatrixRow(int size=0);
        virtual QVariant accept(AbstractExpressionVisitor* e) const override;
        
        MatrixRow* copy() const override;
};

class ANALITZA_EXPORT Matrix : public Object
{
    public:
        typedef QList<MatrixRow*>::iterator iterator;
        typedef QList<MatrixRow*>::const_iterator const_iterator;
        
        Matrix();
        /** Fills the matrix with a fixed numeric @p value, the matrix size will be @p m x @p n */
        Matrix(int m, int n, const Cn *value);
        virtual ~Matrix();
        void appendBranch(MatrixRow* o);
        
        virtual Matrix* copy() const override;
        virtual bool matches(const Object* exp, QMap< QString, const Object* >* found) const override;
        virtual QVariant accept(AbstractExpressionVisitor* exp) const override;
        const_iterator constBegin() const { return m_rows.constBegin(); }
        const_iterator constEnd() const { return m_rows.constEnd(); }
        iterator begin() { return m_rows.begin(); }
        iterator end() { return m_rows.end(); }
        QList< MatrixRow* > rows() const { return m_rows; }
        bool operator==(const Matrix& m) const;
        int rowCount() const { return m_rows.size(); }
        Analitza::Matrix::iterator erase(const Analitza::Matrix::iterator& it) { return m_rows.erase(it); }
        Object* at(int i, int j) const;
        int columnCount() const;
        bool isSquare() const;
        virtual bool isZero() const override;
        bool hasOnlyNumbers() const { return m_rows.isEmpty()? false : m_hasOnlyNumbers; }
        bool isIdentity() const;
        bool isDiagonal() const;
        
    public:
        static Matrix* identity(int n);
        
    private:
        QList<MatrixRow*> m_rows;
        bool m_hasOnlyNumbers;
};

}

#endif // MATRIX_H
