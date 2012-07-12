// /*************************************************************************************
//  *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
//  *  Copyright (C) 2008 Rivo Laks <rivolaks@hot.ee>                                   *
//  *                                                                                   * 
//  *  This program is free software; you can redistribute it and/or                    *
//  *  modify it under the terms of the GNU General Public License                      *
//  *  as published by the Free Software Foundation; either version 2                   *
//  *  of the License, or (at your option) any later version.                           *
//  *                                                                                   *
//  *  This program is distributed in the hope that it will be useful,                  *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
//  *  GNU General Public License for more details.                                     *
//  *                                                                                   *
//  *  You should have received a copy of the GNU General Public License                *
//  *  along with this program; if not, write to the Free Software                      *
//  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
//  *************************************************************************************/
// 
// 
// #include "mathutils.h"
// 
// #include <math.h>
// 
// Camera::Camera()
// {
//     mFoV = 45.0f;
//     mAspect = 1.0f;
//     mDepthNear = 1.0f;
//     mDepthFar = 100.0f;
//     mPosition = QVector3D(-10, 0, 10);
//     mLookAt = QVector3D(0, 0, 0);
//     mUp = QVector3D(0, 1, 0);
// }
// 
// Camera::~Camera()
// {
// }
// 
// void Camera::setAspect(float aspect)
// {
//     mAspect = aspect;
//     
//     recalculateProjectionMatrix();
// }
// 
// void Camera::setDepthRange(float near, float far)
// {
//     mDepthNear = near;
//     mDepthFar = far;
//     
//     recalculateProjectionMatrix();
// }
// 
// QRect Camera::viewport() const
// {
//     return mViewport;
// }
// 
// void Camera::setPosition(const QVector3D& pos)
// {
//     mPosition = pos;
//     mModelviewMatrix.translate(mPosition);
// }
// 
// void Camera::setLookAt(const QVector3D& lookat)
// {
//     mLookAt = lookat;
//     mModelviewMatrix.lookAt(mLookAt, mPosition, mUp);
// }
// 
// void Camera::setUp(const QVector3D& up)
// {
//     mUp = up;
//     mModelviewMatrix.lookAt(mLookAt, mPosition, mUp);
// }
// 
// void Camera::setDirection(const QVector3D& dir)
// {
//     setLookAt(mPosition + dir);
// }
// 
// void Camera::setViewport(int x, int y, int width, int height)
// {
//     mViewport = QRect(x,y,width,height);
// }
// 
// void Camera::recalculateProjectionMatrix()
// {
//     // Code from Mesa project, src/glu/sgi/libutil/project.c
//     mProjectionMatrix.setToIdentity();
//     float radians = mFoV / 2 * M_PI / 180;
// 
//     float deltaZ = mDepthFar - mDepthNear;
//     float sine = sin(radians);
//     if ((deltaZ == 0) || (sine == 0) || (mAspect == 0)) {
//         return;
//     }
//     float cotangent = cos(radians) / sine;
// 
//     mProjectionMatrix(0, 0) = cotangent / mAspect;
//     mProjectionMatrix(1, 1) = cotangent;
//     mProjectionMatrix(2, 2) = -(mDepthFar + mDepthNear) / deltaZ;
//     mProjectionMatrix(3, 2) = -1;
//     mProjectionMatrix(2, 3) = -2 * mDepthNear * mDepthFar / deltaZ;
//     mProjectionMatrix(3, 3) = 0;
// }
// 
// void Camera::setModelviewMatrix(const QMatrix4x4& modelview)
// {
//     mModelviewMatrix = modelview;
// }
// 
// void Camera::setProjectionMatrix(const QMatrix4x4& projection)
// {
//     mProjectionMatrix = projection;
// }
// 
// QMatrix4x4 Camera::modelviewMatrix() const
// {
//     return mModelviewMatrix;
// }
// 
// QMatrix4x4 Camera::projectionMatrix() const
// {
//     return mProjectionMatrix;
// }
// 
// ///
// 
// 
// inline float deg2rad(float d)
// {
//     return d * M_PI/180.0f;
// }
// 
// 
// TrackBall::TrackBall()
// {
//     mMatrix.setToIdentity();
// }
// 
// TrackBall::~TrackBall()
// {
// }
// 
// void TrackBall::rotate(float dx, float dy)
// {
//     rotateY(-dx/5);
//     rotateX(-dy/5);
// }
// 
// void TrackBall::rotateX(float degrees)
// {
//     rotate(degrees, QVector3D(1, 0, 0));
// }
// 
// void TrackBall::rotateY(float degrees)
// {
//     rotate(degrees, QVector3D(0, 1, 0));
// }
// 
// void TrackBall::rotateZ(float degrees)
// {
//     rotate(degrees, QVector3D(0, 0, 1));
// }
// 
// void TrackBall::rotate(float degrees, QVector3D axis)
// {
//     float m_angle = deg2rad(degrees);
//     QVector3D m_axis = axis;
// 
//     QMatrix3x3 res;
//     QVector3D sin_axis  = sin(m_angle) * m_axis;
//     float c = cos(m_angle);
//     QVector3D cos1_axis = (1-c) * m_axis;
// 
//     float tmp;
//     tmp = cos1_axis.x() * m_axis.y();
//     res(0,1) = tmp - sin_axis.z();
//     res(1,0) = tmp + sin_axis.z();
// 
//     tmp = cos1_axis.x() * m_axis.z();
//     res(0,2) = tmp + sin_axis.y();
//     res(2,0) = tmp - sin_axis.y();
// 
//     tmp = cos1_axis.y() * m_axis.z();
//     res(1,2) = tmp - sin_axis.x();
//     res(2,1) = tmp + sin_axis.x();
// 
//     QVector3D cwiseOps(cos1_axis.x()*m_axis.x()+c, cos1_axis.y()*m_axis.y()+c, cos1_axis.z()*m_axis.z()+c);
// 
//     res(0,0) = cwiseOps.x(); 
//     res(1,1) = cwiseOps.y();
//     res(2,2) = cwiseOps.z();
//     
//     mMatrix = QMatrix4x4(res);
// }
// 
// QVector3D TrackBall::transform(const QVector3D& v) const
// {
//     return mMatrix * v;
// }
// 
// Camera TrackBall::transform(const Camera& c)
// {
//     Camera cam(c);
//     cam.setPosition(transform(c.position()));
//     cam.setLookAt(transform(c.lookAt()));
//     cam.setUp(transform(c.up()));
//     return cam;
// }
// 
// QVector3D TrackBall::xAxis() const
// {
//     return QVector3D(mMatrix(0,0), mMatrix(0,1), mMatrix(0,2));
// }
// 
// QVector3D TrackBall::yAxis() const
// {
//     return QVector3D(mMatrix(1,0), mMatrix(1,1), mMatrix(1,2));
// }
// 
// QVector3D TrackBall::zAxis() const
// {
//     return QVector3D(mMatrix(2,0), mMatrix(2,1), mMatrix(2,2));
// }
// 
