/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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


#ifndef ABSTRACTMAPPINGGRAPH_H
#define ABSTRACTMAPPINGGRAPH_H


#include "analitza/analyzer.h"
#include "analitza/value.h"
// #include "function.h"

namespace Analitza
{
class Variables;
class Analyzer;
class Object;
class Cn;
class Expression;
}

#include "utils/mathutils.h"

//NOTE
//para construir estos backends/abstract solo re necesitaa que su ctor tenga exp y varmod ... 
//en la funcion y en el metodo factory id es donde se requiere que tenga mas detalle ademas de exp y varsmod
// por ejemolo coordsys en el caso de surf

class AbstractMappingGraph //strategy pattern
{
public:
    virtual ~AbstractMappingGraph();

    virtual const QString typeName() const = 0;
    virtual QString iconName() const = 0;
    virtual QStringList examples() const = 0;

    virtual Dimension spaceDimension() const = 0;
    virtual CoordinateSystem coordinateSystem() const = 0;
    
protected:
    AbstractMappingGraph() {}
};

#endif // ABSTRACTMAPPINGGRAPH_H
