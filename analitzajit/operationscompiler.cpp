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

#include "operationscompiler.h"

#include <math.h>

#include <cmath>
#include <complex>
#include <QCoreApplication>

#include "analitza/value.h"
#include "analitza/vector.h"
#include "analitza/expression.h"
#include "analitza/list.h"
#include "analitza/expressiontypechecker.h"
#include "analitza/customobject.h"
#include "analitza/matrix.h"
#include "analitza/container.h"
#include "analitza/additionchains.h"

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

static llvm::IRBuilder<> buildr(llvm::getGlobalContext());

using namespace std;
using namespace Analitza;

llvm::Value * compileRealReal(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* val1, llvm::Value* val2, QString& error)
{
	llvm::Value *ret = 0;
	switch(op) {
		case Operator::plus: {
			//oper->setValue(a+b);
			
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
			
// 			a->dump();
// 			b->dump();
			buildr.SetInsertPoint(currentBlock);
			ret = buildr.CreateFAdd(a, b);
			
		}	break;
		case Operator::minus: {
			//oper->setValue(a+b);
			
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
			
// 			a->dump();
// 			b->dump();
			buildr.SetInsertPoint(currentBlock);
			ret = buildr.CreateFSub(a, b);
			
		}	break;
		case Operator::times: {
			//oper->setValue(a*b);
		
			llvm::Value *a = val1;
			llvm::Value *b = val2;
			
// 			a->dump();
// 			b->dump();
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
	
			ret = buildr.CreateFMul(a, b, "multmp");
		}	break;
		case Operator::divide: {
// 			if(Q_LIKELY(b!=0.)) {
// 				//oper->setValue(a / b);
// 			}
// 			else {
// 				error= QString(QCoreApplication::tr("Cannot divide by 0."));
// 				//ret=0;
// 			}
			llvm::Value *a = val1;
			llvm::Value *b = val2;
			
// 			a->dump();
// 			b->dump();
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
			
			ret = buildr.CreateFDiv(a, b, "divtmp");
			
			
		}	break;
		case Operator::power: {
// 			oper->setValue( b==2 ? a*a
// 			: b<1 && b>-1 && a<0 ? pow(complex<double>(a), complex<double>(b)).real()
// 			: pow(a, b));
			
			buildr.SetInsertPoint(currentBlock);
			llvm::Function *fun = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::pow, llvm::Type::getDoubleTy(llvm::getGlobalContext()));
			return buildr.CreateCall2(fun, val1, val2, "adas");
		}	break;
// 		case Operator::rem:
// 			if(Q_LIKELY(floor(b)!=0.))
// 				oper->setValue(int(remainder(a, b)));
// 			else {
// 				correct= QString(QCoreApplication::tr("Cannot calculate the remainder on 0."));
// 				ret=new None();
// 			}
// 			break;
// 		case Operator::quotient:
// 			oper->setValue(int(floor(a / b)));
// 			break;
// 		case Operator::factorof: {
// 			int fb = int(floor(b));
// 			if(Q_LIKELY(fb!=0))
// 				oper->setValue((int(floor(a)) % fb)==0);
// 			else {
// 				correct= QString(QCoreApplication::tr("Cannot calculate the factor on 0."));
// 				ret=new None();
// 			}
// 		}	break;
// 		case Operator::min:
// 			oper->setValue(a < b? a : b);
// 			break;
// 		case Operator::max:
// 			oper->setValue(a > b? a : b);
// 			break;
		case Operator::gt: {
			//oper->setValue(a > b);
			
// 			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_UGT , a, b, "cmpeq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());
			
		}	break;
		case Operator::lt: {
// 			oper->setValue(a < b);
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_ULT  , a, b, "cmpeq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());
			
		}	break;
		case Operator::eq: {
// 			TODO
// 			//oper->setValue(a == b);
// 			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_UEQ, a, b, "cmpeq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());
// // // 			ret->dump();
// 			
		}	break;
// 		case Operator::approx:
// 			oper->setValue(fabs(a-b)<0.001);
// 			break;
		case Operator::neq: {
			//oper->setValue(a != b);
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_UNE, a, b, "cmpneq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());
		}	break;
		case Operator::geq: {
			//oper->setValue(a >= b);
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_UGE , a, b, "cmpneq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());
			
		}	break;
		case Operator::leq: {
			//oper->setValue(a <= b);
			
			llvm::Value *a = val1;
			llvm::Value *b = val2;
// // 			
			if (currentBlock)
				buildr.SetInsertPoint(currentBlock);
// // 			
			ret = buildr.CreateFCmp(llvm::CmpInst::FCMP_ULE  , a, b, "cmpneq"), llvm::Type::getDoubleTy(llvm::getGlobalContext());

		}	break;
// 		case Operator::_and:
// 			oper->setValue(a && b);
// 			break;
// 		case Operator::_or:
// 			oper->setValue(a || b);
// 			break;
// 		case Operator::_xor:
// 			oper->setValue((a || b) && !(a&&b));
// 			break;
// 		case Operator::implies:
// 			oper->setValue(a || !b);
// 			break;
// 		case Operator::gcd:  {
// 			//code by michael cane aka kiko :)
// 			int ia=floor(a), ib=floor(b), residu = 0;
// 			while (ib > 0) {
// 				residu = ia % ib;
// 				ia = ib;
// 				ib = residu;
// 			}
// 			oper->setValue(ia);
// 		}	break;
// 		case Operator::lcm:
// 			//code by michael cane aka kiko :)
// 			if(Q_UNLIKELY(floor(a)==0. || floor(b)==0.)) {
// 				correct= QString(QCoreApplication::tr("Cannot calculate the lcm of 0."));
// 				ret=new None();
// 			}
// 			else {
// 				int ia=floor(a), ib=floor(b);
// 				int ic=ia*ib, residu = 0;
// 				while (ib > 0) {
// 					residu = ia % ib;
// 					ia = ib;
// 					ib = residu;
// 				}
// 				ia=ic/ia;
// 				oper->setValue(ia);
// 			}
// 			break;
// 		case Operator::root:
// 			oper->setValue(			  b==2.0 ? sqrt(a)
// 			: (b>1 || b<-1) && a<0 ? pow(complex<double>(a), complex<double>(1./b)).real()
// 			: pow(a, 1.0/b));
// 			break;
// 		default:
// 			break;
	}
	return ret;
}

