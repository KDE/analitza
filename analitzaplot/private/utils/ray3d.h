// /****************************************************************************
// **
// ** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
// ** Contact: http://www.qt-project.org/
// **
// ** This file is part of the Qt3D module of the Qt Toolkit.
// **
// ** $QT_BEGIN_LICENSE:LGPL$
// ** GNU Lesser General Public License Usage
// ** This file may be used under the terms of the GNU Lesser General Public
// ** License version 2.1 as published by the Free Software Foundation and
// ** appearing in the file LICENSE.LGPL included in the packaging of this
// ** file. Please review the following information to ensure the GNU Lesser
// ** General Public License version 2.1 requirements will be met:
// ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
// **
// ** In addition, as a special exception, Nokia gives you certain additional
// ** rights. These rights are described in the Nokia Qt LGPL Exception
// ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
// **
// ** GNU General Public License Usage
// ** Alternatively, this file may be used under the terms of the GNU General
// ** Public License version 3.0 as published by the Free Software Foundation
// ** and appearing in the file LICENSE.GPL included in the packaging of this
// ** file. Please review the following information to ensure the GNU General
// ** Public License version 3.0 requirements will be met:
// ** http://www.gnu.org/copyleft/gpl.html.
// **
// ** Other Usage
// ** Alternatively, this file may be used in accordance with the terms and
// ** conditions contained in a signed written agreement between you and Nokia.
// **
// **
// **
// **
// **
// **
// ** $QT_END_LICENSE$
// **
// ****************************************************************************/
// 
// #ifndef __QRAY3D_H__
// #define __QRAY3D_H__
// 
// #include <QtGui/qvector3d.h>
// #include <QtGui/qmatrix4x4.h>
// 
// class Ray3D
// {
// public:
//     Ray3D();
//     Ray3D(const QVector3D &origin, const QVector3D &direction);
// 
//     QVector3D origin() const;
//     void setOrigin(const QVector3D & value);
// 
//     QVector3D direction() const;
//     void setDirection(const QVector3D & value);
// 
//     bool contains(const QVector3D &point) const;
//     bool contains(const Ray3D &ray) const;
// 
//     QVector3D point(qreal t) const;
//     qreal fromPoint(const QVector3D &point) const;
// 
//     QVector3D project(const QVector3D &vector) const;
// 
//     qreal distanceTo(const QVector3D &point) const;
// 
//     void transform(const QMatrix4x4 &matrix);
//     Ray3D transformed(const QMatrix4x4 &matrix) const;
// 
//     bool operator==(const Ray3D &other);
//     bool operator!=(const Ray3D &other);
// 
// private:
//     QVector3D m_origin;
//     QVector3D m_direction;
// 
// };
// 
// inline Ray3D::Ray3D() : m_direction(1.0f, 0.0f, 0.0f) {}
// 
// inline Ray3D::Ray3D(const QVector3D &origin, const QVector3D &direction)
//     : m_origin(origin)
//     , m_direction(direction)
// {
// }
// 
// inline QVector3D Ray3D::origin() const
// {
//     return m_origin;
// }
// 
// inline void Ray3D::setOrigin(const QVector3D &value)
// {
//     m_origin = value;
// }
// 
// inline QVector3D Ray3D::direction() const
// {
//     return m_direction;
// }
// 
// inline void Ray3D::setDirection(const QVector3D & value)
// {
//     m_direction = value;
// }
// 
// inline QVector3D Ray3D::point(qreal t) const
// {
//     return m_origin + t * m_direction;
// }
// 
// inline void Ray3D::transform(const QMatrix4x4 &matrix)
// {
//     m_origin = matrix * m_origin;
//     m_direction = matrix.mapVector(m_direction);
// }
// 
// inline Ray3D Ray3D::transformed(const QMatrix4x4 &matrix) const
// {
//     return Ray3D(matrix * m_origin, matrix.mapVector(m_direction));
// }
// 
// inline bool Ray3D::operator==(const Ray3D &other)
// {
//     return m_origin == other.origin() && m_direction == other.direction();
// }
// 
// inline bool Ray3D::operator!=(const Ray3D &other)
// {
//     return m_origin != other.origin() || m_direction != other.direction();
// }
// 
// inline bool qFuzzyCompare(const Ray3D &ray1, const Ray3D &ray2)
// {
//     return qFuzzyCompare(ray1.origin(), ray2.origin()) &&
//            qFuzzyCompare(ray1.direction(), ray2.direction());
// }
// 
// #ifndef QT_NO_DEBUG_STREAM
// QDebug operator<<(QDebug dbg, const Ray3D &ray);
// #endif
// 
// #ifndef QT_NO_DATASTREAM
// QDataStream &operator<<(QDataStream &stream, const Ray3D &ray);
// QDataStream &operator>>(QDataStream &stream, Ray3D &ray);
// #endif
// 
// #endif
