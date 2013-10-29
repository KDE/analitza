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
#include "expressionwriter.h"

#include <QDomElement>

#include "localize.h"

using namespace Analitza;

Object* Cn::copy() const
{
	Cn *v = new Cn;
	v->m_value = m_value;
	v->m_format = m_format;
	return v;
}


QString Cn::visit(ExpressionWriter* e) const
{
	return e->accept(this);
}

bool Cn::setValue(const QDomElement& val)
{
// 	this->m_vformat=whatValueFormat(val);
	bool wrong=false;
	QString tag = val.tagName();
	m_format=Real;
	m_imaginaryPart=0;
	
	if(tag == "cn"){ // a is a number
		if(val.attribute("type", "integer") == "real") {
			m_value= val.text().trimmed().toDouble(&wrong); //TODO: Base on double not implemented
		} else if(val.attribute("type", "integer") == "integer"){
			int base = val.attribute("base", "10").toInt(NULL, 10);
			m_value= val.text().trimmed().toInt(&wrong, base);
			m_format=Integer;
		}
#if 0
		else if(val.attribute("type") == "e-notation")	{ /*TODO: Not implemented */ }
		else if(val.attribute("type") == "rational")		{ /*TODO: Not implemented */ }
		else if(val.attribute("type") == "complex-cartesian")	{ /*TODO: Not implemented */ }
		else if(val.attribute("type") == "complex-polar")	{ /*TODO: Not implemented */ }
#endif
		else if(val.attribute("type") == "constant"){
			if(val.text() == "&pi;")			{ m_value = pi().m_value; }
			else if (val.text() == "&ee;" || val.text() == "&ExponentialE;"){ m_value = e().m_value; }
			else if (val.text() == "&true;")	{ m_value=1.; m_format=Boolean; }
			else if (val.text() == "&false;")	{ m_value=0.; m_format=Boolean; }
			else if (val.text() == "&gamma;")	{ m_value = 0.5772156649; }
			else if (val.text() == "&ImagniaryI;")	{ m_value=0; m_imaginaryPart=1; m_format=Complex; }
#if 0
			else if (val.text() == "&infin;")	; //TODO: Not implemented  }
			else if (val.text() == "&NaN;")		; //TODO: Not implemented  }*/
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

std::complex<float> Cn::complexValue() const
{
	return std::complex<float>(m_value, m_imaginaryPart);
}

void Cn::setValue(std::complex<float> v)
{
	m_format = Complex;
	m_imaginaryPart = v.imag();
	m_value = v.real();
}
