/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
 *  Copyright (C) 2007 by Abderrahman Taha: Basic OpenGL calls like scene, lights    *
 *                                          and mouse behaviour taken from K3DSurf   *
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

#include "plotter3d_es.h"

#include "surface.h"
#include "spacecurve.h"

#include "plotsmodel.h"
#include "private/utils/mathutils.h"
#include "private/export3d.h"

#include <cmath>
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <QUrl>
#include <QPrinter>
#include <QPainter>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

using namespace std;
using namespace Analitza;

const GLubyte Plotter3DES::XAxisArrowColor[] = {250 -1, 1, 1};
const GLubyte Plotter3DES::YAxisArrowColor[] = {1, 255 - 1, 1};
const GLubyte Plotter3DES::ZAxisArrowColor[] = {1, 1, 255 - 1};

static QOpenGLBuffer createSurfaceBuffer(Surface* surf)
{
    Q_ASSERT(QOpenGLContext::currentContext());

    QOpenGLBuffer buffer(QOpenGLBuffer::VertexBuffer);
    buffer.create();
    buffer.bind();

    const auto offsetNormal = 3*sizeof(float)*surf->vertices().size();
    const auto offsetEnd = offsetNormal + 3*sizeof(float)*surf->normals().size();
    buffer.allocate(surf->vertices().constData(), offsetEnd);
    buffer.write(offsetNormal, surf->normals().constData(), 3*sizeof(float)*surf->normals().size());

    Q_ASSERT(buffer.isCreated());
    buffer.release();

    return buffer;
}

Plotter3DES::Plotter3DES(QAbstractItemModel* model)
    : m_model(model)
    , m_plotStyle(Solid)
    , m_plottingFocusPolicy(All)
    , m_depth(-5)
    , m_scale(60)
    , m_currentAxisIndicator(InvalidAxis)
    , m_simpleRotation(false)
    , m_referencePlaneColor(Qt::darkGray)
{
    resetViewPrivate(QVector3D(-45, 0, -135));
}

Plotter3DES::~Plotter3DES()
{
    for (int i = 0; i < m_itemGeometries.size(); ++i)
    {
        m_itemGeometries.take(itemAt(i)).destroy();
    }

//     glDeleteLists(m_sceneObjects.value(RefPlaneXY), 1);
//     glDeleteLists(m_sceneObjects.value(XArrowAxisHint), 1);
//     glDeleteLists(m_sceneObjects.value(YArrowAxisHint), 1);
//     glDeleteLists(m_sceneObjects.value(ZArrowAxisHint), 1);
}

void Plotter3DES::initGL()
{
    initializeOpenGLFunctions();

    //TODO: provide GLSL version
    program.addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute highp vec4 vertex;\n"
        "attribute highp vec4 normal;\n"
        "uniform highp mat4 matrix;\n"

        "void main(void)\n"
        "{\n"
        "   gl_Position = matrix * vertex;\n"
        "}"
    );
    program.addShaderFromSourceCode(QOpenGLShader::Fragment,
        "uniform mediump vec4 color;\n"

        "void main(void)\n"
        "{\n"
        "   float w = 10.*gl_FragCoord.w;\n"
        "   highp vec4 zvec = vec4(w, w, w, 1.0);"
        "   gl_FragColor = mix(color, zvec, vec4(.5,.5,.5,1.));\n"
        "}"
    );
    program.link();

    if (m_model && m_model->rowCount() > 0) {
        updatePlots(QModelIndex(), 0, m_model->rowCount()-1);
    }
}

void Plotter3DES::resetViewport()
{
    m_rot.setToIdentity();
    resetViewPrivate(QVector3D(-45, 0, -135));
    renderGL();
}

void Plotter3DES::resetViewPrivate(const QVector3D& rot)
{
    m_rot.translate(0,0, -20);
    m_rot.rotate(rot.x(), 1, 0, 0);
    m_rot.rotate(rot.y(), 0, 1, 0);
    m_rot.rotate(rot.z(), 0, 0, 1);
    m_simpleRotationVector = rot;
}

