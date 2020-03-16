/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
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


#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <QList>
#include "operator.h"

namespace Analitza {

class Apply;
class Object;
class Operator;

/**
 * \class Monomial
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Monomial object.
 *
 * It allows you compose and create a Polynomial.
 */

class Monomial
{
    public:
        Monomial(const Operator& o, Object* o2, bool& sign);
        Monomial(Monomial &&m) : first(m.first), second(m.second) {}
        Monomial(const Monomial &m) : first(m.first), second(m.second) {}

        void operator=(const Monomial &m) {
            if (this != &m) {
                first = m.first;
                second = m.second;
            }
        }

        Analitza::Object* createMono(const Analitza::Operator& o) const;
        bool isValue() const;
        static bool isScalar(const Object* o);
        
        qreal first;
        Analitza::Object* second;
};

/**
 * \class Polynomial
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Polynomial object.
 *
 * The important method in this class is negate, which change the sign of the Monomial 
 * referenced by an index.
 *
 * \author Aleix Pol <aleixpol@kde.org>
 */

class Polynomial : public QList<Monomial>
{
    public:
        explicit Polynomial(Apply* c);
        
        Analitza::Object* toObject();
        void negate(int i);
        
    private:
        void addMonomial(const Monomial& m);
        void addValue(Analitza::Object* value);
        void simpScalars(bool firstValue);

        QVector<Object*> m_scalars;
        Operator m_operator;
        bool m_sign;
};

}

#endif // POLYNOMIAL_H
