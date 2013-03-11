/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "curve.h"


#include "shapedata_p.h"

#include "analitza/analyzer.h"

using namespace Analitza;

class Curve::CurveData : public QSharedData, public ShapeData
{
public:
    CurveData();
    CurveData(const CurveData &other);
    CurveData(const Analitza::Expression& expresssion, Variables* vars);
    ~CurveData();
    
    QScopedPointer<Analyzer> m_analyzer; // internal expression
};


Curve::CurveData::CurveData()
{

}

Curve::CurveData::CurveData(const CurveData& other)
    : QSharedData(other)
{

}

Curve::CurveData::CurveData(const Expression& expresssion, Variables* vars)
{

}

Curve::CurveData::~CurveData()
{

}



///


Curve::Curve()
    : d(new CurveData)
{
    
}

Curve::Curve(const Curve &other)
    : d(other.d)
{
}

Curve::Curve(const Analitza::Expression &expresssion, Variables* vars)
    : d(new CurveData)
{
}

Curve::Curve(const QString &expresssion, Variables* vars)
    : d(new CurveData)
{
}

Curve::~Curve()
{
}



QColor Curve::color() const
{
    return d->m_color;
}

CoordinateSystem Curve::coordinateSystem() const
{
    return d->m_coordinateSystem;
}

Dimension Curve::dimension() const
{
    return d->m_dimension;
}

QStringList Curve::errors() const
{
    return QStringList();
}

Expression Curve::expression() const
{
    return d->m_expression;
}

QString Curve::iconName() const
{
    return d->m_iconName;
}

bool Curve::isValid() const
{
    return false;
}

bool Curve::isVisible() const
{
    return d->m_visible;
}

QString Curve::name() const
{
    return d->m_name;
}

void Curve::plot(const QGLContext* context)
{
//     QGLFunctions gl(context);
//     bool npot = funcs.hasOpenGLFeature(QGLFunctions::NPOTTextures);
//  
//     
//     GLfloat points[9];
// 
//     points[0] = -39.9;
//     points[1] = -0.9;
//     points[2] = -1;
// 
//     points[3] =  0.9;
//     points[4] = -11110.9;
//     points[5] = -13;
// 
//     points[6] =  0.9;
//     points[7] =  10.9;
//     points[8] = -19;
// 
//     unsigned int vbo = 0;
//     glGenBuffers (1, &vbo);
//     glBindBuffer (GL_ARRAY_BUFFER, vbo);
//     glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (GLfloat), points, GL_STATIC_DRAW);
//     
// const GLchar *str = "\n\
// #version 150\n\
// const mat3 projection = mat3(\n\
//  vec3(3.0/4.0, 0.0, 0.0),\n\
//  vec3(    0.0, 1.0, 0.0),\n\
//  vec3(    0.0, 0.0, 1.0)\n\
// );\n\
// varying vec3 vertex;\n\
// void\n\
// main()\n\
// {\n\
//     gl_Position = vec4(vertex, 1.0);\n\
// }\n\
// ";
//     
// const GLchar *strb = "\n\
// #version 150\n\
// out vec4 fragmentColor;\n\
// void\n\
// main()\n\
// {\n\
//     fragmentColor = vec4(0.0, 1.0, 0.0, 1.0);\n\
// }\n\
// ";
//     
//     unsigned int vs = glCreateShader (GL_VERTEX_SHADER);
//     glShaderSource (vs, 1, &str, NULL);
//     glCompileShader (vs);
//     unsigned int fs = glCreateShader (GL_FRAGMENT_SHADER);
//     glShaderSource (fs, 1, &strb, NULL);
//     glCompileShader (fs);
//     
//     unsigned int shader_programme = glCreateProgram ();
//     glAttachShader (shader_programme, fs);
//     glAttachShader (shader_programme, vs);
//     glLinkProgram (shader_programme);
// 
//     glUseProgram (shader_programme);
//     GLint vl = glGetAttribLocation(shader_programme, "vertex");
//     glBindBuffer (GL_ARRAY_BUFFER, vbo);
//     glVertexAttribPointer(vl, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     glEnableVertexAttribArray(vl);
// //     glEnableClientState(GL_VERTEX_ARRAY);
//     glDrawArrays (GL_TRIANGLES, 0, 3);
// //     glDisableClientState(GL_VERTEX_ARRAY);
//     glDisableVertexAttribArray(vl);
//     glUseProgram(0);
//                     
// //         glBegin(GL_POINTS);
// //         glColor3ub(255,0,0);
// //         glVertex3f(0,0.5,0);
// //         glEnd();
}

void Curve::setColor(const QColor &color)
{
    d->m_color = color;
}

void Curve::setName(const QString &name)
{
    d->m_name = name;
}

void Curve::setVisible(bool visible)
{
    d->m_visible = visible;
}

QString Curve::typeName() const
{
    return d->m_typeName;
}

Variables * Curve::variables() const
{
    return d->m_analyzer->variables();
}


// bool Curve::operator==(const Curve &other) const
// {
//     return false; // TODO
// }
// 
// bool Curve::operator!=(const Curve &other) const
// {
//     return !((*this) == other);
// }
// 
// Curve & Curve::operator=(const Curve &other)
// {
//     return *this; //TODO
// }
// //END AbstractShape interface
// 
// //BEGIN AbstractFunctionGraph interface
// QStringList Curve::arguments() const
// {
//     return d->m_arguments.keys();
// }
// 
// QPair<Expression, Expression> Curve::limits(const QString &arg) const
// {
//     return qMakePair(Expression(), Expression());
// }
// 
// QStringList Curve::parameters() const
// {
//     return QStringList(); //TODO
// }
// 
// void Curve::setLimits(const QString &arg, double min, double max)
// {
//     
// }
// 
// void Curve::setLimits(const QString &arg, const Expression &min, const Expression &max)
// {
//     
// }
// 
// //END AbstractFunctionGraph interface
// 
// QPair<QPointF, QString> Curve::image(const QPointF &mousepos)
// {
//     return qMakePair(QPointF(), QString());
// }
// 
// QVector<int> Curve::jumps() const
// {
//     return d->m_jumps;
// }
// 
// QVector<QPointF> Curve::points() const
// {
//     return d->m_points;
// }
// 
// QLineF Curve::tangent(const QPointF &mousepos)
// {
//     return QLineF(); //TODO
// }
// 
// void Curve::update(const QRectF& viewport)
// {
//     
// }
// 
// //BEGIN static AbstractShape interface
// QStringList Curve::builtinMethods()
// {
//     return QStringList();
// }
// 
// bool Curve::canBuild(const Expression &expression, Analitza::Variables* vars)
// {
//     return true;
// }
// //END static AbstractShape interface
