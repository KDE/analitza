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

/*
 Indice de los nodos

 Capa z+
y+
 -----
 |3|7|
 -----
 |1|5|
 ----- x+

 Capa z-
y+
 -----
 |2|6|
 -----
 |0|4|
 ----- x+
*/

#include "box3d.h"

struct Cube
{
    Cube(const QVector3D cent = QVector3D(0,0,0), double halfe = 1) : c(cent), he(halfe) {}
    Cube(const Cube &other) :c(other.c), he(other.he) {}

    QVector3D c;
    double he;
    
    QVector3D center() const { return c; }
    double halfEdge() const { return he; }
    void setCenter(double x, double y, double z) { c = QVector3D(x,y,z); }
    void setCenter(const QVector3D &cent) { c = cent; }
    void setHalfEdge(double halfe) { he = halfe; }

};

struct sNodo{
    Cube cubo;
    sNodo* nodos[8];
};


//TODO replace by kdtree using ANN library
class Octree
{
private:
    sNodo* raiz;

    //Crea los hijos con los valores adecuados en los vertices
    void inicializar_nodos(sNodo* padre);

    void borrar_rec(sNodo* nodo);
    void crear_rec(sNodo* nodo, unsigned int nivel_actual, unsigned int nivel_max);
public:
    Octree(double largo_mundo);
    Octree(Cube cubo);
    ~Octree();

    sNodo* get_raiz();
    void crearNivel(unsigned int nivel);
    void bajarNivel(sNodo* nodo);
    void borrarHijos(sNodo* padre);

};
