/*************************************************************************************
 *  Copyright (C) 2010-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#define GL_GLEXT_PROTOTYPES
#include <GL/glu.h>

#include "private/shapedata.h"
#include "private/mathutils.h"
#include "private/quadtree.h"

#include "analitza/analyzer.h"
#include <analitza/variable.h>
#include <analitza/value.h>
#include <analitza/variables.h>
#include <KLocalizedString>
#include <QVector2D>

using namespace Analitza;

class Curve::CurveData : public QSharedData, public ShapeData
{
public:
    CurveData();
    CurveData(const CurveData &other);
    CurveData(const Expression& expresssion, Variables* vars);
    ~CurveData();

    Variables *m_vars; // variables module, just ignore m_analyzer->variables: there is not way to know if is owned or external module vars
    QScopedPointer<Analyzer> m_analyzer; // internal expression
    QHash<QString, Cn*> m_args;
};

Curve::CurveData::CurveData()
    : QSharedData()
    , ShapeData()
    , m_vars(0)
    , m_analyzer(0)
{
    //TODO better messages and make this a ... define this in shapedata ... add some ctor there
    m_errors << i18n("Invalid null curve, use other ctors");
}

Curve::CurveData::CurveData(const CurveData& other)
    : QSharedData(other)
    , ShapeData(other)
    , m_vars(0)
    , m_analyzer(0)
{
    m_vars = other.m_vars;
    
    qDeleteAll(m_args);
    m_args.clear();
    
    if (other.m_analyzer)
    {
        if (other.m_vars)
            m_analyzer.reset(new Analyzer(m_vars));
        else
            m_analyzer.reset(new Analyzer);
    }
    
    m_analyzer->setExpression(other.m_expression);
    
    QStack<Object*> runStack;
    
    foreach(const QString &arg, m_args.keys())
    {
        m_args.insert(arg, static_cast<Cn*>(other.m_args.value(arg)->copy()));
        runStack.push(m_args.value(arg));
    }
    
    m_analyzer->setStack(runStack);
}

Curve::CurveData::CurveData(const Expression& expresssion, Variables* vars)
    : QSharedData()
    , ShapeData()
    , m_vars(0)
    , m_analyzer(0)
{
    QScopedPointer<Analyzer> analyzer;
    
    if (expresssion.isCorrect() && !expresssion.toString().isEmpty())
    {
        const ExpressionType onetoonefunc_t = MathUtils::createRealValuedFunctionType();
        
        QList< QPair<ExpressionType, QStringList> > validtypes;
        validtypes.append(qMakePair(onetoonefunc_t, QStringList("x")));
        validtypes.append(qMakePair(onetoonefunc_t, QStringList("y")));
        validtypes.append(qMakePair(onetoonefunc_t, QStringList("r")));
        validtypes.append(qMakePair(MathUtils::createVectorValuedFunctionType(Dim1D, Dim2D), QStringList("t"))); // vector valued function 2D
        validtypes.append(qMakePair(MathUtils::createVectorValuedFunctionType(Dim1D, Dim3D), QStringList("t"))); // vector valued function 3D
        validtypes.append(qMakePair(MathUtils::createRealValuedFunctionType(Dim2D), QStringList("x") << "y")); // implicit curve
        validtypes.append(qMakePair(MathUtils::createListValuedFunctionType(), QStringList("x"))); // integral curve: ode solution
        
        if (vars)
            analyzer.reset(new Analyzer(vars));
        else
            analyzer.reset(new Analyzer);
        
        if (expresssion.isDeclaration())
        {
            analyzer->setExpression(expresssion.declarationValue());
        }
        else
            if (expresssion.isEquation()) // implicit curve
            {
                analyzer->setExpression(expresssion.equationToFunction());
                analyzer->setExpression(analyzer->dependenciesToLambda());
                
                if (analyzer->expression().bvarList().size() != 2)
                    m_errors << "implict curve needs 2 vars";
            }
            else
                if (expresssion.isLambda())
                {
                    analyzer->setExpression(expresssion);
                    
                    if (expresssion.parameters().size() == 1) // ode case
                    {
                        if (expresssion.lambdaBody().isList()) // check if is a list
                        {
                            QList<Expression> list = expresssion.lambdaBody().toExpressionList();
                            
                            // 1st F 
                            // 2nd is n: the order
                            // 3rd: x0
                            // 4rd: y(x0)
                            // we need 3rd and 4rd fix to get a specific solution and not a generic one
                            if (list.size() < 4)
                            {
                                m_errors << "need ode argumetns in list";
                            }
                            else
                            {
                                //check0
                                if (!list.first().toReal().isInteger())
                                {
                                    m_errors << "second arg is the order, is a internet value";
                                }
                                else
                                {
                                //check1 TODO
    //                                 if ()
    //                                 else
                                    {
                                        //check2 to n-1
                                        //check all values
                                        for (int i = 2; i<list.size(); ++i)
                                            if (!list.at(i).isCorrect() || list.at(i).toString().isEmpty() 
                                                || !list.at(i).isReal())
                                            {
                                                m_errors << "Not good values in " << QString::number(i);
                                                break;
                                            }
                                    }
                                }
                            }
                        }
                        else // 
                        {
                            //si es lambda deberia generar la misma expression al llamar de dependenciesToLambda si no es asi es test del bucle abajo fallara
                            analyzer->setExpression(analyzer->dependenciesToLambda());
                        }
                            
                    }
                    else
                        m_errors << i18n("Wrong number of params for lambda ctor");
                }
                else
                {
                    analyzer->setExpression(expresssion);
                }
        
        if (m_errors.isEmpty())
        {
            int nmath = 0;
            
            
            for (int i = 0; i < validtypes.size(); ++i)
            {
    //                     qDebug() << validtypes[i].first.toString() << analyzer->type().toString() << analyzer->expression().toString();
                if (analyzer->expression().bvarList() == validtypes[i].second && analyzer->type().canReduceTo(validtypes[i].first))
                {
                        ++nmath;
                        break;
                }
            }
            if (nmath == 0)
                m_errors << i18n("Curve type not recognized");
        }
    }
    else
        m_errors << i18n("The expression is not correct");
    
    if (m_errors.isEmpty())
    {
        m_vars = vars;
        m_analyzer.reset(analyzer.take());
        
        QStack<Object*> runStack;
        
        foreach(const Ci *arg, m_analyzer->expression().parameters())
        {
            m_args.insert(arg->name(), new Cn);
            runStack.push(m_args.value(arg->name()));
        }
        
        m_analyzer->setStack(runStack);
        
        m_expression = expresssion;
    }
}

Curve::CurveData::~CurveData()
{
    qDeleteAll(m_args);
}

Curve::Curve()
    : d(new CurveData)
{
}

Curve::Curve(const Curve &other)
    : d(other.d)
{
}

Curve::Curve(const Expression &expresssion, Variables* vars)
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

void Curve::createGeometry()
{
    if (d->m_expression.isEquation() && d->m_errors.isEmpty())
    {
        adaptiveQuadTreeSubdivisionImplicitCurve();
    }
}

Dimension Curve::dimension() const
{
    return d->m_dimension;
}

QStringList Curve::errors() const
{
    return d->m_errors;
}

Expression Curve::expression(bool symbolic) const
{
    //TODO SYMBOLIC
    return d->m_expression;
}

QString Curve::iconName() const
{
    return d->m_iconName;
}

bool Curve::isValid() const
{
    return d->m_errors.isEmpty();
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
    //TODO
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

Variables * Curve::variables() const
{
    return d->m_vars;
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
        (d->m_vars == other.d->m_vars) && 
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
    d->m_visible = other.d->m_visible;
    //END basic shape data
    
    qDeleteAll(d->m_args);
    d->m_args.clear();
    
    d->m_vars = other.d->m_vars;
    
    if (other.d->m_analyzer)
    {
        if (other.d->m_vars)
            d->m_analyzer.reset(new Analyzer(d->m_vars));
        else
            d->m_analyzer.reset(new Analyzer);
    }
    
    QStack<Object*> runStack;

    foreach (const QString &key, other.d->m_args.keys())
    {
        d->m_args.insert(key, static_cast<Cn*>(other.d->m_args.value(key)->copy()));
        runStack.push(d->m_args.value(key));
    }
    
    d->m_analyzer->setStack(runStack);
    
    return *this;
}

int deep = 0;

void Curve::adaptiveQuadTreeSubdivisionImplicitCurve()
{
    Cn *x = d->m_args.value("x");
    Cn *y = d->m_args.value("y");

    const double h = 0.2; // box size
    
    const double minx = -5;
    const double maxx = 5;
    const double miny = -5;
    const double maxy = 5;
    
    const int n = (maxx - minx)/h;
    const int m = (maxy - miny)/h;
    
    double xleft = minx;
    
    double southwest = 0;
    double southeast = 0;
    double northeast = 0;
    double northwest = 0;
    
    for (int i = 0; i < m; ++i)
    {
        x->setValue(xleft);
            
        double xright = xleft + h;
        
        double ybottom = miny;
        
        for(int j = 0; j < n; ++j) 
        {
            y->setValue(ybottom);
            
            double ytop = ybottom + h;
            
            const bool eval = (j % 2 == 0); // avoid repeat evaluations
            
            if (eval)
                southwest = d->m_analyzer->calculateLambda().toReal().value();
            else
                southwest = northwest;
            
            x->setValue(xright);
            y->setValue(ybottom);
            
            if (eval)
                southeast = d->m_analyzer->calculateLambda().toReal().value();
            else
                southeast = northeast;
            
            x->setValue(xright);
            y->setValue(ytop);
            
            northeast = d->m_analyzer->calculateLambda().toReal().value();
            
            x->setValue(xleft);
            y->setValue(ytop);
            
            northwest = d->m_analyzer->calculateLambda().toReal().value();
            
            short int topologicalType = 0;
            
            if (northwest > 0)
                topologicalType += 8;

            if (northeast > 0)
                topologicalType += 4;

            if (southeast > 0)
                topologicalType += 2;

            if (southwest > 0)
                topologicalType += 1;
            
            if (topologicalType != 0 && topologicalType != 15) // curve inside the box
            {
                //TODO move this outside
                QVector< QPair<double, double> > intersections; // 0:left 1:top 2:right 3:botton                
                
                if (MathUtils::oppositeSign(southwest, northwest))
                    intersections << qMakePair(xleft, MathUtils::linearInterpolation(0.0, southwest, northwest, ybottom, ytop));
                
                if (MathUtils::oppositeSign(northwest, northeast))
                    intersections << qMakePair(MathUtils::linearInterpolation(0.0, northwest, northeast, xleft, xright), ytop);
                
                if (MathUtils::oppositeSign(southeast, northeast)) 
                    intersections << qMakePair(xright, MathUtils::linearInterpolation(0.0, southeast, northeast, ybottom, ytop));
                
                if (MathUtils::oppositeSign(southwest, southeast)) 
                    intersections << qMakePair(MathUtils::linearInterpolation(0.0,southwest, southeast, xleft, xright), ybottom);
                
                switch (topologicalType)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    {
                        //TODO not opengl just geometry primitives
                        glBegin(GL_LINES);
                        glColor3ub(255,0,0);
                        glVertex2d(intersections[0].first, intersections[0].second);
                        glVertex2d(intersections[1].first, intersections[1].second);
                        glEnd();
                        
                        break;
                    }
                    case 5:
                    {
                        //TODO not opengl just geometry primitives
                        glBegin(GL_LINES);
                        glColor3ub(0,255,0);
                        glVertex2d(intersections[0].first, intersections[0].second);
                        glVertex2d(intersections[2].first, intersections[2].second);
                        glEnd();
                        
                        break;
                    }
                    case 10:
                    {
                        //TODO not opengl just geometry primitives
                        glBegin(GL_LINES);
                        glColor3ub(0,0,255);
                        glVertex2d(intersections[1].first, intersections[1].second);
                        glVertex2d(intersections[3].first, intersections[3].second);
                        glEnd();
                        
                        break;
                    }
                }
            }
            
            ybottom = ytop;
        }
        
        xleft = xright;
    }
}
