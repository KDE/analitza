
#line 54 "exp.g"

/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "analitzaexport.h"
#include "expressiontable_p.h"
#include <QList>
#include <QStringList>
class AbstractLexer;

class ANALITZA_EXPORT ExpressionParser : protected ExpressionTable
{
    public:
        ExpressionParser();
        ~ExpressionParser();

        bool parse(AbstractLexer *lexer);

        bool isCorrect() const { return m_err.isEmpty(); }
        int errorLineNumber() const { return m_errorLineNumber; }
        QStringList error() const { return m_err; }
        QString mathML() const { return m_exp; }
        QStringList comments() const { return m_comments; }

    private:
        void reallocateStack();

        inline QString &sym(int index)
        { return m_symStack[m_tos + index - 1]; }

        int m_tos;
        QList<int> m_stateStack;
        QList<QString> m_symStack;
        int m_errorLineNumber;
        QStringList m_err;
        QString m_exp;
        QStringList m_comments;
};

#endif

