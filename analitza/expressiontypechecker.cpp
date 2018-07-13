/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "expressiontypechecker.h"
#include "variable.h"
#include "variables.h"
#include "list.h"
#include "vector.h"
#include "container.h"
#include "expression.h"
#include "analitzautils.h"
#include "operations.h"
#include "apply.h"
#include "value.h"
#include "matrix.h"
#include <QCoreApplication>

QDebug operator<<(QDebug dbg, const Analitza::ExpressionType &c)
{
    dbg.nospace() << "("<< qPrintable(c.toString());
//     if(!c.assumptions().isEmpty())
//         dbg.nospace() <<", " << c.assumptions();
    dbg.nospace() << /*":"<< c.type() <<*/")";
    
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QMap<int, Analitza::ExpressionType> &c)
{
    dbg.nospace() << "starsMap(";
    for(QMap<int,Analitza::ExpressionType>::const_iterator it=c.constBegin(); it!=c.constEnd(); ++it) {
        QString str = Analitza::ExpressionType(Analitza::ExpressionType::Any, it.key()<0? -it.key():it.key()).toString();
        if(it.key()<0)
            str="<-"+str+"->";
        
        dbg.space() << "["<< str << ":" << it.value() << "]";
    }
    dbg.space() << ")";
    
    return dbg.nospace();
}

