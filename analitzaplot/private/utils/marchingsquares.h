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

#ifndef MARCHINGSQUARES_H
#define MARCHINGSQUARES_H

#include "quadtree.h"

#include <QList>
#include <QPair>
#include <QVector>
#include <qmath.h>
#include <QLineF>

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

//TODO very bad implementation ... we need to use interval arithmetic plus root finding 
//to know if a 0 belongs to f(square)

class MarchingSquares
{
public:
    virtual double evalScalarField(double x, double y) = 0;

    void setWorld(double minx, double maxx, double miny, double maxy);

public:
private:
    double largo_mundo;
    double min_grid;
    sLimitesEspacio2D mundo;
    //Evaluar un cubo
    sMarching_Square evaluar_cubo(const Square& cubo);

    //Busqueda recursiva (breadth search)
    QList<Square> breadth_rec(int cubos_lado);

    //Busqueda recursiva (depth search)
    QList<sMarching_Square> depth_rec(Quadtree *arbol, QNode *nodo);

public:
    MarchingSquares(/*double min_grid, double arista_mundo, sLimitesEspacio2D limites*/);

    virtual ~MarchingSquares();

    QList<sMarching_Square> ejecutar();

    friend class ImplicitSurf;
public:
    void buildGeometry();

    QVector< QPair< QPointF, QPointF > > _faces_;

    void _addTri(const QPointF &a, const QPointF &b);

private:
    QList<sArista2D> calcular_cortes(const sMarching_Square &cubo);
    bool signo_opuesto(double a, double b);
    double lineal(double vert_1, double vert_2);
    void agregar_triangulos(QList<QPointF> &lista_triangulos);
    void identificar_tipo(const sMarching_Square &cubo);
    void tipo01(QList<sArista2D> aristas);
    void tipo05(QList<sArista2D> aristas, const sMarching_Square &cubo);
    
private:
    double fixed_x;
    double fixed_y;
    
    double fy(double y) { return evalScalarField(fixed_x, y); }
    double fx(double x) { return evalScalarField(x, fixed_y); }
};

#endif
