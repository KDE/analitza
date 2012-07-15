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

#ifndef __QPLANE3D_H__
#define __QPLANE3D_H__

#include <QtGui/qvector3d.h>
#include <QtCore/qnumeric.h>
#include "ray3d.h"

class Plane3D
{
public:
    Plane3D();
    Plane3D(const QVector3D &point, const QVector3D &normal);
    Plane3D(const QVector3D &p, const QVector3D &q, const QVector3D &r);

    QVector3D origin() const;
    void setOrigin(const QVector3D& value);

    QVector3D normal() const;
    void setNormal(const QVector3D& value);

    bool contains(const QVector3D &point) const;
    bool contains(const Ray3D &ray) const;

    bool intersects(const Ray3D &ray) const;
    qreal intersection(const Ray3D &ray) const;

    qreal distanceTo(const QVector3D &point) const;

    void transform(const QMatrix4x4 &matrix);
    Plane3D transformed(const QMatrix4x4 &matrix) const;

    bool operator==(const Plane3D &other);
    bool operator!=(const Plane3D &other);

private:
    QVector3D m_origin;
    QVector3D m_normal;
};

inline Plane3D::Plane3D() : m_normal(1.0f, 0.0f, 0.0f) {}

inline Plane3D::Plane3D(const QVector3D &point, const QVector3D &normal)
    : m_origin(point), m_normal(normal)
{
}

inline Plane3D::Plane3D(const QVector3D &p, const QVector3D &q, const QVector3D &r)
    : m_origin(p), m_normal(QVector3D::crossProduct(q - p, r - q))
{
}

inline QVector3D Plane3D::origin() const
{
    return m_origin;
}

inline void Plane3D::setOrigin(const QVector3D &value)
{
    m_origin = value;
}

inline QVector3D Plane3D::normal() const
{
    return m_normal;
}

inline void Plane3D::setNormal(const QVector3D& value)
{
    m_normal = value;
}

inline void Plane3D::transform(const QMatrix4x4 &matrix)
{
    m_origin = matrix * m_origin;
    m_normal = matrix.mapVector(m_normal);
}

inline Plane3D Plane3D::transformed(const QMatrix4x4 &matrix) const
{
    return Plane3D(matrix * m_origin, matrix.mapVector(m_normal));
}

inline bool Plane3D::operator==(const Plane3D &other)
{
    return m_origin == other.origin() && m_normal == other.normal();
}

inline bool Plane3D::operator!=(const Plane3D &other)
{
    return m_origin != other.origin() || m_normal != other.normal();
}

inline bool qFuzzyCompare(const Plane3D &plane1, const Plane3D &plane2)
{
    return qFuzzyCompare(plane1.origin(), plane2.origin()) &&
           qFuzzyCompare(plane1.normal(), plane2.normal());
}

#ifndef QT_NO_DEBUG_STREAM
 QDebug operator<<(QDebug dbg, const Plane3D &plane);
#endif

#ifndef QT_NO_DATASTREAM
 QDataStream &operator<<(QDataStream &stream, const Plane3D &plane);
 QDataStream &operator>>(QDataStream &stream, Plane3D &plane);
#endif

#endif // QPLANE3D_H