namespace Analitza
{

void printAssumptions(const QString& prefix, const ExpressionType& current)
{
    qDebug() << qPrintable(prefix) << current << current.assumptions();
    if(current.type()==ExpressionType::Many) {
        foreach(const ExpressionType& t, current.alternatives()) {
            qDebug() << qPrintable(prefix) << "XXXSSS" << t << t.assumptions();
        }
    }
    qDebug() << qPrintable(prefix) << "--";
}

ExpressionTypeChecker::ExpressionTypeChecker(Variables* v)
    : m_stars(0)
    , m_v(v)
{}

ExpressionType ExpressionTypeChecker::check(const Expression& exp)
{
    m_deps.clear();
    m_stars=1;
    current=ExpressionType(ExpressionType::Error);
    
    exp.tree()->accept(this);
    
//     qDebug() << "cheeeeeeck" << m_vars;
    return current;
}

QVariant ExpressionTypeChecker::visit(const Operator*) { Q_ASSERT(false && "should not get here"); return QVariant(); }

bool ExpressionTypeChecker::inferType(const ExpressionType& found, const ExpressionType& targetType, QMap<QString, ExpressionType>* assumptions)
{
    Q_ASSERT(!targetType.isError() && assumptions);
    Q_ASSERT(found.type()!=ExpressionType::Many);
    
    QMap<int, ExpressionType> stars;
    stars=ExpressionType::computeStars(stars, found, targetType);
    bool ret=found.canReduceTo(targetType);
    ret&=ExpressionType::matchAssumptions(&stars, found.assumptions(), targetType.assumptions());
    ret&=ExpressionType::assumptionsMerge(*assumptions, found.assumptions());
    
    for(QMap<QString, ExpressionType>::iterator it=assumptions->begin(), itEnd=assumptions->end(); it!=itEnd; ++it) {
        *it=it->starsToType(stars);
    }
//     qDebug() << "fuuuuu" << stars << *assumptions << ret;
    
    return ret;
}

QList<ExpressionType> ExpressionTypeChecker::computePairs(const QList<ExpressionType>& options, const ExpressionType& param)
{
    QList<ExpressionType> ret;
    
    if(param.type()==ExpressionType::Any) {
        int basestars=m_stars;
        foreach(const ExpressionType& opt, options) {
            ExpressionType toadd=opt;
            m_stars=qMax<int>(m_stars, toadd.increaseStars(basestars));
            QMap<int, ExpressionType> stars;
            
            //We know the parameter is any, we don't have to infer
            stars=ExpressionType::computeStars(stars, param, toadd.parameters().first());
            
            toadd.parameters().last().addAssumptions(param.assumptions());
            toadd.parameters().last()=toadd.parameters().last().starsToType(stars);
            ret += toadd;
        }
    } else 
        foreach(const ExpressionType& opt, options) {
            if(opt.parameters().at(0).canReduceTo(param)) {//Infer on param!=param but canReduce?
                QMap<int, ExpressionType> stars = ExpressionType::computeStars(QMap<int, ExpressionType>(), opt.parameters().at(0), param);
                ret += opt.starsToType(stars);
            }
        }
    
//     qDebug() << "_______" << param << exp->toString() << ret;
    return ret;
}

ExpressionType ExpressionTypeChecker::solve(const Operator* o, const QVector< Object* >& parameters)
{
    Q_ASSERT(o->operatorType()!=Operator::function);
    
    QList<ExpressionType> paramtypes;
    for(QVector<Object*>::const_iterator it=parameters.constBegin(), itEnd=parameters.constEnd(); it!=itEnd; ++it) {
        (*it)->accept(this);
        paramtypes += current;
    }
    
    QList<ExpressionType> ret;
    if(parameters.size()==1) {
        QList<ExpressionType> types=paramtypes.first().type()==ExpressionType::Many ? paramtypes.first().alternatives() : QList<ExpressionType>() << paramtypes.first();
//         const QMap<QString, ExpressionType> assumptions=current.assumptions();
        
        foreach(const ExpressionType& t, types) {
            QList<ExpressionType> thing=computePairs(Operations::inferUnary(o->operatorType()), t);
            foreach(const ExpressionType& opt, thing) {
                ExpressionType tt(opt.parameters().constLast());
                tt.addAssumptions(t.assumptions());
                ret+=tt;
            }
        }
//         qDebug() << "bam" << ret ;
    } else {
        Q_ASSERT(parameters.size()>=2);
//         QMap<int, ExpressionType> stars;
        
        ExpressionType firstType=paramtypes.first();
        QList<ExpressionType> firstTypes= firstType.type()==ExpressionType::Many ?  firstType.alternatives() : QList<ExpressionType>() << firstType;
        
        const QList<ExpressionType> res=Operations::infer(o->operatorType());
        
        bool firstPair=true;
        for(QList<ExpressionType>::const_iterator it=paramtypes.constBegin()+1, itEnd=paramtypes.constEnd(); it!=itEnd; ++it, firstPair=false)
        {
            ExpressionType secondType=*it;
            
            QList<ExpressionType> secondTypes=secondType.type()==ExpressionType::Many ? secondType.alternatives() : QList<ExpressionType>() << secondType;
            int starsbase=m_stars;
//             static int ind=3;
//             qDebug() << qPrintable("+" +QString(ind++, '-')) << o->toString() << firstType << secondType;
            
            bool found = false;
            foreach(const ExpressionType& _first, firstTypes) {
                foreach(const ExpressionType& _second, secondTypes) {
                    QMap<int, ExpressionType> _starToType;
                    bool matches=ExpressionType::matchAssumptions(&_starToType, _first.assumptions(), _second.assumptions());
                    
                    if(!matches) {
//                         qDebug() << "peee" << ExpressionType::wrongAssumptions(_first.assumptions(), _second.assumptions());
                        continue;
                    }
                    foreach(const ExpressionType& _opt, res) {
                        ExpressionType opt(_opt);
                        m_stars=qMax<int>(m_stars, opt.increaseStars(starsbase));
                        
                        Q_ASSERT(!opt.parameters().last().isError());
                        QMap<int, ExpressionType> starToType, starToParam;
                        
                        bool valid = false;
                        QMap<QString, ExpressionType> assumptions;
                        
                        //NOTE In matrix multiplication (A*B) A may or not be reduced to B, since is allowed 
                        //different matrix sizes provided that are valid sizes for the multiplication, 
                        //thus we need a special case for matrix type
                        if (current.type() == ExpressionType::Matrix) {
                            ExpressionType first =_first .starsToType(_starToType);
                            ExpressionType second=_second.starsToType(_starToType);
                            
    //                         qDebug() << "9999999" << _first.assumptions() << first.assumptions() << starToType;
                            
                            starToType=ExpressionType::computeStars(starToType, first,  opt.parameters()[0]);
                            first =first .starsToType(starToType);
                            starToType=ExpressionType::computeStars(starToType, second, opt.parameters()[1]);
                            second=second.starsToType(starToType);
                            
                            assumptions=first.assumptions();
                            valid=ExpressionType::assumptionsMerge(assumptions, second.assumptions());
                            
    //                         qDebug() << "PPPPPP" << opt << first << second << "|||||" << first.assumptions() << second.assumptions();
                            
                            valid &= !first.isError() && !second.isError();
                            starToParam=ExpressionType::computeStars(starToParam, opt.parameters()[0], first);
    //                         qDebug() << "XXXXXX1" << starToParam;
                            valid &= first .canReduceTo(opt.parameters()[0].starsToType(starToParam));
                            starToParam=ExpressionType::computeStars(starToParam, opt.parameters()[1], second);
    //                         qDebug() << "XXXXXX2" << starToParam;
                            valid &= second.canReduceTo(opt.parameters()[1].starsToType(starToParam));
                        } else {
                            ExpressionType first =_first .starsToType(_starToType);
                            ExpressionType second=_second.starsToType(_starToType);
    //                         qDebug() << "9999999" << _first.assumptions() << first.assumptions() << starToType;
                            
                            starToType=ExpressionType::computeStars(starToType, first,  opt.parameters()[0]);
                            starToType=ExpressionType::computeStars(starToType, second, opt.parameters()[1]);
                            
                            first =first .starsToType(starToType);
                            second=second.starsToType(starToType);
                            
                            starToParam=ExpressionType::computeStars(starToParam, opt.parameters()[0], first);
                            starToParam=ExpressionType::computeStars(starToParam, opt.parameters()[1], second);
    //                         qDebug() << "XXXXXX" << starToParam;
    //                         qDebug() << "PPPPPP" << opt << first << second << "|||||" << first.assumptions() << second.assumptions();
                            
    //                         starToType=ExpressionType::computeStars(starToType, first,  opt.parameters()[0].starsToType(starToParam));
    //                         starToType=ExpressionType::computeStars(starToType, second, opt.parameters()[1].starsToType(starToParam));
                            
    //                         first =first .starsToType(starToType);
    //                         second=second.starsToType(starToType);
                            
                            assumptions=first.assumptions();
                            valid=ExpressionType::assumptionsMerge(assumptions, second.assumptions());
                            
    //                         qDebug() << "fifuuuuuuu" << first << (it-1)->toString() << 
    //                                                     second << it->toString() << assumptions << valid;
                            
                            valid &= !first.isError() && !second.isError();
                            valid &= first .canReduceTo(opt.parameters()[0].starsToType(starToParam));
                            valid &= second.canReduceTo(opt.parameters()[1].starsToType(starToParam));
                            
    //                         qDebug() << "POPOPO" << (it-1)->toString() << it->toString() << valid << first << second << starToParam;
                        }
                        
//                         qDebug() << "POPOPO" << (it-1)->toString() << it->toString() << valid << first << second << starToParam;
                        if(valid) {
                            ExpressionType toadd=opt.parameters().last();
                            toadd.addAssumptions(assumptions);
                            toadd=toadd.starsToType(starToParam);
                            
                            if(firstPair) {
                                ret += toadd;
                                found = true;
                            } else {
                                QList<ExpressionType>::iterator it=ret.begin(), itEnd=ret.end();
                                for(; it!=itEnd; ++it) {
                                    QMap<int, ExpressionType> stars;
                                    if(toadd.canReduceTo(*it) && ExpressionType::matchAssumptions(&stars, it->assumptions(), toadd.assumptions())) {
                                        bool b=ExpressionType::assumptionsMerge(it->assumptions(), toadd.starsToType(stars).assumptions());
                                        Q_ASSERT(b);
                                        found = true;
                                        break;
                                    } else if(it->canReduceTo(toadd) && ExpressionType::matchAssumptions(&stars, it->assumptions(), toadd.assumptions())) {
                                        toadd=toadd.starsToType(stars);
                                        bool b=ExpressionType::assumptionsMerge(toadd.assumptions(), it->assumptions());
                                        Q_ASSERT(b);
                                        *it=toadd;
                                        found = true;
                                        break;
                                    }
                                }
                            }
                        }
//                         else
//                         {
//                             qDebug() << "lalala" << first << second;
//                         }
                    }
                }
            }
            if(!found) {
                addError(QCoreApplication::tr("Could not find a type that unifies '%1'").arg(o->toString()));
            }
//             qDebug() << qPrintable("\\"+QString(--ind, '-')) << o->toString() << ret;
        }
    }
    
    if(ret.isEmpty())
        return ExpressionType(ExpressionType::Error);
    else if(ret.count()==1)
        return ret.first();
    else {
        ExpressionType t(ExpressionType::Many);
        foreach(const ExpressionType& alt, ret)
            t.addAlternative(alt);
        return t;
    }
}

QVariant ExpressionTypeChecker::visit(const Ci* var)
{
    if(m_typeForBVar.contains(var->name())) {
        current=m_typeForBVar.value(var->name());
    } else if(!m_lambdascope.contains(var->name()) && isVariableDefined(var->name())) {
        current=typeForVar(var->name());
    } else {
        current=ExpressionType(Analitza::ExpressionType::Any, m_stars++);
        bool ret = current.addAssumption(var->name(), current);
        Q_ASSERT(ret);
        
        if(var->depth()<0 && !isVariableDefined(var->name()) && !m_calculating.contains(var->name()))
            m_deps += var->name();
    }
    
    return QString();
}

bool ExpressionTypeChecker::isVariableDefined(const QString& id) const
{
    return m_v->contains(id) || m_vars.contains(id);
}

QVariant ExpressionTypeChecker::visit(const Cn* c)
{
    ExpressionType::Type type;
    
    switch(c->format()) {
        case Cn::Boolean:
            type=ExpressionType::Bool;
            break;
        case Cn::Char:
            type=ExpressionType::Char;
            break;
        default:
            type=ExpressionType::Value;
            break;
    }
    
    current=ExpressionType(type);
    return QString();
}

QStringList objectsToString(const QList<Object*>& objs) {
    QStringList ret;
    foreach(const Object* o, objs) {
        ret+=o->toString();
    }
    return ret;
}

ExpressionType ExpressionTypeChecker::commonType(const QList<Object*>& values)
{
    ExpressionType ret;
    
    if(values.isEmpty()) {
        ret=ExpressionType(ExpressionType::Any, m_stars++);
    } else foreach(const Object* o, values) {
        o->accept(this);
        
//         qDebug()<< "sususu" << current << ret << "||" << current.assumptions() << ret.assumptions();
        
//         qDebug() << "falala" << ret << current << stars;
        QList<ExpressionType> retalts;
        QList<ExpressionType> alts1=current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current;
        QList<ExpressionType> alts2=ret.type()==ExpressionType::Many ? ret.alternatives() : QList<ExpressionType>() << ret;
// #error foreach + canReduce + computestars to tell that type of:foldr1==f==elems
        
        foreach(const ExpressionType& t1, alts1) {
            QMap<int, ExpressionType> _stars;
            QMap<QString, ExpressionType> assumptions(ret.assumptions());
            bool b=ExpressionType::matchAssumptions(&_stars, current.assumptions(), assumptions);
            b &= ExpressionType::assumptionsMerge(assumptions, current.assumptions());
            if(!b) {
                continue;
            }
            
            foreach(const ExpressionType& t2, alts2) {
                QMap<int, ExpressionType> stars(_stars);
                
//                 printAssumptions("commonnnnXX", t1);
//                 printAssumptions("commonnnnYY", t2);
                ExpressionType rr=ExpressionType::minimumType(t1, t2);
                if(!rr.isError() && ExpressionType::matchAssumptions(&stars, t2.assumptions(), t1.assumptions())) {
                    stars=ExpressionType::computeStars(stars, t2, t1);
                    bool b=ExpressionType::assumptionsMerge(rr.assumptions(), assumptions);
//                     Q_ASSERT(b);
//                     qDebug() << "*************" << o->toString() << rr << b << "||||||" << stars;
//                     printAssumptions("commonnnnWW", rr);
                    if(b)
                        retalts += rr.starsToType(stars);
//                     printAssumptions("commonnnnZZ", retalts.last());
                }
            }
        }
        
        if(retalts.size()==1)
            ret=retalts.first();
        else if(retalts.isEmpty())
            ret=ExpressionType::Error;
        else
            ret=ExpressionType(ExpressionType::Many, retalts);
    }
    
//     printAssumptions("ffffffff", ret);
    
    return ret;
}

QVariant ExpressionTypeChecker::visit(const Apply* c)
{
    QMap<QString, ExpressionType> ctx=m_typeForBVar;
    QMap<QString, ExpressionType> assumptions;
    Operator o=c->firstOperator();
    
    if(o.isBounded()) {
        Object* ul=c->ulimit();
        ExpressionType tt;
        if(ul) {
            Object* dl=c->dlimit();
            ul->accept(this);
            
            tt=current; //FIXME: should remove when done
            if(!current.isError() && current.type()!=ExpressionType::Any)
                assumptions=typeIs(dl, ExpressionType(current));
            else {
                dl->accept(this);
                tt=current;
                
                if(!current.isError())
                    assumptions=typeIs(ul, ExpressionType(current));
            }
        } else if(c->domain()) {
            c->domain()->accept(this);
            
            if(current.type()==ExpressionType::Any) {
                ExpressionType anyItem(ExpressionType::Any, m_stars++);
                
                ExpressionType anyList(ExpressionType::List, anyItem);
                ExpressionType anyVector(ExpressionType::Vector, anyItem, -1);
                ExpressionType anyMatrix(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, anyItem, -2), -3);
                
                ExpressionType anyContainer(ExpressionType::Many);
                anyContainer.addAlternative(anyList);
                anyContainer.addAlternative(anyVector);
                anyContainer.addAlternative(anyMatrix);
                
                bool ret = anyItem.addAssumption(static_cast<Ci*>(c->domain())->name(), anyContainer);
                if(ret)
                    tt=anyItem;
            } else if(current.type()==ExpressionType::Vector || current.type()==ExpressionType::List || current.type()==ExpressionType::Matrix) {
                tt=current.contained();
                tt.addAssumptions(current.assumptions());
            } else
                addError(QCoreApplication::tr("The domain should be either a vector or a list"));
            //TODO: should control the many case
        }
        
        //FIXME: should remove when done
        foreach(const QString& s, c->bvarStrings())
            m_typeForBVar[s]=ExpressionType(tt);
    }
    
