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
#include "expressiontypechecker.h"
#include "value.h"

#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Target/TargetJITInfo.h>

using namespace Analitza;

//BEGIN JITAnalyzerPrivate

class JITAnalyzer::JITAnalyzerPrivate
{
public:
	JITAnalyzerPrivate()
		: m_ownContext(true)
		, m_context(new llvm::LLVMContext)
	{
		llvmInit();
		
		//TODO add variables to m_bvartypes
	}
	
	JITAnalyzerPrivate(llvm::LLVMContext *context)
		: m_ownContext(false)
		, m_context(context)
	{
		llvmInit();
		
		//TODO add variables to m_bvartypes
	}
	
	~JITAnalyzerPrivate()
	{
		delete m_jitengine;
		//delete m_module; already deleted by delete m_jitengine
		
		if (m_ownContext)
			delete m_context;
	}
	
	void llvmInit()
	{
		llvm::InitializeNativeTarget();
		m_module = new llvm::Module(generateModuleID(), *m_context);
		m_jitengine = llvm::EngineBuilder(m_module).create();
		m_module ->setDataLayout(m_jitengine->getDataLayout());
		
		setupOptimizationPasses();
	}
	
	void setupOptimizationPasses()
	{
		module_pass_manager = new llvm::legacy::PassManager();
		module_pass_manager->add (llvm::createAlwaysInlinerPass());
		
		pass_manager = new llvm::legacy::FunctionPassManager (m_module);
		pass_manager->add(new llvm::DataLayoutPass(m_module));
		pass_manager->add (llvm::createCFGSimplificationPass());
		pass_manager->add (llvm::createBasicAliasAnalysisPass());
		pass_manager->add (llvm::createAggressiveDCEPass());
		pass_manager->add (llvm::createConstantPropagationPass());
		pass_manager->add (llvm::createPromoteMemoryToRegisterPass());
		pass_manager->add (llvm::createInstructionCombiningPass());
		pass_manager->add (llvm::createInstructionSimplifierPass());
		pass_manager->add (llvm::createReassociatePass());
		pass_manager->add (llvm::createGVNPass());
		pass_manager->add (llvm::createCFGSimplificationPass());
		pass_manager->doInitialization();
	}
	
	bool m_ownContext;
	llvm::LLVMContext *m_context;
	llvm::Module *m_module;
	llvm::ExecutionEngine *m_jitengine;
	
	//optimizations
	llvm::legacy::PassManager *module_pass_manager;
	llvm::legacy::FunctionPassManager *pass_manager;
	
	struct FunctionInfo
	{
		llvm::Function *function;
		void *nativeFunction;
		Analitza::ExpressionType returnType;
		size_t nargs; // cache the number of arguments
	};
	
	QHash<QString, FunctionInfo> m_compilationCache;
	FunctionInfo m_currentFunctionInfo;
	
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
	Analyzer::setExpression(expression);
	
	if (isCorrect()) {
// 		d->m_currentCacheKey.clear();
	} else {
		//TODO make Analyzer::M_erros protected so I ca add errors qDebug() << "ERRORS" << errors();
		return false;
	}
	
	QString cacheKey = d->getCacheKey(expression, bvartypes);
	
	//TODO support not only lambdas
	if (expression.isLambda()) {
		if (d->m_compilationCache.contains(cacheKey)) {
// 			qDebug() << "Avoid compilation to LLVM IR and JIT compilation to native platform code, using cached function:" << d->m_currentCacheKey;
			d->m_currentFunctionInfo = d->m_compilationCache[cacheKey];
			
			return true;
		} else {
			ExpressionCompiler v(d->m_module, variables());
			
			//cache
			//TODO support expression too, not only lambda expression
			d->m_currentFunctionInfo.function = llvm::cast<llvm::Function>(v.compileExpression(expression, bvartypes));
			d->m_currentFunctionInfo.returnType = v.compiledType();
			//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
			d->m_currentFunctionInfo.nativeFunction = d->m_jitengine->getPointerToFunction(d->m_currentFunctionInfo.function);
			d->m_currentFunctionInfo.nargs = d->m_currentFunctionInfo.function->getArgumentList().size();
			
			//BEGIN optimizations
			d->module_pass_manager->run(*d->m_module);
			d->pass_manager->run(*d->m_currentFunctionInfo.function);
// 			d->m_currentFunctionInfo.function->dump();
			//END optimizations
			
			d->m_compilationCache[cacheKey] = d->m_currentFunctionInfo;
			
			return true;
		}
	}
	
	return true;
}

bool JITAnalyzer::setExpression(const Expression& expression)
{
	QMap<QString, Analitza::ExpressionType> bvartypes;
	
	if (expression.isLambda()) {
		foreach(const QString &bvar, expression.bvarList()) {
			bvartypes[bvar] = ExpressionType(ExpressionType::Value);
		}
	}
	
	return JITAnalyzer::setExpression(expression, bvartypes);
}

