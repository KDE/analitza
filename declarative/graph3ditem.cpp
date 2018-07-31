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
#include <QTimer>
#include <QQuickItemGrabResult>
#include <QGuiApplication>
#include <QElapsedTimer>

using namespace Analitza;

Plotter3DRenderer::Plotter3DRenderer(Graph3DItem* item)
    : QObject(item)
    , m_item(item)
{}

void Plotter3DRenderer::renderGL()
{
    QTimer::singleShot(0, m_item, &Graph3DItem::update);
}

QQuickWindow* Plotter3DRenderer::window() const
{
    return m_item->window();
}

Graph3DItem::Graph3DItem(QQuickItem* parent)
    : QQuickFramebufferObject(parent)
    , m_plotter(new Plotter3DRenderer(this))
{
    //FIXME
    m_plotter->setUseSimpleRotation(true);
    setModel(new PlotsModel(this));

    setMirrorVertically(true);
}

Graph3DItem::~Graph3DItem()
{}

QStringList Graph3DItem::addFunction(const QString& expression, const QSharedPointer<Analitza::Variables>& vars)
{
    PlotsModel* plotsmodel = qobject_cast<PlotsModel*>(m_plotter->model());
    if(!plotsmodel)
        qWarning() << "only can add plots to a PlotsModel instance";
    else
        return plotsmodel->addFunction(expression, Dim3D, vars);
    return {};
}

QAbstractItemModel* Graph3DItem::model() const
{
    return m_plotter->model();
}

void Graph3DItem::setModel(QAbstractItemModel* model)
{
    if (m_plotter->model())
        disconnect(m_plotter->model(), nullptr, this, nullptr);

    m_plotter->setModel(model);

    connect(model, &QAbstractItemModel::dataChanged, m_plotter, [this](const QModelIndex& start, const QModelIndex& end)
        { m_plotter->updatePlots(QModelIndex(), start.row(), end.row()); }
    );

    auto updateCount = [this](const QModelIndex &parent, int start, int end) { m_plotter->updatePlots(parent, start, end); };
    connect(model, &QAbstractItemModel::rowsInserted, this, updateCount);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, updateCount);

    Q_EMIT modelChanged(model);
}

void Graph3DItem::rotate(qreal x, qreal y)
{
    m_plotter->rotate(x, y);
}

void Graph3DItem::scale(qreal s)
{
    m_plotter->scale(s);
}

void Graph3DItem::resetViewport()
{
    m_plotter->resetViewport();
}

bool Graph3DItem::save(const QUrl& url)
{
    return m_plotter->save(url);
}

QStringList Graph3DItem::filters() const
{
    return m_plotter->filters();
}

class Plotter3DFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    Plotter3DFboRenderer(Plotter3DRenderer* plotter)
        : m_plotter(plotter)
    {
        m_plotter->initGL();
    }

    void render() override {
        m_plotter->drawPlots();
        m_plotter->window()->resetOpenGLState();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        return new QOpenGLFramebufferObject(size, format);
    }

private:
    Plotter3DRenderer* m_plotter;
};

QQuickFramebufferObject::Renderer* Graph3DItem::createRenderer() const
{
    m_plotter->setViewport(QRectF({0,0}, QSizeF(width(), height())));
    return new Plotter3DFboRenderer(m_plotter);
}

QImage Plotter3DRenderer::grabImage()
{
    QSharedPointer<QQuickItemGrabResult> imgGrab = m_item->grabToImage();
    QImage ret;
    connect(imgGrab.data(), &QQuickItemGrabResult::ready, this, [imgGrab, &ret]() {
        ret = imgGrab->image();
    });
    QElapsedTimer timer;
    timer.start();
    while(ret.size().isEmpty() && timer.elapsed()<2000) {
        qGuiApp->processEvents();
    }
    return ret;
}
