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
#include "apply.h"
#include "value.h"
#include "analitzautils.h"
#include "operations.h"
#include "analyzer.h"

using namespace Analitza;

// static QDebug operator<<(QDebug dbg, const Object* c)
// {
//     dbg.nospace() << (c ? c->toString() : "<null>");
//
//     return dbg.space();
// }
//
// static QDebug operator<<(QDebug dbg, const Monomial &c)
// {
//     dbg.nospace() << "(" << c.first << ", " << c.second << ")";
//
//     return dbg.space();
// }

static Object* negateObject(Object* o)
{
    if(o->type()==Operator::value) {
        Cn* v = static_cast<Cn*>(o);
        v->rvalue() *= -1;
        return v;
    } else {
        Apply* a = new Apply;
        a->appendBranch(new Operator(Operator::minus));
        a->appendBranch(o);
        return a;
    }
}

static Object* simpExpression(Object* o)
{
    Analyzer a;
    a.setExpression(Expression(o));
    a.simplify();
    Expression ret = a.expression();
    return ret.takeTree();
}

bool Monomial::isScalar(const Object* o)
{
    return o->type()==Object::value || (o->type()==Object::vector && !AnalitzaUtils::hasVars(o));
}

Object* Monomial::createMono(const Operator& o) const
{
    Operator::OperatorType mult = o.multiplicityOperator();
    
    Object* toAdd=nullptr;
    if(first==0.)
        delete second;
    else if(first==1.)
        toAdd=second;
    else if(first==-1. && mult==Operator::times)
        toAdd=negateObject(second);
    else if(mult==Operator::times && second->isApply() && static_cast<Apply*>(second)->firstOperator()==Operator::times) {
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
//                     qDebug() << ";;;;;;;" << o2->toString();
                    Cn* sc= (Cn*) cx->m_params[scalar];
                    first = sc->value();
                    second = cx->m_params[var];
                    
                    cx->m_params[var]=nullptr;
                    delete cx;
                    
                    ismono=true;
//                     qDebug() << ":::::::" << *this;
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
            *cx->firstValue()=nullptr;
            delete cx;
            ismono=true;
                
            if(o==Operator::times)
                sign = !sign;
            else if(o==Operator::plus || o==Operator::minus)
                first *= -1;
        }
    } else if(o2->type()==Object::value && (o==Operator::plus || o==Operator::minus)) {
        Cn* v=static_cast<Cn*>(o2);
        if(v->value()<0) {
            v->rvalue()*=-1;
            first = -1;
            second = o2;
            ismono = true;
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
    : m_operator(c->firstOperator())
    , m_sign(true)
{
    bool first = true, firstValue = false;
    QList<Monomial> monos;
//     qDebug() << "1.........." << c->toString();
    for(Apply::const_iterator it=c->constBegin(), itEnd=c->constEnd(); it!=itEnd; ++it, first=false) {
        Monomial imono(m_operator, *it, m_sign);
        
        bool added=false;
        if(imono.second->isApply() && imono.first == 1) {
            Apply* a = static_cast<Apply*>(imono.second);
            Operator op=a->firstOperator();
            if(a->firstOperator()==m_operator
                || (m_operator==Operator::minus && op==Operator::plus)
                || (m_operator==Operator::plus && op==Operator::minus)
            ) {
                Polynomial p(a);
                
                a->m_params.clear();
                delete a;
                
                if((!first && m_operator==Operator::plus && op==Operator::minus)
                    || (first && m_operator==Operator::minus && op==Operator::plus)
                    || (first && m_operator==Operator::minus && op==Operator::minus)
                )
                {
                    //if it's a minus, the first is already positive
                    p.negate(1);
                }
                
                monos.append(p);
                added=true;
            }
        }
        
        if(!added)
            monos.append(imono);
    }
    
    first = true;
    for(auto it=monos.begin(), itEnd=monos.end(); it!=itEnd; ++it, first=false) {
        if(m_operator==Operator::minus && !first)
            it->first*=-1;
    }
    
    for(auto it=monos.begin(), itEnd=monos.end(); it!=itEnd; ++it, first=false)
        addMonomial(*it);
    
    simpScalars(firstValue);
}

void Polynomial::addMonomial(const Monomial& m)
{
    if(m.isValue()) {
        m_scalars += m.createMono(m_operator);
        return;
    }
    
    bool found = false;
    auto it1(begin());
    for(; it1!=end(); ++it1) {
        Object *o1=it1->second, *o2=m.second;
        found = AnalitzaUtils::equalTree(o1, o2);
        if(found)
            break;
    }
    
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

void Polynomial::simpScalars(bool firstValue)
{
    Object *value=nullptr;
    if(!firstValue && m_operator==Operator::minus && !m_scalars.isEmpty())
        m_scalars.first() = negateObject(m_scalars.first());

    for(auto i=m_scalars.constBegin(); i!=m_scalars.constEnd(); ++i) {
        bool d=false;
        
        Object* aux = simpExpression(*i);
        if(value) {
            QString* err=nullptr;
            value=Operations::reduce(m_operator.operatorType(), value, aux, &err);
            d=err;
            delete err;
        } else
            value=aux;
        
        if(d) {
            addValue(aux);
            value=nullptr;
        }
    }
    
    addValue(value);
    m_scalars.clear();
}

void Polynomial::addValue(Object* value)
{
    bool sign=false;
    if(!value)
        return;
    
    if(value->isZero())
        delete value;
    else {
        Monomial imono(m_operator, value, sign);
        
        if(m_operator==Operator::plus)
            append(imono);
        else if(m_operator==Operator::minus) {
            imono.first *= -1;
            append(imono);
        } else
            prepend(imono);
    }
}

Object* Polynomial::toObject()
{
    Object* root = nullptr;
    if(count()==1) {
        root = first().createMono(m_operator);
    } else if(count()>1) {
        Apply* c = new Apply;
        c->appendBranch(new Operator(m_operator));
        
        auto i=begin();
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
    
    if(!root) {
        root=new Cn(0.);
    } else if(!m_sign) {
        root = negateObject(root);
    }
    
    return root;
}

void Polynomial::negate(int i)
{
    for(iterator it=begin(); it!=end(); ++it, --i) {
        if(i<=0)
            it->first *= -1;
    }
}
