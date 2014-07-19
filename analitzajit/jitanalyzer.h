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

#ifndef JITANALYZER_H
#define JITANALYZER_H

#include "analitza/analyzer.h"
#include "analitzajitexport.h"

namespace llvm { 
	class Value; 
	class Function;
	class Module;
	class ExecutionEngine;
};

namespace Analitza
{

/**
 * \class LLVMAnalyzer
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Evaluates math expressions using JIT.
 */

class ANALITZAJIT_EXPORT JitAnalyzer : public Analitza::Analyzer
{
//TODO
public:
    JitAnalyzer();
	~JitAnalyzer();
	llvm::Value *foojiteval();
	
	//TODO need to return a C++ type, analitzatype or llvm::type (like genericvalue)?
	
	//TODO WE NEED TO PASS RETURN TYPE TOO
	//TODO better params names
	bool setLambdaExpression(const Analitza::Expression &lambdaExpression, const QMap<QString, Analitza::ExpressionType> &bvartypes);
	
	//convenience method where all bvars are double
	bool setLambdaExpression(const Analitza::Expression &lambdaExpression);
	
private:
	QMap<QString, llvm::Value*> m_jitfnscache;
	llvm::Module *m_mod;
	llvm::ExecutionEngine *TheExecutionEngine;
};

}
#endif
