/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef __QBOX3D_H_
#define __QBOX3D_H_

#include "ray3d.h"


class QMatrix4x4;

class Box3D
{
public:
    Box3D();
    Box3D(const QVector3D& corner1, const QVector3D& corner2);

    bool isNull() const;
    bool isFinite() const;
    bool isInfinite() const;

    QVector3D minimum() const;
    QVector3D maximum() const;
    void setExtents(const QVector3D& corner1, const QVector3D& corner2);

    void setToNull();
    void setToInfinite();

    QVector3D size() const;
    QVector3D center() const;

    bool contains(const QVector3D& point) const;
    bool contains(const Box3D& box) const;

    bool intersects(const Ray3D& ray) const;
    bool intersection(const Ray3D &ray, qreal *minimum_t, qreal *maximum_t) const;
    qreal intersection(const Ray3D& ray) const;

    bool intersects(const Box3D& box) const;
    void intersect(const Box3D& box);
    Box3D intersected(const Box3D& box) const;

    void unite(const QVector3D& point);
    void unite(const Box3D& box);

    Box3D united(const QVector3D& point) const;
    Box3D united(const Box3D& box) const;

    void transform(const QMatrix4x4& matrix);
    Box3D transformed(const QMatrix4x4& matrix) const;

    bool operator==(const Box3D& box) const;
    bool operator!=(const Box3D& box) const;

    friend bool qFuzzyCompare(const Box3D& box1, const Box3D& box2);

private:
    enum Type
    {
        Null,
        Finite,
        Infinite
    };

    Box3D::Type boxtype;
    QVector3D mincorner, maxcorner;
};

inline Box3D::Box3D() : boxtype(Null), mincorner(0, 0, 0), maxcorner(0, 0, 0) {}

inline Box3D::Box3D(const QVector3D& corner1, const QVector3D& corner2)
    : boxtype(Finite),
      mincorner(qMin(corner1.x(), corner2.x()),
                qMin(corner1.y(), corner2.y()),
                qMin(corner1.z(), corner2.z())),
      maxcorner(qMax(corner1.x(), corner2.x()),
                qMax(corner1.y(), corner2.y()),
                qMax(corner1.z(), corner2.z())) {}

inline bool Box3D::isNull() const { return (boxtype == Null); }
inline bool Box3D::isFinite() const { return (boxtype == Finite); }
inline bool Box3D::isInfinite() const { return (boxtype == Infinite); }

inline QVector3D Box3D::minimum() const { return mincorner; }
inline QVector3D Box3D::maximum() const { return maxcorner; }

inline void Box3D::setExtents(const QVector3D& corner1, const QVector3D& corner2)
{
    boxtype = Finite;
    mincorner = QVector3D(qMin(corner1.x(), corner2.x()),
                          qMin(corner1.y(), corner2.y()),
                          qMin(corner1.z(), corner2.z()));
    maxcorner = QVector3D(qMax(corner1.x(), corner2.x()),
                          qMax(corner1.y(), corner2.y()),
                          qMax(corner1.z(), corner2.z()));
}

inline void Box3D::setToNull()
{
    boxtype = Null;
    mincorner = QVector3D(0, 0, 0);
    maxcorner = QVector3D(0, 0, 0);
}

inline void Box3D::setToInfinite()
{
    boxtype = Infinite;
    mincorner = QVector3D(0, 0, 0);
    maxcorner = QVector3D(0, 0, 0);
}

inline QVector3D Box3D::size() const { return maxcorner - mincorner; }
inline QVector3D Box3D::center() const { return (mincorner + maxcorner) * 0.5f; }

inline bool Box3D::contains(const QVector3D& point) const
{
    if (boxtype == Finite) {
        return (point.x() >= mincorner.x() && point.x() <= maxcorner.x() &&
                point.y() >= mincorner.y() && point.y() <= maxcorner.y() &&
                point.z() >= mincorner.z() && point.z() <= maxcorner.z());
    } else if (boxtype == Infinite) {
        return true;
    } else {
        return false;
    }
}

inline bool Box3D::contains(const Box3D& box) const
{
    if (box.boxtype == Finite)
        return contains(box.mincorner) && contains(box.maxcorner);
    else if (box.boxtype == Infinite)
        return (boxtype == Infinite);
    else
        return false;
}

inline bool Box3D::operator==(const Box3D& box) const
{
    return (boxtype == box.boxtype &&
            mincorner == box.mincorner &&
            maxcorner == box.maxcorner);
}

inline bool Box3D::operator!=(const Box3D& box) const
{
    return (boxtype != box.boxtype ||
            mincorner != box.mincorner ||
            maxcorner != box.maxcorner);
}

inline bool qFuzzyCompare(const Box3D& box1, const Box3D& box2)
{
    return box1.boxtype == box2.boxtype &&
           qFuzzyCompare(box1.mincorner, box2.mincorner) &&
           qFuzzyCompare(box1.maxcorner, box2.maxcorner);
}

#ifndef QT_NO_DEBUG_STREAM
 QDebug operator<<(QDebug dbg, const Box3D &box);
#endif

#ifndef QT_NO_DATASTREAM
 QDataStream &operator<<(QDataStream &stream, const Box3D &box);
 QDataStream &operator>>(QDataStream &stream, Box3D &box);
#endif


#endif