    switch(o.operatorType()) {
        case Operator::none:
        case Operator::sum:
        case Operator::product:
            (*c->firstValue())->accept(this);
            current.addAssumptions(assumptions);
            break;
        case Operator::diff:
            //TODO Check inside
            
            current=ExpressionType(ExpressionType::Lambda);
            current.addParameter(ExpressionType::Value);
            current.addParameter(ExpressionType::Value);
            break;
        case Operator::function: {
//                     qDebug() << "calling" << c->toString();
            c->m_params.first()->accept(this);
//                     qDebug() << "retrieved lambda" << c->m_params.first()->toString() << current << current.assumptions();
            ExpressionType returned = current;
            assumptions=current.assumptions();
            
            QList<ExpressionType> exps;
            Apply::const_iterator it=c->firstValue()+1, itEnd=c->constEnd();
            for(; it!=itEnd; ++it) {
                (*it)->accept(this);
                
                exps += current;
            }
            
            if(returned.type()==ExpressionType::Any) {
                ExpressionType ret=ExpressionType(ExpressionType::Any, m_stars++);
//                 qDebug() << "fffffffffffffff" << m_stars;
                ret.addAssumptions(assumptions);
                
                exps += ret;
                QList<ExpressionType> t=ExpressionType::lambdaFromArgs(exps);
//                 qDebug() << "OOOOOOOOOOOOOOOOOOOOOOO" << c->toString() << ret /*<< stars*/ << ret.assumptions() << "||||" << t << exps;
                
                ExpressionType ret2(ExpressionType::Many);
                bool error=false;
                foreach(const ExpressionType& type, t) {
                    ExpressionType returntype(type.parameters().constLast());
                    returntype.addAssumptions(type.assumptions());
                    if(c->m_params.first()->type()==Object::variable) {
                        QString name=static_cast<Ci*>(c->m_params.first())->name();
                        QMap<QString, ExpressionType>::const_iterator itf=returntype.assumptions().constFind(name);
                        
                        if(itf!=returntype.assumptions().constEnd() && itf->type()==ExpressionType::Lambda) {
                            ExpressionType oldt=ExpressionType::minimumType(*itf, type);
                            error=!oldt.addAssumption(name, oldt);
                            if(error)
                                break;
                            QMap<int,ExpressionType> stars;
                            stars=ExpressionType::computeStars(stars, type, oldt);
                            bool b=ExpressionType::matchAssumptions(&stars, oldt.assumptions(), type.assumptions());
                            Q_ASSERT(b);
                            
                            oldt=oldt.starsToType(stars);
//                             qDebug() << "hmmm..." << oldt << stars << type << "::::::\n\t" << oldt.assumptions() << "\n\t" << type.assumptions();
                            returntype=oldt.parameters().constLast();
//                             printAssumptions("reeeeet", returntype);
                            
                            error = !returntype.addAssumption(name, oldt); //can't happen, we already checked it's not an assumption
                            Q_ASSERT(!error);
                        } else {
                            error = !returntype.addAssumption(name, type);
                            if(error)
                                break;
                        }
                    }
                    returntype.addAssumptions(type.assumptions());
                    
                    ret2.addAlternative(returntype);
                }
                
                if(error) {
                    addError(QCoreApplication::translate("type error message", "Cannot call '%1'").arg(c->m_params.first()->toString()));
                    current=ExpressionType(ExpressionType::Error);
                } else
                    current=ret2;
            } else if(returned.isError()) {
                current=ExpressionType(ExpressionType::Error);
            }
            else {
                ExpressionType ret(ExpressionType::Many), signature(returned);
                
                QList<ExpressionType> alts=signature.type()==ExpressionType::Many ? signature.alternatives() : QList<ExpressionType>() << signature;
                QList<ExpressionType> args=ExpressionType::lambdaFromArgs(exps);
                bool exit=false;
                foreach(const ExpressionType& opt, alts) {
                    if(opt.type()!=ExpressionType::Lambda) {
//                                 addError(i18n("We can only call functions."));
                        continue;
                    }
                    
                    const bool isvariadic = (opt.parameters().at(0).type() == ExpressionType::Any);
                    //if we have a variadic check only the return type (that is why we put 0 as args count here)
                    const int nargs = opt.parameters().size()-1;
                    
                    foreach(const ExpressionType& f, args) {
                        QList<ExpressionType> altargs=f.parameters();
                        
                        bool valid=true;
                        
                        if (isvariadic) { // if is variadic don't check the number of args
                            Q_ASSERT(opt.parameters().size() == 2); // Any and return type
                            
                            //when use Any with contained we need to check if all args must be of the contained type
                            if (opt.parameters().at(0).hasContained()) {
                                for(int i=0; valid && i<altargs.size(); i++) {
                                    if(!altargs[i].canReduceTo(opt.parameters().at(0).contained())) {
                                        addError(QCoreApplication::tr("Cannot reduce '%1' to '%2'. Invalid type of parameter '%3' for '%4'").
                                        arg(altargs[i].toString(), opt.parameters().at(0).contained().toString()).
                                        arg(i+1).
                                        arg(c->toString()));
                                        
                                        valid=false;
                                        break;
                                    }
                                }
                            }
                        } else if(opt.parameters().size()!=altargs.size()+1) {
                            addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(opt.parameters().size()-1).arg(c->toString()));
                            exit=true;
                            break;
                        }
                        
                        QMap<QString, ExpressionType> assumptions;
                        QMap<int, ExpressionType> starToType, starToParam;
                        
                        for(int i=0; valid && i<nargs; i++) {
                            if (i<opt.parameters().size() && i<altargs.size()) {
                                if(!altargs[i].canCompareTo(opt.parameters().at(i))) {
                                    if (isvariadic)
                                        addError(QCoreApplication::tr("Cannot compare '%1' to '%2'. Invalid type of parameter '%3' for '%4'").
                                        arg(altargs[i].toString(), opt.parameters().at(i).toString()).
                                        arg(i+1).
                                        arg(c->toString()));
                                    
                                    valid=false;
                                    break;
                                }
                                
                                starToType=ExpressionType::computeStars(starToType, opt.parameters().at(i), altargs[i]);
                                
                                ExpressionType t=opt.parameters().at(i).starsToType(starToType);
                                starToParam=ExpressionType::computeStars(starToParam, altargs[i], t);
                                valid=!t.isError() && inferType(altargs[i].starsToType(starToParam), t, &assumptions);
                                
//                                 qDebug() << "pepeluis" << c->toString() << c->m_params[i+1]->toString() << altargs[i] << t << valid;
                            }
                        }
                        
                        valid &= ExpressionType::matchAssumptions(&starToType, opt.parameters().constLast().assumptions(), assumptions);
                        
                        if(valid) {
                            ExpressionType t=opt.parameters().constLast();
                            
                            t.addAssumptions(assumptions);
                            
                            ret.addAlternative(t.starsToType(starToType));
                        }
                    }
                    
                    if(exit)
                        break;
                }
                
                if(ret.alternatives().isEmpty()) {
//                     qDebug() << "peee" << c->toString() << signature << exps;
                    
                    current=ExpressionType(ExpressionType::Error);
                    addError(QCoreApplication::tr("Could not call '%1'").arg(c->toString()));
                } else
                    current=ret;
            }
        } break;
        default:
            current=solve(&o, c->values());
            if(current.type()==ExpressionType::Error)
                addError(QCoreApplication::tr("Could not solve '%1'").arg(c->toString()));
            break;
    }
    
    current.removeAssumptions(c->bvarStrings());
    
    m_typeForBVar=ctx;
    
    if(current.type()==ExpressionType::Many && current.alternatives().size()==1) {
        current=current.alternatives().at(0);
    }
    
    return QString();
}