llvm::Value * compileComplexComplex(llvm::Module *module, llvm::BasicBlock *currentBlock, enum Operator::OperatorType op, llvm::Value *oper, complex<double> a, complex<double> b, QString& correct)
{
	return 0; //TODO
// 	switch(op) {
// 		case Operator::plus:
// 			oper->setValue(a+b);
// 			break;
// 		case Operator::times:
// 			oper->setValue(a*b);
// 			break;
// 		case Operator::divide:
// 			if(Q_LIKELY(b.real()!=0. || b.imag()!=0.))
// 				oper->setValue(a / b);
// 			else
// 				correct= QString(QCoreApplication::tr("Cannot divide by 0."));
// 			break;
// 		case Operator::minus:
// 			oper->setValue(a - b);
// 			break;
// 		case Operator::power:
// 			oper->setValue(pow(a, b));
// 			break;
// 		case Operator::rem:
// 			if(Q_LIKELY(floor(b.real())!=0.))
// 				oper->setValue(int(remainder(a.real(), b.real())));
// 			else
// 				correct= QString(QCoreApplication::tr("Cannot calculate the remainder on 0."));
// 			break;
// 		case Operator::quotient:
// 			oper->setValue(int(floor((a / b).real())));
// 			break;
// 		case Operator::factorof: {
// 			int fb = int(floor(b.real()));
// 			if(Q_LIKELY(fb!=0))
// 				oper->setValue((int(floor(a.real())) % fb)==0);
// 			else
// 				correct= QString(QCoreApplication::tr("Cannot calculate the factor on 0."));
// 		}	break;
// 		case Operator::min:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a < b ? a : b);
// 			break;
// 		case Operator::max:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a > b? a : b);
// 			break;
// 		case Operator::gt:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a > b);
// 			break;
// 		case Operator::lt:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a < b);
// 			break;
// 		case Operator::eq:
// 			oper->setValue(a == b);
// 			break;
// 		case Operator::approx:
// 			oper->setValue(fabs(a.real()-b.real())+fabs(a.imag()-b.imag())<0.001);
// 			break;
// 		case Operator::neq:
// 			oper->setValue(a != b);
// 			break;
// 		case Operator::geq:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a >= b);
// 			break;
// 		case Operator::leq:
// 			//TODO can not have this operator for complex numbers
// // 			oper->setValue(a <= b);
// 			break;
// 		case Operator::_and:
// 		case Operator::_or:
// 		case Operator::_xor:
// 		case Operator::implies:
// 			correct =  QString(QCoreApplication::tr("Boolean operations on complex numbers not available"));
// 			break;
// 		case Operator::gcd:  {
// 			//code by michael cane aka kiko :)
// 			int ia=floor(a.real()), ib=floor(b.real()), residu = 0;
// 			while (ib > 0) {
// 				residu = ia % ib;
// 				ia = ib;
// 				ib = residu;
// 			}
// 			oper->setValue(ia);
// 		}	break;
// 		case Operator::lcm:
// 			//code by michael cane aka kiko :)
// 			if(Q_UNLIKELY(floor(a.real())==0. || floor(b.real())==0.))
// 				correct= QString(QCoreApplication::tr("Cannot calculate the lcm of 0."));
// 			else {
// 				int ia=floor(a.real()), ib=floor(b.real());
// 				int ic=ia*ib, residu = 0;
// 				while (ib > 0) {
// 					residu = ia % ib;
// 					ia = ib;
// 					ib = residu;
// 				}
// 				ia=ic/ia;
// 				oper->setValue(ia);
// 			}
// 			break;
// 		case Operator::root:
// 			if(b.real()!=2.0 && b.imag()==0)
// 				correct =  QString(QCoreApplication::tr("Only square root implemented for complex numbe"));
// 			else
// 				oper->setValue(sqrt(a));
// 			break;
// 		default:
// 			break;
// 	}
// 	return oper;
}

