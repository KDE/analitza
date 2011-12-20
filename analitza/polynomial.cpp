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

using namespace Analitza;

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
	return second->type()==Object::value || (second->type()==Object::vector && !AnalitzaUtils::hasVars(second));
}

Polynomial::Polynomial(Apply* c, bool& sign)
{
	Operator o(c->firstOperator());
	bool first = true;
	
	for(Apply::const_iterator it=c->constBegin(), itEnd=c->constEnd(); it!=itEnd; ++it, first=false) {
		Monomial imono(o, *it, sign);
		
		if(o==Operator::minus && !first)
			imono.first*=-1;
		
		bool found = false;
		QList<Monomial>::iterator it1(begin());
		for(; it1!=end(); ++it1) {
			Object *o1=it1->second, *o2=imono.second;
			found = AnalitzaUtils::equalTree(o1, o2);
			if(found)
				break;
		}
		
// 		qDebug() << "->" << c->toString() << c->firstOperator().toString() << found;
		if(found) {
			it1->first += imono.first;
			delete imono.second;
			
			if(it1->first==0.) {
				delete it1->second;
				erase(it1);
			}
		} else {
			append(imono);
		}
	}
}