QVariant ExpressionTypeChecker::visit(const CustomObject*)
{
    Q_ASSERT(false && "we shouldn't have to construct any custom object");
    return QString();
}

ExpressionType ExpressionTypeChecker::tellTypeIdentity(const QString& name, const ExpressionType& type)
{
    QList<ExpressionType> opts=type.type()==ExpressionType::Many ? type.alternatives() : QList<ExpressionType>() << type;
    
    for(QList< ExpressionType >::iterator it=opts.begin(), itEnd=opts.end(); it!=itEnd; ++it) {
        QMap< QString, ExpressionType >::const_iterator itFound=it->assumptions().constFind(name);
        
        if(itFound!=it->assumptions().constEnd()) {
            QList<ExpressionType> optsFound=itFound->type()==ExpressionType::Many ? itFound->alternatives() : QList<ExpressionType>() << *itFound;
            for(QList< ExpressionType >::iterator itf=optsFound.begin(), itfEnd=optsFound.end(); itf!=itfEnd; ++itf) {
                if(!itf->canReduceTo(type)) {
//                     qDebug() << "incoherent type" << *itf << type;
                    addError(QCoreApplication::tr("Incoherent type for the variable '%1'").arg(name));
                    break;
                }
                QMap<int, ExpressionType> stars;
                stars=ExpressionType::computeStars(stars, *itf, *it);
//                 qDebug() << "fiiiiiiiii" << stars  << "\n\t" << *it << "\n\t" << *itFound;
                *it=it->starsToType(stars);
            }
        }
    }
    return ExpressionType(ExpressionType::Many, opts);
}

