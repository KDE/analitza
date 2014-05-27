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

#include "matrixcommands.h"

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
static const ExpressionType IndefiniteArityType = ExpressionType(ExpressionType::Any, 1);
//ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
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

//BEGIN FillMatrixConstructor

const QString MatrixCommand::id = QString("matrix");
const ExpressionType MatrixCommand::type = variadicFunctionType(MatrixType);
//TODO better error messages
Expression MatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const int nargs = args.size();
	
	switch(nargs) {
		case 0: {
			ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(MatrixCommand::id));
			
			return ret;
		}	break;
		case 1: {
			if (args.at(0).tree()->type() == Analitza::Object::value) {
				const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
				
				if (nobj->isInteger() && nobj->value() > 0) {
					Analitza::Matrix *matrix = new Analitza::Matrix();
					const int n = nobj->intValue();
					AnalitzaUtils::fillMatrix(matrix, n, n, 0);
					ret.setTree(matrix);
				} else
					ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
				
				return ret;
			}
		}	break;
		case 2: {
			if (args.at(0).tree()->type() == Analitza::Object::value && args.at(1).tree()->type() == Analitza::Object::value) {
				const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
				const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
				
				if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() > 0 && ncolsobj->value() > 0) {
					Analitza::Matrix *matrix = new Analitza::Matrix();
					AnalitzaUtils::fillMatrix(matrix, nrowsobj->intValue(), ncolsobj->intValue(), 0);
					ret.setTree(matrix);
				} else
					ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
				
				return ret;
			}
		}	break;
		case 3: {
			if (args.at(0).tree()->type() == Analitza::Object::value && 
				args.at(1).tree()->type() == Analitza::Object::value && 
				args.at(2).tree()->type() == Analitza::Object::value) {
				const Analitza::Cn *nrowsobj = static_cast<const Analitza::Cn*>(args.at(0).tree());
				const Analitza::Cn *ncolsobj = static_cast<const Analitza::Cn*>(args.at(1).tree());
				
				if (nrowsobj->isInteger() && ncolsobj->isInteger() && nrowsobj->value() > 0 && ncolsobj->value() > 0) {
					Analitza::Matrix *matrix = new Analitza::Matrix();
					AnalitzaUtils::fillMatrix(matrix, nrowsobj->intValue(), ncolsobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())->value());
					ret.setTree(matrix);
				} else
					ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
				
				return ret;
			}
		}	break;
	}
	
	Q_ASSERT(nargs > 0);
	Q_ASSERT(ret.toString().isEmpty());
	Q_ASSERT(ret.isCorrect());
	
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
				default: { // try to build a normal matrix
					
					
					
					
					bool allrows = true; // assumes all are rows
					Analitza::Matrix *matrix = new Analitza::Matrix();
					
					for (int i = 0; i < nargs && allrows; ++i) {
						if (args.at(i).tree()->type() == firstArgType)
						{
							const Analitza::MatrixRow *row = static_cast<const Analitza::MatrixRow*>(args.at(i).tree());
							
							if (row->size() == firstVectorSize)
								matrix->appendBranch(row->copy());
							else {
								allrows = false;
								ret.addError(QCoreApplication::tr("All matrixrow elements must have the same size"));
							}
						}
						else
							allrows = false;
					}
				
					if (!ret.isCorrect()) {
						delete matrix;
						
						return ret;
					} else if (allrows) {
						if (isVector) {
						QString* error=0;
						ret.setTree(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, &error));
						delete matrix;
						
						Q_ASSERT(error == 0);
						} else
							ret.setTree(matrix);
						
						return ret;
					} else {
						ret.addError("Every argument must be a matrixrow element");
						delete matrix;
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


//BEGIN ZeroMatrixConstructor

const QString ZeroMatrixCommand::id = QString("zeromatrix");
const ExpressionType ZeroMatrixCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any, ExpressionType(ExpressionType::Value)))
// .addParameter(ExpressionType(ExpressionType::Any))
.addParameter(MatrixType);

Expression ZeroMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
// 	if (args.size() != 2) {
// 		Expression ret;
// 		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(2).arg(ZeroMatrixCommand::id));
// 		
// 		return ret;
// 	}
	
	MatrixCommand fillMatrix;
	
	return fillMatrix(args);
}

//END ZeroMatrixConstructor


//BEGIN IdentityMatrixConstructor

const QString IdentityMatrixCommand::id = QString("identitymatrix");
const ExpressionType IdentityMatrixCommand::type = variadicFunctionType(MatrixType);

Expression IdentityMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	if (args.size() != 1) {
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IdentityMatrixCommand::id));
		
		return ret;
	}
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.first().tree());
	const int n = nobj->value();
	
	if (nobj->isInteger() && n > 0) {
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int row = 0; row < n; ++row) {
			Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
			
			for (int col= 0; col < n; ++col)
				if (row == col)
					rowobj->appendBranch(new Analitza::Cn(1));
				else
					rowobj->appendBranch(new Analitza::Cn(0));
			
			matrix->appendBranch(rowobj);
		}
		
		ret.setTree(matrix);
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);

	return ret;
}

