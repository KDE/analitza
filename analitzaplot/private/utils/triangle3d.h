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

#ifndef __QTRIANGLE3D_H__
#define __QTRIANGLE3D_H__

#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "ray3d.h"
#include "plane3d.h"



class QMatrix4x4;

class Triangle3D
{
public:
    Triangle3D();
    Triangle3D(const QVector3D &p, const QVector3D &q, const QVector3D &r);

    QVector3D p() const;
    void setP(const QVector3D & point);

    QVector3D q() const;
    void setQ(const QVector3D & point);

    QVector3D r() const;
    void setR(const QVector3D & point);

    Plane3D plane() const;
    QVector3D center() const;
    QVector3D faceNormal() const;

    bool contains(const QVector3D &point) const;

    bool intersects(const Ray3D &ray) const;
    qreal intersection(const Ray3D &ray) const;

    void transform(const QMatrix4x4 &matrix);
    Triangle3D transformed(const QMatrix4x4 &matrix) const;

    QVector2D uv(const QVector3D &point) const;

    bool operator==(const Triangle3D &other);
    bool operator!=(const Triangle3D &other);

private:
    QVector3D m_p, m_q, m_r;
};

inline Triangle3D::Triangle3D()
    : m_p(0.0f, 0.0f, 0.0f)
    , m_q(1.0f, 0.0f, 0.0f)
    , m_r(0.0f, 1.0f, 0.0f) {}

inline Triangle3D::Triangle3D(const QVector3D &p, const QVector3D &q, const QVector3D &r)
    : m_p(p)
    , m_q(q)
    , m_r(r) {}

inline QVector3D Triangle3D::p() const
{
    return m_p;
}

inline void Triangle3D::setP(const QVector3D &point)
{
    m_p = point;
}

inline QVector3D Triangle3D::q() const
{
    return m_q;
}

inline void Triangle3D::setQ(const QVector3D &point)
{
    m_q = point;
}

inline QVector3D Triangle3D::r() const
{
    return m_r;
}

inline void Triangle3D::setR(const QVector3D &point)
{
    m_r = point;
}

inline Plane3D Triangle3D::plane() const
{
    return Plane3D(m_p, m_q, m_r);
}

inline QVector3D Triangle3D::center() const
{
    return (m_p + m_q + m_r) / 3.0f;
}

inline QVector3D Triangle3D::faceNormal() const
{
    return QVector3D::crossProduct(m_q - m_p, m_r - m_q);
}

inline bool Triangle3D::operator==(const Triangle3D &other)
{
    return m_p == other.m_p && m_q == other.m_q && m_r == other.m_r;
}

inline bool Triangle3D::operator!=(const Triangle3D &other)
{
    return m_p != other.m_p || m_q != other.m_q || m_r != other.m_r;
}

inline bool qFuzzyCompare
    (const Triangle3D &triangle1, const Triangle3D &triangle2)
{
    return qFuzzyCompare(triangle1.p(), triangle2.p()) &&
           qFuzzyCompare(triangle1.q(), triangle2.q()) &&
           qFuzzyCompare(triangle1.r(), triangle2.r());
}

#ifndef QT_NO_DEBUG_STREAM
 QDebug operator<<(QDebug dbg, const Triangle3D &triangle);
#endif

#ifndef QT_NO_DATASTREAM
 QDataStream &operator<<(QDataStream &stream, const Triangle3D &triangle);
 QDataStream &operator>>(QDataStream &stream, Triangle3D &triangle);
#endif



#endif // QTRIANGLE3D_H
