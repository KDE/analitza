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

#ifndef ANALITZAJIT_JITANALYZER_H
#define ANALITZAJIT_JITANALYZER_H

#include "analitza/analyzer.h"
#include "analitzajitexport.h"

namespace llvm { 
	class Value;
	class Module;
	class ExecutionEngine;
	class Type;
	class Function;
};

namespace Analitza
{

/**
 * \class LLVMAnalyzer
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Calculates lambda expressions using Just-in-time compilation.
 * 
 * This class calculates any math expression using JIT compilation, so every 
 * expression will be compiled into machine code before its execution. 
 * Also, this class will cache every math expression so it will not be necessary 
 * to compile the expression again.
 * 
 * \internal JITAnalyzer manage/keeps one and only one LLVM::Module.
 */

class ANALITZAJIT_EXPORT JITAnalyzer : public Analitza::Analyzer
{
	public:
		JITAnalyzer();
		~JITAnalyzer();
		
		/**
		 * Sets an expression to calculate.
		 * If the expression is a lambda and/or contains variables, then 
		 * for each variable you must specify its type using @p bvartypes
		 * Also, the expression will be cached, so next time you set the same expression 
		 * it will not be necessary to compile it into machine code again.
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
		//bool calculate(double &result);
		//TODO
		//bool calculate(int &result);
		//bool calculate(bool &result);
		//bool calculate(complex &result);
		//bool calculate(vector &result);
		//bool calculate(matrix &result);
		//bool calculate(list &result);
		
		/**
		 * Calculates the lambda expression using JIT compilation and returns a value alone.
		 * The parameters need to be set by passing a stack instance.
		 * If result type is not the same type of the expression or there was some error, 
		 * then nothing will be done and will return false.
		 * @returns Returns true if calculation was successful, otherwise returns false.
		 */
		bool calculateLambda(double &result);
		bool calculateLambda(bool &result);
		bool calculateLambda(QVector<double> &result);
		
		//TODO
		//bool calculateLambda(int &result);
		//bool calculateLambda(complex &result);
		//bool calculateLambda(matrix &result);
		//bool calculateLambda(list &result);
		
	private:
		//TODO pimpl idiom needs to be applied here
		
		llvm::Module *m_module;
		llvm::ExecutionEngine *m_jitengine;
		
		//TODO better cache structure
		struct function_info {
			llvm::Function *ir_function;
			void *jit_function;
			llvm::Type *ir_retty;
			Analitza::ExpressionType native_retty;
		};
		QMap<QString, function_info> m_jitfnscache;
		QString m_currentfnkey;
};

}

#endif // ANALITZAJIT_JITANALYZER_H