llvm::Value * OperationsCompiler::compileValueValueOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* val1, llvm::Value* val2, QString& error)
{
	//TODO complex numbers case
// 	if(Q_UNLIKELY(oper->isComplex() || oper1->isComplex())) {
// 		const complex<double> a=oper->complexValue(), b=oper1->complexValue();
// 		return compileComplexComplex(op, oper, a, b, correct);
// 	} else {
// 		const double a=oper->value(), b=oper1->value();
		return compileRealReal(module, currentBlock, op, val1, val2, error);
// 	}
}

llvm::Value * compileUnaryComplex(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* oper, QString& correct)
{
	return 0; //TODO
// 	const complex<double> a=oper->complexValue();
// 
// 	switch(op) {
// 		case Operator::minus:
// 			     oper->setValue(-a);
// 			break;
// 		case Operator::sin:
// 			     oper->setValue(sin(a));
// 			break;
// 		case Operator::cos:
// 			     oper->setValue(cos(a));
// 			break;
// 		case Operator::tan:
// 			     oper->setValue(tan(a));
// 			break;
// 		case Operator::sinh:
// 			     oper->setValue(sinh(a));
// 			break;
// 		case Operator::cosh:
// 			     oper->setValue(cosh(a));
// 			break;
// 		case Operator::tanh:
// 			     oper->setValue(tanh(a));
// 			break;
// 		case Operator::coth:
// 			     oper->setValue(cosh(a)/sinh(a));
// 			break;
// 		case Operator::exp:
// 			     oper->setValue(exp(a));
// 			break;
// 		case Operator::ln:
// 			     oper->setValue(log(a));
// 			break;
// 		case Operator::log:
// 			     oper->setValue(log10(a));
// 			break;
// 		case Operator::abs:
// 			     oper->setValue(std::abs(a));
// 			break;
// 		case Operator::conjugate:
// 			     oper->setValue(std::conj(a));
// 			break;
// 			case Operator::arg:
// 				    oper->setValue(std::arg(a));
// 			break;
// 			case Operator::real:
// 				    oper->setValue(a.real());
// 			break;
// 			case Operator::imaginary:
// 				    oper->setValue(a.imag());
// 			break;
// 		default:
// 			correct= QString(QCoreApplication::tr("Could not calculate a value %1").arg(Operator(op).toString()));
// 	}
// 	return oper;
}

llvm::Value * compileUnaryReal(llvm::Module *module, llvm::BasicBlock *currentBlock, Operator::OperatorType op, llvm::Value* oper, QString& correct)
{
	//const double a=oper->value();
	llvm::Value *ret = 0;
// 	
	switch(op) {
		case Operator::minus: {
			//oper->setValue(-a);
			buildr.SetInsertPoint(currentBlock);
			ret = buildr.CreateFNeg(oper, "negfval");
		}	break;
// 		case Operator::factorial: {
// 			//Use gamma from math.h?
// 			uint res=1;
// 			for(int i=a; i>1.; i--) {
// 				res*=floor(i);
// 			}
// 			     oper->setValue(res);
// 		}	break;
		case Operator::sin: {
			//oper->setValue(sin(a));
// 			module->dump();
// 			qDebug() << "PEPEPEPEPE " << module;
// 			oper->getType()->dump();
			
			llvm::Value *a = oper;
			
			buildr.SetInsertPoint(currentBlock);
			llvm::Function *fun = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::sin, a->getType());
			ret = buildr.CreateCall(fun, a, "adas");
		}	break;
		case Operator::cos: {
			//oper->setValue(cos(a));
// 			module->dump();
// 			qDebug() << "PEPEPEPEPE " << module;
// 			oper->getType()->dump();
			
			llvm::Value *a = oper;
			
			buildr.SetInsertPoint(currentBlock);
			llvm::Function *fun = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::cos, a->getType());
			ret = buildr.CreateCall(fun, a, "adas");
		}	break;
		case Operator::tan: {
			//oper->setValue(tan(a));
// 			module->dump();
// 			qDebug() << "PEPEPEPEPE " << module;
// 			oper->getType()->dump();
			
			llvm::Value *a = oper;
			
			buildr.SetInsertPoint(currentBlock);
			llvm::Value *sinv = compileUnaryReal(module, currentBlock, Operator::sin, a, correct);
			llvm::Value *cosv = compileUnaryReal(module, currentBlock, Operator::cos, a, correct);
			ret = compileRealReal(module, currentBlock, Operator::divide, sinv, cosv, correct);
		}	break;
