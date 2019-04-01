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


#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <QMap>
#include <QSharedPointer>

namespace Analitza
{
class Object;

class Transformation
{
    public:
        typedef bool (*treeCheck)(const Object* o);
        Transformation(const Object* first, const Object* second);
        Transformation(const Object* first, const Object* second, const QMap<QString, treeCheck>& conditions);
        
        Object* applyTransformation(const Object* input) const;
        static const Object* parse(const QString& exp);
    private:
        QSharedPointer<const Object> first, second;
        QMap<QString, treeCheck> conditions;
};

}

#endif // TRANSFORMATION_H