void Plotter3DES::setViewport(const QRectF& vp)
{
    m_viewport = vp;

    m_projection.setToIdentity();
    m_projection.perspective(m_scale, m_viewport.width()/m_viewport.height(), 0.1, 3000);

    renderGL();
}

void Plotter3DES::drawPlots()
{
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!m_model)
    {
        return;
    }

    program.bind();
    program.setUniformValue("matrix", m_projection * m_rot);

    const int vertexLocation = program.attributeLocation("vertex");
    const int normalLocation = program.attributeLocation("normal");
    GLenum mode;
    switch (m_plotStyle)
    {
        case Solid: mode = GL_TRIANGLES; break;
        case Wired: mode = GL_LINES; break;
        case Dots:  mode = GL_POINTS; break;
    }

    drawAxes();
    drawRefPlane();
    program.enableAttributeArray(vertexLocation);
    for (int i = 0, c = m_model->rowCount(); i < c; ++i)
    {
        PlotItem *item = itemAt(i);

        if (!item || item->spaceDimension() != Dim3D || !item->isVisible())
            continue;

        program.setUniformValue("color", item->color());
        if (SpaceCurve *curv = dynamic_cast<SpaceCurve*>(item))
        {
            //TODO: implement jumps
            program.setAttributeArray(vertexLocation, GL_FLOAT, curv->points().constData(), 3);
            glDrawArrays(GL_LINE_STRIP, 0, curv->points().size());
        }
        else if (Surface *surf = dynamic_cast<Surface*>(item))
        {
            QOpenGLBuffer it;
            if(!m_itemGeometries.contains(surf)) {
                m_itemGeometries[surf] = it = createSurfaceBuffer(surf);
            } else {
                it = m_itemGeometries[surf];
            }

            Q_ASSERT(it.isCreated());

            program.enableAttributeArray(normalLocation);
            it.bind();

            const auto offsetNormal = 3*sizeof(float)*surf->vertices().size();

            program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3);
            program.setAttributeBuffer(normalLocation, GL_FLOAT, offsetNormal, 3);
            glDrawElements(mode, surf->indexes().size(), GL_UNSIGNED_INT, surf->indexes().constData());

            if (mode == GL_TRIANGLES) {
                program.setUniformValue("color", QColor(Qt::black));
                glDrawElements(GL_POINTS, surf->indexes().size(), GL_UNSIGNED_INT, surf->indexes().constData());
            }
            it.release();
            program.disableAttributeArray(normalLocation);
        }
    }
    program.disableAttributeArray(vertexLocation);
    program.release();
}

void Plotter3DES::exportSurfaces(const QString& path) const
{
    Export3D::exportX3D(path, m_model);
}

void Plotter3DES::updatePlots(const QModelIndex & parent, int s, int e)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(parent);

    for(int i=s; i<=e; i++) {
        PlotItem *item = itemAt(i);

        if (!item)
            return;

        m_itemGeometries.take(item).destroy();

        if (item->isVisible()) {
            //         addFuncs(QModelIndex(), s.row(), s.row());
            //igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data
            addPlots(item);
        }
    }

    const int count = m_model->rowCount();
    if (count <= e)
    {
        for (int i = e; i < count; ++i)
        {
            m_itemGeometries.take(itemAt(i)).destroy();
        }
    }

    renderGL();
}

void Plotter3DES::setModel(QAbstractItemModel* f)
{
    m_model = f;

    if (m_model)
        updatePlots(QModelIndex(), 0, m_model->rowCount()-1);
    modelChanged();
}

void Plotter3DES::setPlottingFocusPolicy(PlottingFocusPolicy fp)
{
    m_plottingFocusPolicy = fp;

    for (int i = 0; i < m_itemGeometries.size(); ++i)
    {
        m_itemGeometries.take(itemAt(i)).destroy();
    }

    updatePlots(QModelIndex(), 0, m_model->rowCount()-1);
}

