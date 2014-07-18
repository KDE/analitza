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
#include "valuecompiler.h"
#include "value.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_os_ostream.h>

static llvm::IRBuilder<> Builder(llvm::getGlobalContext());

Q_DECLARE_METATYPE(llvm::Value*); //TODO see if this goes into visitor header
Q_DECLARE_METATYPE(llvm::Function*);

using namespace Analitza;

JitAnalyzer::JitAnalyzer()
{
	llvm::InitializeNativeTarget();
m_mod = new llvm::Module("mumod", llvm::getGlobalContext());
TheExecutionEngine = llvm::EngineBuilder(m_mod).create();
// 	qDebug() << "fluuuu" << TheExecutionEngine;
}

JitAnalyzer::~JitAnalyzer()
{
delete m_mod;
}

bool JitAnalyzer::setLambdaExpression(const Expression& lambdaExpression, const QMap< QString, Analitza::ExpressionType >& bvartypes)
{
	//TODO better code
	setExpression(lambdaExpression);
	const QString &fnkey = lambdaExpression.toString();
	
	if (expression().isLambda() && isCorrect()) {
		if (m_jitfnscache.contains(fnkey)) {
			return true;
		} else {
			//TODO find better way to save/store IR functions
			TypeCompiler tc;
			QMap<QString, llvm::Type*> paramtyps;
			
	// 		qDebug() << "fluuuu" << bvartypes.keys();
			
			foreach (const QString &str, bvartypes.keys()) {
				paramtyps[str] = tc.mapExpressionType(bvartypes[str]);
			}
			
			ValueCompiler v(expression().tree(), m_mod, paramtyps, variables());
			
	// 		std::string str;
	// 		llvm::raw_string_ostream stringwriter(str);
	// 		v.result().value<llvm::Value*>()->print(stringwriter);
			
	// 		qDebug() << QString::fromStdString(str);
			m_jitfnscache[fnkey] = v.result().value<llvm::Value*>();
			
			return true;
		}
	}
	
	return false;
}

bool JitAnalyzer::setLambdaExpression(const Expression& lambdaExpression)
{
	//TODO check if exp is lambda
	QMap<QString, Analitza::ExpressionType> bvartypes;
	
	foreach(const QString &bvar, lambdaExpression.bvarList()) {
		bvartypes[bvar] = ExpressionType(ExpressionType::Value);
	}
	
	return setLambdaExpression(lambdaExpression, bvartypes);
}

llvm::Value* JitAnalyzer::foojiteval()
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	//TODO check for non empty m_jitfnscache
	
	std::vector<llvm::Value*> ArgsV;
	for (int i = 0; i < runStack().size(); ++i) {
		ValueCompiler vv(runStack().at(i), m_mod);
		ArgsV.push_back(vv.result().value<llvm::Value*>());
	}
	
// 	ArgsV[0]->dump();
	
	
	// Look up the name in the global module table.
	llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache.last();
// 	llvm::CallInst *retcall = Builder.CreateCall(CalleeF, ArgsV, "tmpvarfromcall");
	
	std::vector<llvm::GenericValue> abc;
	llvm::GenericValue a;
	a.DoubleVal = 34.0;
	abc.push_back(a);
	
	

	
	llvm::GenericValue rr = TheExecutionEngine->runFunction(CalleeF, abc);
	
	qDebug() << "fluuuu" << rr.DoubleVal;
	
	
	
// 	void *FPtr = TheExecutionEngine->getPointerToFunction(CalleeF);
// 	double (*FP)(double) = (double (*)(double))(intptr_t)FPtr;
// 	qDebug() << "fluuuu" << FP(12);
	
	return CalleeF;
}
