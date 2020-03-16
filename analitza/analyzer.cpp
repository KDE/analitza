/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "analyzer.h"

#include <QCoreApplication>

#include "operations.h"
#include "value.h"
#include "vector.h"
#include "variables.h"
#include "container.h"
#include "object.h"
#include "list.h"
#include "variable.h"
#include "analitzautils.h"
#include "expressiontypechecker.h"
#include "apply.h"
#include "providederivative.h"
#include "polynomial.h"
#include "transformation.h"
#include "substituteexpression.h"
#include "expressionstream.h"
#include "matrix.h"
#include "commands/realpower.h"
#include "commands/listcommands.h"
#include "commands/vectorcommands.h"
#include "commands/matrixcommands.h"
#include "commands/blockmatrixcommands.h"
#include "commands/matrixqueries.h"

#include <config-analitza.h>
#ifdef HAVE_EIGEN3
#include "commands/eigencommands.h"
#endif

// #define SCRIPT_PROFILER

#ifdef SCRIPT_PROFILER
#include <QTime>
#include <QFile>

class ScriptProfiler
{
public:
    ScriptProfiler()
    {
        f=new QFile("/tmp/analitza_profile");
        bool a=f->open(QFile::WriteOnly);
        Q_ASSERT(a);
        stream=new QTextStream(f);
        
        t.start();
    }
    
    ~ScriptProfiler() {
        delete f; delete stream;
    }
    
    void push(const QString& name)
    {
        if(times.isEmpty())
            t.restart();
        times.push(Node(name, t.elapsed()));
    }
    
    void pop() {
        Node n = times.pop();
        
        *stream << "callf" << n.name << QString::number(t.elapsed()-n.start) << endl;
        if(times.isEmpty()) {
            t.restart();
            *stream << endl;
        }
    }
private:
    struct Node {
        Node(const QString& name, int time) : name(name), start(time) {}
        Node() : start(-1) {}
        
        QString name; int start;
    };
    
    QTime t;
    QStack<Node> times;
    QTextStream* stream;
    QFile* f;
};

ScriptProfiler profiler;
#endif

using namespace AnalitzaUtils;
using namespace Analitza;

namespace Analitza
{
class BoundingIterator
{
    public:
        virtual ~BoundingIterator() {}
        virtual bool hasNext()=0;
};
}

template <class T>
QStringList printAll(const QVector<T*> & p)
{
    QStringList ret;
    foreach(T* v, p)
        if(v)
            ret += v->toString();
        else
            ret += QStringLiteral("<null>");
    return ret;
}

const int defsize = /*500*/0;

Analyzer::Analyzer()
    : m_vars(new Variables), m_runStackTop(-1), m_hasdeps(true)
{
    m_runStack.reserve(defsize);
    registerBuiltinMethods();
}

Analyzer::Analyzer(Variables* v)
    : Analyzer(QSharedPointer<Variables>(new Variables(*v)))
{}

Analyzer::Analyzer(const QSharedPointer<Variables> & v)
    : m_vars(v), m_runStackTop(-1), m_hasdeps(true)
{
    m_runStack.reserve(defsize);
    Q_ASSERT(v);
    registerBuiltinMethods();
}

Analyzer::Analyzer(const Analyzer& a)
    : m_exp(a.m_exp), m_vars(new Variables(*a.m_vars)), m_err(a.m_err), m_runStackTop(-1), m_hasdeps(a.m_hasdeps)
{
    m_runStack.reserve(defsize);
    registerBuiltinMethods();
}

Analyzer::~Analyzer()
{
}

void Analyzer::registerBuiltinMethods()
{
    m_builtin.insertFunction(RealPower::id, RealPower::type, new RealPower);
    m_builtin.insertFunction(RangeCommand::id, RangeCommand::type, new RangeCommand);
    m_builtin.insertFunction(VectorCommand::id, VectorCommand::type, new VectorCommand);
    m_builtin.insertFunction(MatrixCommand::id, MatrixCommand::type, new MatrixCommand);
    m_builtin.insertFunction(BlockMatrixCommand::id, BlockMatrixCommand::type, new BlockMatrixCommand);
    m_builtin.insertFunction(IdentityMatrixCommand::id, IdentityMatrixCommand::type, new IdentityMatrixCommand);
    m_builtin.insertFunction(DiagonalMatrixCommand::id, DiagonalMatrixCommand::type, new DiagonalMatrixCommand);
    m_builtin.insertFunction(BlockDiagonalMatrixCommand::id, BlockDiagonalMatrixCommand::type, new BlockDiagonalMatrixCommand);
    m_builtin.insertFunction(TridiagonalMatrixCommand::id, TridiagonalMatrixCommand::type, new TridiagonalMatrixCommand);
    m_builtin.insertFunction(IsZeroMatrixCommand::id, IsZeroMatrixCommand::type, new IsZeroMatrixCommand);
    m_builtin.insertFunction(IsIdentityMatrixCommand::id, IsIdentityMatrixCommand::type, new IsIdentityMatrixCommand);
    m_builtin.insertFunction(IsDiagonalMatrixCommand::id, IsDiagonalMatrixCommand::type, new IsDiagonalMatrixCommand);
#ifdef HAVE_EIGEN3
    m_builtin.insertFunction(EigenvaluesCommand::id, EigenvaluesCommand::type, new EigenvaluesCommand);
    m_builtin.insertFunction(EigenvectorsCommand::id, EigenvectorsCommand::type, new EigenvectorsCommand);
#endif
}

void Analyzer::setExpression(const Expression & e)
{
    m_exp=e;
    flushErrors();
    if(!e.tree()) {
        m_err << QCoreApplication::tr("Cannot calculate an empty expression");
    } else if(m_exp.isCorrect()) {
        ExpressionTypeChecker check(m_vars.data());
        check.initializeVars(m_builtin.varTypes());
        m_currentType=check.check(m_exp);
        
        QMap<QString, ExpressionType> types=check.variablesTypes();
        for(QMap<QString, ExpressionType>::const_iterator it=types.constBegin(), itEnd=types.constEnd(); it!=itEnd; ++it)
            m_variablesTypes.insert(it.key(), it.value());
        
        m_err += check.errors();
        m_hasdeps = check.hasDependencies();
    }
}

void Analitza::Analyzer::setVariables(const QSharedPointer<Analitza::Variables>& v)
{
    m_vars = v;
}

void Analyzer::importScript(QTextStream* stream)
{
    ExpressionStream s(stream);
    for(; !s.atEnd(); ) {
        setExpression(s.next());
        if(!s.isInterrupted())
            calculate();
        
        if(!isCorrect()) {
            m_err += s.lastLine();
            break;
        }
    }
}

Expression Analyzer::evaluate()
{
    Expression e;
    
    if(isCorrect()) {
        m_runStackTop = 0;
        m_runStack.clear();
        Object *o=eval(m_exp.tree(), true, QSet<QString>());
        
        o=simp(o);
        e.setTree(o);
    } else {
        m_err << QCoreApplication::tr("Must specify a correct operation");
    }
    return e;
}

Expression Analyzer::calculate()
{
    Expression e;
    
    if(!m_hasdeps && isCorrect()) {
        m_runStackTop = 0;
        m_runStack.clear();
        e.setTree(calc(m_exp.tree()));
    } else {
        if(m_exp.isCorrect() && m_hasdeps)
            m_err << QCoreApplication::tr("Unknown identifier: '%1'").arg(
                            dependencies(m_exp.tree(), m_vars->keys()+m_builtin.identifiers()).join(
                                QCoreApplication::translate("identifier separator in error message", "', '")));
        else
            m_err << QCoreApplication::tr("Must specify a correct operation");
    }
    return e;
}

