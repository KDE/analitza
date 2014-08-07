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

//BEGIN JITAnalyzerPrivate

class JITAnalyzer::JITAnalyzerPrivate
{
public:
	JITAnalyzerPrivate()
		: m_ownContext(true)
		, m_context(new llvm::LLVMContext)
	{
		llvm::InitializeNativeTarget();
		m_module = new llvm::Module(generateModuleID(), *m_context);
		m_jitengine = llvm::EngineBuilder(m_module).create();

	}
	
	JITAnalyzerPrivate(llvm::LLVMContext *context)
		: m_ownContext(false)
		, m_context(context)

	{
		llvm::InitializeNativeTarget();
		m_module = new llvm::Module(generateModuleID(), *m_context);
		m_jitengine = llvm::EngineBuilder(m_module).create();
	}
	
	~JITAnalyzerPrivate()
	{
		delete m_jitengine;
		//delete m_module; already deleted by delete m_jitengine
		
		if (m_ownContext)
			delete m_context;
	}
	
	bool m_ownContext;
	llvm::LLVMContext *m_context;
	llvm::Module *m_module;
	llvm::ExecutionEngine *m_jitengine;
	
	struct FunctionInfo
	{
		llvm::Function *function;
		void *nativeFunction;
		Analitza::ExpressionType returnType;
	};
	
	QHash<QString, FunctionInfo> m_compilationCache;
	QString m_currentCacheKey;
	
	const QString getCacheKey(const Expression& expression, const QMap< QString, Analitza::ExpressionType >& bvartypes)
	{
		QString ret = expression.toString();
		
		foreach(const QString &tkey, bvartypes.keys()) {
			ret += "["+tkey+":"+bvartypes[tkey].toString()+"]";
		}
		
		return ret;
	}
	
private:
	static const QString baseModuleID;
	
	static llvm::StringRef generateModuleID()
	{
		static int idSequence = 0;
		return llvm::StringRef(QString(baseModuleID+QString::number(++idSequence)).toStdString());
	}
};

const QString JITAnalyzer::JITAnalyzerPrivate::baseModuleID = QString("JITAnalyzerLLVMModule");

//END JITAnalyzerPrivate

JITAnalyzer::JITAnalyzer()
	: Analyzer()
	, d(new JITAnalyzerPrivate)
{
}


JITAnalyzer::JITAnalyzer(Variables* v)
	: Analyzer(v)
	, d(new JITAnalyzerPrivate)
{
}

JITAnalyzer::JITAnalyzer(llvm::LLVMContext* c)
	: Analyzer()
	, d(new JITAnalyzerPrivate(c))
{
}

JITAnalyzer::JITAnalyzer(Variables* v, llvm::LLVMContext* c)
	: Analyzer(v)
	, d(new JITAnalyzerPrivate(c))
{
}

//TODO
// JITAnalyzer::JITAnalyzer(const Analitza::JITAnalyzer&): Analyzer(v)
// {
// 
// }

JITAnalyzer::~JITAnalyzer()
{
	delete d;
}

llvm::LLVMContext* JITAnalyzer::context() const
{
	return d->m_context;
}

bool JITAnalyzer::setExpression(const Expression& expression, const QMap< QString, Analitza::ExpressionType >& bvartypes)
{
	//TODO better code
	Analyzer::setExpression(expression);
	
	if (isCorrect())
		d->m_currentCacheKey.clear();
	else {
		qDebug() << "ERRORS" << errors();
		return false;
	}
	
	d->m_currentCacheKey = d->getCacheKey(expression, bvartypes);
	
	if (expression.isLambda()) {
		if (d->m_compilationCache.contains(d->m_currentCacheKey)) {
			
// 			qDebug() << "Avoid compilation to LLVM IR, using cached function:" << d->m_currentCacheKey;
			return true;
		} else {
			//TODO find better way to save/store IR functions
	// 		qDebug() << "fluuuu" << bvartypes.keys();
			
			
			ExpressionCompiler v(d->m_module, variables());
			
			//cache
			JITAnalyzerPrivate::FunctionInfo fn;
			//TODO support expression too, not only lambda expression
			fn.function = llvm::cast<llvm::Function>(v.compileExpression(expression, bvartypes));
			fn.returnType = v.compiledType();
			fn.nativeFunction = 0; //NOTE this is important
			
			d->m_compilationCache[d->m_currentCacheKey] = fn;
			
// 			d->m_compilationCache[d->m_currentCacheKey]->dump();
			
			return true;
		}
	}
	
	return true;
}

