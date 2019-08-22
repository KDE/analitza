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


#ifndef GRAPH2DMOBILE_H
#define GRAPH2DMOBILE_H

#include <QtQuick/QQuickPaintedItem>
#include <analitzaplot/plotter2d.h>
#include <QPixmap>

namespace Analitza {
class Variables;
}

class Graph2DMobile : public QQuickItem, public Analitza::Plotter2D
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelHasChanged)
    Q_PROPERTY(QRectF viewport READ lastViewport WRITE setViewport RESET resetViewport)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool showMinorGrid READ showMinorGrid WRITE setShowMinorGrid)
    Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio)
    Q_PROPERTY(bool currentFunction READ currentFunction WRITE setCurrentFunction)
    Q_PROPERTY(bool ticksShown READ ticksShownAtAll WRITE setTicksShownAtAll)
    Q_PROPERTY(bool minorTicksShown READ minorTicksShown WRITE setShowMinorTicks)
    Q_PROPERTY(QStringList filters READ filters CONSTANT)
    public:
        explicit Graph2DMobile(QQuickItem* parent = nullptr);
        
        virtual void forceRepaint() override;
        virtual void viewportChanged() override {}
        virtual void modelChanged() override;
        virtual int currentFunction() const override { return m_currentFunction; }
        
        void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
        QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
        
        void setCurrentFunction(int f) { m_currentFunction = f; }
        bool ticksShownAtAll() const { return ticksShown()!=0; }
        void setTicksShownAtAll(bool shown);
        
        QStringList filters() const;


    public Q_SLOTS:
        void translate(qreal x, qreal y);
        void scale(qreal s, int x, int y);
        void resetViewport();
        QStringList addFunction(const QString& expression, const QSharedPointer<Analitza::Variables>& vars = {});
        bool save(const QUrl &url) const;
        
    Q_SIGNALS:
        void showGridChanged() override;
        void modelHasChanged();

    private:
        void paint();

        bool m_dirty;
        int m_currentFunction;
        
        QImage m_buffer;
        QRectF defViewport;
};

#endif // GRAPH2DMOBILE_H