Expression Analyzer::calculateLambda()
{
    Expression e;
    
    if(Q_LIKELY(!m_hasdeps && m_exp.isCorrect())) {
        Q_ASSERT(m_exp.tree()->isContainer());
        Container* math=(Container*) m_exp.tree();
        Q_ASSERT(math->m_params.first()->isContainer());
        if(math->containerType()==Container::math) {
            math=(Container*) math->m_params.first();
            Q_ASSERT(math->m_params.first()->isContainer());
        }
        
        Container* lambda=(Container*) math;
        Q_ASSERT(lambda->containerType()==Container::lambda);
        
        if(Q_UNLIKELY(m_runStack.first()!=lambda))
            m_runStack.prepend(lambda);
        m_runStackTop = 0;
        e.setTree(calc(lambda->m_params.last()));
    } else {
        m_err << QCoreApplication::tr("Must specify a correct operation");
        
        if(m_exp.isCorrect() && m_hasdeps)
            m_err << QCoreApplication::tr("Unknown identifier: '%1'").arg(
                            dependencies(m_exp.tree(), m_vars->keys()).join(
                                QCoreApplication::translate("identifier separator in error message", "', '")));
    }
    return e;
}

template<typename T, typename Tcontained>
Analitza::Object * Analyzer::evalElements(const Analitza::Object* branch, T* nv, bool resolve, const QSet<QString>& unscoped)
{
    const T* v=static_cast<const T*>(branch);
    auto itEnd=v->constEnd();
    for(auto it=v->constBegin(); it!=itEnd; ++it) {
        Object* res=eval(*it, resolve, unscoped);
        nv->appendBranch(static_cast<Tcontained*>(res));
    }
    return nv;
}

Object* Analyzer::eval(const Object* branch, bool resolve, const QSet<QString>& unscoped)
{
    Q_ASSERT(branch);
    Object *ret=nullptr;
//     qDebug() << "POPOPO" << branch->toString();
    
    //Won't calc() so would be a good idea to have it simplified
    if(branch->isContainer()) {
        const Container* c = (Container*) branch;
        
//         Q_ASSERT(!c->isEmpty());
        switch(c->containerType()) {
            case Container::declare: {
                Ci *var = (Ci*) c->m_params[0];
                delete m_vars->take(var->name());
                ret = eval(c->m_params[1], true, unscoped);
                ret = simp(ret);
                Expression::computeDepth(ret);
                insertVariable(var->name(), ret);
            }    break;
            case Container::piecewise: {
                Container::const_iterator it=c->m_params.constBegin(), itEnd=c->constEnd();
                
                bool allfalse=true;
                for(; !ret && it!=itEnd; ++it) {
                    Container *p=static_cast<Container*>(*it);
                    Q_ASSERT( (*it)->isContainer() &&
                        (p->containerType()==Container::piece || p->containerType()==Container::otherwise) );
                    bool isPiece = p->containerType()==Container::piece;
                    if(isPiece) {
                        Object *cond=simp(eval(p->m_params[1], resolve, unscoped));
                        
                        if(cond->type()==Object::value) {
                            Cn* cval=static_cast<Cn*>(cond);
                            if(cval->isTrue()) {
                                allfalse=false;
                                ret=eval(p->m_params[0], resolve, unscoped);
                            }
                        } else
                            allfalse=false;
//                         qDebug() << "%%%%%" << cond->toString() << p->m_params[1]->toString() << allfalse;
                        
                        delete cond;
                    } else if(allfalse) {
                        ret=eval(p->m_params[0], resolve, unscoped);
                    }
                }
                if(!ret)
                    ret=c->copy();
                
            }    break;
            case Container::lambda: {
                QSet<QString> newUnscoped(unscoped);
                newUnscoped+=c->bvarStrings().toSet();
                
                Container *r = c->copy();
                Object* old=r->m_params.last();
                
                
                int top = m_runStackTop;
                m_runStackTop = m_runStack.size();
                m_runStack.resize(m_runStackTop+c->bvarCount()+1);
                
                r->m_params.last()=eval(old, false, newUnscoped);
                delete old;

                m_runStack.resize(m_runStackTop);
                m_runStackTop = top;
                
                alphaConversion(r, r->bvarCi().at(0)->depth());
                Expression::computeDepth(r);
                ret=r;
            } break;
            case Container::math:
                ret=nullptr;
                foreach(const Object* o, c->m_params) {
                    delete ret;
                    ret=eval(o, resolve, unscoped);
                }
                break;
            default:
                Q_ASSERT(false);
                break;
        }
    } else if(resolve && branch->type()==Object::variable) {
        Ci* var=(Ci*) branch;
        
        if(!unscoped.contains(var->name())) {
            Object* val = variableValue(var);
            
            if(val && !equalTree(var, val)) {
                QSet<QString> newUnscoped=unscoped;
                newUnscoped.insert(var->name());
                ret = eval(val, resolve, newUnscoped);
            }
        }
        
//         qDebug() << "peeee" << var->name() << val << resolve << unscoped;
    } else if(branch->type()==Object::vector) {
        ret = evalElements<Vector>(branch, new Vector(static_cast<const Vector*>(branch)->size()), resolve, unscoped);
    } else if(branch->type()==Object::list) {
        ret = evalElements<List>(branch, new List, resolve, unscoped);
    } else if(branch->type()==Object::matrix) {
        ret = evalElements<Matrix, MatrixRow>(branch, new Matrix, resolve, unscoped);
    } else if(branch->type()==Object::matrixrow) {
        ret = evalElements<MatrixRow>(branch, new MatrixRow, resolve, unscoped);
    } else if(branch->type()==Object::apply) {
        const Apply* c=static_cast<const Apply*>(branch);
        Operator op = c->firstOperator();
        
        switch(op.operatorType()) {
            case Operator::diff: {
                //FIXME: Must support multiple bvars
                QStringList bvars = c->bvarStrings();
                
                Q_ASSERT(!c->isEmpty());
                Object* o=derivative(bvars[0], *c->firstValue());
                
                Container* cc=new Container(Container::lambda);
                foreach(const QString& v, bvars) {
                    Container* bvar=new Container(Container::bvar);
                    bvar->appendBranch(new Ci(v));
                    cc->appendBranch(bvar);
                }
                cc->appendBranch(simp(o));
                ret=cc; 
                
                Expression::computeDepth(ret);
            }    break;
            case Operator::function: {
                //it is a function. I'll take only this case for the moment
                //it is only meant for operations with scoped variables that _change_ its value => have a value
                Object* body=simp(eval(c->m_params[0], true, unscoped));
                
                const Container *cbody = dynamic_cast<Container*>(body);
                if(resolve && cbody && cbody->m_params.size()==c->m_params.size() && cbody->containerType()==Container::lambda) {
                    int bvarsSize = cbody->bvarCount();
                    QVector<Object*> args(bvarsSize+1);
                    
                    args[0]=cbody->copy();
                    for(int i=0; i<bvarsSize; i++) {
                        args[i+1]=simp(eval(c->m_params[i+1], resolve, unscoped));
                    }
                    int aux = m_runStackTop;
                    m_runStackTop = m_runStack.size();
                    m_runStack.resize(m_runStackTop+bvarsSize+1);
                    
                    int i=0;
                    foreach(Object* o, args)
                        m_runStack[m_runStackTop+i++]=o;
                    ret=eval(cbody->m_params.last(), resolve, unscoped);
                    
                    qDeleteAll(m_runStack.begin()+m_runStackTop, m_runStack.end());
                    m_runStack.resize(m_runStackTop);
                    m_runStackTop = aux;
                    
                    Expression::computeDepth(ret);
                }
                
                if(!ret)
                    ret=c->copy();
                
                delete body;
            }    break;
            case Operator::forall:
            case Operator::exists:
            case Operator::sum:
            case Operator::product: {
                Apply *r = c->copy();
                
                QSet<QString> newUnscoped(unscoped);
                int top = m_runStack.size();
                bool resolved=false;
                
                QSet<QString> bvars = c->bvarStrings().toSet();
                newUnscoped += bvars;
                m_runStack.resize(top + bvars.size());
                
                if(r->domain()) {
                    QScopedPointer<Object> o(r->domain());
                    r->domain()=eval(r->domain(), resolve, unscoped);
                    resolved=r->domain()->type()==Object::list || r->domain()->type()==Object::vector || r->domain()->type()==Object::matrix;
                } else {
                    if(r->dlimit()) {
                        QScopedPointer<Object> o(r->dlimit());
                        r->dlimit()=eval(r->dlimit(), resolve, unscoped);
                    }
                    if(r->ulimit()) {
                        QScopedPointer<Object> o(r->ulimit());
                        r->ulimit()=eval(r->ulimit(), resolve, unscoped);
                    }
                    
                    resolved=r->dlimit()->type()==Object::value && r->ulimit()->type()==Object::value;
                }
                
                if(resolved && hasVars(*r->firstValue(), newUnscoped.values())) {
                    BoundingIterator *it = r->domain()? initBVarsContainer(r, top, r->domain()->copy()) : initBVarsRange(r, top, r->dlimit(), r->ulimit());
                    
                    if(it) {
                        QVector<Object*> values;
                        Object* element = r->m_params.first();
                        do {
                            values += eval(element, resolve, unscoped);
                        } while(it->hasNext());
                        
                        if(values.size()==1)
                            ret = values.first();
                        else {
                            r->ulimit()=nullptr;
                            r->dlimit()=nullptr;
                            r->domain()=nullptr;
                            
                            Apply *newC = new Apply;
                            Operator::OperatorType t;
                            switch(op.operatorType()) {
                                case Operator::product: t = Operator::times; break;
                                case Operator::sum:        t = Operator::plus; break;
                                case Operator::forall:    t = Operator::_and; break;
                                default: /*exists*/        t = Operator::_or; break;
                            }
                            
                            newC->appendBranch(new Operator(t));
                            newC->m_params = values;
                            ret = newC;
                        }
                        delete r;
                    } else
                        ret = r;
                    
                    delete it;
                } else {
                    Apply::iterator it=r->firstValue(), itEnd=r->end();
                    for(; it!=itEnd; ++it) {
                        Object *o=*it;
                        *it= eval(*it, resolve, newUnscoped);
                        delete o;
                    }
                    ret=r;
                }
                
//                 qDeleteAll(m_runStack.begin()+top, m_runStack.end());
                m_runStack.resize(top);
                
            }    break;
            default: {
                Q_ASSERT(!op.isBounded());
                Apply *r = c->copy();
                
                Apply::iterator it=r->firstValue(), itEnd=r->end();
                for(; it!=itEnd; ++it) {
                    Object *o=*it;
                    *it= eval(*it, resolve, unscoped);
                    delete o;
                }
                
//                 ret=simp(r);
                ret=r;
                
            }    break;
        }
    }
    
    if(!ret)
        ret=branch->copy();
    Q_ASSERT(ret);
    
    return ret;
}