bool JITAnalyzer::calculateLambda(bool &result)
{
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	Q_ASSERT(d->m_currentFunctionInfo.returnType == ExpressionType(ExpressionType::Bool));
	
	return calculateScalarLambda<bool>(result);
}

bool JITAnalyzer::calculateLambda(double &result)
{
// 	Q_ASSERT(!d->m_compilationCache.isEmpty());
// 	Q_ASSERT(d->m_currentFunctionInfo.returnType == ExpressionType(ExpressionType::Value)); //TODO should be ExpressionType::Real
	
	return calculateScalarLambda<double>(result);
}

bool JITAnalyzer::calculateLambda(QVector< double >& result)
{
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	Q_ASSERT(d->m_currentFunctionInfo.returnType == ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));  //TODO should be ExpressionType::Real
	
	const int n = d->m_currentFunctionInfo.function->getArgumentList().size();
	const int nret = d->m_currentFunctionInfo.returnType.size();
	
	//NOTE llvm::ExecutionEngine::getPointerToFunction performs JIT compilation to native platform code
	void *FPtr = d->m_currentFunctionInfo.nativeFunction;
	
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
	
	return false;
}

bool JITAnalyzer::calculateLambda(QVector< QVector<double> > &result)
{
	Q_ASSERT(!d->m_compilationCache.isEmpty());
	//TODO assert returntype is matrix
	
	const int rows = d->m_currentFunctionInfo.returnType.size();
	const int cols = d->m_currentFunctionInfo.returnType.contained().size();
	
	//TODO check for non empty d->m_compilationCache
	
	const int n = d->m_currentFunctionInfo.function->getArgumentList().size();
	
	void *FPtr = d->m_currentFunctionInfo.nativeFunction;
	
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
	
	//TODO weird ... this d->m_currentFunctionInfoll garbage in vector :S ... e.g. QVector(25, 6.95321e-310) should be QVector(25,35)
// 		for(int i = 0; i < nret; ++i) {
// 			result.append(vret[i]);
// 		}
	//TODO weird behaviour: I can not use loops here ... try next time with other compiler/llvm version
	
	if (rows == 1 && cols == 1) {
		const double a = vret[0];
		QVector<double> row1;
		row1.append(a);
		
		result.append(row1);
		return true;
	}
	
	if (rows == 2 && cols == 2) {
		const double a = vret[0];
		const double b = vret[1];
		const double c = vret[2];
		const double d = vret[3];
		
		QVector<double> row1;
		row1.append(a);
		row1.append(b);
		
		QVector<double> row2;
		row2.append(c);
		row2.append(d);
		
		result.append(row1);
		result.append(row2);
		
		return true;
	}
	
	if (rows == 3 && cols == 5) {
		const double a = vret[0];
		const double b = vret[1];
		const double c = vret[2];
		const double d = vret[3];
		const double e = vret[4];
		const double f = vret[5];
		const double g = vret[6];
		const double h = vret[7];
		const double i = vret[8];
		const double j = vret[9];
		const double k = vret[10];
		const double l = vret[11];
		const double m = vret[12];
		const double n = vret[13];
		const double o = vret[14];
		
		QVector<double> row1;
		row1.append(a);
		row1.append(b);
		row1.append(c);
		row1.append(d);
		row1.append(e);
		
		QVector<double> row2;
		row2.append(f);
		row2.append(g);
		row2.append(h);
		row2.append(i);
		row2.append(j);
		
		QVector<double> row3;
		row3.append(k);
		row3.append(l);
		row3.append(m);
		row3.append(n);
		row3.append(o);
		
		result.append(row1);
		result.append(row2);
		result.append(row3);
		//qDebug() << "GINALLL: " << result;
		
		return true;
	}
	
	return false;
}

template<typename T>
bool JITAnalyzer::calculateScalarLambda(T& result)
{
	const void *FPtr = d->m_currentFunctionInfo.nativeFunction;
	
	switch (d->m_currentFunctionInfo.nargs) {
		case 1: {
			typedef T (*FTY)(double);
			const FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
			
			const double arg1 = ((Cn*)(runStack().at(0)))->value();
			result = FP(arg1);
			return true;
		}	break;
		case 2: {
			typedef T (*FTY)(double, double);
			const FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
			
			const double arg1 = ((Cn*)(runStack().at(0)))->value();
			const double arg2 = ((Cn*)(runStack().at(1)))->value();
			result = FP(arg1, arg2);
			return true;
		}	break;
		case 3: {
			typedef T (*FTY)(double, double, double);
			const FTY FP = reinterpret_cast<FTY>((intptr_t)FPtr);
			
			const double arg1 = ((Cn*)(runStack().at(0)))->value();
			const double arg2 = ((Cn*)(runStack().at(1)))->value();
			const double arg3 = ((Cn*)(runStack().at(1)))->value();
			result = FP(arg1, arg2, arg3);
			return true;
		}	break;
		//TODO more args
		default: {
			return false;
		}
	}
	
	return false;
}
