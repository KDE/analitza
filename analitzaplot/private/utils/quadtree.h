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


#ifndef FUNCTIONGRAPH2_H__oct_q
#define FUNCTIONGRAPH2_H__oct_q

#include <QRectF>

class Square : public QRectF
{
public:
    // QRectF(menor, mayor); en coordenadas de mundo
    Square(const QPointF &c = QPointF(0,0), double hEdge = 1);
    Square(double x, double y, double hEdge = 1);

    void setCeter(const QPointF &c);
    void setCenter(double x, double y);

    double halfEdge() const;
    void setHalfEdge(double he);
};


struct QNode
{
    Square cubo;
    QNode* nodos[8];
};


/*
 Indice de los nodos

 -----
 |2|4|
 -----
 |1|3|
 -----
*/



class Quadtree
{
private:
    QNode* raiz;

    //Crea los hijos con los valores adecuados en los vertices
    void inicializar_nodos(QNode* padre);

    void borrar_rec(QNode* nodo);
    void crear_rec(QNode* nodo, unsigned int nivel_actual, unsigned int nivel_max);

public:
    Quadtree(double largo_mundo);
    Quadtree(Square cubo);
    ~Quadtree();

    QNode* get_raiz();
    void crearNivel(unsigned int nivel);
    void bajarNivel(QNode* nodo);
    void borrarHijos(QNode* padre);

};


#endif
