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

#include "blockmatrixcommands.h"

#include <QCoreApplication>

#include "analitzautils.h"
#include "expression.h"
#include "value.h"
#include "matrix.h"
#include "list.h"
#include "container.h"
#include "operations.h"

using Analitza::Expression;
using Analitza::ExpressionType;

//BEGIN type utils

typedef QList<ExpressionType> ExpressionTypeList;

static const ExpressionType ValueType = ExpressionType(ExpressionType::Value);
static const ExpressionType VectorType = ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1);
static const ExpressionType MatrixType = ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1);
static const ExpressionTypeList VectorAndMatrixTypes = ExpressionTypeList() << VectorType << MatrixType;
static const ExpressionType VectorAndMatrixAlternatives = ExpressionType(ExpressionType::Many, VectorAndMatrixTypes);
static const ExpressionType IndefiniteArityType = ExpressionType(ExpressionType::Any);

static const ExpressionType functionType(const ExpressionTypeList &from, const ExpressionType &to)
{
	ExpressionType ret(ExpressionType::Lambda);
	
	foreach (const ExpressionType &type, from)
		ret.addParameter(type);
	
	ret.addParameter(to);
	
	return ret;
}

static const ExpressionType functionType(const ExpressionType &from, const ExpressionType &to)
{
	return functionType(ExpressionTypeList() << from, to);
}

static const ExpressionType variadicFunctionType(const ExpressionType &to)
{
	return functionType(IndefiniteArityType, to);
}

//END type utils

static const QString MATRIX_SIZE_ERROR_MESSAGE = QCoreApplication::tr("Matrix dimensions must be greater than zero");

// const QString VectorCommand::id = QString("range");
// // const ExpressionType VectorCommand::type = functionType(ExpressionTypeList() << ValueType << ValueType, VectorType);
// 
// Expression RangeCommand::operator()(const QList< Analitza::Expression >& args)
// {
// 
// }



//BEGIN FillMatrixConstructor

