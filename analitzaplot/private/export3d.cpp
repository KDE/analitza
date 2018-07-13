/*************************************************************************************
 *  Copyright (C) 2016 Aleix Pol Gonzalez <aleixpol@kde.org>                         *
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

#include "export3d.h"

#include <QFile>
#include <QDebug>
#include <QVector3D>
#include <cmath>

#include "surface.h"
#include "plotitem.h"
#include "plotsmodel.h"

using namespace Analitza;

template <typename T>
static QByteArray fromNumbers(const QVector<T>& input)
{
    QByteArray ret;
    foreach(qreal r, input) {
        ret += QByteArray::number(r)+' ';
    }
    ret.chop(1);
    return ret;
}

static QByteArray fromVector3D(const QVector3D &r)
{
    return QByteArray::number(r.x())+' '
         + QByteArray::number(r.y())+' '
         + QByteArray::number(r.z());
}

static QByteArray fromNumbers(const QVector<QVector3D>& input)
{
    QByteArray ret;
    foreach(const QVector3D &r, input) {
        ret += fromVector3D(r);
    }
    ret.chop(1);
    return ret;
}

static QVector<int> makeTriangles(const QVector<uint>& input)
{
    QVector<int> ret;
    int i = 0;
    foreach(uint val, input) {
        ret += val;
        if(i==2) {
            ret += -1;
            i = 0;
        } else
            ++i;
    }
    ret += -1;
    return ret;
}

void Export3D::exportX3D(const QString& path, QAbstractItemModel* model)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "couldn't open" << path;
        return;
    }

    f.write(QByteArrayLiteral("<?xml version='1.0' encoding='UTF-8'?>\n"
    "<!DOCTYPE X3D PUBLIC 'ISO//Web3D//DTD X3D 3.2//EN' 'http://www.web3d.org/specifications/x3d-3.2.dtd'>\n"
    "<X3D profile='Interchange' version='3.2' xmlns:xsd='http://www.w3.org/2001/XMLSchema-instance' "
        "xsd:noNamespaceSchemaLocation='http://www.web3d.org/specifications/x3d-3.2.xsd'>\n"
        "<Scene>\n"));
    for (int i = 0; i < model->rowCount(); ++i)
    {
        const QModelIndex pi = model->index(i, 0);

        if (!pi.isValid())
            continue;

        PlotItem* item = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

        Surface *surf = dynamic_cast<Surface*>(item);
        if (!surf || !surf->isVisible())
            continue;

        f.write(QByteArrayLiteral(
            "<Shape>\n"
                "<Appearance><Material diffuseColor='1 0 0' specularColor='0.8 0.7 0.5'/></Appearance>\n"
                "<IndexedFaceSet solid='false' normalPerVertex='false' coordIndex='"));
        f.write(fromNumbers(makeTriangles(surf->indexes())));
        f.write(QByteArrayLiteral("'>\n"
                    "<Coordinate point='"));
        f.write(fromNumbers(surf->vertices()));
        f.write(QByteArrayLiteral("'/>\n"
        "<Normal vector='"));
        f.write(fromNumbers(surf->normals()));
        f.write(QByteArrayLiteral(
                "'/>\n"
                "</IndexedFaceSet>\n"
            "</Shape>\n"));
    }
    f.write(QByteArrayLiteral(
        "</Scene>\n"
    "</X3D>\n"));
}

void Export3D::exportSTL(const QString& path, QAbstractItemModel* model)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "couldn't open" << path;
        return;
    }

    f.write(QByteArrayLiteral("solid myplot\n"));
    for (int i = 0; i < model->rowCount(); ++i)
    {
        const QModelIndex pi = model->index(i, 0);

        if (!pi.isValid())
            continue;

        PlotItem* item = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

        Surface *surf = dynamic_cast<Surface*>(item);
        if (!surf || !surf->isVisible())
            continue;

        const auto vertices = surf->vertices();
        const auto indexes = surf->indexes();

        for (int i = 0, c = indexes.count()/3; i<c; ++i) {
//             f.write("  facet normal " + fromVector3D(normals[i]) + '\n');
            const QVector3D v1 = vertices[indexes[i*3 + 0]]
                          , v2 = vertices[indexes[i*3 + 1]]
                          , v3 = vertices[indexes[i*3 + 2]];

            //TODO: should be using the normals from Surface
            f.write("  facet normal " + fromVector3D(QVector3D::normal(v1, v2, v3)) + '\n');
            f.write("     outer loop\n");
            f.write("       vertex " + fromVector3D(v1) + '\n');
            f.write("       vertex " + fromVector3D(v2) + '\n');
            f.write("       vertex " + fromVector3D(v3) + '\n');
            f.write("     endloop\n");
            f.write("  endfacet\n");
        }
        f.write("\n");
    }
    f.write("endsolid myplot\n");
}