Object* Analyzer::derivative(const QString &var, const Object* o)
{
    Q_ASSERT(o);
    
    ProvideDerivative v(var);
    Object* ret = v.run(o);
    
    if(!v.isCorrect())
        m_err += v.errors();
    return ret;
}

Object* Analyzer::calcPiecewise(const Container* c)
{
    Object* ret=nullptr;
    //Here we have a list of options and finally the otherwise option
    foreach(Object *o, c->m_params) {
        Container *p=static_cast<Container*>(o);
        Q_ASSERT( o->isContainer() &&
                (p->containerType()==Container::piece ||
                p->containerType()==Container::otherwise) );
        bool isPiece = p->containerType()==Container::piece;
        if(isPiece) {
            QScopedPointer<Cn> condition((Cn*) calc(p->m_params[1]));
            if(condition->isTrue()) {
                ret=calc(p->m_params[0]);
                break;
            }
            
        } else {
            //it is an otherwise
            ret=calc(p->m_params.first());
            break;
        }
    }
    
    if(Q_UNLIKELY(!ret)) {
        m_err << QCoreApplication::translate("Error message, no proper condition found.", "Could not find a proper choice for a condition statement.");
        ret=new Cn(0.);
    }
    
    return ret;
}

Object* Analyzer::calcMath(const Container* c)
{
    return calc(*c->constBegin());
}

Object* Analyzer::calcLambda(const Container* c)
{
    Container * cc=c->copy();
    if(cc->bvarCount()>0)
        alphaConversion(cc, cc->bvarCi().at(0)->depth());
    Expression::computeDepth(cc);
    return cc;
}

Object* Analyzer::calcDeclare(const Container* c)
{
    Object *ret=nullptr;
    
    const Ci *var = (const Ci*) c->m_params[0];
    ret=simp(c->m_params[1]->copy());
    Expression::computeDepth(ret);
    bool corr = insertVariable(var->name(), ret);
    
    Q_ASSERT(ret && corr);
    return ret;
}

template<class T, class Tcontained>
Object* Analyzer::calcElements(const Object* root, T* nv)
{
    const T *v=static_cast<const T*>(root);
    auto itEnd=v->constEnd();
    
    for(auto it=v->constBegin(); it!=itEnd; ++it)
        nv->appendBranch(static_cast<Tcontained*>(calc(*it)));
    
    return nv;
}

Object* Analyzer::calc(const Object* root)
{
    Q_ASSERT(root);
    Object* ret=nullptr;
    
    switch(root->type()) {
        case Object::container:
            ret = operate((const Container*) root);
            break;
        case Object::apply:
            ret = operate((const Apply*) root);
            break;
        case Object::vector:
            ret = calcElements<Vector>(root, new Vector(static_cast<const Vector*>(root)->size()));
            break;
        case Object::list:
            ret = calcElements<List>(root, new List);
            break;
        case Object::matrix:
            ret = calcElements<Matrix, MatrixRow>(root, new Matrix);
            break;
        case Object::matrixrow:
            ret = calcElements<MatrixRow>(root, new MatrixRow);
            break;
        case Object::value:
        case Object::custom:
            ret=root->copy();
            break;
        case Object::variable:
            ret=variableValue((Ci*) root);
            if(ret)
                ret = calc(ret);
            else {
                Container* c= new Container(Container::lambda);
                c->appendBranch(root->copy());
                ret=c;
            }
            break;
        case Object::oper:
        case Object::none:
            break;
    }
    Q_ASSERT(ret);
    return ret;
}

bool isNull(Analitza::Operator::OperatorType opt, Object* ret)
{
    return ret->type()==Object::value &&
        ((opt==Operator::_and && static_cast<Cn*>(ret)->value()==0.) || (opt==Operator::_or && static_cast<Cn*>(ret)->value()==1.));
}

Object* Analyzer::operate(const Apply* c)
{
    Object* ret=nullptr;
    const Operator& op = c->firstOperator();
    Operator::OperatorType opt=op.operatorType();
    
    switch(opt) {
        case Operator::sum:
            ret = sum(*c);
            break;
        case Operator::product:
            ret = product(*c);
            break;
        case Operator::forall:
            ret = forall(*c);
            break;
        case Operator::exists:
            ret = exists(*c);
            break;
        case Operator::function:
            ret = func(*c);
            break;
        case Operator::diff:
            ret = calcDiff(c);
            break;
        case Operator::map:
            ret = calcMap(c);
            break;
        case Operator::filter:
            ret = calcFilter(c);
            break;
        default: {
            int count=c->countValues();
            Q_ASSERT(count>0);
            Q_ASSERT(    (op.nparams()< 0 && count>1) ||
                        (op.nparams()>-1 && count==op.nparams()) ||
                        opt==Operator::minus);
            
            QString* error=nullptr;
            if(count>=2) {
                Apply::const_iterator it = c->firstValue(), itEnd=c->constEnd();
                ret = calc(*it);
                ++it;
                bool stop=isNull(opt, ret);
                for(; !stop && it!=itEnd; ++it) {
                    bool isValue = (*it)->type()==Object::value;
                    Object* v = isValue ? *it : calc(*it);
                    if(Q_UNLIKELY(v->isNone())) {
                        Q_ASSERT(!isValue);
                        ret = v;
                        break;
                    }
                    ret=Operations::reduce(opt, ret, v, &error);
                    if(!isValue)
                        delete v;
                    
                    if(Q_UNLIKELY(error)) {
                        m_err.append(*error);
                        delete error;
                        error=nullptr;
                        break;
                    }
                    
                    stop=isNull(opt, ret);
                }
            } else {
                ret = calc(*c->firstValue());
                if(Q_LIKELY(!ret->isNone())) {
                    ret=Operations::reduceUnary(opt, ret, &error);
                    if(Q_UNLIKELY(error)) {
                        m_err.append(*error);
                        delete error;
                    }
                }
            }
        }    break;
    }
    
    Q_ASSERT(ret);
    return ret;
}

