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

#include "plotter.h"

#include "surface.h"
#include "curve.h"
#include <analitza/expression.h>
#include <analitza/variables.h>
#include <analitza/analyzer.h>
#include <analitza/value.h>

#include <cmath>
#include <QDebug>
#include <KLocalizedString>
#include <KColorUtils>
#include <GL/glu.h>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

using namespace std;
using namespace Analitza;

Plotter::Plotter(QAbstractItemModel* model)
    : m_model(model)
{
    circle = Curve(Expression("x*x+y*y=0.3*0.3"));

}

Plotter::~Plotter()
{
}

void Plotter::initialize()
{
    initializeGLFunctions();
    
    glClearColor(0,0,0,0);
    
    
//     GLfloat points[9];
// 
//     points[0] = -0.9;
//     points[1] = -0.9;
//     points[2] = -1;
// 
//     points[3] =  0.9;
//     points[4] = -0.9;
//     points[5] = -1;
// 
//     points[6] =  0.9;
//     points[7] =  0.9;
//     points[8] = -1;
// 
//     vbo = 0;
//     glGenBuffers (1, &vbo);
//     glBindBuffer (GL_ARRAY_BUFFER, vbo);
//     glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (GLfloat), points, GL_STATIC_DRAW);
//     
// const GLchar *str = "\n\
// attribute vec3 vertex;\n\
// void\n\
// main()\n\
// {\n\
//     gl_Position =  vec4( vertex, 1.0 );\n\
// }\n\
// ";
// /*
// const GLchar *str = "\n\
// attribute vec4 vertex;\n\
// mat4 view_frustum(\n\
//     float angle_of_view,\n\
//     float aspect_ratio,\n\
//     float z_near,\n\
//     float z_far\n\
// ) {\n\
//     return mat4(\n\
//         vec4(1.0/tan(angle_of_view),           0.0, 0.0, 0.0),\n\
//         vec4(0.0, aspect_ratio/tan(angle_of_view),  0.0, 0.0),\n\
//         vec4(0.0, 0.0,    (z_far+z_near)/(z_far-z_near), 1.0),\n\
//         vec4(0.0, 0.0, -2.0*z_far*z_near/(z_far-z_near), 0.0)\n\
//     );\n\
// }\n\
// void main()\n\
// {\n\
//     gl_Position = view_frustum(0.7, 4.0/4.0, -2, 1)*vertex;\n\
// }\n\
// ";
//    */ 
//     
// const GLchar *strb = "\n\
// void\n\
// main()\n\
// {\n\
//     gl_FragColor = vec4(1,1,0,1);\n\
// }\n\
// ";
//     
//     GLuint vs = glCreateShader (GL_VERTEX_SHADER);
//     glShaderSource (vs, 1, &str, NULL);
//     glCompileShader (vs);
//     
//     GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
//     glShaderSource (fs, 1, &strb, NULL);
//     glCompileShader (fs);
//     
//     shader_programme = glCreateProgram ();
//     glAttachShader (shader_programme, fs);
//     glAttachShader (shader_programme, vs);
//     glBindAttribLocation(shader_programme, 0, "vertex");
//     glBindAttribLocation(shader_programme, 1, "colour");
//     glLinkProgram (shader_programme);
//     
//     GLchar     log[2000] = {0};
//     glGetProgramInfoLog(shader_programme, 2000, 0, log);
//     
//     qDebug() << QString(log);
}

void Plotter::plot(const QGLContext * context)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
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
    
    
//     static Curve a(Expression("x->x*x"));
    
//     Curve c(Expression("x+y=0.5"));
//     c.plot(context);
    
//     qDebug() << c.expression().toString();
    
//     a.plot(context);
    
///
    
//     Curve c1(Expression("x+y=1"));
//     c1.setName("Linea");
//     
//     Curve c2(Expression("x*x+y*y=0.3*0.3"));
//     c2.setName("big0");
//     
//     Curve c3 = c1;
//     c2 = c3;
//     
//     qDebug() << c2.name() ;
//     
//     c2.plot(context);
    
///
    circle.plot(context);

}

void Plotter::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}