//1. Check if parameters are applied correctly
//2. Return the operator result type
QVariant ExpressionTypeChecker::visit(const Container* c)
{
//     qDebug() << "XIUXIU" << c->toString();
    switch(c->containerType()) {
        case Container::piecewise: {
            ExpressionType type=commonType(c->m_params);
            
            if(type.isError()) {
                addError(QCoreApplication::tr("Could not determine the type for piecewise"));
                current=type;
            } else {
                QList<ExpressionType> alts=type.type()==ExpressionType::Many ? type.alternatives() : QList<ExpressionType>() << type, alts2;
                
                for(QList< ExpressionType >::iterator it=alts.begin(), itEnd=alts.end(); it!=itEnd; ++it) {
                    QMap<QString, ExpressionType> assumptions=typeIs(c->constBegin(), c->constEnd(), *it);
                    
    //                 QMap<int, ExpressionType> stars;
    //                 bool b=ExpressionType::matchAssumptions(&stars, assumptions, type.assumptions());
    //                 Q_ASSERT(b);
    //                 type=type.starsToType(stars);
                    
//                     qDebug() << "suuuuu\n\t" << it->assumptions() << "\n\t" << assumptions;
                    QMap<int, ExpressionType> stars;
                    bool b=ExpressionType::matchAssumptions(&stars, it->assumptions(), assumptions);
                    b&=ExpressionType::assumptionsMerge(it->assumptions(), assumptions);
//                     qDebug() << "fefefe" << b << it->assumptions();
                    
                    if(b) {
                        alts2 += *it;
//                         qDebug() << "!!!" << it->assumptions() << b;
                    }
                }
                current=ExpressionType(ExpressionType::Many, alts2);
            }
            
        }    break;
        case Container::piece: {
            QMap<QString, ExpressionType> assumptions=typeIs(c->m_params.constLast(), ExpressionType(ExpressionType::Bool)); //condition check
            c->m_params.first()->accept(this); //we return the body
            QList<ExpressionType> alts=current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current, rets;
            foreach(const ExpressionType& t, alts) {
                QMap<int, ExpressionType> stars;
                ExpressionType toadd=t;
                bool b=ExpressionType::assumptionsMerge(toadd.assumptions(), assumptions);
                
                b&=ExpressionType::matchAssumptions(&stars, assumptions, t.assumptions());
                
                if(b) {
                    toadd=toadd.starsToType(stars);
                    
                    rets += toadd;
                }
            }
            current=ExpressionType(ExpressionType::Many, rets);
        }    break;
        case Container::declare:{
            Q_ASSERT(c->m_params.first()->type()==Object::variable);
            Ci* var = static_cast<Ci*>(c->m_params.first());
            
            m_calculating.append(var->name());
            c->m_params.constLast()->accept(this);
            m_calculating.removeLast();
            
            current=tellTypeIdentity(var->name(), current);
        }    break;
        case Container::lambda: {
            QSet<QString> aux=m_lambdascope;
            QStringList bvars=c->bvarStrings();
            m_lambdascope+=bvars.toSet();
            c->m_params.constLast()->accept(this);
            m_lambdascope=aux;
            
            QList<ExpressionType> alts=current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current;
            ExpressionType res=ExpressionType(ExpressionType::Many);
            foreach(const ExpressionType& alt, alts) {
                QList<ExpressionType> args;
                foreach(const QString& bvar, bvars) {
                    ExpressionType toadd;
                    if(alt.assumptions().contains(bvar))
                        toadd=alt.assumptionFor(bvar);
                    else
                        toadd=ExpressionType(ExpressionType::Any, m_stars++);
                    
//                     qDebug() << "valueee" << bvar << toadd;
                    args += toadd;
                }
                args += alt;
                
                args=ExpressionType::lambdaFromArgs(args);
                
                res.addAlternative(ExpressionType(ExpressionType::Many, args));
            }
            current=res;
            current.removeAssumptions(bvars);
        }    break;
        case Container::otherwise:
        case Container::math:
        case Container::none:
        case Container::downlimit:
        case Container::uplimit:
        case Container::bvar:
        case Container::domainofapplication:
//             for(Container::const_iterator it=c->constBegin(); it!=c->constEnd(); ++it)
//             Q_ASSERT(c->constBegin()+1==c->constEnd());
            if(c->constBegin()+1==c->constEnd())
                (*c->constBegin())->accept(this);
            break;
    }
    
    if(current.type()==ExpressionType::Many) {
        if(current.alternatives().isEmpty())
            current=ExpressionType(ExpressionType::Error);
        else if(current.alternatives().count()==1)
            current=current.alternatives().at(0);
    }
    
    return QString();
}

