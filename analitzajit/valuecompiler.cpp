/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "valuecompiler.h"

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "value.h"
#include "vector.h"
#include "operator.h"
#include "container.h"
#include <QStringList>
#include "list.h"
#include "variable.h"
#include "apply.h"
#include "analitzautils.h"
#include "matrix.h"

Q_DECLARE_METATYPE(llvm::Value*);
Q_DECLARE_METATYPE(llvm::Function*);

//TODO better names
static llvm::IRBuilder<> Builder(llvm::getGlobalContext());
static std::map<QString, llvm::Value*> NamedValues;

using namespace Analitza;

QMap<Analitza::Operator::OperatorType, QString> llvminitOperators()
{
	QMap<Analitza::Operator::OperatorType, QString> ret;
	ret.insert(Analitza::Operator::plus, "+");
	ret.insert(Analitza::Operator::times, "*");
	ret.insert(Analitza::Operator::divide, "/");
	ret.insert(Analitza::Operator::eq, "=");
	ret.insert(Analitza::Operator::neq, "!=");
	ret.insert(Analitza::Operator::lt, "<");
	ret.insert(Analitza::Operator::leq, "<=");
	ret.insert(Analitza::Operator::gt, ">");
	ret.insert(Analitza::Operator::geq, ">=");
	ret.insert(Analitza::Operator::power, "^");
	ret.insert(Analitza::Operator::minus, "-");
	return ret;
}

TypeCompiler::TypeCompiler(const Analitza::Object* o, llvm::Module* mod, const QVector< Analitza::Object* >& stack, Analitza::Variables* v)
	: m_runStack(stack)
	, m_mod(mod)
{
    if (o)
        m_result=o->accept(this);
}

QVariant TypeCompiler::visit(const Analitza::Ci* var)
{
	//TODO chack in variables too ... since we are playing by efault with stack vars
// 	NamedValues[var->name()]->dump();
	return QVariant::fromValue<llvm::Value*>(NamedValues[var->name()]);
}

QVariant TypeCompiler::visit(const Analitza::Operator* op)
{
// 	switch(op->operatorType()) {
// 		case Operator::lt:
// 		case Operator::gt:
// 		case Operator::eq:
// 		case Operator::neq:
// 		case Operator::leq:
// 		case Operator::geq:
// 			return 1;
// 		case Operator::plus:
// 			return 2;
// 		case Operator::minus:
// 			return size==1 ? 8 : 3;
// 		case Operator::times:
// 			return 4;
// 		case Operator::divide:
// 			return 5 + (pos>0 ? 0 : 1);
// 		case Operator::_and:
// 		case Operator::_or:
// 		case Operator::_xor:
// 			return 6;
// 		case Operator::power:
// 			return 7 + (pos>0 ? 0 : 1);
// 		default:
// 			return 1000;
// 	}
	return op->name();
}

QVariant TypeCompiler::visit(const Analitza::Vector* vec)
{
	return QString();
}

QVariant TypeCompiler::visit(const Analitza::Matrix* m)
{
	return QString();
}

QVariant TypeCompiler::visit(const Analitza::MatrixRow* mr)
{
	return QString();
}

QVariant TypeCompiler::visit(const Analitza::List* vec)
{
	return QString();
}

QVariant TypeCompiler::visit(const Analitza::Cn* val)
{
	llvm::Value *ret = 0;
	
	switch(val->format()) {
		case Analitza::Cn::Boolean: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(bool)*8, val->intValue(), false)); break;
		case Analitza::Cn::Char: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(char)*8, val->intValue(), false)); break;
		case Analitza::Cn::Integer: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(int)*8, val->intValue(), true)); break;
		case Analitza::Cn::Real: ret = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(val->value())); break;
		case Analitza::Cn::Complex: {
			llvm::Type *rawtypes[2] = {llvm::Type::getDoubleTy(llvm::getGlobalContext()), llvm::Type::getDoubleTy(llvm::getGlobalContext())};
			llvm::ArrayRef<llvm::Type*> types = llvm::ArrayRef<llvm::Type*>(rawtypes, 2);
			llvm::StructType *complextype = llvm::StructType::get(llvm::getGlobalContext(), types, true);
			
			std::complex<double> complexval = val->complexValue();
			llvm::Constant *rawvalues[2] = {llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(complexval.real())),
				llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(complexval.imag()))};
			llvm::ArrayRef<llvm::Constant*> values = llvm::ArrayRef<llvm::Constant*>(rawvalues, 2);
			
			ret = llvm::ConstantStruct::get(complextype,values);
		}	break;
	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

