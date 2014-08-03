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

#include "jitanalyzer.h"

#include "typecompiler.h"
#include "expressioncompiler.h"
#include "value.h"
#include <expressiontypechecker.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm-3.5/llvm/IR/InstrTypes.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

Q_DECLARE_METATYPE(llvm::Value*);

using namespace Analitza;

JITAnalyzer::JITAnalyzer()
{
	llvm::InitializeNativeTarget();
	
	m_module = new llvm::Module("mumod", llvm::getGlobalContext());
	m_jitengine = llvm::EngineBuilder(m_module).create();
// 	std::string ErrStr;
// 	m_jitengine = llvm::EngineBuilder(m_module).setErrorStr(&ErrStr).setMCPU("amd64").create();
}

JITAnalyzer::~JITAnalyzer()
{
	m_jitengine->removeModule(m_module);
	
	delete m_module;
	delete m_jitengine;
}

bool JITAnalyzer::setExpression(const Expression& lambdaExpression, const QMap< QString, Analitza::ExpressionType >& bvartypes)
{
	//TODO better code
	Analyzer::setExpression(lambdaExpression);
	
	if (isCorrect())
		m_currentfnkey.clear();
	else {
		qDebug() << "ERRORS" << errors();
		return false;
	}
	m_currentfnkey = lambdaExpression.toString();
	
	foreach(const QString &tkey, bvartypes.keys()) {
		m_currentfnkey += "["+tkey+":"+bvartypes[tkey].toString()+"]";
	}
	
	if (expression().isLambda()) {
		if (m_jitfnscache.contains(m_currentfnkey)) {
			
// 			qDebug() << "Avoid compilation to LLVM IR, using cached function:" << m_currentfnkey;
			return true;
		} else {
			//TODO find better way to save/store IR functions
	// 		qDebug() << "fluuuu" << bvartypes.keys();
			
			
			ExpressionCompiler v(m_module, variables());
			
			//cache
			function_info fn;
			//TODO support expression too, not only lambda expression
			fn.ir_function = llvm::cast<llvm::Function>(v.compileExpression(expression(), bvartypes));
			fn.ir_retty = v.compiledType().first;
			fn.native_retty = v.compiledType().second;
			fn.jit_function = 0; //NOTE this is important
			
			m_jitfnscache[m_currentfnkey] = fn;
			
// 			m_jitfnscache[m_currentfnkey]->dump();
			
			return true;
		}
	}
	
	return true;
}

bool JITAnalyzer::setExpression(const Expression& lambdaExpression)
{
	//TODO check if exp is lambda
	QMap<QString, Analitza::ExpressionType> bvartypes;
	
	if (lambdaExpression.isLambda()) {
		foreach(const QString &bvar, lambdaExpression.bvarList()) {
			bvartypes[bvar] = ExpressionType(ExpressionType::Value);
		}
	}
	
	return JITAnalyzer::setExpression(lambdaExpression, bvartypes);
}

