/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@gmail.com>                             *
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
#include "container.h"

#include "expression.h"
#include "expressionwriter.h"
#include "vector.h"
#include "value.h"

#include <KDebug>
#include <KLocale>

char Container::m_typeStr[][10] = {
		"none",
		"math",
		"apply",
		"declare",
		"lambda",
		"bvar",
		"uplimit",
		"downlimit",
		"piece",
		"piecewise",
		"otherwise" };

QMap<QString, Container::ContainerType> createNameToType()
{
	QMap<QString, Container::ContainerType> ret;
	ret["apply"]=Container::apply;
	ret["declare"]=Container::declare;
	ret["math"]=Container::math;
	ret["lambda"]=Container::lambda;
	ret["bvar"]=Container::bvar;
	ret["uplimit"]=Container::uplimit;
	ret["downlimit"]=Container::downlimit;
	ret["piecewise"]=Container::piecewise;
	ret["piece"]=Container::piece;
	ret["otherwise"]=Container::otherwise;
	
	return ret;
}

QMap<QString, Container::ContainerType> Container::m_nameToType=createNameToType();

Container::Container(const Container& c) : Object(Object::container), m_cont_type(c.m_cont_type)
{
	Q_ASSERT(c.type()==Object::container);
	
	m_params = c.copyParams();
}

Container* Container::copy() const
{
	return new Container(*this);
}

Operator Container::firstOperator() const
{
	const_iterator it=m_params.constBegin(), itEnd=m_params.constEnd();
	for(; it!=itEnd; ++it) {
		if((*it)->type()==Object::oper) {
			return *static_cast<Operator*>(*it);
		} else if(it==m_params.constBegin() && m_cont_type==apply && (*it)->type()==Object::variable) {
			return Operator(Operator::function);
		}
	}
	
	return Operator(Operator::none);
}

QString Container::visit(ExpressionWriter* e) const
{
	return e->accept(this);
}

void Container::negate()
{
	foreach(Object* o, m_params) {
		o->negate();
	}
}

bool Container::isZero() const
{
	bool a=true;
	foreach(const Object* o, m_params) {
		a = a && o->isZero();
	}
	return a;
}

QList<Object*> Container::copyParams() const
{
	QList<Object*> ret;
	
	for(Container::const_iterator it=m_params.constBegin(); it!=m_params.constEnd(); ++it) {
		ret.append((*it)->copy());
	}
	return ret;
}

Container::ContainerType Container::toContainerType(const QString& tag)
{
	return m_nameToType[tag];
}

QStringList Container::bvarList() const //NOTE: Should we return Ci's instead of Strings?
{
	QStringList bvars;
	QList<Object*>::const_iterator it;
	
	for(it=m_params.constBegin(); it!=m_params.constEnd(); ++it) {
		if((*it)->isContainer()) {
			Container* c = (Container*) (*it);
			if(c->containerType() == Container::bvar && !c->m_params.isEmpty() && c->m_params[0]->type()==Object::variable)
				bvars.append(((Ci*)c->m_params[0])->name());
		}
	}
	
	return bvars;
}

Object* Container::ulimit() const
{
	return extractType(uplimit);
}

Object* Container::dlimit() const
{
	return extractType(downlimit);
}

bool Container::hasVars() const
{
	bool ret=false;
	
	if(m_params.isEmpty())
		ret = false;
	else {
		for(QList<Object*>::const_iterator i=m_params.begin(); !ret && i!=m_params.end(); ++i) {
			switch((*i)->type()) {
				case Object::variable:
					ret=true;
					break;
				case Object::container:
				{
					Container *c = (Container*) *i;
					ret |= c->hasVars();
					break;
				}
				default:
					ret=false;
			}
		}
	}
	return ret;
}

bool Container::operator==(const Container& c) const
{
	bool eq=c.m_params.count()==m_params.count();
	
	for(int i=0; eq && i<m_params.count(); ++i) {
		Object *o=m_params[i], *o1=c.m_params[i];
		eq = eq && equalTree(o, o1);
	}
	return eq;
}

