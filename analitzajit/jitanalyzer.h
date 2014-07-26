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

extern Analitza::Analyzer a;
namespace llvm { 
	class Value; 
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
 * \brief Calculates lambda expressions using Just-in-time compilation.
 * \internal JitAnalyzer manage/keeps a one and only one LLVM::Module.
 * 
 * This class calculates any math expression using JIT compilation, so every 
 * expression will be compiled into machine code before its execution. 
 * Also, this class will cache every math expression so it will not be necessary 
 * to compile the expression again.
 */

class ANALITZAJIT_EXPORT JitAnalyzer : public Analitza::Analyzer
{
	public:
		JitAnalyzer();
		~JitAnalyzer();
		
		/**
		 * Sets an expression to calculate.
		 * If the expression is a lambda and/or contains variables, then 
		 * for each variable you must specify its type.
		 * @returns Returns true if expression is ready to be calculated using JIT compilation.
		 */ 
		bool setExpression(const Analitza::Expression& expression, const QMap< QString, Analitza::ExpressionType >& bvartypes);
		
		/**
		 * Convenience method where all variables types of the expression 
		 * are set to be floating point types (real numbers)
		 */
		bool setExpression(const Analitza::Expression &expression);
		
		/**
		 * TODO
		 * Calculates the expression using JIT compilation and returns a value alone.
		 * If result type is not the same type of the expression, then nothing will be done.
		 */
		//void calculate(double &result);
		//TODO
		//void calculate(int &result);
		//void calculate(bool &result);
		//void calculate(complex &result);
		//void calculate(vector &result);
		//void calculate(matrix &result);
		//void calculate(list &result);
		
		/**
		 * Calculates the lambda expression using JIT compilation and returns a value alone.
		 * The parameters need to be set by passing a stack instance.
		 * If result type is not the same type of the expression, then nothing will be done.
		 */
		void calculateLambda(double &result);
		
		//TODO
		//void calculateLambda(int &result);
		//void calculateLambda(bool &result);
		//void calculateLambda(complex &result);
		//void calculateLambda(vector &result);
		//void calculateLambda(matrix &result);
		//void calculateLambda(list &result);
		
	private:
		//TODO pimpl idiom needs to be applied here
		
		llvm::Module *m_module;
		llvm::ExecutionEngine *m_jitengine;
		
		//TODO better cache structure
		QMap<QString, llvm::Value*> m_jitfnscache;
};

}
#endif