template <class T>
QVariant ExpressionTypeChecker::visitListOrVector(const T* v, ExpressionType::Type t, int size)
{
    ExpressionType cont=commonType(v->values());
    
    if(cont.type()==ExpressionType::Many) {
        ExpressionType toret(ExpressionType::Many);
        foreach(const ExpressionType& contalt, cont.alternatives()) {
            QMap<QString, ExpressionType> assumptions;
            assumptions=typeIs(v->constBegin(), v->constEnd(), contalt);
            ExpressionType cc(t, contalt, size);
            
            bool b=ExpressionType::assumptionsMerge(cc.assumptions(), assumptions);
            if(b)
                toret.addAlternative(cc);
        }
        
        current=toret;
    } else if(!cont.isError()) {
        QMap<QString, ExpressionType> assumptions=typeIs(v->constBegin(), v->constEnd(), cont);
        current=ExpressionType(t, cont, size);
        current.addAssumptions(assumptions);
    } else
        current=ExpressionType(ExpressionType::Error);
    
    return QString();
}

QVariant ExpressionTypeChecker::visit(const List* l)
{
    visitListOrVector(l, ExpressionType::List, 0);
    return QString();
}

QVariant ExpressionTypeChecker::visit(const Vector* l)
{
    visitListOrVector(l, ExpressionType::Vector, l->size());
    return QString();
}