void Plotter3DES::scale(qreal factor)
{
    m_scale = qBound(1., factor*m_scale, 140.);

    setViewport(m_viewport);
}

void Plotter3DES::setUseSimpleRotation(bool simplerot)
{
    m_simpleRotation = simplerot;
}

void Plotter3DES::rotate(int dx, int dy)
{
    const qreal ax = -dy;
    const qreal ay = -dx;
    const double angle = sqrt(ax*ax + ay*ay)/(m_viewport.width() + 1)*360.0;

    if (m_simpleRotation) {
        m_rot.setToIdentity();
        resetViewPrivate(m_simpleRotationVector + QVector3D(ax, 0, ay));
        renderGL();
    } else if (!m_rotFixed.isNull()) {
        m_rot.rotate(angle, m_rotFixed.normalized());
        renderGL();
    } else {
//         TODO: figure out how to do this on an opengl es compatible way

//         GLfloat matrix[16] = {0}; // model view matrix from current OpenGL state
//
//         glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
//
//         QMatrix4x4 matrix4(matrix, 4, 4);
//         bool couldInvert;
//         matrix4 = matrix4.inverted(&couldInvert);
//
//         if (couldInvert) {
//             QVector3D rot(matrix4.row(0).x()*ax + matrix4.row(1).x()*ay,
//                           matrix4.row(0).y()*ax + matrix4.row(1).y()*ay,
//                           matrix4.row(0).z()*ax + matrix4.row(1).z()*ay);
//
//             m_rot.rotate(rot.length(), rot.normalized());
//
//             renderGL();
//         }
    }
}

CartesianAxis Plotter3DES::selectAxisArrow(int x, int y)
{
    GLint viewport[4];
    GLubyte pixel[3];

    glGetIntegerv(GL_VIEWPORT,viewport);

    glReadPixels(x, viewport[3]-y, 1, 1, GL_RGB,GL_UNSIGNED_BYTE,(void *)pixel);

    if (memcmp(pixel, XAxisArrowColor, sizeof(pixel)) == 0) return XAxis;
    if (memcmp(pixel, YAxisArrowColor, sizeof(pixel)) == 0) return YAxis;
    if (memcmp(pixel, ZAxisArrowColor, sizeof(pixel)) == 0) return ZAxis;

    return InvalidAxis;
}

void Plotter3DES::fixRotation(const QVector3D& vec)
{
    m_rotFixed = vec;
}

void Plotter3DES::showAxisArrowHint(CartesianAxis axis)
{
    if (axis == InvalidAxis)
        return ;

    m_currentAxisIndicator = axis;

    renderGL();
}

void Plotter3DES::hideAxisHint()
{
    m_currentAxisIndicator = InvalidAxis;

    renderGL();
}

void Plotter3DES::addPlots(PlotItem* item)
{
    Q_ASSERT(item);

    if (SpaceCurve *curve = dynamic_cast<SpaceCurve*>(item))
    {
        if (curve->points().isEmpty())
            curve->update(QVector3D(), QVector3D());
    }
    else if (Surface* surf = dynamic_cast<Surface*>(item))
    {
        if (surf->indexes().isEmpty())
            surf->update(QVector3D(), QVector3D());

        Q_ASSERT(!surf->indexes().isEmpty());
    }
}

PlotItem* Plotter3DES::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return nullptr;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return nullptr;

    return plot;
}