Analyzer::funcContainer Analyzer::operateContainer[Container::domainofapplication+1] =
    {nullptr, &Analyzer::calcMath, &Analyzer::calcDeclare, &Analyzer::calcLambda, nullptr,nullptr,nullptr,nullptr,&Analyzer::calcPiecewise,nullptr,nullptr};

Object* Analyzer::operate(const Container* c)
{
    Q_ASSERT(c);
    funcContainer f=operateContainer[c->containerType()];
    Q_ASSERT(f);
    Object* ret=(this->*f)(c);
    
    Q_ASSERT(ret);
    return ret;
}

namespace Analitza
{
    template <typename T, typename Iterator>
    class TypeBoundingIterator : public BoundingIterator
    {
        public:
            TypeBoundingIterator(QVector<Object*>& runStack, int top, const QVector<Ci*>& vars, T* l)
                : iterators(vars.size()), list(l)
                , itBegin(l->constBegin()), itEnd(l->constEnd())
                , m_runStack(runStack), m_top(top)
            {
                int s=vars.size();
                for(int i=0; i<s; i++) {
                    m_runStack[m_top+i]=*itBegin;
                    iterators[i]=itBegin;
                }
            }
            
            ~TypeBoundingIterator() { delete list; }
            
            virtual bool hasNext() override
            {
                bool cont=true;
                for(int i=iterators.size()-1; cont && i>=0; i--) {
                    ++iterators[i];
                    cont=(iterators[i]==itEnd);
                    
                    if(cont)
                        iterators[i]=itBegin;
                    m_runStack[m_top+i]=*iterators[i];
                }
                
                return !cont;
            }
        private:
            QVector<Iterator> iterators;
            T* list;
            const Iterator itBegin, itEnd;
            QVector<Object*>& m_runStack;
            int m_top;
    };
    
    class RangeBoundingIterator : public BoundingIterator
    {
        public:
            RangeBoundingIterator(const QVector<Cn*>& values, Cn* oul, Cn* odl, double step)
                : values(values), dl(odl->value()), ul(oul->value()), step(step), objdl(odl), objul(oul)
            {}
            
            ~RangeBoundingIterator()
            {
                qDeleteAll(values);
                delete objdl;
                delete objul;
            }
            
            bool hasNext() override
            {
                bool cont=true;
                for(int i=values.size()-1; cont && i>=0; i--) {
                    Cn* v=values[i];
                    double curr=v->value()+step;
                    cont=curr>ul;
                    
                    v->setValue(cont ? dl : curr);
                }
                
                return !cont;
            }
            
        private:
            const QVector<Cn*> values;
            const double dl, ul, step;
            Object* objdl;
            Object* objul;
    };
}

BoundingIterator* Analyzer::initializeBVars(const Apply* n, int base)
{
    BoundingIterator* ret=nullptr;
    
    Object* domain=n->domain();
    
    if(domain)
    {
        domain=calc(domain);
        ret = initBVarsContainer(n, base, domain);
        
        if(!ret)
            delete domain;
    }
    else
    {
        Object *objul=calc(n->ulimit());
        Object *objdl=calc(n->dlimit());
        
        ret = initBVarsRange(n, base, objdl, objul);
        
        if(!ret) {
            delete objdl;
            delete objul;
        }
    }
    return ret;
}
BoundingIterator* Analyzer::initBVarsContainer(const Analitza::Apply* n, int base, Object* domain)
{
    BoundingIterator* ret = nullptr;
    QVector<Ci*> bvars=n->bvarCi();
    
    switch(domain->type()) {
        case Object::matrix:
            Q_ASSERT(false && "fixme: properly iterate through elements when bounding");
            if(static_cast<Matrix*>(domain)->rowCount()>0)
                ret=new TypeBoundingIterator<Matrix, Matrix::const_iterator>(m_runStack, base, bvars, static_cast<Matrix*>(domain));
            break;
        case Object::list:
            if(static_cast<List*>(domain)->size()>0)
                ret=new TypeBoundingIterator<List, List::const_iterator>(m_runStack, base, bvars, static_cast<List*>(domain));
            break;
        case Object::vector:
            if(static_cast<Vector*>(domain)->size()>0)
                ret=new TypeBoundingIterator<Vector, Vector::const_iterator>(m_runStack, base, bvars, static_cast<Vector*>(domain));
            break;
        default:
            Q_ASSERT(false && "Type not supported for bounding.");
            m_err.append(QCoreApplication::tr("Type not supported for bounding."));
    }
    return ret;
}

BoundingIterator* Analyzer::initBVarsRange(const Apply* n, int base, Object* objdl, Object* objul)
{
    BoundingIterator* ret = nullptr;
    if(isCorrect() && objul->type()==Object::value && objdl->type()==Object::value) {
        Cn *u=static_cast<Cn*>(objul);
        Cn *d=static_cast<Cn*>(objdl);
        double ul=u->value();
        double dl=d->value();
        
        if(dl<=ul) {
            QVector<Ci*> bvars=n->bvarCi();
            QVector<Cn*> rr(bvars.size());
            
            for(int i=0; i<bvars.size(); ++i) {
                rr[i]=new Cn(dl);
                m_runStack[base+i]=rr[i];
            }
            
            ret=new RangeBoundingIterator(rr, u, d, 1);
        } else
            m_err.append(QCoreApplication::tr("The downlimit is greater than the uplimit"));
    } else
        m_err.append(QCoreApplication::tr("Incorrect uplimit or downlimit."));
    return ret;
}

Object* Analyzer::boundedOperation(const Apply& n, const Operator& t, Object* initial)
{
    Object* ret=initial;
    int top = m_runStack.size();
    m_runStack.resize(top+n.bvarCi().size());
    
    BoundingIterator* it=initializeBVars(&n, top);
    if(!it)
        return initial;
    
    QString* correct=nullptr;
    Operator::OperatorType type=t.operatorType();
    do {
        Object *val=calc(n.m_params.last());
        ret=Operations::reduce(type, ret, val, &correct);
        delete val;
        delete correct;
    } while(Q_LIKELY(it->hasNext() && !correct && !isNull(type, ret)));
    
    m_runStack.resize(top);
    
    delete it;
    Q_ASSERT(ret);
    return ret;
}

Object* Analyzer::product(const Apply& n)
{
    return boundedOperation(n, Operator(Operator::times), new Cn(1.));
}

Object* Analyzer::sum(const Apply& n)
{
    return boundedOperation(n, Operator(Operator::plus), new Cn(0.));
}

Object* Analyzer::forall(const Apply& n)
{
    return boundedOperation(n, Operator(Operator::_and), new Cn(true));
}

Object* Analyzer::exists(const Apply& n)
{
    return boundedOperation(n, Operator(Operator::_or), new Cn(false));
}