QVariant ExpressionTypeChecker::visit(const Matrix* m)
{
    const Matrix* v = m;
    ExpressionType::Type t = ExpressionType::Matrix;
    int size = m->rowCount();
    
    QList<Object*> values;
    foreach(MatrixRow *row, m->rows())
        values.append(row);
    
    ExpressionType cont=commonType(values);
    
    if(cont.type()==ExpressionType::Many) {
        ExpressionType toret(ExpressionType::Many);
        foreach(const ExpressionType& contalt, cont.alternatives()) {
            QMap<QString, ExpressionType> assumptions;
            assumptions=typeIs(v->constBegin(), v->constEnd(), contalt);
            ExpressionType cc(t, contalt, size);
            
            bool b=ExpressionType::assumptionsMerge(cc.assumptions(), assumptions);
            if(b)
                toret.addAlternative(cc);
        }
        
        current=toret;
    } else if(!cont.isError()) {
        QMap<QString, ExpressionType> assumptions=typeIs(v->constBegin(), v->constEnd(), cont);
        current=ExpressionType(t, cont, size);
        current.addAssumptions(assumptions);
    } else
        current=ExpressionType(ExpressionType::Error);
    
    return QString();
}

QVariant ExpressionTypeChecker::visit(const MatrixRow* mr)
{
    visitListOrVector(mr, ExpressionType::Vector, mr->size());
    return QString();
}

