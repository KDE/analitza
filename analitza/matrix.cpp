/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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
#include "vector.h"

using namespace Analitza;

Matrix::Matrix()
	: Object(matrix)
{}

Matrix::~Matrix()
{
	qDeleteAll(m_rows);
}

Object* Matrix::copy() const
{
	Matrix* nm = new Matrix;
	Q_FOREACH(Object* r, m_rows) {
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
	for(it=m_rows.constBegin(), it2=c->m_rows.constBegin(); matching && it!=itEnd; ++it, ++it2)
	{
		matching &= (*it)->matches(*it2, found);
	}
	return matching;
}

QVariant Matrix::accept(AbstractExpressionVisitor* exp) const
{
	return exp->visit(this);
}

void Matrix::appendBranch(Object* o)
{
	Q_ASSERT(o->type()==Object::matrixrow);
	Q_ASSERT(dynamic_cast<MatrixRow*>(o));
	Q_ASSERT(dynamic_cast<MatrixRow*>(o)->size()!=0);
	m_rows += static_cast<MatrixRow*>(o);
}

QList<Object*> Matrix::values() const
{
	QList<Object*> ret;
	foreach(Object* o, m_rows)
		ret += o;
	return ret;
}

bool Matrix::operator==(const Matrix& m) const
{
	bool eq = m.size()==size();
	
	for(int i=0; eq && i<m_rows.count(); ++i) {
		eq = eq && *static_cast<MatrixRow*>(m_rows[i])==*static_cast<MatrixRow*>(m.m_rows[i]);
	}
	return eq;
}

Object* Matrix::at(int i, int j) const
{
	return static_cast<const MatrixRow*>(m_rows.at(i))->at(j);
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
	for(MatrixRow::const_iterator it=constBegin(); it!=constEnd(); ++it)
	{
		m->appendBranch((*it)->copy());
	}
	return m;
}
