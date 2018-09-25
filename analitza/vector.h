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

#ifndef VECTOR_H
#define VECTOR_H

#include "object.h"
#include "analitzaexport.h"

namespace Analitza
{

class Cn;

/**
 * \class Vector
 * 
 * \ingroup AnalitzaModule
 */

class ANALITZA_EXPORT Vector : public Object
{
    public:
        typedef QList<Object*>::iterator iterator;
        typedef QList<Object*>::const_iterator const_iterator;
        
        Vector(const Vector& v);
        explicit Vector(int size);
        /** Fills the vector with a fixed numeric @p value */
        Vector(int size, const Cn *value);
        virtual ~Vector();
        
        void appendBranch(Object* );
        int size() const { return m_elements.size(); }
        
        iterator erase(const iterator& it) { return m_elements.erase(it); }
        
        iterator begin() { return m_elements.begin(); }
        iterator end() { return m_elements.end(); }
        const_iterator constBegin() const { return m_elements.constBegin(); }
        const_iterator constEnd() const { return m_elements.constEnd(); }
        
        Object* at(int i) const { return m_elements[i]; }
        void setAt(int i, Object* o) { m_elements[i]=o; }
        QList<Object*> values() const { return m_elements; }
        
        virtual QVariant accept(AbstractExpressionVisitor* e) const override;
        virtual bool isZero() const override;
        
        virtual bool matches(const Object* pattern, QMap< QString, const Object* >* found) const override;
        Vector* copy() const override;
        bool operator==(const Vector& v) const;
        bool hasOnlyNumbers() const { return m_elements.isEmpty()? false : m_hasOnlyNumbers; }
        bool isStandardBasisVector() const;
        bool isDiagonalRow() const { return m_elements.isEmpty()? false : m_isDiagonalRowVector; }
        
    protected:
        Vector(Analitza::Object::ObjectType t, int size);
        
    private:
        QList<Object*> m_elements;
        bool m_hasOnlyNumbers;
        bool m_nonZeroTaken;
        bool m_isDiagonalRowVector;
        short int m_nonZeros;
};

}

#endif // VECTOR_H
