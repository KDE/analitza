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

#include "value.h"
#include "operator.h"
#include "abstractexpressionvisitor.h"

#include <QDomElement>



using namespace Analitza;

Object* Cn::copy() const
{
    Cn *v = new Cn;
    v->m_value = m_value;
    v->m_imaginaryPart = m_imaginaryPart;
    v->m_format = m_format;
    return v;
}


QVariant Cn::accept(AbstractExpressionVisitor* e) const
{
    return e->visit(this);
}

bool Cn::setValue(const QDomElement& val)
{
//     this->m_vformat=whatValueFormat(val);
    bool wrong=false;
    QString tag = val.tagName();
    m_format=Real;
    m_imaginaryPart=0;
    
    if(tag == QLatin1String("cn")){ // a is a number
        if(val.attribute(QStringLiteral("type"), QStringLiteral("integer")) == QLatin1String("real")) {
            m_value= val.text().trimmed().toDouble(&wrong); //TODO: Base on double not implemented
        } else if(val.attribute(QStringLiteral("type"), QStringLiteral("integer")) == QLatin1String("integer")){
            int base = val.attribute(QStringLiteral("base"), QStringLiteral("10")).toInt(nullptr, 10);
            m_value= val.text().trimmed().toInt(&wrong, base);
            m_format=Integer;
        }
#if 0
        else if(val.attribute("type") == "e-notation")    { /*TODO: Not implemented */ }
        else if(val.attribute("type") == "rational")        { /*TODO: Not implemented */ }
        else if(val.attribute("type") == "complex-cartesian")    { /*TODO: Not implemented */ }
        else if(val.attribute("type") == "complex-polar")    { /*TODO: Not implemented */ }
#endif
        else if(val.attribute(QStringLiteral("type")) == QLatin1String("constant")){
            if(val.text() == QLatin1String("&pi;"))            { m_value = pi().m_value; }
            else if (val.text() == QLatin1String("&ee;") || val.text() == QLatin1String("&ExponentialE;")){ m_value = e().m_value; }
            else if (val.text() == QLatin1String("&true;"))    { m_value=1.; m_format=Boolean; }
            else if (val.text() == QLatin1String("&false;"))    { m_value=0.; m_format=Boolean; }
            else if (val.text() == QLatin1String("&gamma;"))    { m_value = 0.5772156649; }
            else if (val.text() == QLatin1String("&ImagniaryI;"))    { m_value=0; m_imaginaryPart=1; m_format=Complex; }
#if 0
            else if (val.text() == "&infin;")    ; //TODO: Not implemented  }
            else if (val.text() == "&NaN;")        ; //TODO: Not implemented  }*/
#endif
        }
    }
    return wrong;
}

Cn Cn::pi() { return Cn(3.1415926535897932384626433); }
Cn Cn::e() { return Cn(2.718281828); }
Cn Cn::euler() { return Cn(0.5772156649); }

bool Cn::matches(const Object* exp, QMap< QString, const Object* >*) const
{
    return exp->type()==type() && *static_cast<const Cn*>(exp)==*this;
}

void Cn::setValue(const double& v)
{
    m_format = Real;
    m_value = v;
    m_imaginaryPart = 0;
}

void Cn::setValue(int v)
{
    m_format = Integer;
    m_value = v;
    m_imaginaryPart = 0;
}

void Cn::setValue(uint v)
{
    m_format = Integer;
    m_value = v;
    m_imaginaryPart = 0;
}

void Cn::setValue(bool v)
{
    m_format = Boolean;
    m_value = v;
    m_imaginaryPart = 0;
}

std::complex<double> Cn::complexValue() const
{
    return std::complex<double>(m_value, m_imaginaryPart);
}

void Cn::setValue(std::complex<double> v)
{
    if(v.imag() == 0)
        setValue(v.real());
    else {
        m_format = Complex;
        m_imaginaryPart = v.imag();
        m_value = v.real();
    }
}

constexpr bool ourFuzzyCompare(qreal a, qreal b)
{
    return qAbs(a - b) < std::numeric_limits<double>::epsilon()*2;
}

bool Cn::operator==(const Analitza::Cn& d) const{
    return ourFuzzyCompare(m_value, d.m_value) && ourFuzzyCompare(m_imaginaryPart, d.m_imaginaryPart);
}