const QString BlockMatrixCommand::id = QString("blockmatrix");
const ExpressionType BlockMatrixCommand::type = variadicFunctionType(MatrixType);
//TODO better error messages
Expression BlockMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const int nargs = args.size();
	
		switch(nargs) {
		case 0: {
			ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(BlockMatrixCommand::id));
			
			return ret;
		}	break;
		}
	//BEGIN commom
	const Analitza::Object::ObjectType firstArgType = args.first().tree()->type();
	
	if (firstArgType == Analitza::Object::vector || firstArgType == Analitza::Object::matrixrow) {
		const bool isVector = (firstArgType == Analitza::Object::vector);
		const Analitza::Vector *firstVector = static_cast<const Analitza::Vector*>(args.first().tree());
		
		if (firstVector->size() > 0) { // we will check this for all vectors later
			const int firstVectorSize = firstVector->size();
			const Analitza::Object::ObjectType firstVectorElementType = firstVector->at(0)->type();
			
			switch(firstVectorElementType) {
				case Analitza::Object::matrix: { // try to build a block matrix
					const Analitza::Matrix *firstBlock = static_cast<const Analitza::Matrix*>(firstVector->at(0));
					
					bool isCorrect = true; // this flag tells if is ok to build the block matrix
					int nrows = 0;
					int ncols = 0;
					int blockpattern[firstVectorSize]; // if vectors(matrixrow) this tells the row(column) pattern
					
					const int blocklength = isVector? firstBlock->columnCount() : firstBlock->rowCount();
					
					// we need to know the pattern first, this run only on first arg (first vector) 
					for (int blockIndex = 0; blockIndex < firstVectorSize && isCorrect; ++blockIndex) {
						if (firstVector->at(blockIndex)->type() == Analitza::Object::matrix) {
							const Analitza::Matrix* block = static_cast<const Analitza::Matrix*>(firstVector->at(blockIndex));
							
							if (block->rowCount() > 0 && block->columnCount() > 0) {
								const int currentlength = isVector? block->columnCount() : block->rowCount();
								const int currentpattern = isVector? block->rowCount() : block->columnCount();
								
								if (currentlength == blocklength) {
									blockpattern[blockIndex] = isVector? block->rowCount() : block->columnCount();
									
									if (isVector)
										nrows += blockpattern[blockIndex];
									else
										ncols += blockpattern[blockIndex];
								} else {
									isCorrect = false;
									ret.addError(QCoreApplication::tr("bloques rows .. altura o numrows debe ser same"));
								}
							} else {
								isCorrect = false;
								ret.addError(QCoreApplication::tr("no se aceptan bloques/matrices vacias"));
							}
						} else {
							ret.addError(QCoreApplication::tr("not all are matrix i cant build a block matrix"));
							isCorrect = false;
						}
					}
					
					// check if all args are ok to build a block matrix
					for (int argIndex = 0; argIndex < nargs && isCorrect; ++argIndex) {
						const Analitza::Object::ObjectType currentArgType = args.at(argIndex).tree()->type();
						const Analitza::Vector *vector = static_cast<const Analitza::Vector*>(args.at(argIndex).tree());
						
						if (currentArgType == firstArgType) {
							if (vector->size() > 0) {
								if (vector->size() == firstVectorSize) {
									const Analitza::Matrix *currentFirstBlock = static_cast<const Analitza::Matrix*>(vector->at(0));
									const int blocklength = isVector? currentFirstBlock->columnCount() : currentFirstBlock->rowCount();
									
									for (int blockIndex = 0; blockIndex < firstVectorSize && isCorrect; ++blockIndex) {
										if (vector->at(blockIndex)->type() == Analitza::Object::matrix) {
											const Analitza::Matrix* block = static_cast<const Analitza::Matrix*>(vector->at(blockIndex));
											
											if (block->rowCount() > 0 && block->columnCount() > 0) {
												const int currentlength = isVector? block->columnCount() : block->rowCount();
												const int currentpattern = isVector? block->rowCount() : block->columnCount();
												
												if (currentlength != blocklength) {
													isCorrect = false;
													ret.addError(QCoreApplication::tr("cols cada block de row debe ser igual de row a row"));
												} else if (blockpattern[blockIndex] != currentpattern) {
													isCorrect = false;
													ret.addError(QCoreApplication::tr("bloques rows .. altura o numrows debe ser same"));
												}
											} else {
												isCorrect = false;
												ret.addError(QCoreApplication::tr("no se aceptan bloques/matrices vacias"));
											}
										} else {
											isCorrect = false;
											ret.addError(QCoreApplication::tr("no se aceptan bloques/matrices vacias"));
										}
									}
									
									if (isCorrect)
										if (isVector)
											ncols += blocklength;
										else
											nrows += blocklength;
								} else {
									isCorrect = false;
									ret.addError(QCoreApplication::tr("all argument (vec or row) must have the same size"));
								}
							} else {
								ret.addError("we dont allow empty vector or rows"); // TODO better message
								isCorrect = false;
							}
						} else {
							isCorrect = false;
							ret.addError(QCoreApplication::tr("all argument must be of the same type: all rows or all vectors"));
						}
					}
					
					if (isCorrect) {
						Analitza::Matrix *matrix = new Analitza::Matrix();
						
						QVector< QVector< const Analitza::Object* > > objmatrix(nrows, QVector< const Analitza::Object* >(ncols, 0));
						
						int nrowsoffset = isVector? nrows : 0;
						int ncolsoffset = isVector? 0 : ncols;
						
						for (int argIndex = 0; argIndex < nargs && isCorrect; ++argIndex) {
							const Analitza::Vector *vector = static_cast<const Analitza::Vector*>(args.at(argIndex).tree());
							
							int blockpattern = 0;
							
							if (isVector)
								nrowsoffset = 0;
							else
								ncolsoffset = 0;
							
							for (int blockIndex = 0; blockIndex < firstVectorSize && isCorrect; ++blockIndex) {
								const Analitza::Matrix* block = static_cast<const Analitza::Matrix*>(vector->at(blockIndex));
								const int m = block->rowCount();
								const int n = block->columnCount();
								
								blockpattern = isVector? n : m;
								
								for (int i = 0; i < m; ++i)
									for (int j = 0; j < n; ++j)
										objmatrix[i+nrowsoffset][j+ncolsoffset] = block->at(i,j);
								
								if (isVector)
									nrowsoffset += m;
								else if (blockIndex == 0) // el patron de cols se define en el primer matrixrow
									ncolsoffset += n;
							}
							
							if (!isVector)
								nrowsoffset += blockpattern;
							else if (argIndex == 0)
								ncolsoffset += blockpattern;
						}
						
						for (int i = 0; i < nrows; ++i) {
							Analitza::MatrixRow *row = new Analitza::MatrixRow(ncols);
							
							for (int j = 0; j < ncols; ++j)
								row->appendBranch(objmatrix[i][j]->copy());
							
							matrix->appendBranch(row);
						}
						
						ret.setTree(matrix);
					
						return ret;
					}
				}	break;
				
			}
		} else
			ret.addError("we dont allow empty vector or rows"); // TODO better message
	} else
		ret.addError("to build a matrix use vector or rows as args"); // TODO better message
	//END commom
	
	return ret;
}

