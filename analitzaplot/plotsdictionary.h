/*************************************************************************************
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



#ifndef ANALITZAPLOT_FUNCTION_H_DICT
#define ANALITZAPLOT_FUNCTION_H_DICT
#include <qstringlist.h>

class PlotItem;
class PlotsModel;

//NOTE
//WIP
//singleton class ofrecera un modelo de los plots leidos de un archivo
// dicho archivo debe estar an algun path conocido de kde
// el archivo contine una bd de plots famosos (curvas famosas y superficies famosas)
// Esta clase es la interface a esa base de datos
class PlotsDictionary
{
public:
    //TODO query interface
    
//     PlotItem *getByName() ...
    
    //
    PlotsModel * model() const;
    
    QStringList log() const; // ofrece errores al cargar o warnings ...
    
private:
    // casi todo lo demas será private
    
    bool load();
    
    
//     static self ...
};


#endif

