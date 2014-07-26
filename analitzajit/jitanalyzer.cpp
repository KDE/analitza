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

			ExpressionTypeChecker check(variables());
			check.initializeVars(bvartypes);
			
			ExpressionType rett = check.check(expression().lambdaBody());
			//TODO check if rett (return type) has error ret.Errors
			
// 			qDebug() << "FUNT RET TYPE "<< rett.toString();
			
			llvm::Type *rettype = tc.mapExpressionType(rett);
			
			ExpressionCompiler v(expression().tree(), m_module, rettype, paramtyps, variables());
			
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

bool JITAnalyzer::setExpression(const Expression& lambdaExpression)
{
	//TODO check if exp is lambda
	QMap<QString, Analitza::ExpressionType> bvartypes;
	
	foreach(const QString &bvar, lambdaExpression.bvarList()) {
		bvartypes[bvar] = ExpressionType(ExpressionType::Value);
	}
	
	return JITAnalyzer::setExpression(lambdaExpression, bvartypes);
}

void JITAnalyzer::calculateLambda(double &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	//TODO check for non empty m_jitfnscache
	
	std::vector<llvm::Value*> ArgsV;
	for (int i = 0; i < runStack().size(); ++i) {
		ExpressionCompiler vv(runStack().at(i), m_module);
		ArgsV.push_back(vv.result().value<llvm::Value*>());
	}
	
// 	ArgsV[0]->dump();
	
	
	// Look up the name in the global module table.
	llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache.last();
// 	llvm::CallInst *retcall = Builder.CreateCall(CalleeF, ArgsV, "tmpvarfromcall");
	
	std::vector<llvm::GenericValue> abc;
	
	for (int i = 0; i < this->expression().bvarList().size(); ++i) {
		llvm::GenericValue a;
		a.DoubleVal = ((Cn*)(runStack().at(i)))->value();
		abc.push_back(a);
	}
	
	llvm::GenericValue rr = m_jitengine->runFunction(CalleeF, abc);
	
	switch (CalleeF->getReturnType()->getTypeID()) {
		case llvm::Type::DoubleTyID: {
			result = rr.DoubleVal;
		}	break;
	}
}
void JITAnalyzer::calculateLambda(bool &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	//TODO check for non empty m_jitfnscache
	
	std::vector<llvm::Value*> ArgsV;
	for (int i = 0; i < runStack().size(); ++i) {
		ExpressionCompiler vv(runStack().at(i), m_module);
		ArgsV.push_back(vv.result().value<llvm::Value*>());
	}
	
// 	ArgsV[0]->dump();
	
	
	// Look up the name in the global module table.
	llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache.last();
// 	llvm::CallInst *retcall = Builder.CreateCall(CalleeF, ArgsV, "tmpvarfromcall");
	
	std::vector<llvm::GenericValue> abc;
	
	for (int i = 0; i < this->expression().bvarList().size(); ++i) {
		llvm::GenericValue a;
		a.DoubleVal = ((Cn*)(runStack().at(i)))->value();
		abc.push_back(a);
	}
	
	llvm::GenericValue rr = m_jitengine->runFunction(CalleeF, abc);
	
	switch (CalleeF->getReturnType()->getTypeID()) {
		case llvm::Type::IntegerTyID: {
			llvm::IntegerType *inttype = (llvm::IntegerType *)CalleeF->getReturnType();
			
			switch (inttype->getBitWidth()) {
				case 1:  { //for bool expressiontype 
					result = rr.IntVal.getBoolValue();
				}	break;
			}
		}	break;
	}
}