// 		case Operator::sec:
// 			     oper->setValue(1./cos(a));
// 			break;
// 		case Operator::csc:
// 			     oper->setValue(1./sin(a));
// 			break;
		case Operator::cot: {
			//oper->setValue(cot(a));
// 			module->dump();
// 			qDebug() << "PEPEPEPEPE " << module;
// 			oper->getType()->dump();
			
			llvm::Value *a = oper;
			
			buildr.SetInsertPoint(currentBlock);
			llvm::Value *sinv = compileUnaryReal(module, currentBlock, Operator::sin, a, correct);
			llvm::Value *cosv = compileUnaryReal(module, currentBlock, Operator::cos, a, correct);
			ret = compileRealReal(module, currentBlock, Operator::divide, cosv, sinv, correct);
		}	break;
// 		case Operator::sinh:
// 			     oper->setValue(sinh(a));
// 			break;
// 		case Operator::cosh:
// 			     oper->setValue(cosh(a));
// 			break;
// 		case Operator::tanh:
// 			     oper->setValue(tanh(a));
// 			break;
// 		case Operator::sech:
// 			     oper->setValue(1.0/cosh(a));
// 			break;
// 		case Operator::csch:
// 			     oper->setValue(1.0/sinh(a));
// 			break;
// 		case Operator::coth:
// 			     oper->setValue(cosh(a)/sinh(a));
// 			break;
// 		case Operator::arcsin:
// 			     oper->setValue(asin(a));
// 			break;
// 		case Operator::arccos:
// 			     oper->setValue(acos(a));
// 			break;
// 		case Operator::arctan:
// 			     oper->setValue(atan(a));
// 			break;
// 		case Operator::arccot:
// 			     oper->setValue(log(a+pow(a*a+1., 0.5)));
// 			break;
// 		case Operator::arcsinh: //see http://en.wikipedia.org/wiki/Inverse_hyperbolic_function
// 			     oper->setValue(asinh(a));
// 			break;
// 		case Operator::arccosh:
// 			     oper->setValue(acosh(a));
// 			break;
// 		case Operator::arccsc:
// 			     oper->setValue(1/asin(a));
// 			break;
// 		case Operator::arccsch:
// 			     oper->setValue(log(1/a+sqrt(1/(a*a)+1)));
// 			break;
// 		case Operator::arcsec:
// 			     oper->setValue(1/(acos(a)));
// 			break;
// 		case Operator::arcsech:
// 			     oper->setValue(log(1/a+sqrt(1/a+1)*sqrt(1/a-1)));
// 			break;
// 		case Operator::arctanh:
// 			     oper->setValue(atanh(a));
// 			break;
// 		case Operator::exp:
// 			     oper->setValue(exp(a));
// 			break;
// 		case Operator::ln:
// 			     oper->setValue(log(a));
// 			break;
// 		case Operator::log:
// 			     oper->setValue(log10(a));
// 			break;
// 		case Operator::abs:
// 			     oper->setValue(a>=0. ? a : -a);
// 			break;
// 		case Operator::floor:
// 			     oper->setValue(floor(a));
// 			break;
// 		case Operator::ceiling:
// 			     oper->setValue(ceil(a));
// 			break;
// 		case Operator::_not:
// 			     oper->setValue(!a);
// 			break;
		default:
			correct= QString(QCoreApplication::tr("Could not calculate a value %1").arg(Operator(op).toString()));
			break;
	}
	return ret;
}

llvm::Value* OperationsCompiler::compileUnaryValueOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* a, QString& error)
{
// 	//TODO complex case
// 	if(Q_UNLIKELY(oper->isComplex()))
// 		return compileUnaryComplex(module, currentBlock, op, a, error);
// 	else
	return compileUnaryReal(module, currentBlock, op, a, error);
}

llvm::Value* OperationsCompiler::compileNoneValueOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* none, llvm::Value* val, QString& error)
{
	return 0; //TODO
// 	return errorCase(QCoreApplication::tr("Cannot calculate %1 between a value and an error type").arg(Operator(op).name()), correct);
}

llvm::Value* OperationsCompiler::compileValueNoneOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* val, llvm::Value* none, QString& error)
{
	return 0; //TODO
// 	return compileNoneValue(op, cntr, oper, correct);
}

llvm::Value * OperationsCompiler::compileValueVectorOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* val, llvm::Value* vec, QString& error)
{
	return 0; //TODO
// 	switch(op) {
// 		case Operator::selector: {
// 			int select=oper->intValue();
// 			delete oper;
// 			Object* ret=0;
// 			if(select<1 || (select-1) >= v1->size()) {
// 				correct= QString(QCoreApplication::tr("Invalid index for a container"));
// 				ret=new None();
// 			} else {
// 				ret=v1->at(select-1)->copy();
// 			}
// 			return ret;
// 		}	break;
// 		default: {
// 			Vector *ret = v1->copy();
// 			for(Vector::iterator it=ret->begin(); it!=ret->end(); ++it)
// 			{
// 				*it=compile(op, new Cn(*oper), *it, correct);
// 			}
// 			
// 			delete oper;
// 			return ret;
// 		}
// 	}
}