Object* Analyzer::func(const Apply& n)
{
    bool borrowed = n.m_params[0]->type()==Object::variable;
    Container *function = static_cast<Container*>(borrowed ? variableValue((Ci*) n.m_params[0]) : calc(n.m_params[0]));
    
//     static int ind=0;
//     qDebug() << "calling" << qPrintable(QString(++ind, '.')) << n.m_params.first()->toString() << n.toString();
    
    int bvarsize = n.m_params.size()-1;
    QVector<Object*> args(bvarsize);
    
    for(int i=1; i<bvarsize+1; i++) {
        args[i-1]=calc(n.m_params[i]);
//         qDebug() << "argumen" << qPrintable(QString(ind, '.')) << n.m_params[i]->toString() << "=" << args[i-1]->toString();
    }
    
    Object* ret = calcCallFunction(function, args, n.m_params[0]);
    
//     qDebug() << "called " << qPrintable(QString(ind--, '.')) << n.m_params.first()->toString() << ret->toString();
    if(!borrowed)
        delete function;
    
    return ret;
}

Object* Analyzer::calcCallFunction(Container* function, const QVector<Object*>& args, const Object* oper)
{
    Object* ret=nullptr;
    int bvarsize = args.size();
    
    if(function && function->m_params.size()>1) {
#ifdef SCRIPT_PROFILER
        profiler.push(borrowed? static_cast<Ci*>(n.m_params[0])->name() : function->toString());
#endif
        int top = m_runStack.size(), aux=m_runStackTop;
        m_runStack.resize(top+bvarsize+1);
        
        m_runStack[top] = function;
        for(int i=0; i<args.size(); i++) {
            if (args[i]->type() != Object::none) {
                m_runStack[top+i+1] = args[i];
            } else {
                m_err += QCoreApplication::tr("Invalid type for parameter '%1'").arg(i+1);
                ret = new None();
                
                return ret;
            }
        }
        m_runStackTop = top;
        
//         qDebug() << "diiiiiiiii" << function->toString() << m_runStack.size() << bvarsize << m_runStackTop << printAll(m_runStack);
        ret=calc(function->m_params.last());
        
        qDeleteAll(m_runStack.begin()+top+1, m_runStack.end());
        
        m_runStackTop = aux;
        m_runStack.resize(top);
    } else {
//         Q_ASSERT(function ? (function->m_params[0]->type()==Object::variable && function->m_params.size()==1) : n.m_params[0]->type()==Object::variable);
        QString id=static_cast<const Ci*>(function ? function->m_params[0] : oper)->name();
        FunctionDefinition* func=m_builtin.function(id);
        QList<Expression> expargs;
        
        for(int i=0; i<args.size(); i++) {
            if (args[i]->type() != Object::none) {
                expargs += Expression(args[i]);
            } else {
                m_err += QCoreApplication::tr("Invalid type for parameter '%1'").arg(i+1);
                ret = new None();
                
                return ret;
            }
        }
#ifdef SCRIPT_PROFILER
        profiler.push(id);
#endif
        Expression exp=(*func)(expargs);
        if(Q_UNLIKELY(exp.isCorrect())) {
            ret=exp.tree();
            exp.setTree(nullptr);
        } else {
            m_err += exp.error();
            ret = new None();
        }
    }
#ifdef SCRIPT_PROFILER
    profiler.pop();
#endif
    
    return ret;
}

Object* Analyzer::calcMap(const Apply* c)
{
    Container* f=static_cast<Container*>(calc(*c->firstValue()));
    List* l=static_cast<List*>(calc(*(c->firstValue()+1)));
    
    List::iterator it=l->begin(), itEnd=l->end();
    for(; it!=itEnd; ++it) {
        *it = calcCallFunction(f, { *it }, f);
    }
    
    delete f;
    return l;
}

Object* Analyzer::calcFilter(const Apply* c)
{
    Container* f=static_cast<Container*>(calc(*c->firstValue()));
    List* l=static_cast<List*>(calc(*(c->firstValue()+1)));
    
    List::iterator it=l->begin(), itEnd=l->end();
    List* ret = new List;
    for(; it!=itEnd; ++it) {
        QVector<Object*> args(1, (*it)->copy());
        
        Object* ss=*it;
        Cn* val = static_cast<Cn*>(calcCallFunction(f, args, f));
        
        if(val->isTrue()) {
            ret->appendBranch(ss->copy());
        }
        delete val;
    }
    
    delete l;
    delete f;
    return ret;
}