bool JITAnalyzer::setExpression(const Expression& expression)
{
	//TODO check if exp is lambda
	QMap<QString, Analitza::ExpressionType> bvartypes;
	
	if (expression.isLambda()) {
		foreach(const QString &bvar, expression.bvarList()) {
			bvartypes[bvar] = ExpressionType(ExpressionType::Value);
		}
	}
	
	return JITAnalyzer::setExpression(expression, bvartypes);
}

bool JITAnalyzer::calculateLambda(double &result)
{
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	
	if (d->m_compilationCache.contains(d->m_currentCacheKey)) {
		//TODO check for non empty d->m_compilationCache
		const int n = this->expression().bvarList().size();
		const int nret = d->m_compilationCache[d->m_currentCacheKey].returnType.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = d->m_compilationCache[d->m_currentCacheKey].function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(d->m_compilationCache[d->m_currentCacheKey].returnType.type() == ExpressionType::Value);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (d->m_compilationCache[d->m_currentCacheKey].nativeFunction) {
			FPtr = d->m_compilationCache[d->m_currentCacheKey].nativeFunction;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << d->m_currentCacheKey;
		} else {
			FPtr = d->m_jitengine->getPointerToFunction(CalleeF);
			d->m_compilationCache[d->m_currentCacheKey].nativeFunction = FPtr;
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
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	
	if (d->m_compilationCache.contains(d->m_currentCacheKey)) {
		//TODO check for non empty d->m_compilationCache
		const int n = this->expression().bvarList().size();
		const int nret = d->m_compilationCache[d->m_currentCacheKey].returnType.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = d->m_compilationCache[d->m_currentCacheKey].function;
		
		//TODO
// 		Q_ASSERT(d->m_compilationCache[d->m_currentCacheKey].returnType.type() == ExpressionType::Value);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (d->m_compilationCache[d->m_currentCacheKey].nativeFunction) {
			FPtr = d->m_compilationCache[d->m_currentCacheKey].nativeFunction;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << d->m_currentCacheKey;
		} else {
			FPtr = d->m_jitengine->getPointerToFunction(CalleeF);
			d->m_compilationCache[d->m_currentCacheKey].nativeFunction = FPtr;
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
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	
	if (d->m_compilationCache.contains(d->m_currentCacheKey)) {
		//TODO check for non empty d->m_compilationCache
		
		const int n = this->expression().bvarList().size();
		const int nret = d->m_compilationCache[d->m_currentCacheKey].returnType.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = d->m_compilationCache[d->m_currentCacheKey].function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(d->m_compilationCache[d->m_currentCacheKey].returnType.type() == ExpressionType::Vector);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (d->m_compilationCache[d->m_currentCacheKey].nativeFunction) {
			FPtr = d->m_compilationCache[d->m_currentCacheKey].nativeFunction;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << d->m_currentCacheKey;
		} else {
			FPtr = d->m_jitengine->getPointerToFunction(CalleeF);
			d->m_compilationCache[d->m_currentCacheKey].nativeFunction = FPtr;
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
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	
	if (d->m_compilationCache.contains(d->m_currentCacheKey)) {
		//TODO check for non empty d->m_compilationCache
		
		const int n = this->expression().bvarList().size();
		const int nret = d->m_compilationCache[d->m_currentCacheKey].returnType.size();
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = d->m_compilationCache[d->m_currentCacheKey].function;
		
// 		CalleeF->dump();
		
		//TODO
// 		Q_ASSERT(d->m_compilationCache[d->m_currentCacheKey].returnType.type() == ExpressionType::Vector);
		
		//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
		void *FPtr = 0;
		if (d->m_compilationCache[d->m_currentCacheKey].nativeFunction) {
			FPtr = d->m_compilationCache[d->m_currentCacheKey].nativeFunction;
// 			qDebug() << "Avoid JIT compilation to native platform code, using cached function:" << d->m_currentCacheKey;
		} else {
			FPtr = d->m_jitengine->getPointerToFunction(CalleeF);
			d->m_compilationCache[d->m_currentCacheKey].nativeFunction = FPtr;
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

