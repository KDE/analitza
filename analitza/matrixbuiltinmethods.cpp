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

#include "matrixbuiltinmethods.h"

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

const QString VectorCommand::id = QString("vector");
const ExpressionType VectorCommand::type = functionType(ExpressionTypeList() << ValueType << ValueType, VectorType);

Expression VectorCommand::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret;
	
	const Analitza::Cn *lengthobj = static_cast<const Analitza::Cn*>(args.first().tree());
	
	if (lengthobj->isInteger() && lengthobj->value() > 0) {
		Analitza::Vector *vector = new Analitza::Vector(lengthobj->intValue());
		AnalitzaUtils::fillVector(vector, lengthobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())->value());
		ret.setTree(vector);
	}
	else
		ret.addError(QCoreApplication::tr("Vector size must be some integer value greater or equal to zero"));
	
	return ret;
}


//BEGIN FillMatrixConstructor

const QString MatrixCommand::id = QString("matrix");
const ExpressionType MatrixCommand::type = variadicFunctionType(MatrixType);

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
	
	if (args.first().tree()->type() == Analitza::Object::matrixrow) {
		const int rowsize = static_cast<const Analitza::MatrixRow*>(args.first().tree())->size();
	
		//TODO review messages
		//BEGIN block matrix by rows
		if (static_cast<const Analitza::MatrixRow*>(args.first().tree())->at(0)->type() == Analitza::Object::matrix) {
			const Analitza::MatrixRow *firstrow = static_cast<const Analitza::MatrixRow*>(args.first().tree());
			
			bool isCorrect = true; // this flag tells if is ok to build the block matrix
			int totalnrows = 0;
			int totalncols = 0;
			int ncolspattern[rowsize]; // basta con la priemra fila para saber el patron de columnas de cada bloque
			
			for (int k = 0; k < rowsize && isCorrect; ++k)
				if (firstrow->at(k)->type() == Analitza::Object::matrix) {
					ncolspattern[k] = static_cast<const Analitza::Matrix*>(firstrow->at(k))->columnCount();
					totalncols += ncolspattern[k];
				}
				else {
					ret.addError(QCoreApplication::tr("not all are matrix i cant build a block matrix"));
					isCorrect = false;
				}
			
			for (int l = 0; l < nargs && isCorrect; ++l) {
				if (args.at(l).tree()->type() == Analitza::Object::matrixrow)
				{
					const Analitza::MatrixRow *row = static_cast<const Analitza::MatrixRow*>(args.at(l).tree());
					
					if (row->size() == rowsize) {
						const int m = static_cast<const Analitza::Matrix*>(row->at(0))->rowCount();
						
						for (int k = 0; k < rowsize && isCorrect; ++k) {
							if (firstrow->at(k)->type() == Analitza::Object::matrix) {
								const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(row->at(k));
								
								if (block->rowCount() == m) {
									if (ncolspattern[k] == block->columnCount()) {
// 										nrows += m;
// 										ncols += ncolspattern[k];
									} else  {
										                              isCorrect = false;
										ret.addError(QCoreApplication::tr("cols cada block de row debe ser igual de row a row"));
									}
								} else {
									                           isCorrect = false;
									ret.addError(QCoreApplication::tr("bloques rows .. altura o numrows debe ser same"));
								}
							} else {
								ret.addError(QCoreApplication::tr("not all are matrix i cant build a block matrix"));//TODO arg di las pos de arg y su tipo
								                        isCorrect = false;
							}
						}
						
						if (isCorrect) {
							totalnrows += m;
						}
					}
					else {
						                  isCorrect = false;
						ret.addError(QCoreApplication::tr("All matrixrow elements must have the same size"));
					}
				}
				else
					isCorrect = false;
			}
			
			if (isCorrect) {
				Analitza::Matrix *matrix = new Analitza::Matrix();
				
				QVector< QVector< const Analitza::Object* > > objmatrix(totalnrows, QVector< const Analitza::Object* >(totalncols, 0));
				
				int nrows = 0;
				
				
				for (int l = 0; l < nargs && isCorrect; ++l) {
					const Analitza::MatrixRow *row = static_cast<const Analitza::MatrixRow*>(args.at(l).tree());
					const int nblocks = row->size();
					int m = 0;
					int ncols = 0;
					for (int k = 0; k < nblocks; ++k) {
						const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(row->at(k));
						const int n = block->columnCount();
						m = block->rowCount();
						
						for (int i = 0; i < m; ++i)
							for (int j = 0; j < n; ++j)
								objmatrix[i+nrows][j+ncols] = block->at(i,j);
						
						if (k ==0) // el patron de cols se define en el primer matrixrow
							ncols += n;
					}
					nrows += m;
				}
				
				for (int i = 0; i < totalnrows; ++i) {
					Analitza::MatrixRow *row = new Analitza::MatrixRow(totalncols);
					
					for (int j = 0; j < totalncols; ++j)
						row->appendBranch(objmatrix[i][j]->copy());
					
					matrix->appendBranch(row);
				}
				
				ret.setTree(matrix);
			
				return ret;
			}
		}
		//END block matrix by rows
		
		bool allrows = true; // assumes all are rows
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int i = 0; i < nargs && allrows; ++i) {
			if (args.at(i).tree()->type() == Analitza::Object::matrixrow)
			{
				const Analitza::MatrixRow *row = static_cast<const Analitza::MatrixRow*>(args.at(i).tree());
				
				if (row->size() == rowsize)
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
			ret.setTree(matrix);
			
			return ret;
		} else {
			ret.addError("Every argument must be a matrixrow element");
			delete matrix;
		}
	} else if (args.first().tree()->type() == Analitza::Object::vector) {
		const int colsize = static_cast<const Analitza::Vector*>(args.first().tree())->size();
		
		//TODO review messages
		//BEGIN block matrix by vectors/cols
		if (static_cast<const Analitza::Vector*>(args.first().tree())->at(0)->type() == Analitza::Object::matrix) {
			const Analitza::Vector *firstcol = static_cast<const Analitza::Vector*>(args.first().tree());
			
			bool isCorrect = true; // this flag tells if is ok to build the block matrix
			int totalnrows = 0;
			int totalncols = 0;
			int nrowspattern[colsize]; // basta con la priemra fila para saber el patron de columnas de cada bloque
			
			for (int k = 0; k < colsize && isCorrect; ++k)
				if (firstcol->at(k)->type() == Analitza::Object::matrix) {
					nrowspattern[k] = static_cast<const Analitza::Matrix*>(firstcol->at(k))->rowCount();
					totalnrows += nrowspattern[k];
				}
				else {
					ret.addError(QCoreApplication::tr("not all are matrix i cant build a block matrix"));
					isCorrect = false;
				}
			
			for (int l = 0; l < nargs && isCorrect; ++l) {
				if (args.at(l).tree()->type() == Analitza::Object::vector)
				{
					const Analitza::Vector *col = static_cast<const Analitza::Vector*>(args.at(l).tree());
					
					if (col->size() == colsize) {
						const int n = static_cast<const Analitza::Matrix*>(col->at(0))->columnCount();
						
						for (int k = 0; k < colsize && isCorrect; ++k) {
							if (firstcol->at(k)->type() == Analitza::Object::matrix) {
								const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(col->at(k));
								
								if (block->columnCount() == n) {
									if (nrowspattern[k] == block->rowCount()) {
// 										nrows += m;
// 										ncols += ncolspattern[k];
									} else  {
										isCorrect = false;
										ret.addError(QCoreApplication::tr("cols cada block de row debe ser igual de row a row"));
									}
								} else {
									isCorrect = false;
									ret.addError(QCoreApplication::tr("bloques rows .. altura o numrows debe ser same"));
								}
							} else {
								ret.addError(QCoreApplication::tr("not all are matrix i cant build a block matrix"));//TODO arg di las pos de arg y su tipo
								isCorrect = false;
							}
						}
						
						if (isCorrect) {
							totalncols += n;
						}
					}
					else {
						isCorrect = false;
						ret.addError(QCoreApplication::tr("All matrixrow elements must have the same size"));
					}
				}
				else
					isCorrect = false;
			}
			
			if (isCorrect) {
				Analitza::Matrix *matrix = new Analitza::Matrix();
				
				QVector< QVector< const Analitza::Object* > > objmatrix(totalnrows, QVector< const Analitza::Object* >(totalncols, 0));
				
				int ncols = 0;
				
				
				for (int l = 0; l < nargs && isCorrect; ++l) {
					const Analitza::Vector *col = static_cast<const Analitza::MatrixRow*>(args.at(l).tree());
					const int nblocks = col->size();
					int n = 0;
					int nrows = 0;
					for (int k = 0; k < nblocks; ++k) {
						const Analitza::Matrix *block = static_cast<const Analitza::Matrix*>(col->at(k));
						const int m = block->rowCount();
						n = block->columnCount();
						
						for (int i = 0; i < m; ++i)
							for (int j = 0; j < n; ++j)
							{
								qDebug() << "llenando ... " << i+nrows << j+ncols << block->at(i,j)->toString();
								objmatrix[i+nrows][j+ncols] = block->at(i,j);
							}
						
						 // el patron de cols se define en el primer matrixrow
							nrows += m;
					}
					if (l ==0)
						ncols += n;
				}
				
				for (int i = 0; i < totalnrows; ++i) {
					Analitza::MatrixRow *row = new Analitza::MatrixRow(totalncols);
					
					for (int j = 0; j < totalncols; ++j)
						row->appendBranch(objmatrix[i][j]->copy());
					
					matrix->appendBranch(row);
				}
				
				ret.setTree(matrix);
			
				return ret;
			}
		}
		//END block matrix by vectors/cols
		
		bool allcols = true; // assumes all are columns (vectors)
		int lastsize = static_cast<const Analitza::Vector*>(args.first().tree())->size();
		Analitza::Matrix *matrix = new Analitza::Matrix();
		
		for (int j = 0; j < nargs && allcols; ++j) {
			if (args.at(j).tree()->type() == Analitza::Object::vector)
			{
				const Analitza::Vector *col = static_cast<const Analitza::Vector*>(args.at(j).tree());
				const int length = col->size();
				
				if (length == lastsize) {
					Analitza::MatrixRow *row = new Analitza::MatrixRow(length);
					
					for (int i = 0; i < col->size(); ++i)
						row->appendBranch(col->at(i)->copy());
					
					matrix->appendBranch(row);
				}
				else {
					allcols = false;
					//TODO better msgs
					ret.addError(QCoreApplication::tr("All vectors must have the same size"));
				}
			}
			else
				allcols = false;
		}
		
		if (!ret.isCorrect()) {
			delete matrix;
			
			return ret;
		} else if (allcols) {
			QString* error=0;
			ret.setTree(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, &error));
			delete matrix;
			
			Q_ASSERT(error == 0);
			
			return ret;
		} else {
			ret.addError("Every argument must be a vector");
			delete matrix;
		}
	} else 
		ret.addError("bad types for matrix command"); //TODO better message
	
	return ret;
}