template <class T>
QMap<QString, ExpressionType> ExpressionTypeChecker::typeIs(T it, const T& itEnd, const ExpressionType& type)
{
    QList<ExpressionType> types;
    for(; it!=itEnd; ++it) {
        (*it)->accept(this);
        types+=current;
    }
    types=ExpressionType::manyFromArgs(types);
    
    bool valid=false;
    QMap<QString, ExpressionType> ret;
    
    foreach(const ExpressionType& tmany, types) {
        bool v=true;
        QMap<QString, ExpressionType> ret2;
        foreach(const ExpressionType& t, tmany.alternatives()) {
            v&=inferType(t, type, &ret2);
        }
        
        if(v) {
            valid=true;
            ExpressionType::assumptionsUnion(ret, ret2);
        }
    }
    
    if(!valid)
        addError(QCoreApplication::tr("Unexpected type"));
    return ret;
}

QMap<QString, ExpressionType> ExpressionTypeChecker::typeIs(const Object* o, const ExpressionType& type)
{
    o->accept(this);
//     qDebug() << "fluuuu" << current << type;
    
    bool corr=false;
    QList<ExpressionType> alts = current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current;
    
    QMap<QString, ExpressionType> assumptions(m_typeForBVar);
    foreach(const ExpressionType& t, alts) {
        QMap<QString, ExpressionType> ass;
        bool correct = inferType(t, type, &ass);
        if(correct) {
            ExpressionType::assumptionsUnion(assumptions, ass);
        }
        corr|=correct;
    }
    
    if(!corr)
        addError(QCoreApplication::tr("Cannot convert '%1' to '%2'").arg(o->toString(), type.toString()));
    
    return assumptions;
}

ExpressionType ExpressionTypeChecker::typeForVar(const QString& var)
{
    if(m_calculating.contains(var))
        return ExpressionType(ExpressionType::Any, m_stars++);
    else if(!m_vars.contains(var)) {
//         qDebug() << "checking..." << var;
        Q_ASSERT(m_v->contains(var));
        m_calculating += var;
        m_v->value(var)->accept(this);
        m_calculating.removeLast();
        current=tellTypeIdentity(var, current);
        current.clearAssumptions();
        current.simplifyStars();
        m_vars[var]=current;
//         qDebug() << "checked type" << var << "=" << current;
    }
    
    ExpressionType ret=m_vars.value(var);
    
//     qDebug() << "P/P/P/P/P/" << ret;
    m_stars=ret.increaseStars(m_stars);
//     qDebug() << "P/P/P/P/P/" << ret;
    return ret;
}

QStringList ExpressionTypeChecker::errors() const
{
    QStringList ret;
    foreach(const QStringList& errs, m_err)
        ret << errs;
    return ret;
}

void ExpressionTypeChecker::addError(const QString& err)
{
    if(m_err.isEmpty())
        m_err += QStringList();
    
    m_err.last().append(err);
}

void ExpressionTypeChecker::initializeVars(const QMap< QString, ExpressionType >& types)
{
    m_vars=types;
}

QMap<QString, ExpressionType> ExpressionTypeChecker::variablesTypes() const
{
    return m_vars;
}

QVariant ExpressionTypeChecker::visit(const None* ) { Q_ASSERT(false && "should not get here"); return QVariant(); }

}
