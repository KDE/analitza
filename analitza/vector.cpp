/*************************************************************************************
 *  Copyright (C) 2009 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "vector.h"
#include "expression.h"
#include "abstractexpressionvisitor.h"
#include "analitzautils.h"
#include "value.h"

using namespace Analitza;

Vector::Vector(const Vector& v)
    : Object(Object::vector)//, m_elements(v.m_elements.size())
    , m_hasOnlyNumbers(true)
    , m_nonZeroTaken(false) , m_isDiagonalRowVector(true) , m_nonZeros(0)
{
    m_elements.reserve(v.m_elements.size());
    foreach(const Object* o, v.m_elements) {
        m_elements.append(o->copy());
    }
}

Vector::Vector(int size)
    : Object(Object::vector)//, m_elements(size)
    , m_hasOnlyNumbers(true)
    , m_nonZeroTaken(false)
    , m_isDiagonalRowVector(true)
    , m_nonZeros(0)
{
    m_elements.reserve(size);
}

Vector::Vector(Object::ObjectType t, int size)
    : Object(t)
    , m_hasOnlyNumbers(true)
    , m_nonZeroTaken(false)
    , m_isDiagonalRowVector(true)
    , m_nonZeros(0)
{
    m_elements.reserve(size);
}

Vector::Vector(int size, const Cn* value)
    : Object(Object::vector)//, m_elements(size)
    , m_hasOnlyNumbers(true)
    , m_nonZeroTaken(false)
    , m_isDiagonalRowVector(true)
    , m_nonZeros(0)
{
    Q_ASSERT(size > 0);
    Q_ASSERT(value);
    
    for (int i = 0; i < size; ++i)
        appendBranch(value->copy());
}

Vector::~Vector()
{
    qDeleteAll(m_elements);
}

Vector* Vector::copy() const
{
    Vector *v=new Vector(m_type, size());
    foreach(const Object* o, m_elements) {
        v->m_elements.append(o->copy());
    }
    return v;
}

void Vector::appendBranch(Object* o)
{
    Q_ASSERT(o);
    
    if (o->type() != Object::value && m_hasOnlyNumbers)
        m_hasOnlyNumbers = false;
    
    const bool isobjzero = o->isZero();
    
    if (!isobjzero) {
        if (!m_nonZeroTaken)
            ++m_nonZeros;
    }
    
    if (o->type() == Object::value) {
        //puse && !m_nonZeroTaken sin testing
        if (m_nonZeros > 1 && !m_nonZeroTaken) {
            m_isDiagonalRowVector = false;
            m_nonZeroTaken = true;
        }
    }
    
    m_elements.append(o);
}

QVariant Vector::accept(AbstractExpressionVisitor* e) const
{
    return e->visit(this);
}

bool Vector::matches(const Object* exp, QMap< QString, const Object* >* found) const
{
    if(Object::vector!=exp->type())
        return false;
    const Vector* c=(const Vector*) exp;
    if(m_elements.count()!=c->m_elements.count())
        return false;
    
    bool matching=true;
    Vector::const_iterator it, it2, itEnd=m_elements.constEnd();
    for(it=m_elements.constBegin(), it2=c->m_elements.constBegin(); matching && it!=itEnd; ++it, ++it2) {
        matching &= (*it)->matches(*it2, found);
    }
    return matching;
}

bool Vector::operator==(const Vector& v) const
{
    bool eq=v.size()==size();
    
    for(int i=0; eq && i<m_elements.count(); ++i) {
        eq = eq && AnalitzaUtils::equalTree(m_elements[i], v.m_elements[i]);
    }
    return eq;
}

bool Vector::isZero() const
{
    bool zero = false;
    foreach(const Object* o, m_elements) {
        zero |= o->isZero();
    }
    return zero;
}

bool Vector::isStandardBasisVector() const
{
    bool hasOne = false;
    foreach(const Object* o, m_elements) {
        Q_ASSERT(o->type() == Object::value);
        Cn* v = (Cn*) o;
        switch (v->intValue()) {
            case 0:
                continue;
            case 1:
                hasOne = true;
                break;
            default:
                return false;
        }
    }
    return hasOne;
}