bool Container::equalTree(const Object * o1, const Object * o2)
{
	Q_ASSERT(o1 && o2);
	if(o1==o2)
		return true;
	else if(o1->type()!=o2->type())
		return false;
	
	bool eq;
	switch(o2->type()) {
		case Object::variable:
			eq = *static_cast<const Ci*>(o1)==*static_cast<const Ci*>(o2);
			break;
		case Object::value:
			eq = *static_cast<const Cn*>(o1)==*static_cast<const Cn*>(o2);
			break;
		case Object::container:
			eq = *static_cast<const Container*>(o1)==*static_cast<const Container*>(o2);
			break;
		case Object::oper:
			eq = *static_cast<const Operator*>(o1)==*static_cast<const Operator*>(o2);
			break;
		case Object::vector:
			eq = *static_cast<const Vector*>(o1)==*static_cast<const Vector*>(o2);
			break;
		case Object::none:
			eq=false;
			Q_ASSERT(false && "Should not get here");
			break;
	}
	return eq;
}

#if 0
Container::Container(Cn* base, Object* var, Ci* degree) : Object(Object::container)
{
	if(!var)
		return;
	else if(!base && var && !degree) {
		m_params.append(new Operator(Operator::times));
		m_params.append(new Cn(1.));
		m_params.append(var);
	} else if(base && var && !degree) {
		m_params.append(new Operator(Operator::times));
		m_params.append(base);
		m_params.append(var);
	} else if(!base && var && degree) {
		m_params.append(new Operator(Operator::power));
		m_params.append(var);
		m_params.append(degree);
	} else {
		m_params.append(new Operator(Operator::times));
		m_params.append(base);
		m_params.append(var);
		m_params.append(new Container(0, var, degree));
	}
}

Cn* Container::monomialDegree(const Container& c)
{
	bool valid=false;
	int scalar=-1, var=-1;
	
	if(c.m_params[1]->type()==Object::value) {
		scalar=1;
		var=2;
		valid=true;
	} else if(c.m_params[2]->type()==Object::value) {
		scalar=2;
		var=1;
		valid=true;
	}
	
	if(c.firstOperator()==Operator::power) {
		return (Cn*) c.m_params[scalar];
	} else if(c.firstOperator()==Operator::times) {
		return monomialDegree(c.m_params[var]);
	}
	return 0;
}

Cn* Container::monomialBase(const Container& c)
{
	if(c.firstOperator()==Operator::times) {
		bool valid=false;
		int scalar=-1, var=-1;
		
		if(c.m_params[1]->type()==Object::value) {
			scalar=1;
			var=2;
			valid=true;
		} else if(c.m_params[2]->type()==Object::value) {
			scalar=2;
			var=1;
			valid=true;
		}
		
		if(valid)
			return (Cn*) c.m_params[scalar];
	}
	return 0;
}

Object* Container::monomialVar(const Container& c) //FIXME: Must improve these vars
{
	bool valid=false;
	int scalar=-1, var=-1;
	if(c.m_params[1]->type()==Object::value) {
		scalar=1;
		var=2;
		valid=true;
	} else if(c.m_params[2]->type()==Object::value) {
		scalar=2;
		var=1;
		valid=true;
	}
	
	if(valid) {
		Object *o = monomialVar(c.m_params[var]);
		if(!o)
			return c.m_params[var];
		else
			return o;
	}
	
	return 0;
}
#endif

