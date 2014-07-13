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

#include "valuecompiler.h"
#include "value.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_os_ostream.h>

static llvm::IRBuilder<> Builder(llvm::getGlobalContext());

Q_DECLARE_METATYPE(llvm::Value*); //TODO see if this goes into visitor header
Q_DECLARE_METATYPE(llvm::Function*);

using namespace Analitza;

JitAnalyzer::JitAnalyzer()
{
m_mod = new llvm::Module("mumod", llvm::getGlobalContext());
}

JitAnalyzer::~JitAnalyzer()
{
delete m_mod;
}


llvm::Value* JitAnalyzer::foojiteval()
{
	if (expression().isLambda() && isCorrect()) {
		//TODO find better way to save/store IR functions
		TypeCompiler v(expression().tree(), m_mod,runStack(), variables());
		
		std::string str;
		llvm::raw_string_ostream stringwriter(str);
		v.result().value<llvm::Value*>()->print(stringwriter);
		
// 		qDebug() << QString::fromStdString(str);
		
		if (m_jitfnscache.contains(str)) { //call jit
			// Look up the name in the global module table.
			llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache[str];
			
			std::vector<llvm::Value*> ArgsV;
			for (int i = 0; i < runStack().size(); ++i) {
				TypeCompiler vv(runStack().at(i), m_mod, runStack(), variables());
				ArgsV.push_back(vv.result().value<llvm::Value*>());
			}
			
			return Builder.CreateCall(CalleeF, ArgsV, "tmpvarfromcall");
		} else {
			m_jitfnscache[str] = v.result().value<llvm::Value*>();
			
			// Look up the name in the global module table.
			llvm::Function *CalleeF = (llvm::Function*)m_jitfnscache[str];
			
			std::vector<llvm::Value*> ArgsV;
			for (int i = 0; i < runStack().size(); ++i) {
				TypeCompiler vv(runStack().at(i), m_mod, runStack(), variables());
				ArgsV.push_back(vv.result().value<llvm::Value*>());
			}
			
			return Builder.CreateCall(CalleeF, ArgsV, "tmpvarfromcall");
		}
		
		return m_jitfnscache[str]; //TODO
	}
	
	return 0; //TODO
}