void Plotter3DES::drawAxes()
{
    glLineWidth(1.5f);

    static GLfloat const xVertices[] = {
        0.f, 0.f, 0.f,
        10.f, 0.f, 0.f,
    };
    static GLfloat const yVertices[] = {
        0.f, 0.f, 0.f,
        0.f, 10.f, 0.f,
    };
    static GLfloat const zVertices[] = {
        0.f, 0.f, 0.f,
        0.f, 0.f, 10.f,
    };

    static QVector<GLuint> const idxs = {0,1};

    const int vertexLocation = program.attributeLocation("vertex");
    program.enableAttributeArray(vertexLocation);

    program.setUniformValue("color", QColor(Qt::red));
    program.setAttributeArray(vertexLocation, xVertices, 3);
    glDrawElements(GL_LINES, idxs.size(), GL_UNSIGNED_INT, idxs.constData());

    static GLfloat const XArrowVertices[] = {
        10.f,   0.f, 0.f,

        9.8f,  0.1f, 0.f,
        9.8f,   0.f, 0.1f,
        9.8f, -0.1f, 0.f,
        9.8f,   0.f, -0.1f,
        9.8f,  0.1f, 0.f
    };
    program.setAttributeArray(vertexLocation, XArrowVertices, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    program.setUniformValue("color", QColor(Qt::green));
    program.setAttributeArray(vertexLocation, yVertices, 3);
    glDrawElements(GL_LINES, idxs.size(), GL_UNSIGNED_INT, idxs.constData());

    static GLfloat const YArrowVertices[] = {
           0.f, 10.f,  0.f,

          0.1f, 9.8f,  0.f,
           0.f, 9.8f, 0.1f,
         -0.1f, 9.8f,  0.f,
           0.f, 9.8f,-0.1f,
          0.1f, 9.8f,  0.f
    };
    program.setAttributeArray(vertexLocation, YArrowVertices, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    program.setUniformValue("color", QColor(Qt::blue));
    program.setAttributeArray(vertexLocation, zVertices, 3);
    glDrawElements(GL_LINES, idxs.size(), GL_UNSIGNED_INT, idxs.constData());

    static GLfloat const ZArrowVertices[] = {
           0.f,  0.f, 10.f,
          0.1f,  0.f, 9.8f,
           0.f, 0.1f, 9.8f,
         -0.1f,  0.f, 9.8f,
           0.f,-0.1f, 9.8f,
          0.1f,  0.f, 9.8f
    };
    program.setAttributeArray(vertexLocation, ZArrowVertices, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    program.disableAttributeArray(vertexLocation);
}

void Plotter3DES::drawRefPlane()
{
    glLineWidth(1.f);
    const float lims = 10;
    QVector<QVector3D> vxs;

    for(float x=-lims; x<=lims; ++x) {
        vxs += { x, -lims, m_depth };
        vxs += { x, lims, m_depth };
    }

    for(float y=-lims; y<=lims; ++y) {
        vxs += { -lims, y, m_depth };
        vxs += { lims, y, m_depth };
    }

    const int vertexLocation = program.attributeLocation("vertex");
    program.enableAttributeArray(vertexLocation);
    program.setUniformValue("color", m_referencePlaneColor);
    program.setAttributeArray(vertexLocation, GL_FLOAT, vxs.constData(), 3);
    glDrawArrays(GL_LINES, 0, vxs.size());
    program.disableAttributeArray(vertexLocation);
}

bool Plotter3DES::save(const QUrl& url)
{
    if (!url.isLocalFile())
        return false;

    const QString path = url.toLocalFile();
    if(path.endsWith(QLatin1String(".x3d")) || path.endsWith(QLatin1String(".stl"))) {
        exportSurfaces(path);
    } else if(path.endsWith(QLatin1String(".pdf"))) {
        auto px = grabImage();
//this pulls widgets
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(path);
        printer.setPaperSize(px.size(), QPrinter::DevicePixel);
        printer.setPageMargins(0,0,0,0, QPrinter::DevicePixel);
        QPainter painter;
        painter.begin(&printer);
        painter.drawImage(QPoint(0,0), px);
        painter.end();
    } else {
        auto px = grabImage();
        return px.save(path);
    }
    return true;
}

QStringList Plotter3DES::filters() const
{
    return {QObject::tr("PNG Image (*.png)"), QObject::tr("PDF Document (*.pdf)"), QObject::tr("X3D Document (*.x3d)"), QObject::tr("STL Document (*.stl)")};
}
