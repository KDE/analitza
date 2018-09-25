/*************************************************************************************
 *  Copyright (C) 2012 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef EXPRESSIONSTREAM_H
#define EXPRESSIONSTREAM_H

#include "analitzaexport.h"
#include <analitza/expression.h>

namespace Analitza
{

class ANALITZA_EXPORT ExpressionStream
{
    public:
        explicit ExpressionStream(QTextStream* dev);
        
        bool atEnd() const;
        bool isInterrupted() const;
        Analitza::Expression next();
        QString lastLine() const;
        
    private:
        QTextStream* m_dev;
        QString m_last;
        bool m_interrupted;
};

}

#endif
