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
#include <stdlib.h>
#include <time.h>

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
    srand(time(0));
}

Curve::Curve(const Curve &other)
    : d(other.d)
{
    srand(time(0));
}

Curve::Curve(const Expression &expresssion, Variables* vars)
    : d(new CurveData(expresssion, vars))
{
    srand(time(0));
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
        adaptiveQuadTreeSubdivisionImplicitCurve(-5,5,-5,5, 0.4);
//         MathUtils::QuadTree *quadtree = new MathUtils::QuadTree(0,0, 10);
//         
//         adaptiveQuadTreeSubdivisionImplicitCurve(quadtree);
//         
//         delete quadtree;

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

void Curve::hilbert(double x0, double y0, double xi, double xj, double yi, double yj, int n, bool &found)
{
    static double old = 0;
 
    if (found) return;
    
    if (n <= 0)
    {
        double X = x0 + (xi + yi)/2;
        double Y = y0 + (xj + yj)/2;
        
        ///
        
        Cn *x = d->m_args.value("x");
        Cn *y = d->m_args.value("y");
    
        x->setValue(X);
        y->setValue(Y);
        
        double val = d->m_analyzer->calculateLambda().toReal().value();
        
//         glVertex2d(X,Y);
        
        if (MathUtils::oppositeSign(old,val))
            found = true;
        
        old = val;
    }
    else
    {
        hilbert(x0,               y0,               yi/2, yj/2, xi/2, xj/2, n - 1, found);
        hilbert(x0 + xi/2,        y0 + xj/2,        xi/2, xj/2, yi/2, yj/2, n - 1, found);
        hilbert(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, found);
        hilbert(x0 + xi/2 + yi,   y0 + xj/2 + yj,  -yi/2,-yj/2,-xi/2,-xj/2, n - 1, found);
    }
}

//TODO aplicar un solo algortimo y una sola pasada para todas las curvas de este tipo que esten
// en el canvas: NO APLICAR UNA PASADA POR CURVA, aprovechar el bucle para evaluar todos
void Curve::adaptiveQuadTreeSubdivisionImplicitCurve(double minx, double maxx, double miny, double maxy, double h)
{
//     double min_grid = qMin(fabs(maxx-minx), fabs(maxy-miny))/256;
//     
//     if (min_grid>0.05 && min_grid < 1)
//         min_grid = 0.05; // 0.05 es el minimo valor para la presicion
    
    //     const int order = 4;
//     const double size = 1;//maxx - minx;
//     double blx = 0;
//     double bly = 0;
//     
//     glBegin(GL_LINE_STRIP);
//     glColor3ub(50,50,50);
// //     hilbert(blx, bly, size, 0, 0, size, order);
//     glEnd();
//     
//     glBegin(GL_POINTS);
//     glColor3ub(255,255,0);
//     glVertex2d(0,0);
//     glEnd();
//     
//     return ;
    Cn *x = d->m_args.value("x");
    Cn *y = d->m_args.value("y");

//     const int n = (maxx - minx)/h;
//     const int m = (maxy - miny)/h;
    
    double southwest = 0;
    double southeast = 0;
    double northeast = 0;
    double northwest = 0;
    
    double xleft = minx;
    
    //BEGIN debug
    for (double x_ = minx; x_ <= maxx; x_ += h)
    {
        double xright = xleft + h;
        double ybottom = miny;
        
        for (double y_ = miny; y_ <= maxy; y_ += h)
        {
            double ytop = ybottom + h;
            
            glColor3ub(10,10,10);
            glBegin(GL_LINES);
            glVertex2d(xright, ytop);
            glVertex2d(xleft, ytop);
            glEnd();
            glBegin(GL_LINES);
            glVertex2d(xleft, ybottom);
            glVertex2d(xleft, ytop);
            glEnd();
            
            ybottom = ytop;
        }
        
        xleft = xright;
    }
    //END debug
    
    xleft = minx;
    for (double x_ = minx; x_ <= maxx; x_ += h)
    {
        x->setValue(xleft);
        
        double xright = xleft + h;
        
        double ybottom = miny;
        
        int j = 0;
        
        for (double y_ = miny; y_ <= maxy; y_ += h)
        {
            y->setValue(ybottom);
            
            double ytop = ybottom + h;
            
            const bool eval = (j % 2 == 0); // avoid repeat evaluations
            ++j;
            
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
                //sestoy en el mas grande box que tiene la curvas debo segir subdividiendo
                
                const double tol = 0.04;
                
                if (h >= tol)
                    adaptiveQuadTreeSubdivisionImplicitCurve(xleft, xright, ybottom, ytop, h*0.5);
                else
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
            }
            else
            {
                
//                 double oldx = x->value();
//                 double oldy = y->value();
// //                 
// //                 x->setValue(xleft + h*.5);
// //                 y->setValue(ybottom + h*.5);
// // //                 double val = MathUtils::bilinearInterpolation(xleft + h/8, ybottom + h/3, xleft, xright, ybottom, ytop, southwest, southeast, northwest, northeast);
// //                 double val = d->m_analyzer->calculateLambda().toReal().value();
// //                 
// // //                 Analyzer a;
// // //                 a.setExpression(d->m_analyzer->derivative("x"));
// // //                 QVector<Object*> rs;
// // //                 rs << (Object*)(d->m_args.value("x")) << (Object*)(d->m_args.value("y"));
// // //                 d->m_analyzer->setStack(rs);
// // //                 qDebug() << d->m_analyzer->calculateLambda().toReal().value();
// // //                 qDebug() << x->value() << "\t" << y->value();
// //                 
// //                 if (MathUtils::oppositeSign(southwest, val) || MathUtils::oppositeSign(southeast, val) || 
// //                     MathUtils::oppositeSign(northwest, val) || MathUtils::oppositeSign(southeast, val) ||
// //                     MathUtils::oppositeSign(southwest, northeast) || MathUtils::oppositeSign(southeast, northwest))
// //                 {
// //                     glBegin(GL_POINTS);
// //                     glColor3ub(255,255,0);
// //                     glVertex2d(x->value(), y->value());
// //                     glEnd();
// //                 }
//                 
//                 x->setValue(xleft + h/2 + 0.5*0.5*h*cos(0));
//                 y->setValue(ybottom + h/2 + 0.5*0.5*h*sin(0));
// 
//                 double oldv = d->m_analyzer->calculateLambda().toReal().value();
//                 
//                 
//                 
//                 
//                 for (double t = 0; t <= M_PI*2; t+=M_PI/3)
//                 {
// //                     double r = rand()%10 + 1;
//                     double rho = 0.5*0.5*h/1;
//                     
//                     double cx = xleft + h/2 + rho*cos(t);
//                     double cy = ybottom + h/2 + rho*sin(t); 
//                     
//                     x->setValue(cx);
//                     y->setValue(cy);
//                     
//                     glBegin(GL_POINTS);
//                     glColor3ub(0,100,100);
//                     glVertex2d(cx, cy); // 1/4 de h
//                     glEnd();
//                     
//                     double v = d->m_analyzer->calculateLambda().toReal().value();
//                     
// //                     if (MathUtils::oppositeSign(v, oldv))
// //                     {
// //                         glBegin(GL_POINTS);
// //                             glColor3ub(0,255,255);
// //                             glVertex2d(cx, cy); // 1/4 de h
// //                         glEnd();
// //                         
// //                         break;
// //                     }
//                     
//                     oldv = v;
//                 }
//                 
//                 x->setValue(oldx);
//                 y->setValue(oldy);
            }
            
            ybottom = ytop;
        }
        
        xleft = xright;
    }
}

