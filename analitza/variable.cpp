/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "variable.h"
#include "abstractexpressionvisitor.h"
#include "analitzautils.h"

using namespace Analitza;

Ci::Ci(const QString& b)
    : Object(variable), m_name(b), m_function(false), m_depth(-1)
{
    Q_ASSERT(!b.isEmpty());
}

Ci::~Ci()
{}

Ci* Ci::copy() const
{
    Ci *c = new Ci(m_name);
    c->m_function = m_function;
    c->m_depth = m_depth;
    return c;
}

QVariant Ci::accept(AbstractExpressionVisitor* e) const
{
    return e->visit(this);
}

QString Ci::toMathML() const
{
    if(m_function)
        return QStringLiteral("<ci type='function'>%1</ci>").arg(m_name);
    else
        return QStringLiteral("<ci>%1</ci>").arg(m_name);
}

bool Ci::matches(const Object* exp, QMap<QString, const Object*>* found) const
{
    bool ret=false;
    const Object* v=found->value(m_name);
    if(v) {
        ret=AnalitzaUtils::equalTree(exp, v);
    } else {
        Q_ASSERT(!found->contains(m_name));
        found->insert(m_name, exp);
        ret=true;
    }
//     qDebug() << "maaatch" << toString() << exp->toString() << ret;
    return ret;
}

QString Ci::toHtml() const
{
    return QStringLiteral("<span class='%1'>%2</span>").arg(m_function ? QStringLiteral("func") : QStringLiteral("var"), m_name);
}
