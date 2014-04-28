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

#include "expression.h"
#include "operations.h"
#include "list.h"
#include "matrix.h"
#include "container.h"
#include "customobject.h"

#include <QDebug>

using Analitza::Expression;
using Analitza::ExpressionType;

const QString MatrixConstructor::id = QString("matrix");

const ExpressionType MatrixConstructor::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any), -2), -1))
.addParameter(ExpressionType(ExpressionType::Many));

//TODO fix error messages
//TODO Matrix (2,3) crea una metrix nula de 2x3
Expression MatrixConstructor::operator()(const QList< Analitza::Expression >& args)
{
	Expression ret("matrix{}"); // si no pongo matrix{} analyzer se queja pues se evalua un objeto expresion nulo
	
	if (args.isEmpty())
		return ret;
	
// 	bool usevectors = false;
// 	bool usesizes = false;
	
	int oldsize = -1;
	QString oldVectorTypeInfo;
	const Analitza::CustomObject *info; // after the loop ends this give us if the input are rows or cols 
	
	//check args are comming from row or col commands
	for (int i = 0; i < args.size(); ++i)
	{
		//Q_ASSERT(args.at(i).isList());
		if (!args.at(i).isList())
		{
			ret.addError("i list, no es list");
			
			return ret;
		}
		
		const Analitza::List *list = static_cast<const Analitza::List*>(args.at(i).tree());
		
		//Q_ASSERT(list->size() > 1);
		if (list->size() <= 1)
		{
			ret.addError("i list, es vacia o no tiene suf elements");
			
			return ret;
		}
		
		//Q_ASSERT(list->at(0)->type() == Analitza::Object::custom);
		if (list->at(0)->type() != Analitza::Object::custom)
		{
			ret.addError("i list, no parece have venido de col o row: first must be custom");
			
			return ret;
		}
		if (i == 0) oldsize = list->size(); //TODO this kind of validation is missing in matrix {...}
		
// 		qDebug() << "adasd" << oldsize << list->size() << i;
		
		//Q_ASSERT(list->size() == oldsize); // all args/list must have the same size
		if (list->size() != oldsize)
		{
			ret.addError("i list, debe ser de tamanio uniforme con respecto al resto");
			
			return ret;
		}
		
		info = static_cast<const Analitza::CustomObject*>(list->at(0));
	
		//Q_ASSERT(info->value() == MatrixRowConstructor::id || info->value() == MatrixColConstructor::id);
		if (info->value() != MatrixRowConstructor::id && info->value() != MatrixColConstructor::id)
		{
			ret.addError("i list, debe venir de col o row");
			
			return ret;
		}
		
		if (i == 0) oldVectorTypeInfo = info->value().toString();
		
		//Q_ASSERT(info->value() == oldVectorTypeInfo);
		if (info->value() != oldVectorTypeInfo)
		{
			ret.addError("i list, debe formarsolo de rows o cols nada de hibridos");
			
			return ret;
		}
	}
	
// 	at this point the input is correct and not empty and info is not nul and has if the input are rows or cols
// 	also info only has 2 posible values MatrixRowConstructor::id or MatrixColConstructor::id
// 	and finaly at this point ret is a matrix Object
	
	Analitza::Container *container = new Analitza::Container(Analitza::Container::math);
	Analitza::Matrix *matrix = new Analitza::Matrix();
	
	for (int row = 0; row < args.size(); ++row)
	{
		const Analitza::List *list = static_cast<const Analitza::List*>(args.at(row).tree());
		Analitza::MatrixRow *rowobj = new Analitza::MatrixRow(list->size()-1); // -1 since customobjt (at 0 pos) is not needed
		
		for (int i = 1; i < list->size(); ++i) // ignore first element
			rowobj->appendBranch(list->at(i));
		
		matrix->appendBranch(rowobj);
	}
	
	if (info->value() == MatrixRowConstructor::id) // if we have rows
		container->appendBranch(matrix);
	else // we have cols
		container->appendBranch(Analitza::Operations::reduceUnary(Analitza::Operator::transpose, matrix, 0));
	
	ret = Expression(container->toString());
	
	return ret;
}



