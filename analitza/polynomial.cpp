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

#include "polynomial.h"
#include "operator.h"
#include "apply.h"
#include "value.h"
#include "analitzautils.h"
#include "operations.h"

using namespace Analitza;

static QDebug operator<<(QDebug dbg, const Monomial &c)
{
	dbg.nospace() << "(" << c.first << ", " << (c.second ? c.second->toString() : "<null>") << ")";

	return dbg.space();
}

bool Monomial::isScalar(const Object* o)
{
	return o->type()==Object::value || (o->type()==Object::vector && !AnalitzaUtils::hasVars(o));
}

Object* Monomial::createMono(const Operator& o)
{
	Operator::OperatorType mult = o.multiplicityOperator();
	
	Object* toAdd=0;
	if(first==0.) {
		delete second;
	} else if(first==1.) {
		toAdd=second;
	} else if(first==-1. && mult==Operator::times) {
		Apply *cint = new Apply;
		cint->appendBranch(new Operator(Operator::minus));
		cint->appendBranch(second);
		toAdd=cint;
	} else if(mult==Operator::times && second->isApply() && static_cast<Apply*>(second)->firstOperator()==Operator::times) {
		Apply* res = static_cast<Apply*>(second);
		res->prependBranch(new Cn(first));
		toAdd=res;
	} else {
		Apply *cint = new Apply;
		cint->appendBranch(new Operator(mult));
		if(mult==Operator::times) {
			cint->appendBranch(new Cn(first));
			cint->appendBranch(second);
		} else {
			cint->appendBranch(second);
			cint->appendBranch(new Cn(first));
		}
		toAdd=cint;
	}
	return toAdd;
}

Monomial::Monomial(const Operator& o, Object* o2, bool& sign)
{
	bool ismono=false;
	Operator::OperatorType mult = o.multiplicityOperator();
	
	if(o2->isApply()) {
		Apply *cx = (Apply*) o2;
		if(cx->firstOperator()==mult) {
			if(cx->countValues()==2) {
				bool valid=false;
				int scalar, var;
				
				if(mult!=Operator::power && cx->m_params[0]->type()==Object::value) {
					scalar=0;
					var=1;
					valid=true;
				} else if(cx->m_params[1]->type()==Object::value) {
					scalar=1;
					var=0;
					valid=true;
				}
				
				if(valid) {
					Cn* sc= (Cn*) cx->m_params[scalar];
					first = sc->value();
					second = cx->m_params[var];
					
					cx->m_params[var]=0;
					delete cx;
					
					ismono=true;
				}
			} else if(mult==Operator::times) {
				first=1;
				Apply::iterator it=cx->firstValue(), itEnd=cx->end();
				QVector<Object*> vars;
				QVector<Object*> values;
				
				for(; it!=itEnd; ++it) {
					if((*it)->type()==Object::value) {
						first *= static_cast<Cn*>(*it)->value();
						values += *it;
						ismono=true;
					} else {
						vars += *it;
					}
				}
				
				if(ismono) {
					cx->m_params = vars;
					second = cx;
					qDeleteAll(values);
				}
			}
		} else if(cx->firstOperator()==Operator::minus && cx->isUnary()) {
			*this = Monomial(o, *cx->firstValue(), sign);
			*cx->firstValue()=0;
			delete cx;
			ismono=true;
				
			if(o==Operator::times)
				sign = !sign;
			else if(o==Operator::plus || o==Operator::minus)
				first *= -1;
		}
	}
	
	if(!ismono) {
		first = 1.;
		second = o2;
	}
}

bool Monomial::isValue() const
{
	return isScalar(second);
}

Polynomial::Polynomial(Apply* c)
	: m_firstValue(false)
	, m_operator(c->firstOperator())
	, m_sign(true)
{
	bool first = true;
	QList<Monomial> monos;
	for(Apply::const_iterator it=c->constBegin(), itEnd=c->constEnd(); it!=itEnd; ++it, first=false) {
		if(Monomial::isScalar(*it)) {
			m_scalars += *it;
			m_firstValue = m_firstValue || first;
			continue;
		}
		Monomial imono(m_operator, *it, m_sign);
		if(m_operator==Operator::minus && !first)
			imono.first*=-1;
		
		bool added=false;
		if(imono.second->isApply()) {
			Apply* a = static_cast<Apply*>(imono.second);
			if(a->firstOperator()==m_operator) {
				Polynomial p(a);
				
				a->m_params.clear();
				delete a;
				
				monos.append(p);
				m_scalars.append(p.m_scalars);
				added=true;
			}
		}
		
		if(!added)
			monos.append(imono);
	}
	
// 	qDebug() << "++++++" << monos;
	for(iterator it=monos.begin(), itEnd=monos.end(); it!=itEnd; ++it) {
		addMonomial(*it);
	}
	
// 	qDebug() << "aaaaaaaa" << *this << size();
}

void Polynomial::addMonomial(const Monomial& m)
{
	bool found = false;
	QList<Monomial>::iterator it1(begin());
	for(; it1!=end(); ++it1) {
		Object *o1=it1->second, *o2=m.second;
		found = AnalitzaUtils::equalTree(o1, o2);
		if(found)
			break;
	}
	
// 	qDebug() << "looo" << *this << m << found;
	if(found) {
		it1->first += m.first;
		delete m.second;
		
		if(it1->first==0.) {
			delete it1->second;
			erase(it1);
		}
	} else {
		append(m);
	}
}

void Polynomial::simpScalars()
{
	Object *value=0;
	bool first = true;
	
	for(QList<Object*>::iterator i=m_scalars.begin(); i!=m_scalars.end(); ++i, first=false) {
		bool d=false;
		
		Object* aux = *i;
		if(value) {
			QString* err=0;
			value=Operations::reduce(m_operator.operatorType(), value, aux, &err);
			delete err;
			d=err;
		} else
			value=aux;
		
		if(d) {
			bool sign = false;
			Monomial imono(m_operator, *i, sign);
			if(m_operator==Operator::minus && !first)
				imono.first*=-1;
			append(imono);
		}
	}
	
	if(value) {
		bool sign=true;
		if(value->isZero())
			delete value;
		else {
			Monomial imono(m_operator, value, sign);
			
			if(m_operator==Operator::plus || (!m_firstValue && m_operator==Operator::minus)) {
				if(m_operator==Operator::minus && !first)
					imono.first*=-1;
				append(imono);
			} else
				prepend(imono);
		}
	}
	
	m_scalars.clear();
}

Object* Polynomial::toObject()
{
	if(!m_scalars.isEmpty())
		simpScalars();
	
	Object* root = 0;
	if(count()==1) {
		root = first().createMono(m_operator);
	} else if(count()>1) {
		Apply* c = new Apply;
		c->appendBranch(new Operator(m_operator));
		
		QList<Monomial>::iterator i=begin();
		bool first=true;
		for(; i!=end(); ++i, first=false) {
			if(!first && m_operator==Operator::minus)
				i->first *= -1;
			Object* toAdd=i->createMono(m_operator);
			
			if(toAdd)
				c->appendBranch(toAdd);
		}
		root=c;
	}
	
	if(!m_sign && root) {
		Apply *cn=new Apply;
		cn->appendBranch(new Operator(Operator::minus));
		cn->appendBranch(root);
		root=cn;
	} else if(!root) {
		root=new Cn(0.);
	}
	
// 	qDebug() << "tuuuuuu" << *this << root->toString();
	return root;
}
