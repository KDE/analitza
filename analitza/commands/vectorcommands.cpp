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

#include "vectorcommands.h"

#include <QCoreApplication>

#include "analitzautils.h"
#include "expression.h"
#include "list.h"
#include "vector.h"
#include "value.h"

using Analitza::Expression;
using Analitza::ExpressionType;

const QString VectorCommand::id = QStringLiteral("vector");
const ExpressionType VectorCommand::type = ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any))
.addParameter(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any), -1));

Expression VectorCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    const int nargs = args.size();
    
    switch(nargs) {
        case 0: {
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(VectorCommand::id));
        }    break;
        case 1: {
            const Analitza::Object *arg = args.first().tree();
            
            //TODO vector(5) := vector{0,0,0,0,0}
            if (arg->type() == Analitza::Object::list) {
                const Analitza::List *list = static_cast<const Analitza::List*>(arg);
                const int n = list->size();
                
                Analitza::Vector *vector = new Analitza::Vector(n);
                
                for (int i = 0; i < n; ++i)
                    vector->appendBranch(list->at(i)->copy());
                
                ret.setTree(vector);
            } else {
                ret.addError(QCoreApplication::tr("Invalid parameter type for '%1', was expected a list").arg(VectorCommand::id));
            }
        }    break;
        case 2: {
            const Analitza::Object *arg = args.first().tree();
            
            if (arg->type() == Analitza::Object::value) {
                const Analitza::Cn *lengthobj = static_cast<const Analitza::Cn*>(args.first().tree());
                
                if (lengthobj->isInteger() && lengthobj->value() > 0) {
                    ret.setTree(new Analitza::Vector(lengthobj->intValue(), static_cast<const Analitza::Cn*>(args.last().tree())));
                }
                else {
                    ret.addError(QCoreApplication::tr("Vector size must be some integer value greater to zero"));
                }
            } else {
                ret.addError(QCoreApplication::tr("Vector size must be some integer value greater to zero"));
            }
        }    break;
        default:
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(VectorCommand::id));
            break;
    }
    
    return ret;
}