//END FillMatrixConstructor

//BEGIN DiagonalMatrixConstructor

const QString BlockDiagonalMatrixCommand::id = QString("blockdiag");
const ExpressionType BlockDiagonalMatrixCommand::type  = variadicFunctionType(VectorAndMatrixAlternatives);

Expression BlockDiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	int nargs = args.size();
	bool byvector = false;
	
		switch(nargs) {
		case 0: {
			ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(BlockDiagonalMatrixCommand::id));
			
			return ret;
		}	break;
		}
	const Analitza::Vector *v = byvector? static_cast<const Analitza::Vector*>(args.first().tree()) : 0;
	
	if (byvector) nargs = v->size();
	
	for (int k = 0; k < nargs; ++k)
		if ((byvector? v->at(k)->type() : args.at(k).tree()->type()) == Analitza::Object::none) {
			ret.addError(QCoreApplication::tr("the arg %1 is invalid or is error").arg(k+1));
			return ret;
		}
	
							
	//BEGIN block diag matrix
	// first try to build block diag matrix
	if (args.first().tree()->type() == Analitza::Object::matrix) {
		bool failbyblockdiag = false;
		int nrows = 0;
		int ncols = 0;

		for (int k = 0; k < nargs && !failbyblockdiag; ++k)
			if (args.at(k).tree()->type() == Analitza::Object::matrix) {
				const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(args.at(k).tree());
				const int m = block->rowCount();
				const int n = block->columnCount();
				if (m > 0 && n > 0) {
					nrows += m;
					ncols += n;
				} else {
					ret.addError("not se quiere empty matrix/blocks"); //TODO better messages
					failbyblockdiag = true;
				}
			} else {
				ret.addError("not all are matrices/blocks can't build bok diag matrix"); //TODO better messages
				failbyblockdiag = true;
			}
		
		if (!failbyblockdiag) {
			Analitza::Matrix *matrix = new Analitza::Matrix();
			QVector< QVector< const Analitza::Object* > > objmatrix(nrows, QVector< const Analitza::Object* >(ncols, 0));
			
			nrows = 0;
			ncols = 0;
		
			for (int k = 0; k < nargs; ++k) {
				const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(args.at(k).tree());
				const int m = block->rowCount();
				const int n = block->columnCount();
				
				for (int i = 0; i < m; ++i)
					for (int j = 0; j < n; ++j)
						objmatrix[i+nrows][j+ncols] = block->at(i,j);
				
				nrows += m;
				ncols += n;
			}
			
			for (int i = 0; i < nrows; ++i) {
				Analitza::MatrixRow *row = new Analitza::MatrixRow(ncols);
				
				for (int j = 0; j < ncols; ++j) {
					const Analitza::Object *obj = objmatrix[i][j];
					
					if (obj)
						row->appendBranch(obj->copy());
					else
						row->appendBranch(new Analitza::Cn(0));
				}
				
				matrix->appendBranch(row);
			}
			
			ret.setTree(matrix);
		}
		
		return ret;
	}
	//END block diag matrix
	

	return ret;
}

//END DiagonalMatrixConstructor
