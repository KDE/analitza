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
#include "plotsmodel.h"
#include <analitza/expression.h>
#include <analitza/expressionstream.h>
#include <analitzaplot/functiongraph.h>
#include <analitzaplot/plotsfactory.h>
#include <QFile>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

using namespace Analitza;

PlotsDictionaryModel::PlotsDictionaryModel(QObject* parent)
    : QStandardItemModel(parent)
    , m_currentItem(-1)
{
    setHorizontalHeaderLabels(QStringList() << QCoreApplication::translate("@title:column", "Name"));
}

PlotsDictionaryModel::~PlotsDictionaryModel()
{}

void PlotsDictionaryModel::createDictionary(const QString& file)
{
    QFile device(file);
    if (device.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&device);
        Analitza::ExpressionStream s(&stream);
        while(!s.atEnd()) {
            Analitza::Expression expression(s.next());
            Q_ASSERT(expression.isCorrect());
            Q_ASSERT(!expression.name().isEmpty());
            QStringList comments = expression.comments();

            QStandardItem* item = new QStandardItem;
            item->setText(expression.name());
            if(!comments.isEmpty())
                item->setToolTip(QCoreApplication::translate("dictionary", comments.first().trimmed().toUtf8())); //see Messages.sh for more info
            item->setData(expression.toString(), ExpressionRole);
            item->setData(file, FileRole);
            appendRow(item);
        }
    } else
        qWarning() << "couldn't open" << file;
}

void PlotsDictionaryModel::createAllDictionaries()
{
//     QStringList res = KGlobal::dirs()->findAllResources("data", "libanalitza/plots/*.plots");
    QStringList res = QStandardPaths::locateAll(QStandardPaths::DataLocation, QStringLiteral("libanalitza/plots"));
    foreach(const QString& dir, res) {
        QDir d(dir);
        foreach(const QString& f, d.entryList(QStringList("*.plots"))) {
            createDictionary(f);
        }
    }
}

PlotsModel* PlotsDictionaryModel::plotModel()
{
    if(!m_plots) {
        m_plots = new PlotsModel(this);
        updatePlotsModel();
    }
    return m_plots;
}

int PlotsDictionaryModel::currentRow() const
{
    return m_currentItem;
}

void PlotsDictionaryModel::setCurrentRow(int row)
{
    if(row == m_currentItem)
        return;
    m_currentItem = row;
    if(m_plots)
        updatePlotsModel();
}

void PlotsDictionaryModel::updatePlotsModel()
{
    Q_ASSERT(m_plots);
    m_plots->clear();
    if(m_currentItem<0)
        return;

    QModelIndex idx = index(m_currentItem, 0);
    Analitza::Expression exp(idx.data(ExpressionRole).toString());
    PlotBuilder req = PlotsFactory::self()->requestPlot(exp, Dim2D);

    if (!req.canDraw()){ // preference is given to 2D
        PlotBuilder req = PlotsFactory::self()->requestPlot(exp, Dim3D);
        Q_ASSERT(req.canDraw());
        m_plots->addPlot(req.create(Qt::blue, idx.data(Qt::DisplayRole).toString()));
        return;
    }
    m_plots->addPlot(req.create(Qt::blue, idx.data(Qt::DisplayRole).toString()));
}

Analitza::Dimension PlotsDictionaryModel::dimension()
{
    return Dimension(m_plots->index(0,0).data(PlotsModel::DimensionRole).toInt());
}

void PlotsDictionaryModel::setCurrentIndex(const QModelIndex& idx)
{
    setCurrentRow(idx.row());
}
