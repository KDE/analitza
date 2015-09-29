/*************************************************************************************
 *  Copyright (C) 2015 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "graph3ditem.h"
#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <analitzaplot/plotsmodel.h>

using namespace Analitza;

Plotter3DRenderer::Plotter3DRenderer(Graph3DItem* item)
    : QObject(item)
    , m_item(item)
{}

void Plotter3DRenderer::renderGL()
{
    m_item->window()->update();
}

Graph3DItem::Graph3DItem(QQuickItem* parent)
    : QQuickFramebufferObject(parent)
    , m_plotter(new Plotter3DRenderer(this))
{
    //FIXME
    m_plotter->setUseSimpleRotation(true);
    setModel(new PlotsModel(this));

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    setMirrorVertically(true);
#endif
}

Graph3DItem::~Graph3DItem()
{}

QStringList Graph3DItem::addFunction(const QString& expression, Analitza::Variables* vars)
{
    PlotsModel* plotsmodel = qobject_cast<PlotsModel*>(m_plotter->model());
	if(!plotsmodel)
		qWarning() << "only can add plots to a PlotsModel instance";

    qDebug() << "add!";
	return plotsmodel->addFunction(expression, Dim3D, vars);
}

QAbstractItemModel* Graph3DItem::model() const
{
    return m_plotter->model();
}

void Graph3DItem::setModel(QAbstractItemModel* model)
{
    disconnect(m_plotter->model(), 0, this, 0);

    m_plotter->setModel(model);

    connect(model, &QAbstractItemModel::dataChanged, m_plotter, [this](const QModelIndex& start, const QModelIndex& end)
        { m_plotter->updatePlots(QModelIndex(), start.row(), end.row()); }
    );

    auto updateCount = [this](const QModelIndex &parent, int start, int end) { m_plotter->updatePlots(parent, start, end); };
    connect(model, &QAbstractItemModel::rowsInserted, this, updateCount);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, updateCount);
}

void Graph3DItem::rotate(qreal x, qreal y)
{
    m_plotter->rotate(x, y);
    update();
}

void Graph3DItem::scale(qreal s)
{
    m_plotter->scale(s);
}

class Plotter3DFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    Plotter3DFboRenderer(Plotter3DRenderer* plotter)
        : m_plotter(plotter)
    {
        m_plotter->initGL();
    }

    void render() {
        m_plotter->drawPlots();
    }

private:
    Plotter3DRenderer* m_plotter;
};

QQuickFramebufferObject::Renderer* Graph3DItem::createRenderer() const
{
    m_plotter->setViewport(QRectF({0,0}, QSizeF(width(), height())));
    return new Plotter3DFboRenderer(m_plotter);
}