bool JITAnalyzer::calculateLambda(double &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		const int n = this->expression().bvarList().size();
		const int nret = m_jitfnscache[m_currentfnkey].native_retty.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = m_jitfnscache[m_currentfnkey].ir_function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(m_jitfnscache[m_currentfnkey].native_retty.type() == ExpressionType::Value);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (m_jitfnscache[m_currentfnkey].jit_function) {
			FPtr = m_jitfnscache[m_currentfnkey].jit_function;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << m_currentfnkey;
		} else {
			FPtr = m_jitengine->getPointerToFunction(CalleeF);
			m_jitfnscache[m_currentfnkey].jit_function = FPtr;
		}
		
		switch (n) {
			case 1: {
				typedef double (*FTY)(double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				result = FP(arg1);
				return true;
			}	break;
			case 2: {
				typedef double (*FTY)(double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				result = FP(arg1, arg2);
				return true;
			}	break;
			case 3: {
				typedef double (*FTY)(double, double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				double arg3 = ((Cn*)(runStack().at(1)))->value();
				result = FP(arg1, arg2, arg3);
				return true;
			}	break;
			//TODO more args
			default: {
				return false;
			}
		}
	} else {
		//TODO add error
		return false;
	}
	
	return true;
}

bool JITAnalyzer::calculateLambda(bool &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		const int n = this->expression().bvarList().size();
		const int nret = m_jitfnscache[m_currentfnkey].native_retty.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = m_jitfnscache[m_currentfnkey].ir_function;
		
		//TODO
// 		Q_ASSERT(m_jitfnscache[m_currentfnkey].native_retty.type() == ExpressionType::Value);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (m_jitfnscache[m_currentfnkey].jit_function) {
			FPtr = m_jitfnscache[m_currentfnkey].jit_function;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << m_currentfnkey;
		} else {
			FPtr = m_jitengine->getPointerToFunction(CalleeF);
			m_jitfnscache[m_currentfnkey].jit_function = FPtr;
		}
		
		switch (n) {
			case 1: {
				typedef bool (*FTY)(double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				result = FP(arg1);
				return true;
			}	break;
			case 2: {
				typedef bool (*FTY)(double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				result = FP(arg1, arg2);
				return true;
			}	break;
			case 3: {
				typedef bool (*FTY)(double, double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				double arg3 = ((Cn*)(runStack().at(1)))->value();
				result = FP(arg1, arg2, arg3);
				return true;
			}	break;
			//TODO more args
			default: {
				return false;
			}
		}
	} else {
		//TODO add error
		return false;
	}
	
	return true;
}

bool JITAnalyzer::calculateLambda(QVector< double >& result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		
		const int n = this->expression().bvarList().size();
		const int nret = m_jitfnscache[m_currentfnkey].native_retty.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = m_jitfnscache[m_currentfnkey].ir_function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(m_jitfnscache[m_currentfnkey].native_retty.type() == ExpressionType::Vector);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (m_jitfnscache[m_currentfnkey].jit_function) {
			FPtr = m_jitfnscache[m_currentfnkey].jit_function;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << m_currentfnkey;
		} else {
			FPtr = m_jitengine->getPointerToFunction(CalleeF);
			m_jitfnscache[m_currentfnkey].jit_function = FPtr;
		}
		
		double *vret = 0;
		
		switch (n) {
			case 1: {
				typedef double* (*FTY)(double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				vret = FP(arg1);
				
			}	break;
			case 2: {
				typedef double* (*FTY)(double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				vret = FP(arg1, arg2);
			}	break;
			case 3: {
				typedef double* (*FTY)(double, double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				double arg3 = ((Cn*)(runStack().at(1)))->value();
				vret = FP(arg1, arg2, arg3);
			}	break;
			//TODO more args
			default: {
			}
		}
		
		//TODO weird ... this fill garbage in vector :S ... e.g. QVector(25, 6.95321e-310) should be QVector(25,35)
// 		for(int i = 0; i < nret; ++i) {
// 			result.append(vret[i]);
// 		}
		//TODO weird behaviour: I can not use loops here ... try next time with other compiler/llvm version
		switch (nret) {
			case 1: {
				const double a = vret[0];
				result.append(a);
				return true;
			}	break;
			case 2: {
				const double a = vret[0];
				const double b = vret[1];
				result.append(a);
				result.append(b);
				
// 				qDebug() << "GINALLL: " << result;
				
				return true;
			}	break;
			case 3: {
				const double a = vret[0];
				const double b = vret[1];
				const double c = vret[2];
				result.append(a);
				result.append(b);
				result.append(c);
				
// 				qDebug() << "GINALLL: " << result;
				
				return true;
			}	break;
			//TODO vector should be of any size ...
			default: {
				return false;
			}
		}
	} else {
		//TODO add error
		return false;
	}
	
	return true;
}

bool JITAnalyzer::calculateLambda(QVector< QVector<double> > &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		
		const int n = this->expression().bvarList().size();
		const int nret = m_jitfnscache[m_currentfnkey].native_retty.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = m_jitfnscache[m_currentfnkey].ir_function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(m_jitfnscache[m_currentfnkey].native_retty.type() == ExpressionType::Vector);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (m_jitfnscache[m_currentfnkey].jit_function) {
			FPtr = m_jitfnscache[m_currentfnkey].jit_function;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << m_currentfnkey;
		} else {
			FPtr = m_jitengine->getPointerToFunction(CalleeF);
			m_jitfnscache[m_currentfnkey].jit_function = FPtr;
		}
		
		double *vret = 0;
		
		switch (n) {
			case 1: {
				typedef double* (*FTY)(double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				vret = FP(arg1);
				
			}	break;
			case 2: {
				typedef double* (*FTY)(double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				vret = FP(arg1, arg2);
			}	break;
			case 3: {
				typedef double* (*FTY)(double, double, double);
				FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
				
				double arg1 = ((Cn*)(runStack().at(0)))->value();
				double arg2 = ((Cn*)(runStack().at(1)))->value();
				double arg3 = ((Cn*)(runStack().at(1)))->value();
				vret = FP(arg1, arg2, arg3);
			}	break;
			//TODO more args
			default: {
			}
		}
		
		//TODO weird ... this fill garbage in vector :S ... e.g. QVector(25, 6.95321e-310) should be QVector(25,35)
// 		for(int i = 0; i < nret; ++i) {
// 			result.append(vret[i]);
// 		}
		//TODO weird behaviour: I can not use loops here ... try next time with other compiler/llvm version
// 		switch (nret) {
// 			case 1: {
// 				const double a = vret[0];
// 				result.append(a);
// 				return true;
// 			}	break;
// 			case 2: {
// 				const double a = vret[0];
// 				const double b = vret[1];
// 				result.append(a);
// 				result.append(b);
// 				
// // 				qDebug() << "GINALLL: " << result;
// 				
// 				return true;
// 			}	break;
// 			case 3: {
// 				const double a = vret[0];
// 				const double b = vret[1];
// 				const double c = vret[2];
// 				result.append(a);
// 				result.append(b);
// 				result.append(c);
// 				
// // 				qDebug() << "GINALLL: " << result;
// 				
// 				return true;
// 			}	break;
// 			//TODO vector should be of any size ...
// 			default: {
// 				return false;
// 			}
// 		}
	} else {
		//TODO add error
		return false;
	}
	
	return true;
}

