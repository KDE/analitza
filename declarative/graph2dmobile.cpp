/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "graph2dmobile.h"
#include <QPainter>
#include <QEvent>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <analitza/variables.h>
#include <analitzaplot/planecurve.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/plotsfactory.h>

using namespace Analitza;

Graph2DMobile::Graph2DMobile(QQuickItem* parent)
    : QQuickItem(parent), Plotter2D(boundingRect().size())
    , m_dirty(true), m_currentFunction(-1)
{
    setSize(QSizeF(100,100));
    
    defViewport = QRectF(QPointF(-5., 5.), QSizeF(10., -10.));
    resetViewport();
    setFlags(QQuickItem::ItemHasContents);
}

void Graph2DMobile::paint()
{
    if (!m_dirty)
        return;

//     qDebug() << "hellooo" << boundingRect();
    const auto dpr = window()->effectiveDevicePixelRatio();
    const QSize bounding = (boundingRect().size() * dpr).toSize();
    if(bounding.isEmpty())
        return;
    
    if(m_buffer.size()!=bounding) {
        m_buffer = QImage(bounding, QImage::Format_ARGB32);
        m_buffer.setDevicePixelRatio(dpr);
        setDevicePixelRatio(dpr);
        setPaintedSize(bounding);
    }
    
    Q_ASSERT(!m_buffer.isNull());
    
    if(m_dirty) {
        m_buffer.fill(Qt::transparent);
        drawFunctions(&m_buffer);
        m_dirty=false;
    }
}

void Graph2DMobile::forceRepaint()
{
    m_dirty=true;
    update();
}

void Graph2DMobile::resetViewport()
{
    setViewport(defViewport);
}

void Graph2DMobile::modelChanged()
{
    Q_EMIT modelHasChanged();
}

void Graph2DMobile::scale(qreal s, int x, int y)
{
    QRectF userViewport = lastUserViewport();
    if(s>1 || (userViewport.height() < -3. && userViewport.width() > 3.)) {
        scaleViewport(s, QPoint(x,y));
    }
}

void Graph2DMobile::translate(qreal x, qreal y)
{
    moveViewport(QPoint(x,y) * window()->effectiveDevicePixelRatio());
}

QStringList Graph2DMobile::addFunction(const QString& expression, const QSharedPointer<Analitza::Variables>& vars)
{
    PlotsModel* plotsmodel = qobject_cast<PlotsModel*>(model());
    if(!plotsmodel)
        qWarning() << "only can add plots to a PlotsModel instance";
    else
        return plotsmodel->addFunction(expression, Dim2D, vars);
    return {};
}

void Graph2DMobile::setTicksShownAtAll(bool shown)
{
    Qt::Orientations show = shown ? Qt::Vertical|Qt::Horizontal : Qt::Orientations(0);
    setShowTicks(show);
    setShowTickLabels(show);
}

void Graph2DMobile::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    m_dirty = true;
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode* Graph2DMobile::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* /*data*/)
{
    if (!window()) {
        delete node;
        return nullptr;
    }

    QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(node);
    if (!n) {
        n = new QSGSimpleTextureNode();
        n->setOwnsTexture(true);
    }
    paint();
    n->setTexture(window()->createTextureFromImage(m_buffer));
    n->setRect(boundingRect());
    return n;
}

QStringList Graph2DMobile::filters() const
{
    return {QObject::tr("PNG Image (*.png)")};
}

bool Graph2DMobile::save(const QUrl& url) const
{
    if(!url.isLocalFile())
        return false;

    return m_buffer.save(url.toLocalFile());
}