llvm::Value * OperationsCompiler::compileVectorValueOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* vec, llvm::Value* val, QString& error)
{
	return 0; //TODO
// 	for(Vector::iterator it=v1->begin(); it!=v1->end(); ++it)
// 	{
// 		*it=compile(op, *it, new Cn(*oper), correct);
// 	}
// 	return v1;
}

llvm::Value * OperationsCompiler::compileVectorVectorOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* vec1, llvm::Value* vec2, QString& error)
{
	return 0; //TODO
// 	if(v1->size()!=v2->size()) { //FIXME: unneeded? ... aucahuasi: I think is needed ...
// 		correct= QString(QCoreApplication::tr("Cannot operate '%1' on different sized vectors.").arg(Operator(op).name()));
// 		return new None();
// 	}
// 	
// 	if(op==Operator::scalarproduct)
// 		op=Operator::times;
// 	Vector::iterator it2=v2->begin();
// 	for(Vector::iterator it1=v1->begin(); it1!=v1->end(); ++it1, ++it2)
// 	{
// 		*it1=compile(op, *it1, *it2, correct);
// 	}
// 	return v1;
}

llvm::Value* OperationsCompiler::compileMatrixVectorOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* matrix, llvm::Value* vector, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	if (op == Operator::times) {
// 		if (matrix->hasOnlyNumbers() && vector->hasOnlyNumbers()) {
// 			const int maxk = matrix->columnCount();
// 			
// 			if (maxk == vector->size()) {
// 				const int m = matrix->rowCount();
// 				
// 				Vector *newvec = new Vector(m);
// 				
// 				for (int i = 0; i < m; ++i) {
// 					std::complex<double> sum = 0.;
// 					for (int k = 0; k < maxk; ++k)
// 						sum += static_cast<const Cn*>(compileValueValue(op, (Cn*)matrix->at(i,k), (Cn*)vector->at(k), correct))->complexValue();
// 					
// 					if (sum.imag() == 0)
// 						newvec->appendBranch(new Cn(sum.real()));
// 					else
// 						newvec->appendBranch(new Cn(sum.real(), sum.imag()));
// 				}
// 				
// 				ret = newvec;
// 			} else {
// 				correct= QString(QCoreApplication::tr("Multiplication between a matrix and a vector is allowed provided that the number of columns of the matrix equals the size of the vector"));
// 				ret=new None();
// 			}
// 		} else {
// 			correct= QString(QCoreApplication::tr("Matrix and vector entries must be numbers"));
// 			ret=new None();
// 		}
// 	}
// 	
// 	return ret;
}

llvm::Value* OperationsCompiler::compileUnaryVectorOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* c, QString& error)
{
	return 0; //TODO
// 	Object *ret=0;
// 	switch(op) {
// 		case Operator::card:
// 			ret=new Cn(c->size());
// 			break;
// 		case Operator::transpose: {
// 			const int n = c->size();
// 			
// 			Matrix* mret = new Matrix;
// 			MatrixRow* row = new MatrixRow(n);
// 			for(int j=0; j<n; ++j) {
// 				row->appendBranch(c->at(j)->copy());
// 			}
// 			
// 			mret->appendBranch(row);
// 			ret = mret;
// 		}	break;
// 		default:
// 			//Should be dealt by typechecker. not necessary
// 			correct= QString(QCoreApplication::tr("Could not calculate a vector's %1").arg(Operator(op).toString()));
// 			ret=new None();
// 			break;
// 	}
// 	delete c;
// 	return ret;
}

llvm::Value* OperationsCompiler::compileListListOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* l1, llvm::Value* l2, QString& error)
{
	return 0; //TODO
// 	Object* ret=0;
// 	switch(op) {
// 		case Operator::_union: {
// 			List::iterator itEnd=l2->end();
// 			for(List::iterator it=l2->begin(); it!=itEnd; ++it) {
// 				l1->appendBranch((*it)->copy());
// 			}
// 			
// 			ret=l1;
// 		}	break;
// 		default:
// 			//Should be dealt by typechecker. not necessary
// 			correct= QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
// 			delete l1;
// 			ret=new None();
// 			break;
// 	}
// 	return ret;
}

llvm::Value* OperationsCompiler::compileUnaryListOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value*, QString& error)
{
	return 0; //TODO
// 	Object *ret=0;
// 	switch(op) {
// 		case Operator::card:
// 			ret=new Cn(l->size());
// 			break;
// 		default:
// 			correct= QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
// 			ret=new None();
// 			break;
// 	}
// 	delete l;
// 	return ret;
}

