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

#include <QVector3D>
#include <QMatrix4x4>



#include "shapedata_p.h"

#include "analitza/analyzer.h"
#include <analitza/variable.h>
#include <analitza/value.h>
#include <KLocalizedString>
#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <QGLWidget>




using namespace Analitza;

class Curve::CurveData : public QSharedData, public ShapeData
{
public:
    CurveData();
    CurveData(const CurveData &other);
    CurveData(const Analitza::Expression& expresssion, Variables* vars);
    ~CurveData();

    Analyzer *m_analyzer; // internal expression
    QHash<QString, Cn*> m_args;
    bool m_glready;
    QFuture<void> m_geometrize; // geometry acces to thread
    bool m_geocalled; // if thread is called then there is not need to call it again
    bool m_done;
    
};

Curve::CurveData::CurveData()
    : m_glready(false)
    , m_geocalled(false)
    , m_done(false)
    , m_analyzer(0)
{

}

Curve::CurveData::CurveData(const CurveData& other)
    : QSharedData(other)
    , ShapeData(other)
    , m_glready(false)
    , m_done(false)
    , m_geocalled(false)
{
    //TODO is exp = other.exp then geometrize and geocalled doesn't need to be cleared
    m_analyzer = new Analyzer(other.m_analyzer->variables());
    
    if (other.m_expression.tree())
    {
        m_analyzer->setExpression(other.m_expression); // TODO
    
        m_args["x"] = static_cast<Cn*>(other.m_args.value("x")->copy());
        m_args["y"] = static_cast<Cn*>(other.m_args.value("y")->copy());
        QStack<Object*> runStack;
        runStack.push(m_args.value("x"));
        runStack.push(m_args.value("y"));
                        
        m_analyzer->setStack(runStack);
    }
}

Curve::CurveData::CurveData(const Expression& expresssion, Variables* vars)
    : m_glready(false)
    , m_done(false)
    , m_geocalled(false)
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
//     cancelnow = true;
//     m_geometrize.cancel(); run doesn't support cancel
//     m_geometrize.waitForFinished();
    
    qDeleteAll(m_args);
    
    if (m_analyzer)
        delete m_analyzer;
}



///

bool isSimilar(double a, double b, double diff)
{
    return std::fabs(a-b) < diff;
}


Curve::Curve()
    : d(new CurveData)
{
    vbo = 0;
}

Curve::Curve(const Curve &other)
    : d(other.d)
{
    vbo = 0;
}

Curve::Curve(const Analitza::Expression &expresssion, Variables* vars)
    : d(new CurveData(expresssion, vars))
{
    vbo = 0;
}

Curve::~Curve()
{
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_programme);
}



QColor Curve::color() const
{
    return d->m_color;
}

CoordinateSystem Curve::coordinateSystem() const
{
    return d->m_coordinateSystem;
}

void Curve::createGeometry()
{
    //NOTE STRONG checks policy here, because this method is in a new thread but with working with "this" ... 
    // if this is destroyed (call to dtor) then we need to be carefull before use analitza
    
    //It may happends that current instance is destroyed then we need to check if we are using the same instance
    
    Cn *x = d->m_args.value("x");
    Cn *y = d->m_args.value("y");

    double step = 0.015;
    
    for (double xval = -1; xval <= 1; xval += step)
    {
        for (double yval = -1; yval <= 1; yval += step)
        {
            if (x && y) 
            {
//                 if (d->m_analyzer->expression().tree())
                {
                    x->setValue(xval);
                    y->setValue(yval);
                    
                    if (d->m_analyzer->isCorrect())
                    {
                        Expression expval = d->m_analyzer->calculateLambda();
                        
                        if (expval.isCorrect() && !expval.toString().isEmpty())
                        {
                            Cn val = expval.toReal();

                            if (val.format() == Analitza::Cn::Real)
                                if (isSimilar(val.value(), 0, step))
                                    points << x->value() << y->value() << 0.;
                        }
                    }
                }
            }
        }
    }
        d->m_done = true;
//         qDebug() << "DONE" << d->m_done << this;
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
    return true; // TODO
}

bool Curve::isVisible() const
{
    return d->m_visible;
}

QString Curve::name() const
{
    return d->m_name;
}

void Curve::plot(/*const QGLContext* context*/)
{    
    
// qDebug() << points.size();
    
    if (!d->m_geocalled)
    {
//         qDebug() << this;
//NOTE if *this is passed then we have a copy ctr call and thread will work over a new Curve ... so we need to pass "this"
//         d->m_geometrize = QtConcurrent::run<void>(this, &Curve::geometrize);
//         geometrize();
createGeometry();
        
        d->m_geocalled = true;
    }
    else
    {
        if (d->m_done)
        {

            //load gl buffer
            if (!d->m_glready)
            {
                d->m_glready = true;
//                 qDebug() << "data" << points.size();
                glGenBuffers (1, &vbo);

                glBindBuffer (GL_ARRAY_BUFFER, vbo);
                glBufferData (GL_ARRAY_BUFFER, sizeof(double)*points.size(), points.data(), GL_STATIC_DRAW);
                
            const GLchar *str = "\n\
            attribute vec3 vertex;\n\
            void\n\
            main()\n\
            {\n\
                gl_Position =  vec4( vertex, 1.0 );\n\
            }\n\
            ";
                
            const GLchar *strb = "\n\
            void\n\
            main()\n\
            {\n\
                gl_FragColor = vec4(1,1,0,1);\n\
            }\n\
            ";
                

                GLuint vs = glCreateShader (GL_VERTEX_SHADER);
                glShaderSource (vs, 1, &str, NULL);
                glCompileShader (vs);
                
                GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
                glShaderSource (fs, 1, &strb, NULL);
                glCompileShader (fs);
                
                shader_programme = glCreateProgram ();
                glAttachShader (shader_programme, fs);
                glAttachShader (shader_programme, vs);
                glBindAttribLocation(shader_programme, 0, "vertex");
                glBindAttribLocation(shader_programme, 1, "colour");
                glLinkProgram (shader_programme);
                
                GLchar     log[2000] = {0};
                glGetProgramInfoLog(shader_programme, 2000, 0, log);
                
                qDebug() << "err load buffer "<< QString(log);
//                     plot();
            }
        }
    }
//     qDebug() << vbo;
    if (vbo)
    {
    glUseProgram (shader_programme);
    GLint vl = glGetAttribLocation(shader_programme, "vertex");
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vl, 3, GL_DOUBLE, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vl);
//     glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays (GL_POINTS, 0, points.size());
//     glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(vl);
    glUseProgram(0);
    }
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
    
    //NOTE force to create opengl buffer again in plot method
    d->m_glready = false;
    d->m_geocalled = false; //TODO see copy ctor
    d->m_done = false;
    
    qDeleteAll(d->m_args);
    
    if (d->m_analyzer)
        delete d->m_analyzer;
    
    d->m_analyzer = new Analyzer(*other.d->m_analyzer);
    d->m_args["x"] = static_cast<Cn*>(other.d->m_args.value("x")->copy());
    d->m_args["y"] = static_cast<Cn*>(other.d->m_args.value("y")->copy());
    QStack<Object*> runStack;
    runStack.push(d->m_args.value("x"));
    runStack.push(d->m_args.value("y"));
                    
    d->m_analyzer->setStack(runStack);
    
    return *this;
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