//END IdentityMatrixConstructor


//BEGIN DiagonalMatrixConstructor

const QString DiagonalMatrixCommand::id = QString("diag");
const ExpressionType DiagonalMatrixCommand::type  = variadicFunctionType(VectorAndMatrixAlternatives);

Expression DiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	int nargs = args.size();
	bool byvector = false;
	
	switch(nargs) {
		case 0: {
			ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(DiagonalMatrixCommand::id));
			
			return ret;
		}	break;
		case 1: {
			if (args.first().tree()->type() == Analitza::Object::matrix) {
				const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
				const int n = std::min(matrix->rowCount(), matrix->columnCount());
				
				Analitza::Vector *diagonal = new Analitza::Vector(n);
				
				for (int i = 0; i < n; ++i)
					diagonal->appendBranch(matrix->at(i, i)->copy());
				
				ret.setTree(diagonal);
				
				return ret;
			} else if (args.first().tree()->type() == Analitza::Object::vector)
				byvector = true;
		}	break;
		case 2: {
			if (args.first().tree()->type() == Analitza::Object::matrix && args.last().tree()->type() == Analitza::Object::value) {
				const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
				
				if (nobj->isInteger()) {
					const Analitza::Matrix *matrix = static_cast<const Analitza::Matrix*>(args.first().tree());
					const int nrows = matrix->rowCount();
					const int ncols = matrix->columnCount();
					const int npos = nobj->value();
					const int absnpos = std::abs(npos);
					const int absnpos1 = absnpos + 1;
					const bool isneg = npos < 0;
					
					int n = 0; // or until/to
					int rowoffset = 0;
					int coloffset = 0;
					
					if (isneg) {
						if (absnpos1 > nrows) {
							ret.addError("The nth diagonal index must be less than the row count");
							return ret;
						}
						
						n = std::min(nrows - absnpos, ncols);
						rowoffset = absnpos;
					} else { // square matrix case too
						if (absnpos1 > ncols) {
							ret.addError("The nth diagonal index must be less than the column count");
							return ret;
						}
						
						n = std::min(nrows, ncols - absnpos);
						coloffset = absnpos;
					}
					
					Analitza::Vector *diagonal = new Analitza::Vector(n);
					
					for (int i = 0; i < n; ++i)
						diagonal->appendBranch(matrix->at(rowoffset + i, coloffset + i)->copy());
						
					ret.setTree(diagonal);
				}
				else
					ret.addError(QCoreApplication::tr("nth diagonal index must be integer number"));
				
				return ret;
			} else if (args.last().tree()->type() != Analitza::Object::value && args.last().tree()->type() != Analitza::Object::matrix) {
				ret.addError(QCoreApplication::tr("to specifi the diag index you must use positve integer value")); //TODO better messages
				
				return ret;
			}
		}	break;
	}
	
	Q_ASSERT(nargs > 0);
	Q_ASSERT(ret.toString().isEmpty());
	Q_ASSERT(ret.isCorrect());
	
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
	
	Q_ASSERT(ret.toString().isEmpty());
	Q_ASSERT(ret.isCorrect());
	
	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	for (int i = 0; i < nargs; ++i) {
		Analitza::MatrixRow *row = new Analitza::MatrixRow(nargs);
		
		for (int j = 0; j < nargs; ++j)
			if (i == j)
				row->appendBranch(byvector? v->at(j)->copy() : args.at(j).tree()->copy());
			else
				row->appendBranch(new Analitza::Cn(0));
		
		matrix->appendBranch(row);
	}
	
	ret.setTree(matrix);

	return ret;
}

//END DiagonalMatrixConstructor


//BEGIN TridiagonalMatrixConstructor

const QString TridiagonalMatrixCommand::id = QString("tridiag");
const ExpressionType TridiagonalMatrixCommand::type = variadicFunctionType(MatrixType);

Expression TridiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	if (args.size() != 4) {
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(4).arg(TridiagonalMatrixCommand::id));
		
		return ret;
	}
	
	const Analitza::Cn *nobj = static_cast<const Analitza::Cn*>(args.last().tree());
	const int n = nobj->value();
	
	if (nobj->isInteger() && n > 0) {
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int row = 0; row < n; ++row) {
			Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(n);
			
			for (int col= 0; col < n; ++col)
				if (row == col + 1) // a
					rowobj->appendBranch(args.at(0).tree()->copy());
				else
					if (row == col) // b
						rowobj->appendBranch(args.at(1).tree()->copy());
					else
						if (row == col - 1) // c
							rowobj->appendBranch(args.at(2).tree()->copy());
						else
							rowobj->appendBranch(new Analitza::Cn(0));
			
			matrix->appendBranch(rowobj);
		}
		
		ret.setTree(matrix);
	} else
		ret.addError(MATRIX_SIZE_ERROR_MESSAGE);
	
	return ret;
}

//END TridiagonalMatrixConstructor