void Curve::adaptiveQuadTreeSubdivisionImplicitCurve(MathUtils::QuadTree *root)
{
    Cn *x = d->m_args.value("x");
    Cn *y = d->m_args.value("y");
    
    const double h = root->size*0.5; // half
    const double hh = root->size*0.5*0.5; // halfhalf
    
    const double rxmhh = root->x - hh; // root x minus halfhalf
    const double rxphh = root->x + hh;
    const double rymhh = root->y - hh;
    const double ryphh = root->y + hh;
    
    root->nodes[0] = new MathUtils::QuadTree(rxphh, ryphh, h); // NE
    root->nodes[1] = new MathUtils::QuadTree(rxmhh, ryphh, h); // NW
    root->nodes[2] = new MathUtils::QuadTree(rxmhh, rymhh, h); // SW
    root->nodes[3] = new MathUtils::QuadTree(rxphh, rymhh, h); // SE
    
    for (int i = 0; i < 4; ++i)
    {
        MathUtils::QuadTree *node = root->nodes[i];
        
        const int order = 4;
        const double size = node->size;
        double blx = node->x - hh;
        double bly = node->y - hh;
        
        bool f = false;
        hilbert(blx, bly, size, 0, 0, size, order, f);
        
        if (f)
        {
            if ((node->size < 0.5))
            {
                const double nxmhh = node->x - hh; // node x minus halfhalf
                const double nxphh = node->x + hh;
                const double nymhh = node->y - hh;
                const double nyphh = node->y + hh;

                const double xleft = nxmhh;
                const double xright = nxphh;
                const double ybottom = nymhh;
                const double ytop = nyphh;
            
                x->setValue(nxphh);
                y->setValue(nyphh);
                
                const double northeast = d->m_analyzer->calculateLambda().toReal().value();
                
                x->setValue(nxmhh);
                y->setValue(nyphh);
                
                const double northwest = d->m_analyzer->calculateLambda().toReal().value();
                
                x->setValue(nxmhh);
                y->setValue(nymhh);
                
                const double southwest = d->m_analyzer->calculateLambda().toReal().value();

                x->setValue(nxphh);
                y->setValue(nymhh);
                
                const double southeast = d->m_analyzer->calculateLambda().toReal().value();
                
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
                
//                 glBegin(GL_POINTS);
//                 glColor3ub(255,255,0);
//                 glVertex2d(node->x, node->y);
//                 glEnd();
            }
            else
            {
//                 if (hilbert(node->x - hh, node->y - hh, node->size, 0, 0, node->size, 8))
                    adaptiveQuadTreeSubdivisionImplicitCurve(node);
            }
        }
        else
        {
        }
        
        //DEBUG
//         glBegin(GL_LINES);
//         glColor3ub(50,50,50);
//         // | <-
//         glVertex2d(node->x - hh, node->y - hh);
//         glVertex2d(node->x - hh, node->y + hh);
//         glEnd();
//         
//         // -
//         glBegin(GL_LINES);
//         glColor3ub(50,50,50);
//         glVertex2d(node->x - hh, node->y + hh);
//         glVertex2d(node->x + hh, node->y + hh);
//         glEnd();
//         
//         // -> |
//         glBegin(GL_LINES);
//         glColor3ub(50,50,50);
//         glVertex2d(node->x + hh, node->y + hh);
//         glVertex2d(node->x + hh, node->y - hh);
//         glEnd();
//         
//         // _
//         glBegin(GL_LINES);
//         glColor3ub(50,50,50);
//         glVertex2d(node->x - hh, node->y - hh);
//         glVertex2d(node->x + hh, node->y - hh);
//         glEnd();
    }
}
