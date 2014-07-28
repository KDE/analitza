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
	
	if (isCorrect())
		m_currentfnkey.clear();
	
	m_currentfnkey = lambdaExpression.toString();
	
	foreach(const QString &tkey, bvartypes.keys()) {
		m_currentfnkey += "["+tkey+":"+bvartypes[tkey].toString()+"]";
	}
	
	if (expression().isLambda()) {
		if (m_jitfnscache.contains(m_currentfnkey)) {
			return true;
		} else {
			//TODO find better way to save/store IR functions
			QMap<QString, llvm::Type*> paramtyps = TypeCompiler::compileTypes(bvartypes);
			
	// 		qDebug() << "fluuuu" << bvartypes.keys();
			
			ExpressionTypeChecker check(variables());
			check.initializeVars(bvartypes);
			
			ExpressionType rett = check.check(expression().lambdaBody());
			//TODO check if rett (return type) has error ret.Errors
			
// 			qDebug() << "FUNT RET TYPE "<< rett.toString();
			
			llvm::Type *rettype = TypeCompiler::compileType(rett);
			
			ExpressionCompiler v(expression().tree(), m_module, rettype, paramtyps, variables());
			
			//cache
			m_jitfnscache[m_currentfnkey] = v.result().value<llvm::Value*>();
			
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
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		
		std::vector<llvm::Value*> ArgsV;
		for (int i = 0; i < runStack().size(); ++i) {
			ExpressionCompiler vv(runStack().at(i), m_module);
			ArgsV.push_back(vv.result().value<llvm::Value*>());
		}
		
	// 	ArgsV[0]->dump();
		
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache[m_currentfnkey];
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
	} else {
		//TODO add error
	}
}
void JITAnalyzer::calculateLambda(bool &result)
{
	Q_ASSERT(!m_jitfnscache.isEmpty());
	
	if (m_jitfnscache.contains(m_currentfnkey)) {
		//TODO check for non empty m_jitfnscache
		
		std::vector<llvm::Value*> ArgsV;
		for (int i = 0; i < runStack().size(); ++i) {
			ExpressionCompiler vv(runStack().at(i), m_module);
			ArgsV.push_back(vv.result().value<llvm::Value*>());
		}
		
	// 	ArgsV[0]->dump();
		
		
		// Look up the name in the global module table.
		llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache[m_currentfnkey];
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
	} else {
		//TODO add error
	}
}
