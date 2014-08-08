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

namespace llvm
{
class LLVMContext;
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
		/**
		 * Constructor. Creates an empty JITAnalyzer module that manages its own 
		 * Variables and its own LLVMContext.
		 * Use this constructor if you do require isolation of LLVM, i.e. when 
		 * you will use JITAnalyzer instances concurrently.
		 */
		JITAnalyzer();
		
		/**
		 * Constructor. Creates an empty JITAnalyzer module. 
		 * @param v: Sets a custom variables module. This module will _not_ be deleted along with JITAnalyzer.
		 * Whit this constructor JITAnalyzer manages its own LLVMContext.
		 * Use this constructor if you do require isolation of LLVM, i.e. when 
		 * you will use JITAnalyzer concurrently.
		 */ 
		JITAnalyzer(Variables* v);
		
		/**
		 * Constructor. Creates an empty JITAnalyzer module. 
		 * @param c: Sets an extern LLVMContext. This context will _not_ be deleted along with JITAnalyzer.
		 */ 
		JITAnalyzer(llvm::LLVMContext *c);
		
		/**
		 * Constructor. Creates an empty JITAnalyzer module. 
		 * @param v: Sets a custom variables module. This module will _not_ be deleted along with JITAnalyzer.
		 * @param c: Sets a custom LLVMContext reference. This context will _not_ be deleted along with JITAnalyzer.
		 */ 
		JITAnalyzer(Variables* v, llvm::LLVMContext *c);
		
		//TODO
// 		/**
// 		 * Copy constructor. Creates a copy of the @p other JITAnalyzer instance.
// 		 * Inherits its Variable and LLVMContext structures. */
// 		JITAnalyzer(const JITAnalyzer& other);
		
		/**
		 * Destructor.
		 */
		virtual ~JITAnalyzer();
		
		llvm::LLVMContext *context() const;
		
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
		
		/** Convenience method used when the @p result expression is a vector. */
		bool calculateLambda(QVector<double> &result);
		
		/** Convenience method used when the @p result expression is a matrix. */
		bool calculateLambda(QVector< QVector<double> > &result);
		
		//TODO
		//bool calculateLambda(int &result);
		//bool calculateLambda(complex &result);
		//bool calculateLambda(list &result);
		
	private:
		class JITAnalyzerPrivate;
		JITAnalyzerPrivate* const d;
};

}

#endif // ANALITZAJIT_JITANALYZER_H
