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

#include "listcommands.h"

#include <QCoreApplication>

#include "expression.h"
#include "list.h"
#include "value.h"

using Analitza::Expression;
using Analitza::ExpressionType;

const QString RangeCommand::id = QStringLiteral("range");
const ExpressionType RangeCommand::type =     ExpressionType(ExpressionType::Lambda)
.addParameter(ExpressionType(ExpressionType::Any, ExpressionType(ExpressionType::Value)))
.addParameter(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Value)));

Expression RangeCommand::operator()(const QList< Analitza::Expression >& args)
{
    Expression ret;
    
    const int nargs = args.size();
    
    double a = 1;
    double b = 0;
    double h = 1;
    
    switch(nargs) {
        case 0: {
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(RangeCommand::id));
            
            return ret;
        }    break;
        case 1: {
            b = static_cast<const Analitza::Cn*>(args.first().tree())->value();
        }    break;
        case 2: {
            a = static_cast<const Analitza::Cn*>(args.first().tree())->value();
            b = static_cast<const Analitza::Cn*>(args.last().tree())->value();
        }    break;
        case 3: {
            a = static_cast<const Analitza::Cn*>(args.at(0).tree())->value();
            b = static_cast<const Analitza::Cn*>(args.at(1).tree())->value();
            h = static_cast<const Analitza::Cn*>(args.at(2).tree())->value();
        }    break;
        default:
            ret.addError(QCoreApplication::tr("Invalid parameter count for '%1'").arg(RangeCommand::id));
            
            return ret;
            break;
    }
    
    Analitza::List *seq = new Analitza::List;
        
    for (double x = a; x < b || qFuzzyCompare(x, b); x += h) {
        seq->appendBranch(new Analitza::Cn(x));
    }
    
    ret.setTree(seq);
    
    return ret;
}