//END FillMatrixConstructor


//BEGIN ZeroMatrixConstructor

const QString ZeroMatrixCommand::id = QString("zeromatrix");
const ExpressionType ZeroMatrixCommand::type = variadicFunctionType(MatrixType);

Expression ZeroMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 2) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(2).arg(ZeroMatrixCommand::id));
		
		return ret;
	}
	
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

//BEGIN IsZeroMatrix

const QString IsZeroMatrixCommand::id = QString("iszeromatrix");
const ExpressionType IsZeroMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsZeroMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsZeroMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(static_cast<const Analitza::Matrix*>(args.first().tree())->isZero()));
}

//END IsZeroMatrix


const QString IsIdentityMatrixCommand::id = QString("isidentitymatrix");
const ExpressionType IsIdentityMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsIdentityMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsIdentityMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}

const QString IsDiagonalMatrixCommand::id = QString("isdiag");
const ExpressionType IsDiagonalMatrixCommand::type = variadicFunctionType(VectorType);

Expression IsDiagonalMatrixCommand::operator()(const QList< Analitza::Expression >& args)
{
	if (args.size() != 1) {
		Expression ret;
		ret.addError(QCoreApplication::tr("Invalid parameter count for '%2'. Should have %1 parameters.").arg(1).arg(IsDiagonalMatrixCommand::id));
		
		return ret;
	}
	
	return Expression(new Analitza::Cn(AnalitzaUtils::isIdentityMatrix(static_cast<const Analitza::Matrix*>(args.first().tree()))));
}
