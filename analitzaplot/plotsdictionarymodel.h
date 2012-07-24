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


#include "plotsmodel.h"
#include "plotsproxymodel.h"
#include <kjob.h>

//TODO debe adivinar las variables
//El ctr carga el archivo, en la documentacion final se debe indicar
// que solo es necesario usar una sola instancia de esta clase


//TODO 
//static QStringlist registred dictionaries
//ctor (QString dic, QObjetc ...)

//load the registred dictionary with a thread
class DictionaryLoader : public QObject
{
    Q_OBJECT
    
public:
    DictionaryLoader(PlotsModel *model, QObject *parent = 0);
    ~DictionaryLoader();
    
public slots:
    void load();

signals:
    void loaded();
    void errorFound(const QString &error);
    
private:
    PlotsModel *m_model;
};

class ANALITZAPLOT_EXPORT PlotsDictionaryModel : public PlotsProxyModel
{
Q_OBJECT    

friend PlotsDictionaryModel * getDictionary(QObject *); // for private slots

public:
    PlotsDictionaryModel(QObject* parent = 0);
    ~PlotsDictionaryModel();
    
//     PlotItem *getByName() ...
    
    bool isLoaded() const { return rowCount() > 0 && m_errors.isEmpty(); }
    QStringList errors() const { return m_errors; }
    
private slots:
    //al terminar de cargar el archivo del dictionay entonces configuramos el proxy (this)
    void setupModelSourceModel();
    void registerError(const QString &error);

private: //TODO gsoc pimpl


    PlotsModel *m_model;

    QStringList m_errors;
};


//load the dict in a thread
ANALITZAPLOT_EXPORT PlotsDictionaryModel * getDictionary(QObject *parent);

#endif
