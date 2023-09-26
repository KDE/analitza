// SPDX-FileCopyrightText: 2015 Aleix Pol <aleixpol@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef GRAPH3DITEM_H
#define GRAPH3DITEM_H

#include <QQuickFramebufferObject>
#include "plotter3d_es.h"
#include <qqmlregistration.h>

class Graph3DItem;

class Plotter3DRenderer : public QObject, public Analitza::Plotter3DES
{
public:
    explicit Plotter3DRenderer(Graph3DItem* item);

    int currentPlot() const override { return 0; }
    void modelChanged() override {}
    void renderGL() override;

    QQuickWindow* window() const;
    QImage grabImage() override;

private:
    Graph3DItem * m_item;
};

class Graph3DItem : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Graph3DView)
    Q_PROPERTY(QStringList filters READ filters CONSTANT)
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    public:
        explicit Graph3DItem(QQuickItem* parent = Q_NULLPTR);
        ~Graph3DItem() override;

        QAbstractItemModel* model() const;
        void setModel(QAbstractItemModel* model);

        QQuickFramebufferObject::Renderer * createRenderer() const override;

        Q_SCRIPTABLE QStringList addFunction(const QString& expression, const QSharedPointer<Analitza::Variables>& vars = {});
        Q_SCRIPTABLE void rotate(qreal x, qreal y);
        Q_SCRIPTABLE void scale(qreal s);
        Q_SCRIPTABLE void resetViewport();

        Q_SCRIPTABLE bool save(const QUrl &url);

        QStringList filters() const;

    Q_SIGNALS:
        void modelChanged(QAbstractItemModel* model);

    private:
        Plotter3DRenderer *m_plotter;
};

#endif
