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
#include <analitza/variable.h>
#include <analitza/value.h>
#include <KLocalizedString>

using namespace Analitza;

class Curve::CurveData : public QSharedData
{
public:
    CurveData();
    CurveData(const CurveData &other);
    CurveData(const Analitza::Expression& expresssion, Variables* vars);
    ~CurveData();
        QColor m_color;
    CoordinateSystem m_coordinateSystem;
    Dimension m_dimension;
    QStringList m_errors;
    Expression m_expression; //visible expression
    QString m_iconName;
    QString m_name;
    QString m_typeName;
    bool m_visible;
    Analyzer *m_analyzer; // internal expression
    QHash<QString, Cn*> m_args;
    bool m_glready;
};

Curve::CurveData::CurveData()
    : m_glready(false)
{

}

Curve::CurveData::CurveData(const CurveData& other)
    : QSharedData(other)
//     , ShapeData(other)
    , m_glready(false)
{
    m_analyzer = new Analyzer(*other.m_analyzer);
    m_args["x"] = static_cast<Cn*>(other.m_args.value("x")->copy());
    m_args["y"] = static_cast<Cn*>(other.m_args.value("y")->copy());
    QStack<Object*> runStack;
    runStack.push(m_args.value("x"));
    runStack.push(m_args.value("y"));
                    
    m_analyzer->setStack(runStack);
}

Curve::CurveData::CurveData(const Expression& expresssion, Variables* vars)
    : m_glready(false)
{
    if (expresssion.isCorrect())
    {
        if (expresssion.isEquation()) 
        {
            if (vars)
                m_analyzer = new Analyzer(vars);
            else
                m_analyzer = new Analyzer;

            m_analyzer->setExpression(expresssion.equationToFunction());
            m_analyzer->setExpression(m_analyzer->dependenciesToLambda());
            
            if (m_analyzer->expression().parameters().size() == 2)
            {
                if ((m_analyzer->expression().parameters().at(0)->name() == "x") && 
                    (m_analyzer->expression().parameters().at(1)->name() == "y"))
                {
                    m_expression = expresssion;
                    
                    m_args.insert("x", new Cn);
                    m_args.insert("y", new Cn);
                    
                    QStack<Object*> runStack;
                    runStack.push(m_args.value("x"));
                    runStack.push(m_args.value("y"));
                    
                    m_analyzer->setStack(runStack);
                }
                else
                {
                    m_errors << i18n("Implicit curve has only 2 vars x and y");
                }
            }
            else
            {
                m_errors << i18n("Implicit curve has only 2 vars");
            }
        }
        else if (expresssion.isLambda()) // args->Func(args)
        {
            
        }
        else // BUILTHMETHODS if (expresssion.isCustomObject())
        {
            m_errors << i18n("Curve type not recognized");
        }
    }
    else
    {
        m_errors << i18n("The expression is not correct");
    }

//     if (!m_errors.isEmpty())
//         delete m_analyzer;
}

Curve::CurveData::~CurveData()
{
//     qDebug() << "FERE";
    
    qDeleteAll(m_args);
    delete m_analyzer;
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
    : d(new CurveData(expresssion, vars))
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
    return d->m_errors;
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

bool isSimilar(double a, double b, double diff)
{
    return std::fabs(a-b) < diff;
}

void Curve::plot(const QGLContext* context)
{    
//     if (!d->m_glready)
//     {
//         qDebug() << "init" << d->m_analyzer->calculateLambda().toString();
//         
//         d->m_glready = true;
//     }
//     else
//     {
//         qDebug() << "render" << d->m_analyzer->calculateLambda().toString();
//     }

    Cn *x = d->m_args.value("x");
    Cn *y = d->m_args.value("y");

    
    for (double xval = -1; xval <= 1; xval += 0.01)
        for (double yval = -1; yval <= 1; yval += 0.01)
        {
            x->setValue(xval);
            y->setValue(yval);
            
            if (isSimilar(d->m_analyzer->calculateLambda().toReal().value(), 0, 0.001))
            {
                glBegin(GL_POINTS);
                
                glColor3ub(255,255,0);
                glVertex2d(x->value(), y->value());
                
                glEnd();
            }
        }
    
    
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

bool Curve::operator==(const Curve &other) const
{
    return (d->m_color == other.d->m_color) &&
        (d->m_coordinateSystem == other.d->m_coordinateSystem) &&
        (d->m_dimension == other.d->m_dimension) &&
        (d->m_errors == other.d->m_errors) &&
        (d->m_expression == other.d->m_expression) &&
        (d->m_iconName == other.d->m_iconName) &&
        (d->m_name == other.d->m_name) &&
        (d->m_typeName == other.d->m_typeName) &&
        (d->m_visible == other.d->m_visible);
}

bool Curve::operator!=(const Curve &other) const
{
    return !((*this) == other);
}

Curve & Curve::operator=(const Curve &other)
{
    //BEGIN basic shape data
    d->m_color = other.d->m_color;
    d->m_coordinateSystem = other.d->m_coordinateSystem;
    d->m_dimension = other.d->m_dimension;
    d->m_errors = other.d->m_errors;
    d->m_expression = other.d->m_expression;
    d->m_iconName = other.d->m_iconName;
    d->m_name = other.d->m_name;
    d->m_typeName = other.d->m_typeName;
    d->m_visible = other.d->m_visible;
    //END basic shape data
    
    //force to create opengl buffer again in plot method
    d->m_glready = false;
    
    qDeleteAll(d->m_args);
    delete d->m_analyzer;
    
    d->m_analyzer = new Analyzer(*other.d->m_analyzer);
    d->m_args["x"] = static_cast<Cn*>(other.d->m_args.value("x")->copy());
    d->m_args["y"] = static_cast<Cn*>(other.d->m_args.value("y")->copy());
    QStack<Object*> runStack;
    runStack.push(d->m_args.value("x"));
    runStack.push(d->m_args.value("y"));
                    
    d->m_analyzer->setStack(runStack);
    
    return *this; //TODO
}









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