llvm::Value* OperationsCompiler::compileValueListOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value*, llvm::Value*, QString& error)
{
	return 0; //TODO
// 	switch(op) {
// 		case Operator::selector: {
// 			int select=oper->intValue();
// 			Object* ret=0;
// 			if(select<1 || (select-1) >= v1->size()) {
// 				correct= QString(QCoreApplication::tr("Invalid index for a container"));
// 				ret=new None();
// 			} else {
// 				ret=v1->at(select-1);
// 				v1->setAt(select-1, 0);
// 			}
// 			delete oper;
// 			return ret;
// 		}	break;
// 		default:
// 			break;
// 	}
// 	return 0;
}

llvm::Value* OperationsCompiler::compileCustomCustomOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* v1, llvm::Value* v2, QString& error)
{
	return 0; //TODO
// 	switch(op) {
// 		case Operator::neq:
// 			return new Cn(v1->value()!=v2->value());
// 		case Operator::eq:
// 			return new Cn(v1->value()==v2->value());
// 		default:
// 			break;
// 	}
// 	
// 	Q_ASSERT(false && "not implemented, please report");
// 	return 0;
}

llvm::Value* OperationsCompiler::compileVectorMatrixOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* vector, llvm::Value* matrix, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	if (op == Operator::times) {
// 		if (vector->hasOnlyNumbers() && matrix->hasOnlyNumbers()) {
// 			if (1 == matrix->rowCount()) {
// 				const int m = vector->size();
// 				const int n = matrix->columnCount();
// 				
// 				Matrix *newmat = new Matrix();
// 				
// 				for (int i = 0; i < m; ++i) {
// 					MatrixRow *row = new MatrixRow(n);
// 					for (int j = 0; j < n; ++j)
// 						row->appendBranch(compileValueValue(op, (Cn*)vector->at(i), (Cn*)matrix->at(0,j), correct)->copy());
// 					
// 					newmat->appendBranch(row);
// 				}
// 				
// 				ret = newmat;
// 			} else {
// 				correct= QString(QCoreApplication::tr("Multiplication between a vector and a matrix is allowed provided that the matrix has only one matrixrow element"));
// 				ret=new None();
// 			}
// 		} else {
// 			correct= QString(QCoreApplication::tr("Matrix and vector entries must be numbers"));
// 			ret=new None();
// 		}
// 	}
// 	
// 	return ret;
}

llvm::Value* OperationsCompiler::compileMatrixMatrixOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* m1, llvm::Value* m2, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	switch(op) {
// 		//TODO see if we can use here and or xor for matrix too
// 		case Operator::plus:
// 		case Operator::minus: {
// 			if(m1->rowCount() == m2->rowCount() && m1->columnCount() == m2->columnCount()) {
// 				Matrix::iterator it2=m2->begin();
// 				for(Matrix::iterator it1=m1->begin(); it1!=m1->end(); ++it1, ++it2)
// 				{
// 					*it1 = static_cast<MatrixRow*>(compileVectorVector(op, *it1, *it2, correct));
// 				}
// 				ret = m1;
// 			} else {
// 				correct= QString(QCoreApplication::tr("Addition of two matrices is allowed provided that both matrices have the same number of rows and the same number of columns"));
// 				ret=new None();
// 			}
// 		}	break;
// 		case Operator::times: {
// 			if (m1->hasOnlyNumbers() && m2->hasOnlyNumbers()) {
// 				const int maxk = m1->columnCount();
// 				if (maxk == m2->rowCount()) {
// 					const int m = m1->rowCount();
// 					const int n = m2->columnCount();
// 					
// 					Matrix *matrix = new Matrix();
// 					
// 					for (int i = 0; i < m; ++i) {
// 						MatrixRow *row = new MatrixRow(n);
// 						for (int j = 0; j < n; ++j) {
// 							std::complex<double> sum = 0;
// 							for (int k = 0; k < maxk; ++k) {
// 								sum += static_cast<const Cn*>(compileValueValue(op, (Cn*)m1->at(i,k)->copy(), (Cn*)m2->at(k,j)->copy(), correct))->complexValue();
// 							}
// 							
// 							if (sum.imag() == 0)
// 								row->appendBranch(new Cn(sum.real()));
// 							else
// 								row->appendBranch(new Cn(sum.real(), sum.imag()));
// 						}
// 						
// 						matrix->appendBranch(row);
// 					}
// 					
// 					ret = matrix;
// 				} else {
// 					correct= QString(QCoreApplication::tr("Multiplication of two matrices is allowed provided that the number of columns of the first matrix equals the number of rows of the second matrix"));
// 					ret=new None();
// 				}
// 			} else {
// 				correct= QString(QCoreApplication::tr("Matrix entries must be numbers"));
// 				ret=new None();
// 			}
// 		}	break;
// 		default:
// 			break;
// 	}
// 	
// 	return ret;
}

