/*************************************************************************************
 *  Copyright (C) 2007-2010 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef APPLY_H
#define APPLY_H
#include "object.h"
#include "analitzaexport.h"
#include "operator.h"

namespace Analitza
{
class Operator;
class Ci;

/**
 * \class Apply
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This class is the one that will correspond to MathML apply tags.
 *
 * Stores and helps to retrieve any data it has inside like bvars, 
 * operators and values.
 */

class ANALITZA_EXPORT Apply : public Object
{
    public:
        Apply();
        ~Apply() override;

        typedef QList<Object *>::const_iterator const_iterator;
        typedef QList<Object *>::iterator iterator;

        virtual Apply* copy() const override;
        virtual bool matches(const Analitza::Object* exp, QMap< QString, const Analitza::Object* >* found) const override;
        virtual QVariant accept(AbstractExpressionVisitor* exp) const override;
        const Operator& firstOperator() const { return m_op; }
        int countValues() const { return m_params.size(); }
        
        void prependBranch(Object* o);
        void appendBranch(Object* o);
        
        /** Adds @p bvar into the list of bvars */
        void addBVar(Analitza::Ci* bvar);
        
        /** Returns whether that apply has any bvar */
        bool hasBVars() const { return !m_bvars.isEmpty(); }
        
        /** Returns the apply's bvars names */
        QStringList bvarStrings() const;
        Object* ulimit() const { return m_ulimit; }
        Object* dlimit() const { return m_dlimit; }
        Object* domain() const { return m_domain; }
        
        Object*& ulimit() { return m_ulimit; }
        Object*& dlimit() { return m_dlimit; }
        Object*& domain() { return m_domain; }
        
        /** @deprecated should use begin() now */
        iterator firstValue() { return m_params.begin(); }
        iterator begin() { return m_params.begin(); }
        iterator end() { return m_params.end(); }
        const_iterator firstValue() const { return m_params.constBegin(); }
        const_iterator constBegin() const { return m_params.constBegin(); }
        const_iterator constEnd() const { return m_params.constEnd(); }
        QList<Ci *> bvarCi() const { return m_bvars; }
        bool isUnary() const { return m_params.size()==1; }
        bool isEmpty() const { return m_params.isEmpty(); }
        bool operator==(const Apply& a) const;
        
        /** Adds a @p o branch right after @p before of the Container. */
        void insertBranch(const Apply::iterator &before, Object* o) { m_params.insert(before, o); }
        QList<Object *> values() const { return m_params; }
        Object* at(int p) const;
        
        /** @returns if there's any bounding specified */
        bool hasBoundings() const;

        QList<Object *> m_params;

      private:
        Apply(const Apply& );
        bool addBranch(Object* o);
        
        Object* m_ulimit, *m_dlimit, *m_domain;
        QList<Ci *> m_bvars;
        Operator m_op;
};

}
#endif // APPLY_H
