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

#include "plotsdictionarymodel.h"
#include <analitza/expression.h>
#include <KStandardDirs>
#include <KLocale>
#include <QFile>
#include <qthread.h>
#include <QTimer>
#include <QDomDocument>


DictionaryLoader::DictionaryLoader(PlotsModel* model, QObject* parent)
: QObject(parent), m_model(model)
{

}

DictionaryLoader::~DictionaryLoader()
{

}

void DictionaryLoader::load()
{
//     por el momento usaremos esta base de datos, luego sera la de las traducciones en el nuevo formato
    QString localurl = KStandardDirs::locate("data", QString("khipu/data/functionlibrary.kfl"));

    QFile device(localurl);

    if (device.open(QFile::ReadOnly | QFile::Text))
    {
        QDomDocument domDocument;

        QString errorStr;
        int errorLine;
        int errorColumn;

        if (!domDocument.setContent(&device, true, &errorStr, &errorLine, &errorColumn))
        {
//             m_errors << "Parse error" << errorStr << QString::number(errorLine) << QString::number(errorColumn);
//TODO mejorar el envio de error
                emit errorFound(errorStr);
            return ;
        }

        QDomElement root = domDocument.documentElement();
        QDomElement functionElement = root.firstChildElement("function");
        QString lang = KGlobal::locale()->language();

        QString name;
        QString lambda;
        int dimension =  -1;
        QStringList arguments;

        while (!functionElement.isNull())
        {
            QDomNodeList functionDataElements = functionElement.childNodes();

            name = functionDataElements.at(0).toElement().text();
            lambda = functionDataElements.at(1).toElement().text();
            dimension = functionDataElements.at(2).toElement().text().toInt();
            arguments.clear();

            for (int i = 0; i < functionDataElements.at(3).toElement().childNodes().size(); i +=1)
                arguments << functionDataElements.at(3).toElement().childNodes().at(i).toElement().text();

            if (dimension == 2)
            {
                m_model->addPlaneCurve(Analitza::Expression(lambda), name);
            }
            else if (dimension == 3)
            {
                if (arguments.size() == 1&& arguments.first() == "t")
                {
                    m_model->addSpaceCurve(Analitza::Expression(lambda), name);
                }
                else
                {
//                     qDebug() << "("+arguments.join(",")+")->"+ lambda;
                    m_model->addSurface(Analitza::Expression("("+arguments.join(",")+")->"+ lambda), name);
                }
            }
            functionElement = functionElement.nextSiblingElement("function");
        }
    }
    
    emit loaded();
}

///

PlotsDictionaryModel::PlotsDictionaryModel(QObject* parent): PlotsProxyModel(parent)
{
    m_model = new PlotsModel(this);
    m_model->setCheckable(false);
    
}

PlotsDictionaryModel::~PlotsDictionaryModel()
{

}

void PlotsDictionaryModel::setupModelSourceModel()
{
    setFilterSpaceDimension(All);
    setSourceModel(m_model);
}

void PlotsDictionaryModel::registerError(const QString& error)
{
    m_errors.append(error);
}

///TODO
//Diseniar encapsulando en una clase ... 
PlotsDictionaryModel* getDictionary(QObject *parent)
{
    PlotsDictionaryModel *ret = new PlotsDictionaryModel(parent);

   QThread* thread = new QThread(parent);
   
   DictionaryLoader* loader = new DictionaryLoader(ret->m_model);

   loader->moveToThread(thread);

   ret->connect(thread, SIGNAL(started()), loader, SLOT(load()));
   ret->connect(loader, SIGNAL(errorFound(QString)),ret, SLOT(registerError(QString)));
   ret->connect(loader, SIGNAL(loaded()), ret, SLOT(setupModelSourceModel()));
   ret->connect(loader, SIGNAL(loaded()), thread, SLOT(quit()));
//    ret->connect(loader, SIGNAL(loaded()), thread, SLOT(deleteLater()));
   ret->connect(loader, SIGNAL(loaded()), loader, SLOT(deleteLater()));

    thread->start();
    
    return ret;
}