llvm::Value* OperationsCompiler::compileValueMatrixOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* v, llvm::Value* m1, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	switch(op) {
// 		case Operator::selector: {
// 			int select=v->intValue();
// 			if(select<1 || (select-1) >= m1->rowCount()) {
// 				correct= QString(QCoreApplication::tr("Invalid index for a container"));
// 				ret=new None();
// 			} else {
// 				MatrixRow* row = static_cast<MatrixRow*>(m1->rows()[select-1]);
// 				Vector* nv = new Vector(row->size());
// 				for(Vector::iterator it=row->begin(); it!=row->end(); ++it) {
// 					nv->appendBranch((*it));
// 					*it = 0;
// 				}
// 				ret = nv;
// 			}
// 			delete v;
// 		}	break;
// 		case Operator::times: {
// 			if (m1->hasOnlyNumbers()) {
// 				Matrix *nm = new Matrix();
// 				for(Matrix::iterator it=m1->begin(); it!=m1->end(); ++it)
// 					nm->appendBranch(static_cast<MatrixRow*>(compileValueVector(op, static_cast<Cn*>(v->copy()), static_cast<MatrixRow*>(*it), correct)));
// 				ret = nm;
// 			} else {
// 				correct= QString(QCoreApplication::tr("Matrix entries must be numbers"));
// 				ret=new None();
// 			}
// 		}	break;
// 		default:
// 			break;
// 	}
// 	return ret;
}

llvm::Value* OperationsCompiler::compileMatrixValueOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* m1, llvm::Value* v, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	switch(op) {
// 		case Operator::power: {
// 			if (matrix->hasOnlyNumbers()) {
// 				if (matrix->isSquare()) {
// 					if (value->isInteger()) {
// 						const int exp = qAbs(value->intValue());
// 						
// 						switch(exp) {
// 							case 0: ret = Matrix::identity(matrix->rowCount()); break;
// 							case 1: ret = matrix; break;
// 							default: {
// 								//base = value->intValue() < 0? invert(matrix) : matrix;//TODO negative exponents
// 								Matrix *base = matrix;
// 								
// 								if (exp <= MAX_EXPONENT) { // then: use Addition-chain exponentiation
// 									const int len = additionChains[exp][0];
// 									int i, j, k;
// 									QVector<Matrix*> products(len+1);
// 									products[0] = base;
// 									
// 									if (exp>1)
// 										products[1] = static_cast<Matrix*>(compileMatrixMatrix(Operator::times, matrix, matrix, correct));
// 									
// 									//NOTE see http://rosettacode.org/wiki/Addition-chain_exponentiation#C for more details
// 									for (i = 2; i <= len; i++) 
// 										for (j = i - 1; j; j--) 
// 											for (k = j; k >= 0; k--) 
// 											{
// 												if (additionChains[exp][k+1] + additionChains[exp][j+1] < additionChains[exp][i+1]) break;
// 												if (additionChains[exp][k+1] + additionChains[exp][j+1] > additionChains[exp][i+1]) continue;
// 												products[i] = static_cast<Matrix*>(compileMatrixMatrix(Operator::times, products[j], products[k], correct));
// 												j = 1;
// 												break;
// 											}
// 									ret = products[len];
// 									
// 									//NOTE free the memory, except products[len]
// 									for (i = 0; i < len; i++)
// 										delete products[i];
// 								} else { // else: use Exponentiation by squaring
// 									Matrix *product = Matrix::identity(base->rowCount());
// 									Matrix *newbase = base->copy();
// 									int n = exp;
// 									
// 									while (n != 0) {
// 										if (n % 2 != 0) {
// 											Matrix *oldproduct = product;
// 											product = static_cast<Matrix*>(compileMatrixMatrix(Operator::times, product, newbase, correct));
// 											delete oldproduct;
// 											--n;
// 										}
// 										Matrix *oldbase = newbase;
// 										newbase = static_cast<Matrix*>(compileMatrixMatrix(Operator::times, newbase, newbase, correct));
// 										delete oldbase;
// 										n /= 2;
// 									}
// 									
// 									ret = product;
// 								}
// 							}	break;
// 						}
// 					} else {
// 						correct= QString(QCoreApplication::tr("The exponent of 'power' must be some integer number"));
// 						ret=new None();
// 					}
// 				} else {
// 					correct= QString(QCoreApplication::tr("Cannot compute 'power' for non square matrix"));
// 					ret=new None();
// 				}
// 			} else {
// 				correct= QString(QCoreApplication::tr("Matrix entries must be numbers"));
// 				ret=new None();
// 			}
// 		}	break;
// 		//TODO root
// 		default:
// 			break;
// 	}
// 	return ret;
}

llvm::Value* OperationsCompiler::compileUnaryMatrixOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* m, QString& error)
{
	return 0; //TODO
// 	Object* ret = 0;
// 	switch(op) {
// 		case Operator::transpose: {
// 			int sizeA = m->rowCount(), sizeB = static_cast<MatrixRow*>(*m->constBegin())->size();
// 			Matrix* mret = new Matrix;
// 			for(int i=0; i<sizeB; ++i) {
// 				MatrixRow* row = new MatrixRow(sizeA);
// 				for(int j=0; j<sizeA; ++j) {
// 					row->appendBranch(m->at(j, i)->copy());
// 				}
// 				mret->appendBranch(row);
// 			}
// 			ret = mret;
// 		}	break;
// 		default:
// 			break;
// 	}
// 	return ret;
}

