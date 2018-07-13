/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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


#include "transformation.h"
#include <analitza/object.h>
#include "substituteexpression.h"
#include <analitza/expression.h>
#include "container.h"

using namespace Analitza;

Transformation::Transformation(const Object* first, const Object* second, const QMap< QString, Transformation::treeCheck >& conditions)
    : first(first), second(second)
    , conditions(conditions)
{}

Transformation::Transformation(const Object* first, const Object* second)
    : first(first), second(second)
{}

Analitza::Object* Transformation::applyTransformation(const Analitza::Object* input) const {
    QMap<QString, const Object*> matchedValues;
    bool match = first->matches(input, &matchedValues);
    
//     qDebug() << "beeeeee" << input->toString() << first->toString() << match;
    if(match) {
        bool satisfied=true;
        for(QMap<QString, treeCheck>::const_iterator it=conditions.constBegin(), itEnd=conditions.constEnd(); satisfied && it!=itEnd; ++it) {
            Q_ASSERT(matchedValues.contains(it.key()));
            const Object* value = matchedValues.value(it.key());
            
            satisfied = it.value()(value);
        }
        
        if(satisfied) {
//             qDebug() << "match!" << first->toString() << input->toString();
            SubstituteExpression exp;
            Object* obj=exp.run(second.data(), matchedValues);
            return obj;
        }
    }
    return nullptr;
}

const Object* Transformation::parse(const QString& exp)
{
    Expression e(exp);
//     if(!e.isCorrect()) qDebug() << "lelele" << exp << e.error();
    Q_ASSERT(e.isCorrect());
    Object* tree = e.tree();
    e.setTree(nullptr);
    
    //We remove the math node
    Container* root = static_cast<Container*>(tree);
    
    Q_ASSERT(root->m_params.size()==1);
    tree=root->m_params.takeFirst();
    delete root;
    
    return tree;
}
