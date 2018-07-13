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

#include "providederivative.h"
#include "apply.h"
#include "expression.h"
#include "analitzautils.h"
#include "value.h"
#include "container.h"
#include "variable.h"

#include "list.h"
#include "vector.h"
#include "transformation.h"
#include <QCoreApplication>

using namespace Analitza;
using namespace AnalitzaUtils;

QList<Transformation> s_transformations;

bool independentTree(const Object* o)
{
    return !hasVars(o);
}

ProvideDerivative::ProvideDerivative(const QString& var) : var(var)
{
    if(s_transformations.isEmpty()) {
        QMap<QString, Transformation::treeCheck> nat;
        nat.insert(QStringLiteral("Real"), independentTree);
        
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(x:x)")), Transformation::parse(QStringLiteral("1")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(sin(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)*cos(p)")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(cos(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)*(-sin p)")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(tan(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)/(cos(p)**2)")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(f/g:x)")), Transformation::parse(QStringLiteral("(diff(f:x)*g-f*diff(g:x))/g**2")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(ln(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)/p")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(log(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)/(ln(10)*p)")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(f**Real:x)")), Transformation::parse(QStringLiteral("Real*diff(f:x)*f**(Real-1)")), nat); //this is just a simplification, should be deprecated
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(f**g:x)")), Transformation::parse(QStringLiteral("f**g*(diff(g:x)*ln f+g/f*diff(f:x))")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(abs(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)*p/abs(p)")));
        s_transformations += Transformation(Transformation::parse(QStringLiteral("diff(exp(p):x)")), Transformation::parse(QStringLiteral("diff(p:x)*exp(p)")));
    }
}

Object* ProvideDerivative::run(const Object* o)
{
    Apply* a=makeDiff(const_cast<Object*>(o));
    Object* ret = walk(a);
    clearDiff(a);
    return ret;
}

Object* ProvideDerivative::walkApply(const Apply* a)
{
    if(a->firstOperator().operatorType()==Operator::diff) {
        Object* val = *a->firstValue();
        if(!hasTheVar(QSet<QString>() << var, val))
            return new Cn(0.);
        
        foreach(const Transformation& t, s_transformations) {
            Object* newTree = t.applyTransformation(a);
            if(newTree) {
                Object* ret=walk(newTree);
                delete newTree;
                return ret;
            }
        }
        Object* ret = nullptr;
        if(val->isApply()) ret=derivativeApply(static_cast<Apply*>(val));
        else if(val->isContainer()) ret=derivativeContainer(static_cast<Container*>(val));
        else if(val->type()==Object::list) ret=derivateContentList(static_cast<const List*>(val));
        else if(val->type()==Object::vector) ret=derivateContentVector(static_cast<const Vector*>(val));
        
        if(!ret) {
            ret = a->copy();
            m_errors += QCoreApplication::tr("Could not calculate the derivative for '%1'").arg(ret->toString());
        }
        return ret;
    } else
        return AbstractExpressionTransformer::walkApply(a);
}

#define ITERATE(T, ...)\
Object* ProvideDerivative::derivateContent##T(const T * v)\
{\
    T* ret = new T(__VA_ARGS__);\
    T::const_iterator it=v->constBegin(), itEnd=v->constEnd();\
    for(; it!=itEnd; ++it) {\
        Apply* a=makeDiff(*it);\
        ret->appendBranch(walk(a));\
        clearDiff(a);\
    }\
    return ret;\
}

ITERATE(List, {})
ITERATE(Vector, v->size())

Object* ProvideDerivative::derivativeApply(const Apply* c)
{
    Operator op = c->firstOperator();
    switch(op.operatorType()) {
        case Operator::minus:
        case Operator::plus: {
            Apply *r= new Apply;
            r->appendBranch(new Operator(op));
            
            Apply::const_iterator it(c->firstValue());
            for(; it!=c->constEnd(); ++it) {
                Apply* a=makeDiff(*it);
                r->appendBranch(walk(a));
                clearDiff(a);
            }
            return r;
        } break;
        case Operator::times: {
            Apply *nx = new Apply;
            nx->appendBranch(new Operator(Operator::plus));
            
            Apply::const_iterator it(c->firstValue());
            for(; it!=c->constEnd(); ++it) {
                Apply *neach = new Apply;
                neach->appendBranch(new Operator(Operator::times));
                
                Apply::const_iterator iobj(c->firstValue());
                for(; iobj!=c->constEnd(); ++iobj) {
                    Object* o;
                    if(iobj==it) {
                        Apply* a=makeDiff(*iobj);
                        o=walk(a);
                        clearDiff(a);
                    } else
                        o=(*iobj)->copy();
                    
                    neach->appendBranch(o);
                }
                nx->appendBranch(neach);
            }
            return nx;
        } break;
        default:
            break;
    }
    return nullptr;
}

Object* ProvideDerivative::derivativeContainer(const Container *c)
{
    if(c->containerType()==Container::lambda) {
        //TODO REVIEW
        return walk(makeDiff(c->m_params.last()));
    } else if(c->containerType()==Container::piecewise) {
        Container *newPw = new Container(Container::piecewise);
        
        foreach(Object* o, c->m_params) {
            Q_ASSERT(o->isContainer());
            Container *p = (Container *) o;
            Container *np = new Container(p->containerType());
            
            Apply* a=makeDiff(p->m_params[0]);
            np->m_params += walk(a);
            clearDiff(a);
            if(p->m_params.size()>1)
                np->m_params += p->m_params[1]->copy();
            newPw->appendBranch(np);
        }
        return newPw;
    } else if(c->containerType()==Container::declare) {
        Q_ASSERT(false);
        return c->copy();
    } else {
        Container *cret = new Container(c->containerType());
        foreach(Object* o, c->m_params) {
            Apply* a=makeDiff(o);
            cret->appendBranch(walk(a));
            clearDiff(a);
        }
        
        return cret;
    }
    return nullptr;
}

Apply* ProvideDerivative::makeDiff(Object* o) const
{
    Apply* a = new Apply;
    a->appendBranch(new Operator(Operator::diff));
    a->appendBranch(o);
    a->addBVar(new Ci(var));
    
    return a;
}

void ProvideDerivative::clearDiff(Apply* a)
{
    *a->firstValue()=nullptr;
    delete a;
}
