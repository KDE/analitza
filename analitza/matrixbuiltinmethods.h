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

#ifndef MATRIXBUILTINMETHODS_H
#define MATRIXBUILTINMETHODS_H

#include "builtinmethods.h"

#include "expression.h"

namespace Analitza {
class Matrix;
};

//TODO expression::toConsoleString() ... to print matrix properly ...

//BEGIN Normal/Standard Matrix

template<int idval> // idval = 0 means row and idval = 1 means col
class MatrixVectorConstructor : public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args)
	{
		//TODO check when the args ammount is 0
		return Analitza::Expression::constructList(QList< Analitza::Expression >() << Analitza::Expression::constructCustomObject(vectorTypeInfo, 0) << args);
	}
	
	static const QString id;
	static const Analitza::ExpressionType type;
	
private:
	static const QVariant vectorTypeInfo; // row or col
};

template<int idval>
const QString MatrixVectorConstructor<idval>::id = idval == 0? "row" : (idval == 1? "col" : QString());


#include <iostream>
using Analitza::ExpressionType;
//TODO statc in this file static:supportedrettypes and supportedargstypes
static const ExpressionType variasmatrices()
{
	QList<ExpressionType> alternatives;
	
// 	//NOTE this constraint must be admissible since Analitza is not dynamically typed
// 	const int max_rows_supported = 10;
// 	const int max_cols_supported = 10;
// 	
// 	for (int row = 1; row <= max_rows_supported; ++row)
// 		for (int col = 1; col <= max_cols_supported; ++col)
// 			alternatives.append(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), col), row));
	
// 	alternatives.append(Analitza::ExpressionType(Analitza::ExpressionType::List, Analitza::ExpressionType(Analitza::ExpressionType::Any)));
	
	ExpressionType theone(ExpressionType::Lambda);
	const int maxargs = 50;
	for (int row = 1; row <= 2; ++row)
	{
		theone=theone.addParameter(ExpressionType(ExpressionType::Value));
		alternatives.append(theone);
	}
	
// 	for (int row = 0; row < alternatives.size(); ++row)
// 	{
// 		alternatives[row]=alternatives[row].addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
// 		std::cout << "ALT: " << alternatives.at(row).toString().toStdString() << std::endl;
// 	}
// 
	ExpressionType ab = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Many, QList<ExpressionType>() << 
ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Value)).addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1)) << 
ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Value)).addParameter(ExpressionType(ExpressionType::Value)).addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1))
))
;

		std::cout << "ABT: " << ab.toString().toStdString() << std::endl;
// return ab;

// 	return alternatives.first();
	return ExpressionType(ExpressionType::Many, alternatives);
}

template<int idval>
const Analitza::ExpressionType MatrixVectorConstructor<idval>::type = Analitza::ExpressionType(Analitza::ExpressionType::Many,
QList<ExpressionType>() << 
Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Any))
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Any))
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::List, Analitza::ExpressionType(Analitza::ExpressionType::Any)))
<< 
Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Any))
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Any))
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Any))
.addParameter(Analitza::ExpressionType(Analitza::ExpressionType::List, Analitza::ExpressionType(Analitza::ExpressionType::Any))));

template<int idval>
const QVariant MatrixVectorConstructor<idval>::vectorTypeInfo = QVariant(QString(MatrixVectorConstructor<idval>::id));

typedef MatrixVectorConstructor<0> MatrixRowConstructor;
typedef MatrixVectorConstructor<1> MatrixColConstructor;

/**
 * \class MatrixConstructor
 * 
 * \brief Defines four ways to construct a normal/standard matrix.
 *TODO matrix(n,m,VAL) 
 * matrix(n) := nxn square matrix filled with zeros
 * matrix(m,n) := mxn-matrix filled with zeros
Plotter3D provides an agnostic way to manage a 3d scene for draw math plots, 
 * Contains just OpenGL calls, so is uncoupled with QWidget nor QtQuick. This 
 * class needs the PlotsModel (to create the geometry for 3D plots) and also 
 * exposes some methods to change the scene (like hide/show the axis or 
 * reference planes for example)
 */

class MatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

//END Normal/Standard Matrix

//BEGIN IdentityMatrixConstructor

class IdentityMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

//END IdentityMatrixConstructor

//BEGIN DiagonalMatrixConstructor


//TODO diag(a,b,c...) as nomrla math semantics and
//diag(matrix) to obtain the diaonl of a SQUARE matrix as a vector
class DiagonalMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

//END DiagonalMatrixConstructor

//some bands contrs

//tridiag(a, b, c, n)

class TridiagonalMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

#endif // MATRIXBUILTINMETHODS_H