Object* Analyzer::calcDiff(const Apply* c)
{
    //TODO: Make multibvar
    QVector<Ci*> bvars=c->bvarCi();
    
    //We construct the lambda
    Object* o=derivative(bvars[0]->name(), *c->firstValue());
    o=simp(o);
    
    Container* cc=new Container(Container::lambda);
    foreach(const Ci* v, bvars) {
        Container* bvar=new Container(Container::bvar);
        bvar->appendBranch(v->copy());
        cc->appendBranch(bvar);
    }
    
    cc->appendBranch(o);
    
    Expression::computeDepth(cc);
    return cc;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

void Analyzer::simplify()
{
    if(m_exp.isCorrect() && m_exp.tree()) {
        m_runStackTop = 0;
        Object* o=simp(m_exp.tree());
        m_exp.setTree(o);
        setExpression(m_exp);
    }
}

template <class T, class Tcontained>
void Analyzer::iterateAndSimp(T* v)
{
    auto it = v->begin(), itEnd=v->end();
    
    for(; it!=itEnd; ++it)
        *it = static_cast<Tcontained*>(simp(*it));
}

Object* Analyzer::simp(Object* root)
{
    Q_ASSERT(root && root->type()!=Object::none);
    if(!isCorrect())
        return root;
    
    if(!root->isContainer() && !hasVars(root))
    {
        if(root->type()!=Object::value && root->type() !=Object::oper) {
            Object* aux=root;
            root = calc(root);
            delete aux;
            
            if(isLambda(root))
                root = simp(root);
        }
    } else if(root->type()==Object::vector) {
        iterateAndSimp<Vector>(static_cast<Vector*>(root));
    } else if(root->type()==Object::matrix) {
        iterateAndSimp<Matrix, MatrixRow>(static_cast<Matrix*>(root));
    } else if(root->type()==Object::matrixrow) {
        iterateAndSimp<MatrixRow>(static_cast<MatrixRow*>(root));
    } else if(root->type()==Object::list) {
        iterateAndSimp<List>(static_cast<List*>(root));
    } else if(root->type()==Object::apply) {
        root = simpApply((Apply*) root);
    } else if(root->isContainer()) {
        Container *c= (Container*) root;
        
        switch(c->containerType()) {
            case Container::piecewise:
                root=simpPiecewise(c);
                break;
            case Container::lambda: {
                int top = m_runStackTop;
                m_runStackTop = m_runStack.size();
                m_runStack.resize(m_runStackTop+c->bvarCount()+1);
                
                c->m_params.last()=simp(c->m_params.last());
                m_runStack.resize(m_runStackTop);
                m_runStackTop = top;
            }    break;
            default:
                iterateAndSimp<Container>(c);
                break;
        }
    }
    return root;
}

Object* applyTransformations(Object* root, const QList<Transformation>& trans)
{
    foreach(const Transformation& t, trans) {
        Object* o = t.applyTransformation(root);
        if(o) {
            delete root;
            return o;
        }
    }
    return root;
}

bool actuallyE(const Object* o)
{
    return o->type()==Object::variable && static_cast<const Ci*>(o)->name()==QLatin1String("e");
}

QList<Transformation> simplifications()
{
    static QList<Transformation> ret;
    if(Q_UNLIKELY(ret.isEmpty())) {
        //divide
        ret += Transformation(Transformation::parse(QStringLiteral("f/f")), Transformation::parse(QStringLiteral("1")));
        ret += Transformation(Transformation::parse(QStringLiteral("f/1")), Transformation::parse(QStringLiteral("f")));
        
        //power
        ret += Transformation(Transformation::parse(QStringLiteral("0**k")), Transformation::parse(QStringLiteral("0")));
        ret += Transformation(Transformation::parse(QStringLiteral("1**k")), Transformation::parse(QStringLiteral("1")));
        ret += Transformation(Transformation::parse(QStringLiteral("x**1")), Transformation::parse(QStringLiteral("x")));
        ret += Transformation(Transformation::parse(QStringLiteral("(x**y)**z")), Transformation::parse(QStringLiteral("x**(y*z)")));

        //ln
        QMap<QString, Transformation::treeCheck> eulerNumber;
        eulerNumber.insert(QStringLiteral("e"), actuallyE);
        ret += Transformation(Transformation::parse(QStringLiteral("ln e")), Transformation::parse(QStringLiteral("1")), eulerNumber);
    }
    
    return ret;
}

Object* Analyzer::simpApply(Apply* c)
{
    Object* root=c;
    Apply::iterator it;
    Operator o = c->firstOperator();
    
    switch(o.operatorType()) {
        case Operator::times:
            for(it=c->firstValue(); c->countValues()>1 && it!=c->end();) {
                bool d=false;
                *it = simp(*it);
                
                if((*it)->type() == Object::value) {
                    Cn* n = (Cn*) (*it);
                    if(n->value()==1. && c->countValues()>1) { //1*exp=exp
                        d=true;
                    } else if(n->value()==0.) { //0*exp=0 //TODO Change to isZero and return the same type in 0
                        delete root;
                        root = new Cn(0.);
                        return root;
                    }
                }
                
                if(!d)
                    ++it;
                else {
                    delete *it;
                    it = c->m_params.erase(it);
                }
            }
            
            root=simpPolynomials(c);
            break;
        case Operator::minus:
        case Operator::plus: {
//             qDebug() << "........................" << c->toString();
            for(Apply::iterator it=c->begin(), itEnd=c->end(); it!=itEnd; ++it) {
                *it = simp(*it);
            }
            
//             qDebug()<< "PEPEPE" << c->toString();
            if(c->isUnary()) {
                if(o==Operator::minus && (*c->firstValue())->type()==Object::value) {
                    Cn* v = static_cast<Cn*>(*c->firstValue());
                    v->rvalue() *= -1;
                    
                    root=v;
                    *c->firstValue()=nullptr;
                    delete c;
                    c=nullptr;
                }
            } else {
                root=simpPolynomials(c);
                c=nullptr;
            }
//             qDebug()<< "PAAPPA" << root->toString();
            static QList<Transformation> addTrans;
            if(addTrans.isEmpty()) {
                addTrans += Transformation(Transformation::parse(QStringLiteral("--x")), Transformation::parse(QStringLiteral("x")));
                addTrans += Transformation(Transformation::parse(QStringLiteral("-a--b")), Transformation::parse(QStringLiteral("b-a")));
            }
            
            root = applyTransformations(root, addTrans);
        }    break;
        //TODO: extend to ::product
        case Operator::sum: {
            
            QStringList bvars=c->bvarStrings();
            if(c->ulimit()) c->ulimit()=simp(c->ulimit());
            if(c->dlimit()) c->dlimit()=simp(c->dlimit());
            if(c->domain()) c->domain()=simp(c->domain());
            
            Object *uplimit=c->ulimit(), *downlimit=c->dlimit(), *domain=c->domain();
            
            //TODO: simplify this code
            for(it = c->m_params.begin(); it!=c->end(); ++it)
                *it = simp(*it);
            
            //if bvars is empty, we are dealing with an invalid sum()
            Object* function = *c->firstValue();
            if(!bvars.isEmpty() && !domain && !hasTheVar(bvars.toSet(), function)) {
                Apply *cDiff=new Apply;
                cDiff->appendBranch(new Operator(Operator::minus));
                cDiff->appendBranch(uplimit  ->copy());
                cDiff->appendBranch(downlimit->copy());
                
                Apply *aPlusOne = new Apply;
                aPlusOne->appendBranch(new Operator(Operator::plus));
                aPlusOne->appendBranch(new Cn(1.));
                aPlusOne->appendBranch(cDiff);
                
                Apply *nc=new Apply;
                nc->appendBranch(new Operator(Operator::times));
                nc->appendBranch(aPlusOne);
                nc->appendBranch(function);
                
                c->m_params.last()=nullptr;
                delete c;
                root=simp(nc);
            } else if(function->isApply()) {
                root=simpSum(c);
            }
            
        }    break;
        case Operator::card: {
            Object* val=simp(*c->firstValue());
            if(val->type()==Object::vector)
            {
                c->m_params.last()=nullptr;
                QString* err=nullptr;
                val=Operations::reduceUnary(Operator::card, val, &err);
                if(Q_UNLIKELY(err)) { delete err; }
                delete c;
                root=val;
            }
        }    break;
        case Operator::selector: {
            c->m_params[0]=simp(c->m_params[0]);
            c->m_params[1]=simp(c->m_params[1]);
            
            Object* idx=c->m_params[0];
            Object* value=c->m_params[1];
            if(idx->type()==Object::value && value->type()==Object::vector) {
                QString* err=nullptr;
                Object* ret=Operations::reduce(Operator::selector, idx, value, &err);
                delete err;
                
                if(ret) {
                    root=ret;
                    c->m_params[0]=nullptr;
                    c->m_params[1]=nullptr;
                    
                    delete c;
                }
            }
        }    break;
        case Operator::_union: {
            Apply::iterator it=c->firstValue(), itEnd=c->end();
            
            QVector<Object*> newParams;
            for(; it!=itEnd; ++it) {
                *it=simp(*it);
                
                if(newParams.isEmpty())
                    newParams.append(*it);
                else {
                    if((*it)->type()==Object::list && newParams.last()->type()==Object::list) {
                        QString* err=nullptr;
                        Object* ret=Operations::reduce(Operator::_union, newParams.last(), *it, &err);
                        delete err;
                        newParams.last()=ret;
                        delete *it;
                    } else {
                        newParams.append(*it);
                    }
                }
                *it=nullptr;
            }
            
            if(newParams.last()==nullptr)
                newParams.resize(newParams.size()-1);
            
            //if only one, remove union
            if(newParams.size()==1) {
                delete c;
                root=newParams.last();
            } else {
                qDeleteAll(c->m_params);
                c->m_params=newParams;
                root=c;
            }
            
        }    break;
        case Operator::eq: {
            bool alleq=true, existsjustvar=false, allButFirstZero=false;
            QStringList deps = AnalitzaUtils::dependencies(c, QStringList());
            
            for(Apply::iterator it=c->firstValue(), itEnd=c->end(); it!=itEnd; ++it) {
                *it = simp(*it);
                alleq = alleq && equalTree(*c->firstValue(), *it);
                existsjustvar = existsjustvar || (*it)->type()==Object::variable;
                allButFirstZero = (it==c->firstValue() || (*it)->isZero());
            }
            
            if(alleq) {
                delete c;
                root = new Cn(true);
            } else if(!existsjustvar && deps.size()==1) {
                if(allButFirstZero) {
                    Analitza::Apply::iterator first = c->firstValue();
                    root = *first;
                    c->m_params.erase(first);
                    delete c;
                } else {
                    Apply* a = new Apply;
                    a->appendBranch(new Operator(Operator::minus));
                    
                    for(Apply::const_iterator it=c->constBegin(), itEnd=c->constEnd(); it!=itEnd; ++it) {
                        a->appendBranch(*it);
                    }
                    c->m_params.clear();
                    delete c;
                    
                    root = simp(a);
                }
                
                if(root->type()==Object::apply) {
                    QList<Object*> r = findRoots(deps.first(), static_cast<Apply*>(root));
                    
                    if(r.isEmpty()) {
                        Apply* na = new Apply;
                        na->appendBranch(new Operator(Operator::eq));
                        na->appendBranch(root);
                        na->appendBranch(new Cn(0.));
                        root=na;
                    } else if(r.size() == 1) {
                        Apply* a = new Apply;
                        a->appendBranch(new Operator(Operator::eq));
                        a->appendBranch(new Ci(deps.first()));
                        a->appendBranch(r.first());
                        delete root;
                        root = a;
                    } else {
                        Apply* na = new Apply;
                        na->appendBranch(new Operator(Operator::_or));
                        foreach(Object* o, r) {
                            Apply* a = new Apply;
                            a->appendBranch(new Operator(Operator::eq));
                            a->appendBranch(new Ci(deps.first()));
                            a->appendBranch(o);
                            na->appendBranch(a);
                        }
                        delete root;
                        root = na;
                    }
                } else if(!root->isZero()) {
                    delete root;
                    root = new Cn(false);
                }
            }
            
        }    break;
        case Operator::function: {
            Object* function=c->m_params[0];
            
            Container* cfunc=nullptr;
            QList<Ci*> bvars;
            if(function->isContainer()) {
                cfunc=(Container*) function;
                Q_ASSERT(cfunc->containerType()==Container::lambda);
                bvars=cfunc->bvarCi();
            }
            
            bool canRemove=true;
            it=c->begin()+1;
            for(int i=0; it!=c->end(); ++it, ++i) {
                *it = simp(*it);
                canRemove &= (*it)->type()==Object::variable || (cfunc && countDepth(bvars[i]->depth(), cfunc->m_params.last())==1);
            }
            
            if(cfunc && canRemove) {
                int i=0;
                foreach(Ci* var, bvars) {
                    replaceDepth(var->depth(), cfunc->m_params.last(), c->m_params[i+1]);
                    i++;
                }
                
                root=simp(cfunc->m_params.last());
                cfunc->m_params.last()=nullptr;
                delete c;
            }
        }    break;
        default:
            if(c->ulimit())
                c->ulimit()=simp(c->ulimit());
            if(c->dlimit())
                c->dlimit()=simp(c->dlimit());
            if(c->domain())
                c->domain()=simp(c->domain());
            
            iterateAndSimp<Apply>(c);
            
            root = applyTransformations(root, simplifications());
            break;
    }
    
    return root;
}

QList<Object*> Analyzer::findRoots(const QString& dep, const Object* o)
{
    switch(o->type()) {
        case Object::apply:        return findRootsApply(dep, static_cast<const Apply*>(o));
        case Object::variable:    return QList<Object*>() << new Cn(0.);
        default:
            return QList<Object*>();
    }
}

QList<Object*> Analyzer::findRootsApply(const QString& dep, const Apply* a)
{
    Operator op=a->firstOperator();
    QList<Object*> ret;
    switch(op.operatorType()) {
        case Operator::plus:
        case Operator::minus: {
            Object* varTree = nullptr;
            //f(x)-w=0 => f(x)=w => x=f-1(x)
            for(Apply::const_iterator it=a->constBegin(), itEnd=a->constEnd(); it!=itEnd; ++it) {
                bool hasvars = hasVars(*it);
                if(hasvars && varTree) {
                    varTree = nullptr; //we don't support having more than 1 variable in the minus yet
                    return QList<Object*>();
                }
                
                if(hasvars && ((*it)->type() == Object::variable || (*it)->isApply())) {
                    if((*it)->isApply()) {
                        const Apply* a = static_cast<const Apply*>(*it);
                        if(!a->isUnary() || a->firstOperator().inverse().operatorType()==Operator::none)
                            break;
                    }
                    varTree = *it;
                }
            }
            
            if(varTree) {
                Apply* na = nullptr;
                Object* value = nullptr;
                if(a->countValues()>2) {
                    na = new Apply;
                    na->appendBranch(new Operator(a->firstOperator().inverse()));
                    value = na;
                }
                
                for(Apply::const_iterator it=a->constBegin(), itEnd=a->constEnd(); it!=itEnd; ++it) {
                    if(*it != varTree) {
                        if(na)
                            na->appendBranch((*it)->copy());
                        else {
                            Q_ASSERT(!value);
                            value = (*it)->copy();
                        }
                    }
                }
                
                Q_ASSERT(value);
                if(varTree->isApply()) {
                    Operator inv = static_cast<const Apply*>(varTree)->firstOperator().inverse();
                    Apply* aa = new Apply;
                    aa->appendBranch(inv.copy());
                    aa->appendBranch(value);
                    value = calc(aa);
                    delete aa;
                }
                
                if(op==Operator::minus) {
                    ret += value;
                } else {
                    Apply* aa = new Apply;
                    aa->appendBranch(new Operator(Operator::minus));
                    aa->appendBranch(value);
                    ret += calc(aa);
                    delete aa;
                }
            }
        }    break;
        case Operator::times:
            for(Apply::const_iterator it=a->constBegin(), itEnd=a->constEnd(); it!=itEnd; ++it) {
                ret += findRoots(dep, static_cast<Apply*>(*it));
            }
            break;
        case Operator::divide: { // f/g
            Object* f = *a->firstValue();
            Object* g = *(a->firstValue()+1);
            ret = findRoots(dep, f);
            
            for(QList<Object*>::iterator it = ret.begin(), itEnd=ret.end(); it!=itEnd; ) {
                bool erase = false;
                
                Object* val = testResult(g, dep, *it);
                erase = val->isZero();
                delete val;
                
                if(erase) {
                    delete *it;
                    it = ret.erase(it);
                } else
                    ++it;
            }
        }    break;
        default: {
            Operator inv = op.inverse();
            if(inv.operatorType()!=Operator::none && a->isUnary()) {
                Apply* aa = new Apply;
                aa->appendBranch(inv.copy());
                aa->appendBranch(new Cn(0.));
                ret += calc(aa);
                delete aa;
            }
        }    break;
    }
    return ret;
}

Object* Analyzer::testResult(const Object* o, const QString& var, const Object* val)
{
    SubstituteExpression s;
    QMap<QString, const Object*> subs;
    subs.insert(var, val);
    
    QScopedPointer<Object> sometree(s.run(o, subs));
    return calc(sometree.data());
}

Object* Analyzer::simpPolynomials(Apply* c)
{
    Q_ASSERT(c!=nullptr && c->isApply());
    
    Polynomial monos(c);
    
    c->m_params.clear();
    delete c;
    c=nullptr;
    
    Object *root=monos.toObject();
    
    return root;
}

Object* Analyzer::simpSum(Apply* c)
{
    Object* ret=c;
    Apply* cval=static_cast<Apply*>(*c->firstValue());
    
    if(cval->isApply() && cval->firstOperator()==Operator::times) {
        QSet<QString> bvars=c->bvarStrings().toSet();
        QVector<Object*> sum, out;
        Apply::iterator it=cval->firstValue(), itEnd=cval->end();
        int removed=0;
        for(; it!=itEnd; ++it) {
            if(hasTheVar(bvars, *it)) {
                sum.append(*it);
            } else {
                out.append(*it);
                *it=nullptr;
                ++removed;
            }
        }
        
        if(removed>0) {
            Apply* nc=new Apply;
            nc->appendBranch(new Operator(Operator::times));
            nc->m_params << out << c;
            
            if(sum.count()==1) {
                cval->m_params.clear();
                delete cval;
                c->m_params.last()=sum.last();
            } else {
                cval->m_params=sum;
            }
            
            ret=simp(nc);
        }
    }
    
    return ret;
}

Object* Analyzer::simpPiecewise(Container *c)
{
    Object *root=c;
    //Here we have a list of options and finally the otherwise option
    Container *otherwise=nullptr;
    Container::const_iterator it=c->m_params.constBegin(), itEnd=c->constEnd();
    QList<Object*> newList;
    
    for(; !otherwise && it!=itEnd; ++it) {
        Container *p=static_cast<Container*>(*it);
        Q_ASSERT( (*it)->isContainer() &&
                (p->containerType()==Container::piece || p->containerType()==Container::otherwise) );
        bool isPiece = p->containerType()==Container::piece;
        
        p->m_params.last()=simp(p->m_params.last());
            
        if(isPiece) {
            if(p->m_params[1]->type()==Object::value) {
                Cn* cond=static_cast<Cn*>(p->m_params[1]);
                if(cond->isTrue()) {
                    delete p->m_params.takeLast();
                    p->setContainerType(Container::otherwise);
                    isPiece=false;
                    
                    p->m_params[0]=simp(p->m_params[0]);
                    otherwise=p;
                    newList.append(p);
                } else {
                    delete p;
                }
            } else {
                //TODO: It would be nice if we could simplify:
                // if(x=n) simplify x as n
                p->m_params[0]=simp(p->m_params[0]);
                newList.append(p);
            }
        } else { //it is an otherwise
            otherwise=p;
            newList.append(p);
        }
    }
    qDeleteAll(it, itEnd);
    
    if(newList.count()==1 && otherwise) {
        root=otherwise->m_params.takeAt(0);
        delete otherwise;
        c->m_params.clear();
        delete c;
    } else
        c->m_params = newList;
    return root;
}

Expression Analyzer::derivative(const QString& var)
{
    Q_ASSERT(m_exp.isCorrect() && m_exp.tree());
    
    QStringList vars;
    Object* deriv=m_exp.tree();
    if(m_exp.isLambda()){
        Q_ASSERT(deriv->isContainer());
        Container* lambda=(Container*) deriv;
        if(lambda->containerType()==Container::math) {
            Q_ASSERT(lambda->m_params.first()->isContainer());
            lambda = (Container*) lambda->m_params.first();
        }
        Q_ASSERT(lambda->containerType()==Container::lambda);
        
        vars=lambda->bvarStrings();
        deriv=lambda->m_params.last();
    } else
        vars += var;
    
//     Q_ASSERT(hasTheVar(QSet<QString>() << var, deriv));
    Object* o = derivative(var, deriv);
    o=simp(o);
    Container* lambda=new Container(Container::lambda);
    foreach(const QString& dep, vars) {
        Container* bvar=new Container(Container::bvar);
        bvar->appendBranch(new Ci(dep));
        lambda->appendBranch(bvar);
    }
    lambda->appendBranch(o);
    Expression::computeDepth(lambda);
    return Expression(lambda);
}

Expression Analyzer::dependenciesToLambda() const
{
    if(m_hasdeps && m_exp.tree()) {
        QStringList deps=dependencies(m_exp.tree(), m_vars->keys());
        Container* cc=new Container(Container::lambda);
        foreach(const QString& dep, deps) {
            Container* bvar=new Container(Container::bvar);
            bvar->appendBranch(new Ci(dep));
            cc->appendBranch(bvar);
        }
        
        const Object* root=m_exp.tree();
        if(root->isContainer()) {
            const Container* c=static_cast<const Container*>(root);
            if(c->containerType()==Container::math) {
                root=c->m_params.first();
            }
        }
        cc->appendBranch(root->copy());
        
        Container* math=new Container(Container::math);
        math->appendBranch(cc);
        
        Expression::computeDepth(math);
        return Expression(math);
    } else {
        return m_exp;
    }
}

bool Analyzer::insertVariable(const QString & name, const Expression & value)
{
    return insertVariable(name, value.tree());
}

bool Analyzer::insertVariable(const QString & name, const Object * value)
{
    bool wrong=!isLambda(value) && hasTheVar(QSet<QString>() << name, value);
    if(wrong)
        m_err << QCoreApplication::translate("By a cycle i mean a variable that depends on itself", "Defined a variable cycle");
    else
        m_vars->modify(name, value);
    
    return !wrong;
}

Cn* Analyzer::insertValueVariable(const QString& name, double value)
{
    Cn* val=m_vars->modify(name, value);
    return val;
}

double Analyzer::derivative(const QVector<Object*>& values )
{
    //c++ numerical recipes p. 192. Only for f'
    //Image
    Q_ASSERT(m_exp.isCorrect() && m_exp.tree());
    Q_ASSERT(values.size()==m_exp.bvarList().size());
    
    setStack(values);
    
    Expression e1(calculateLambda());
    if(!isCorrect())
        return 0.;
    
    //Image+h
    double h=0.0000000001;
    for(int i=0; i<values.size(); i++) {
//         volatile double temp=valp.second+h;
//         double hh=temp-valp.second;
        
        Q_ASSERT(values[i]->type()==Object::value);
        Cn* v=static_cast<Cn*>(values[i]);
        v->setValue(v->value()+h);
    }
    
    Expression e2(calculateLambda());
    if(!isCorrect())
        return 0.;
    
    if(!e1.isReal() || !e2.isReal()) {
        m_err << QCoreApplication::tr("The result is not a number");
        return 0;
    }
    
    return (e2.toReal().value()-e1.toReal().value())/h;
}

Analitza::Object* Analyzer::variableValue(Ci* var)
{
    Object* ret;
    
//     qDebug() << "ziiiiiiii" << var->name() << '('<< m_runStackTop << '+' << var->depth() << ')' << printAll(m_runStack);
    if(var->depth()>=0)
        ret = m_runStack[m_runStackTop + var->depth()];
    else
        ret = m_vars->value(var->name());
    
//     static int hits = 0, misses = 0;
//     if(var->depth()>=0) hits++; else misses++;
//     qDebug() << "pepepe" << hits << misses;
    return ret;
}

Object* Analyzer::applyAlpha(Object* o, int min)
{
    if(o)
        switch(o->type()) {
            case Object::container:    alphaConversion(static_cast<Container*>(o), min); break;
            case Object::vector:    alphaConversion<Vector>(static_cast<Vector*>(o), min); break;
            case Object::list:        alphaConversion<List>(static_cast<List*>(o), min); break;
            case Object::matrix:       alphaConversion<Matrix, MatrixRow>(static_cast<Matrix*>(o), min); break;
            case Object::matrixrow:    alphaConversion<MatrixRow>(static_cast<MatrixRow*>(o), min); break;
            case Object::apply:        alphaConversion(static_cast<Apply*>(o), min); break;
            case Object::variable: {
                Ci *var = static_cast<Ci*>(o);
                int depth = var->depth();
//                 qDebug() << "puuuu" << var->name() << depth << '<' << min << printAll(m_runStack);
                if(depth>0 && depth<min && m_runStackTop+var->depth()<m_runStack.size()) {
                    Object* newvalue = variableValue(var);
                    if(newvalue) {
                        delete var;
                        return newvalue->copy();
                    }
                }
            }    break;
            case Object::none:
            case Object::value:
            case Object::oper:
            case Object::custom:
                break;
        }
    return o;
}

template <class T, class Tcontained>
void Analyzer::alphaConversion(T* o, int min)
{
    Q_ASSERT(o);
    auto it=o->begin(), itEnd=o->end();
    for(; it!=itEnd; ++it) {
        *it = static_cast<Tcontained*>(applyAlpha(*it, min));
    }
}

void Analyzer::alphaConversion(Container* o, int min)
{
    Q_ASSERT(o);
    Container::iterator it=o->begin(), itEnd=o->end();
    for(; it!=itEnd; ++it) {
        if((*it)->type()==Object::container && static_cast<Container*>(*it)->containerType()==Container::bvar)
            continue;
        
        *it = applyAlpha(*it, min);
    }
}

void Analyzer::alphaConversion(Apply* o, int min)
{
    Q_ASSERT(o);
    o->ulimit()=applyAlpha(o->ulimit(), min);
    o->dlimit()=applyAlpha(o->dlimit(), min);
    o->domain()=applyAlpha(o->domain(), min);
    
    Apply::iterator it=o->firstValue(), itEnd=o->end();
    for(; it!=itEnd; ++it)
        *it = applyAlpha(*it, min);
}

BuiltinMethods* Analyzer::builtinMethods()
{
    return &m_builtin;
}