llvm::Value* OperationsCompiler::compileMatrixNoneOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* m, llvm::Value* cntr, QString& error)
{
	return 0; //TODO
// 	return errorCase(QCoreApplication::tr("Cannot calculate %1 between a matrix and an error type").arg(Operator(op).name()), correct);
}

llvm::Value* OperationsCompiler::compileNoneMatrixOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, llvm::Value* cntr, llvm::Value* m, QString& error)
{
	return 0; //TODO
// 	return compileMatrixNone(op, m, cntr, correct);
}

//TODO we need to choice based on llvm::Valye::type instead of Analitza type 
//this we could avoid use analitza::type1 for a llvm::value that is not (avoid inconsistence)
//currently we¿ll cast all types to double (analitza value)
OperationsCompiler::BinaryOp OperationsCompiler::opsBinary[Object::custom+1][Object::custom+1] = {
	{0,(OperationsCompiler::BinaryOp) compileNoneValueOperation,0,0,0,0,0,0,(OperationsCompiler::BinaryOp) compileNoneMatrixOperation,0,0},
	{(OperationsCompiler::BinaryOp) compileValueNoneOperation, (OperationsCompiler::BinaryOp) compileValueValueOperation, 0, (OperationsCompiler::BinaryOp) compileValueVectorOperation, (OperationsCompiler::BinaryOp) compileValueListOperation,0,0,0,(OperationsCompiler::BinaryOp) compileValueMatrixOperation,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0, (OperationsCompiler::BinaryOp) compileVectorValueOperation, 0, (OperationsCompiler::BinaryOp) compileVectorVectorOperation, 0,0,0,0,(OperationsCompiler::BinaryOp) compileVectorMatrixOperation,0,0},
	{0, 0, 0,0, (OperationsCompiler::BinaryOp) compileListListOperation, 0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{(OperationsCompiler::BinaryOp) compileMatrixNoneOperation, (OperationsCompiler::BinaryOp) compileMatrixValueOperation,0, (OperationsCompiler::BinaryOp) compileMatrixVectorOperation,0,0,0,0, (OperationsCompiler::BinaryOp) compileMatrixMatrixOperation,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,(OperationsCompiler::BinaryOp) compileCustomCustomOperation}
};

llvm::Value * OperationsCompiler::compileBinaryOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, Object::ObjectType type1, Object::ObjectType type2, llvm::Value* val1, llvm::Value* val2, QString& error)
{
	Object::ObjectType finaltype1 = type1;
	
	if (finaltype1 == Object::variable) {
		if (val1->getType()->getTypeID() == llvm::Type::DoubleTyID)
			         finaltype1 = Object::value;
		//TODO complex
	}
	
	Object::ObjectType finaltype2 = type2;
	
	if (finaltype2 == Object::variable) {
		if (val2->getType()->getTypeID() == llvm::Type::DoubleTyID)
			         finaltype2 = Object::value;
		//TODO complex
	}
	
// 	qDebug() << "final types for binary op " << finaltype1 << finaltype2;
	
	BinaryOp f=opsBinary[finaltype1][finaltype2];
	Q_ASSERT(f && "using compile (for binary operator) in a wrong way");
	return f(module, currentBlock ,op, val1, val2, error);
}
//TODO we need to choice based on llvm::Valye::type instead of Analitza type 
//this we could avoid use analitza::type1 for a llvm::value that is not (avoid inconsistence)
//currently we¿ll cast all types to double (analitza value)
OperationsCompiler::UnaryOp OperationsCompiler::opsUnary[] = {
	0,
	(OperationsCompiler::UnaryOp) OperationsCompiler::compileUnaryValueOperation,
	0, //variable
	(OperationsCompiler::UnaryOp) OperationsCompiler::compileUnaryVectorOperation,
	(OperationsCompiler::UnaryOp) OperationsCompiler::compileUnaryListOperation,
	0, //apply
	0, //oper
	0, //container
	(OperationsCompiler::UnaryOp) OperationsCompiler::compileUnaryMatrixOperation
};

llvm::Value * OperationsCompiler::compileUnaryOperation(llvm::Module* module, llvm::BasicBlock* currentBlock, Operator::OperatorType op, Object::ObjectType type, llvm::Value* val, QString& error)
{
	Object::ObjectType finaltype = type;
	
	if (finaltype == Object::variable) {
		if (val->getType()->getTypeID() == llvm::Type::DoubleTyID)
			finaltype = Object::value;
		//TODO complex
	}
	
	UnaryOp f=opsUnary[finaltype];
	
	Q_ASSERT(f && "using compileUnary in a wrong way");
	return f(module, currentBlock, op, val, error);
}

llvm::Value* OperationsCompiler::errorCase(const QString& error, QString& correct)
{
	return 0; //TODO
// 	correct =  QString(error);
// 	return new None;
}
