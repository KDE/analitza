/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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



#ifndef FUNCTIONGRAPH2_H_mc
#define FUNCTIONGRAPH2_H_mc


#include "quadtree.h"

#include <QList>
#include <QPair>
#include <QVector>
#include <qmath.h>

struct sLimitesEspacio2D {
    double minX;
    double maxX;
    double minY;
    double maxY;
};




struct sMarching_Square {
    QPointF centro;
    double medio_lado;
    unsigned short int tipo;
    double vertices[4];
};

struct sArista2D {
    QPointF corte;
    unsigned int vertices[2];
};



//TODO esta clase se debe parametrizar para que pueda ser usada por implictcurves:
//MarchingSquares ... en general debe recibir como parametro el tree (octree,kdtree...)
//y las subclases deben implementar evalScalarfield ...pues esta clase sirve para eso:
//para convertir un campo K escalar en poligonos.
class MarchingSquares
{

public:
    virtual double evalScalarField(double x, double y) = 0;

    void setWorld(double minx, double maxx, double miny, double maxy)
    {
//         sLimitesEspacio2D _esp;
// 
//         double a = 4;
// 
//         _esp.minX = minx;
//         _esp.maxX = maxx;
//         _esp.minY = miny;
//         _esp.maxY = maxy;
// 
// //         qDebug() << _esp.minX << _esp.maxX << _esp.minY << _esp.maxY;
//         
//         largo_mundo = qMax(fabs(maxx-minx), fabs(maxy-miny));
// 
// //         qDebug() << largo_mundo;
//         if (largo_mundo < 2)
//             min_grid = 0.005;
//         else
//             min_grid = 0.09;
//             
//         largo_mundo = 2*4;
//         mundo = _esp;

///

    sLimitesEspacio2D _esp;

    double w = maxx-minx;
    double h = maxy-miny;
    
    double presc = 0.1;
    
    _esp.minX = minx-presc*w;
    _esp.maxX = maxx+presc*w;
    _esp.minY = miny-presc*h;
    _esp.maxY = maxy+presc*h;

    //a mas pequenio el size se detectan las singularidades
    min_grid = qMin(fabs(maxx-minx), fabs(maxy-miny))/256;
    
    if (min_grid>0.05 && min_grid < 1)
        min_grid = 0.05; // 0.05 es el minimo valor para la presicion
    
    largo_mundo = 1;
    mundo = _esp;
    }


public:
private:
    /*
     largo_mundo: arista del mundo
     min_grid: arista minima del cubo a evaluar
    */
    double largo_mundo;
    double min_grid;
    sLimitesEspacio2D mundo;
    //Evaluar un cubo
    sMarching_Square evaluar_cubo(Square cubo);

    //Busqueda recursiva (breadth search)
    QList<Square> breadth_rec(int cubos_lado);

    //Busqueda recursiva (depth search)
    QList<sMarching_Square> depth_rec(Quadtree *arbol, QNode *nodo);

public:
    //Recibe el tamaño de grilla deseado y el largo del mundo
    //Produce un min_grid menor o igual al proporcionado
    MarchingSquares(/*double min_grid, double arista_mundo, sLimitesEspacio2D limites*/);

    //Destructor
    ~MarchingSquares();

    //Ejecutar
    QList<sMarching_Square> ejecutar();

    friend class ImplicitSurf;
public:
    void buildGeometry();

    QVector< QPair< QPointF, QPointF > > _faces_;

    void _addTri(const QPointF &a, const QPointF &b);

private:


    //Calcular los cortes
    QList<sArista2D> calcular_cortes(sMarching_Square cubo);
    bool signo_opuesto(double a, double b);

    //Calcular la posicion en la arista
    double lineal(double vert_1, double vert_2);

    //Agregar triangulos
    void agregar_triangulos(QList<QPointF> &lista_triangulos);

    //Tipos:
    void identificar_tipo(sMarching_Square cubo);
    // corta 2 aristas y tiene un vertice en el cruce de las aristas
    //el tipo 1 cubre los casos: 1,2,3,4,6,7,8,9,11,12,13,14
    void tipo01(QList<sArista2D> aristas);

    //el tipo 5 cubre los casos 5 y 10
    // los casos 5 y 10 son donde se presnetan singularidades: cortes, cusps, etc
    void tipo05(QList<sArista2D> aristas, sMarching_Square cubo);
    
    
private:
    double fixed_x;
    double fixed_y;
    
    double fy(double y) { return evalScalarField(fixed_x, y); }
    double fx(double x) { return evalScalarField(x, fixed_y); }
};

#endif 