void objectWalker(const Object* root, int ind)
{
	Container *c; Cn *num; Ci *var;
	QString s;
	
	if(ind>100) return;
	
	for(int i=0; i<ind; i++)
		s += " |_____";
	if(!root) {
		qDebug() << qPrintable(s) << "This is an null object: " << root;
		return;
	}
	
	switch(root->type()) { //TODO: include the function into a module and use toString
		case Object::container:
			Q_ASSERT(dynamic_cast<const Container*>(root));
			c= (Container*) root;
			qDebug() << qPrintable(s) << "| cont: " << c->tagName();// << "=" << c->toString();
			for(Container::const_iterator it=c->m_params.constBegin(); it<c->m_params.constEnd(); ++it)
				objectWalker(*it, ind+1);
			
			break;
		case Object::value:
			Q_ASSERT(dynamic_cast<const Cn*>(root));
			num= (Cn*) root;
			qDebug() << qPrintable(s) << "| num: " << num->value() << num->format();
			break;
		case Object::oper:
			Q_ASSERT(dynamic_cast<const Operator*>(root));
			qDebug() << qPrintable(s) << "| operator: " << root->toString();
			break;
		case Object::variable:
			Q_ASSERT(dynamic_cast<const Ci*>(root));
			var = (Ci*) root;
			qDebug() << qPrintable(s) << "| variable: " << var->name() << "Func:" << var->isFunction();
			break;
		case Object::vector: {
			const Vector* v;
			Q_ASSERT(v=dynamic_cast<const Vector*>(root));
			qDebug() << qPrintable(s) << "| vector: " << v->size();
			for(Vector::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
				objectWalker(*it, ind+1);
		}	break;
		default:
			qDebug() << qPrintable(s) << "| dunno: " << (int) root->type() << root;
			break;
	}
	if(ind==0)
		qDebug(";");
}

bool Container::isNumber() const
{
	return m_cont_type==apply || m_cont_type==math || m_cont_type==lambda || m_cont_type==declare ||
		m_cont_type==piecewise || m_cont_type==piece || m_cont_type==otherwise;
}

Container::iterator Container::firstValue()
{
	QList<Object *>::iterator it(m_params.begin()), itEnd(m_params.end());
	bool found=false;
	for(; it!=itEnd; ++it) {
		switch((*it)->type()) {
			case Object::value:
			case Object::variable:
			case Object::vector:
				found=true;
				break;
			case Object::container:
				if(((Container*) *it)->isNumber())
					found=true;
				break;
			case Object::oper:
			case Object::none:
				break;
		}
		if(found)
			break;
	}
	
	return it;
}

Container::const_iterator Container::firstValue() const
{
	QList<Object *>::const_iterator it(m_params.constBegin()), itEnd(m_params.constEnd());
	for(; it!=itEnd; ++it) {
		bool found=false;
		
		switch((*it)->type()) {
			case Object::value:
			case Object::vector:
			case Object::variable:
				found=true;
				break;
			case Object::container:
				if(((Container*) *it)->isNumber())
					found=true;
				break;
			case Object::oper:
			case Object::none:
				break;
		}
		if(found)
			break;
	}
	return it;
}

bool Container::isUnary() const
{
	QList<Object*>::const_iterator it(firstValue());
	return ++it==m_params.end();
}

bool Container::isCorrect() const
{
	bool ret=m_type==Object::container && m_cont_type!=none;
	if(m_cont_type==Container::apply) {
		Operator o=firstOperator();
		ret &= o.nparams()<0 || countValues()==o.nparams();
	}
	return ret;
}

QString Container::tagName() const
{
	return QString(m_typeStr[m_cont_type]);
}

bool Container::matches(const Object* exp, QMap<QString, const Object*>* found) const
{
	if(Object::container!=exp->type())
		return false;
	const Container* c=(const Container*) exp;
	if(m_params.count()!=c->m_params.count())
		return false;
	
	bool matching=true;
	Container::const_iterator it, it2, itEnd=m_params.constEnd();
	for(it=m_params.constBegin(), it2=c->m_params.constBegin(); matching && it!=itEnd; ++it, ++it2)
	{
		matching &= (*it)->matches(*it2, found);
	}
	return matching;
}

int Container::countValues() const
{
	Container::const_iterator it=firstValue(), itEnd=m_params.constEnd();
	int count=0;
	
	for(; it!=itEnd; ++it)
		count++;
	
	return count;
}

Object* Container::extractType(Container::ContainerType t) const
{
	for(Container::const_iterator it=m_params.begin(); it!=m_params.end(); ++it) {
		Container *c = (Container*) (*it);
		if(c->isContainer() && c->containerType()==t)
			return c->m_params.first();
	}
	return 0;
}