QVariant TypeCompiler::visit(const Analitza::Apply* c)
{
	llvm::Value *ret = 0;
	
	//TODO just check for binary and unary operators .. exclude functions/commands (handle that the next iteration)
	switch(c->firstOperator().operatorType()) {
// 		case Operator::sum:
// 			ret = sum(*c);
// 			break;
		case Analitza::Operator::times: {
			QVariant a = c->at(0)->accept(this);
			QVariant b = c->at(1)->accept(this);
			
			ret = Builder.CreateFMul(a.value<llvm::Value*>(), b.value<llvm::Value*>(), "multmp");
		}	break;
// 		case Operator::product:
// 			ret = product(*c);
// 			break;
// 		case Operator::forall:
// 			ret = forall(*c);
// 			break;
// 		case Operator::exists:
// 			ret = exists(*c);
// 			break;
// 		case Operator::function:
// 			ret = func(*c);
// 			break;
// 		case Operator::diff:
// 			ret = calcDiff(c);
// 			break;
// 		case Operator::map:
// 			ret = calcMap(c);
// 			break;
// 		case Operator::filter:
// 			ret = calcFilter(c);
// 			break;
// 		default: {
// 			int count=c->countValues();
// 			Q_ASSERT(count>0);
// 			Q_ASSERT(	(op.nparams()< 0 && count>1) ||
// 						(op.nparams()>-1 && count==op.nparams()) ||
// 						opt==Operator::minus);
// 			
// 			QString* error=0;
// 			if(count>=2) {
// 				Apply::const_iterator it = c->firstValue(), itEnd=c->constEnd();
// 				ret = calc(*it);
// 				++it;
// 				bool stop=isNull(opt, ret);
// 				for(; !stop && it!=itEnd; ++it) {
// 					bool isValue = (*it)->type()==Object::value;
// 					Object* v = isValue ? *it : calc(*it);
// 					ret=Operations::reduce(opt, ret, v, &error);
// 					if(!isValue)
// 						delete v;
// 					
// 					if(Q_UNLIKELY(error)) {
// 						m_err.append(*error);
// 						delete error;
// 						error=0;
// 						break;
// 					}
// 					
// 					stop=isNull(opt, ret);
// 				}
// 			} else {
// 				ret=Operations::reduceUnary(opt, calc(*c->firstValue()), &error);
// 				if(Q_UNLIKELY(error)) {
// 					m_err.append(*error);
// 					delete error;
// 				}
// 			}
// 		}	break;
	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

QVariant TypeCompiler::visit(const Analitza::Container* c)
{
	llvm::Value *ret = 0;
	switch(c->containerType()) {
		case Analitza::Container::piecewise: {
// 			ExpressionType type=commonType(c->m_params);
// 			
// 			if(type.isError()) {
// 				addError(QCoreApplication::tr("Could not determine the type for piecewise"));
// 				current=type;
// 			} else {
// 				QList<ExpressionType> alts=type.type()==ExpressionType::Many ? type.alternatives() : QList<ExpressionType>() << type, alts2;
// 				
// 				for(QList< ExpressionType >::iterator it=alts.begin(), itEnd=alts.end(); it!=itEnd; ++it) {
// 					QMap<QString, ExpressionType> assumptions=typeIs(c->constBegin(), c->constEnd(), *it);
// 					
// 	// 				QMap<int, ExpressionType> stars;
// 	// 				bool b=ExpressionType::matchAssumptions(&stars, assumptions, type.assumptions());
// 	// 				Q_ASSERT(b);
// 	// 				type=type.starsToType(stars);
// 					
// // 					qDebug() << "suuuuu\n\t" << it->assumptions() << "\n\t" << assumptions;
// 					QMap<int, ExpressionType> stars;
// 					bool b=ExpressionType::matchAssumptions(&stars, it->assumptions(), assumptions);
// 					b&=ExpressionType::assumptionsMerge(it->assumptions(), assumptions);
// // 					qDebug() << "fefefe" << b << it->assumptions();
// 					
// 					if(b) {
// 						alts2 += *it;
// // 						qDebug() << "!!!" << it->assumptions() << b;
// 					}
// 				}
// 				current=ExpressionType(ExpressionType::Many, alts2);
// 			}
		}	break;
		case Analitza::Container::piece: {
// 			QMap<QString, ExpressionType> assumptions=typeIs(c->m_params.last(), ExpressionType(ExpressionType::Bool)); //condition check
// 			c->m_params.first()->accept(this); //we return the body
// 			QList<ExpressionType> alts=current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current, rets;
// 			foreach(const ExpressionType& t, alts) {
// 				QMap<int, ExpressionType> stars;
// 				ExpressionType toadd=t;
// 				bool b=ExpressionType::assumptionsMerge(toadd.assumptions(), assumptions);
// 				
// 				b&=ExpressionType::matchAssumptions(&stars, assumptions, t.assumptions());
// 				
// 				if(b) {
// 					toadd=toadd.starsToType(stars);
// 					
// 					rets += toadd;
// 				}
// 			}
// 			current=ExpressionType(ExpressionType::Many, rets);
		}	break;
		case Analitza::Container::declare:{
// 			Q_ASSERT(c->m_params.first()->type()==Object::variable);
// 			Ci* var = static_cast<Ci*>(c->m_params.first());
// 			
// 			m_calculating.append(var->name());
// 			c->m_params.last()->accept(this);
// 			m_calculating.removeLast();
// 			
// 			current=tellTypeIdentity(var->name(), current);
		}	break;
		case Analitza::Container::lambda: {
			const QStringList bvars = c->bvarStrings();
			
			Q_ASSERT(m_runStack.size() == bvars.size());
			
			std::vector<llvm::Type*> tparams;
			for (int i = 0; i < m_runStack.size(); ++i) {
				switch(m_runStack.at(i)->type()) {
					case Analitza::Object::value: {
						switch(((Analitza::Cn*)m_runStack.at(i))->format()) {
							case Analitza::Cn::Integer://TODO get ineger bit width then decide if 32 or 64 bits
								tparams.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
							break;
							case Analitza::Cn::Char:
								tparams.push_back(llvm::Type::getInt8Ty(llvm::getGlobalContext()));
							break;
// 							case Cn::Boolean: //TODO
// 								tparams.push_back(llvm::Type::get(llvm::getGlobalContext()));
// 							break;
							case Analitza::Cn::Real:
								tparams.push_back(llvm::Type::getDoubleTy(llvm::getGlobalContext()));
							break;
						}
					}	break;
// 					case Vec, mats://TODO
// 						type=ExpressionType::Char;
// 						break;
// 					default:
// 						type=ExpressionType::Value;
// 						break;
				}
			}
			
			//TODO we need the return type
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), tparams, false);
			
			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "mylam", m_mod);//TODO count how many lambdas we have generated i.e mylam0, mylam1 ...
			
			//TODO better code
			int Idx = 0;
			for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != bvars.size(); ++AI, ++Idx) {
				AI->setName(bvars[Idx].toStdString());
				AI->dump();
				NamedValues[bvars[Idx]] = AI;
			}
			
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", F);
			Builder.SetInsertPoint(BB);
			Builder.CreateRet(c->m_params.last()->accept(this).value<llvm::Value*>());
			
// 			QVariant al = QVariant::fromValue<llvm::Value*>(F);
			
// 			al.value<llvm::Value*>()->dump();
			llvm::verifyFunction(*F);
			
			ret = F;
		}	break;
		case Analitza::Container::otherwise:
		case Analitza::Container::math:
		case Analitza::Container::none:
		case Analitza::Container::downlimit:
		case Analitza::Container::uplimit:
		case Analitza::Container::bvar:
		case Analitza::Container::domainofapplication: {
			Q_ASSERT(c->constBegin()+1==c->constEnd());
			ret = (*c->constBegin())->accept(this).value<llvm::Value*>();
		}	break;
	}
	
// 	if(current.type()==ExpressionType::Many) {
// 		if(current.alternatives().isEmpty())
// 			current=ExpressionType(ExpressionType::Error);
// 		else if(current.alternatives().count()==1)
// 			current=current.alternatives().first();
// 	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

QVariant TypeCompiler::visit(const Analitza::CustomObject*)
{
	return "CustomObject";
}

QVariant TypeCompiler::visit(const Analitza::None* )
{
	return QString();
}
