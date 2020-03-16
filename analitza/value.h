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

#ifndef VALUE_H
#define VALUE_H

#include "object.h"
#include "analitzaexport.h"

#include <cmath>
#include <complex>

class QDomElement;

namespace Analitza
{

/**
 * \class Cn
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Cn is a value in MathML.
 *
 * The Cn class is the one that represents a value in the expression trees.
 */

class ANALITZA_EXPORT Cn : public Object
{
    public:
        enum ValueFormat { Char=8, Real=7, Integer=3, Boolean=1, Complex=0x10|Real };
        /** Copy constructor. Creates a Cn from another one. */
        Cn(const Cn& v) : Object(v), m_value(v.m_value), m_imaginaryPart(v.m_imaginaryPart), m_format(v.m_format) { Q_ASSERT(m_type==Object::value); }

        /** Constructor. Creates a boolean value with @p b. */
        explicit Cn(const double &b=0.) : Object(Object::value), m_value(b), m_imaginaryPart(0), m_format(Real) {}

        /** Constructor. Creates an integer value with @p i. */
        explicit Cn(int i) : Object(Object::value), m_value(i), m_imaginaryPart(0), m_format(Integer) {}

        /** @copydoc */
        explicit Cn(uint i) : Object(Object::value), m_value(i), m_imaginaryPart(0), m_format(Integer) {}

        /** Constructor. Creates a boolean value with value @p b. */
        explicit Cn(bool b) : Object(Object::value), m_value(b?1.:0.), m_imaginaryPart(0), m_format(Boolean) {}

        /** Constructor. Creates a value that represents a character. */
        explicit Cn(const QChar& c) : Object(Object::value), m_char(c.unicode()), m_imaginaryPart(0), m_format(Char) {}

        /** Constructor. Creates a value that represents a complex. */
        explicit Cn(double value, double imaginaryPart) : Object(Object::value), m_value(value), m_imaginaryPart(imaginaryPart), m_format(Complex) {}
        
        /**
         *    Extracts the number from the @p e Dom element and saves it.
         */
        bool setValue(const QDomElement& e);

        /**
         *    Sets the new value of this function
         *    @param v the new value
         */
        void setValue(const double& v);
        void setValue(int v);
        void setValue(uint v);
        void setValue(bool v);
        void setValue(std::complex<double> v);

        /**
         *    Returns the value.
         */
        inline double value() const { return m_value; }
        inline double& rvalue() { return m_value; }

        /**
         *    @returns the value as an int.
         */
        int intValue() const { return static_cast<int>(std::floor(m_value)); }

        /**
         *    @returns whether it is a boolean value or not.
         */
        bool isBoolean() const { return m_format==Boolean; }

        /**
         *    @returns whether it is a character value or not.
         */
        bool isCharacter() const { return m_format==Char; }

        ValueFormat format() const { return m_format; }

        /**
         *    @return If it is a boolean value, returns if it is true or not, otherwise retuns false.
         */
        bool isTrue() const { Q_ASSERT(m_format==Boolean); return m_value!=0.; }

//         static double toNum(const QString& num, const QString& type, int base);
//         static enum ValueFormat whatValueFormat(const QDomElement&);

        /**
         *    @returns whether it is an integer value or not.
         */
        bool isInteger() const { return m_format&Integer && std::floor(m_value)==m_value; }

        /**
         *    @returns whether @p d is equal than this object.
         */
        bool operator==(const Cn& d) const;

        /**
         *    @returns whether @p d is less than this object.
         */
        bool operator<(const Cn& d) const { return m_value<d.m_value; }

        /**
         *    @returns whether @p d is less than this object's value.
         */
        bool operator<(double d) const { return m_value<d; }

        /**
         *    @returns whether @p d is less or equal than this object.
         */
        bool operator<=(const Cn& d) const { return m_value<=d.m_value; }

        /**
         *    @returns whether @p d is less or equal than this object's value.
         */
        bool operator<=(double d) const { return m_value<=d; }

        /**
         *    Sets the new value to @p d.
         */
        Cn operator=(double d) { m_value=d; m_format=Real; return *this; }

        QChar character() const { Q_ASSERT(m_format==Char); return QChar(m_char); }

        /** @returns whether the value has an imaginary part */
        bool isComplex() const { return m_format == Complex && m_imaginaryPart!=0.; }

        virtual QVariant accept(AbstractExpressionVisitor*) const override;
        virtual bool isZero() const override { return m_value==0. && m_imaginaryPart==0.; }

        virtual bool matches(const Object* exp, QMap< QString, const Object* >* found) const override;
        /*/** Sets whether it is a correct Cn.
        void setCorrect(bool b) {m_correct = b; }*/

        std::complex<double> complexValue() const;

        virtual Object* copy() const override;

        static Cn pi();
        static Cn e();
        static Cn euler();
    private:
        union { double m_value; ushort m_char; };
        double m_imaginaryPart = 0.;
        enum ValueFormat m_format;
};

}

#endif
