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

#include <cmath>
#include <QDebug>
#include <QFile>
#include <QOpenGLFunctions>
#include <QPixmap>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

using namespace std;
using namespace Analitza;

Q_DECLARE_METATYPE(PlotItem*)

const GLubyte Plotter3DES::XAxisArrowColor[] = {250 -1 , 1, 1};
const GLubyte Plotter3DES::YAxisArrowColor[] = {1, 255 - 1, 1};
const GLubyte Plotter3DES::ZAxisArrowColor[] = {1, 1, 255 - 1};

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

    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
//     glEnable(GL_CULL_FACE);

    program.addShaderFromSourceCode(QOpenGLShader::Vertex,
        "#version 130\n" //TODO: compute version at runtime
        "attribute highp vec4 vertex;\n"
        "attribute highp vec4 normal;\n"
        "varying vec4 vx;\n"
        "varying vec4 vertNormal;\n"
        "varying vec4 lp;\n"
        "uniform highp mat4 matrix;\n"
        "uniform highp mat4 projection;\n"
        "uniform highp vec4 lightPos;\n"

        "void main(void)\n"
        "{\n"
        "   gl_Position = matrix * vertex;\n"
        "   vx = vertex;\n"
        "   lp = lightPos * matrix;\n"
        "   vertNormal = normalize(normal * vertex);\n"
        "}"
    );
    program.addShaderFromSourceCode(QOpenGLShader::Fragment,
        "#version 130\n" //TODO: compute version at runtime
        "uniform mediump vec4 color;\n"

        "in highp vec4 lp;\n"
        "in highp vec4 vx;\n"
        "in highp vec4 vertNormal;\n"
        "void main(void)\n"
        "{\n"
        "   highp vec4 Lv = normalize(lp - vx);\n"
        "   highp float incidence = max(dot(vertNormal, Lv), 0.5);\n"
        "   highp vec3 col = incidence * color.rgb;\n"
        "   gl_FragColor = vec4(col, 1.0);\n"
        "}"
    );
    program.link();
}

void Plotter3DES::resetView()
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!m_model)
    {
        return;
    }

    program.bind();
    program.setUniformValue("projection", m_projection);
    program.setUniformValue("matrix", m_projection * m_rot);

    program.setUniformValue("lightPos", QVector4D { 500.0, 500.0, 500.0, 0 });

    const int vertexLocation = program.attributeLocation("vertex");
    const int normalLocation = program.attributeLocation("normal");

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
            if(!m_itemGeometries.contains(item))
                continue;
            program.enableAttributeArray(normalLocation);
            auto it = m_itemGeometries[item];
            it.bind();

            const auto offsetNormal = 3*sizeof(float)*surf->vertices().size();

            program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3);
            program.setAttributeBuffer(normalLocation, GL_FLOAT, offsetNormal, 3);
            glDrawElements(GL_TRIANGLES, surf->indexes().size(), GL_UNSIGNED_INT, surf->indexes().constData());
            it.release();
            program.disableAttributeArray(normalLocation);
        }
    }
    program.disableAttributeArray(vertexLocation);
    program.release();
}

template <typename T>
QByteArray fromNumbers(const QVector<T>& input)
{
    QByteArray ret;
    foreach(qreal r, input) {
        ret += QByteArray::number(r)+QByteArrayLiteral(" ");
    }
    ret.chop(1);
    return ret;
}

QByteArray fromNumbers(const QVector<QVector3D>& input)
{
    QByteArray ret;
    foreach(const QVector3D& r, input) {
        ret += QByteArray::number(r.x())+QByteArrayLiteral(" ");
        ret += QByteArray::number(r.y())+QByteArrayLiteral(" ");
        ret += QByteArray::number(r.z())+QByteArrayLiteral(" ");
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

void Plotter3DES::exportSurfaces(const QString& path) const
{
    QFile f(path);
    bool b = f.open(QIODevice::WriteOnly | QIODevice::Text);
    Q_ASSERT(b);
    f.write(QByteArrayLiteral("<?xml version='1.0' encoding='UTF-8'?>\n"
    "<!DOCTYPE X3D PUBLIC 'ISO//Web3D//DTD X3D 3.2//EN' 'http://www.web3d.org/specifications/x3d-3.2.dtd'>\n"
    "<X3D profile='Interchange' version='3.2' xmlns:xsd='http://www.w3.org/2001/XMLSchema-instance' "
        "xsd:noNamespaceSchemaLocation='http://www.web3d.org/specifications/x3d-3.2.xsd'>\n"
        "<Scene>\n"));
    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        PlotItem *item = itemAt(i);

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

void Plotter3DES::updatePlots(const QModelIndex & parent, int s, int e)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(parent);

    for(int i=s; i<=e; i++) {
        PlotItem *item = itemAt(i);

        if (!item)
            return;

        m_itemGeometries[item].destroy();

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
            m_itemGeometries.remove(itemAt(i));
        }
    }

    renderGL();
}

void Plotter3DES::setModel(QAbstractItemModel* f)
{
    m_model = f;

    modelChanged();
}

void Plotter3DES::setPlottingFocusPolicy(PlottingFocusPolicy fp)
{
    m_plottingFocusPolicy = fp;

    for (int i = 0; i < m_itemGeometries.size(); ++i)
    {
        m_itemGeometries[itemAt(i)].destroy();
    }

    updatePlots(QModelIndex(), 0, m_model->rowCount()-1);
}

void Plotter3DES::scale(qreal factor)
{
    m_scale = qBound(1., factor*m_scale, 140.);
    setViewport(m_viewport);
    renderGL();
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
        Q_UNUSED(curve);
    }
    else if (Surface* surf = dynamic_cast<Surface*>(item))
    {
        if (surf->indexes().isEmpty())
            surf->update(QVector3D(), QVector3D());

        Q_ASSERT(!surf->indexes().isEmpty());

        QOpenGLBuffer buffer(QOpenGLBuffer::VertexBuffer);
        buffer.create();
        buffer.bind();

        const auto offsetNormal = 3*sizeof(float)*surf->vertices().size();
        const auto offsetEnd = offsetNormal + 3*sizeof(float)*surf->normals().size();
        buffer.allocate(surf->vertices().constData(), offsetEnd);
        buffer.write(offsetNormal, surf->normals().constData(), 3*sizeof(float)*surf->normals().size());

        Q_ASSERT(buffer.isCreated());

        m_itemGeometries[item] = buffer;
        buffer.release();
    }
}

PlotItem* Plotter3DES::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}

void Plotter3DES::drawAxes()
{
    glLineWidth(1.5f);

    program.setUniformValue("color", QColor(Qt::red));
    static GLfloat const axisVertices[] = {
        0.f, 0.f, 0.f,

        10.f, .0f, 0.f,
        0.f, 10.f, 0.f,
        0.f, 0.f, 10.f
    };
    static QVector<GLuint> const idxs = {0,1,0,2,0,3};


    const int vertexLocation = program.attributeLocation("vertex");
    program.enableAttributeArray(vertexLocation);

    program.setAttributeArray(vertexLocation, axisVertices, 3);
    glDrawElements(GL_LINES, idxs.size(), GL_UNSIGNED_INT, idxs.constData());

//     program.setUniformValue("color", XAxisArrowColor);
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

//     program.setUniformValue("color", YAxisArrowColor);
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

//     program.setUniformValue("color", ZAxisArrowColor);
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
    const int lims = 10;
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

QPixmap Plotter3DES::renderPixmap() const
{
#warning TODO
    return QPixmap();
}
