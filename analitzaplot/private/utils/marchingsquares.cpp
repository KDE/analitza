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

#include "marchingsquares.h"
#include <QLineF>
#include <QDebug>
#include <qpolygon.h>
#include <QVector2D>

sMarching_Square MarchingSquares::evaluar_cubo(const Square& cubo)
{
    sMarching_Square res;
    QPointF punto;
    unsigned short int val;

    //Datos generales
    res.centro = cubo.center();
    res.medio_lado = cubo.halfEdge();

    //Llenar vertices
    double x = res.centro.x();
    double y = res.centro.y();
    double hedge = res.medio_lado;

    res.vertices[0] = evalScalarField(x-hedge, y-hedge);
    res.vertices[1] = evalScalarField(x-hedge, y+hedge);
    res.vertices[2] = evalScalarField(x+hedge, y-hedge);
    res.vertices[3] = evalScalarField(x+hedge, y+hedge);

//     //Definir tipo

//  -----
//  |1|3|
//  -----
//  |0|2|
//  -----

    res.tipo = 0;

    if (res.vertices[1]>0)
        res.tipo += 8;

    if (res.vertices[3]>0)
        res.tipo += 4;

    if (res.vertices[2]>0)
        res.tipo += 2;

    if (res.vertices[0]>0)
        res.tipo += 1;

    return res;
}

QList<Square> MarchingSquares::breadth_rec(int cubos_lado) {
    Square cubo;
    sMarching_Square m_cubo;
    bool salir = false;
    QList<Square> cubos;
    cubo.setHalfEdge(largo_mundo/(2*cubos_lado));

    double x = 0;
    double y = 0;

    static const double iteration_square_val = 0.5;
    
    for(double i=mundo.minX; i<=mundo.maxX; i+=iteration_square_val) 
    {
        x = (2*i+1)*cubo.halfEdge();

        for(double j=mundo.minY; j<=mundo.maxY; j+=iteration_square_val) 
        {
            y = (2*j+1)*cubo.halfEdge();
            cubo.setCenter(x,y);
            m_cubo = evaluar_cubo(cubo);
            if(m_cubo.tipo != 0 && m_cubo.tipo != 15) {
                //Esta dentro del cubo. Detener busqueda
                salir = true;
                cubos.append(cubo);
            }
        }
    }
    
    if(!salir && 2*cubo.halfEdge() > min_grid) 
        cubos.append(breadth_rec(cubos_lado*2));
    
    return cubos;
}

QList<sMarching_Square> MarchingSquares::depth_rec(Quadtree *arbol, QNode *nodo) {
    QList<sMarching_Square> cubos;
    sMarching_Square m_cubo;

    //Calcular si la superfice lo corta
    m_cubo = evaluar_cubo(nodo->cubo);

    if(m_cubo.tipo != 0 && m_cubo.tipo != 15) {
        //Superfice corta
        if(m_cubo.medio_lado*2 > min_grid) {
            //Seguir bajando
            arbol->bajarNivel(nodo);
            for(unsigned int i=0; i<4; i++) {
                cubos.append(depth_rec(arbol,nodo->nodos[i]));
            }
            //No ayuda mucho, pero se puede borrar el arbol conforme ya no se necesite
            //arbol->borrarHijos(nodo);
        } else {
            //Detener
            cubos.append(m_cubo);
        }
    }
    return cubos;
}

MarchingSquares::MarchingSquares(/*double min_grid, double arista_mundo, sLimitesEspacio2D limites*/

) {
    //TODO enlazar con arg interval
//     sLimitesEspacio2D _esp;
// 
//     double a = 4;
// 
//     _esp.minX = -a;
//     _esp.maxX = a;
//     _esp.minY = -a;
//     _esp.maxY = a;
// 
//     //a mas pequenio el size se detectan las singularidades
//     this->min_grid = 0.05;
//     largo_mundo = 2*4;
//     mundo = _esp;
}

MarchingSquares::~MarchingSquares() {
}

QList<sMarching_Square> MarchingSquares::ejecutar() {
    QList<sMarching_Square> cubos;
    QList<Square> encontrados;
    Square iterador;
    Quadtree *arbol;

    //Buscar la superficie (breadth search octree)
    encontrados = breadth_rec(largo_mundo);
    //Si no encuentra la superficie, retorna lista vacia
    if(encontrados.isEmpty()) {
        return cubos;
    }
//     printf("Encontrados: %d\n",encontrados.count());

    //Ubicar los cubos (depth search octree)
    foreach(iterador, encontrados) {
        arbol = new Quadtree(iterador);
        cubos.append(depth_rec(arbol, arbol->get_raiz()));
        delete arbol;
    }

    //Devolver los cubos
    return cubos;
}

void MarchingSquares::_addTri(const QPointF& a, const QPointF& b)
{
    QPair<QPointF,QPointF> _f = qMakePair(a,b);
    _faces_.append(_f);

}

// #include <boost/math/tools/roots.hpp>
// #include "boost/bind.hpp"
// 
// struct TerminationCondition  {
//   bool operator() (double min, double max)  {
//     return abs(min - max) <= 0.0000001;
//   }
// };
/*
// ...
using boost::math::tools::bisect;
using boost::math::tools::toms748_solve;

// double root = (result.first + result.second) / 2;  // = 0.381966...

using namespace boost::math::policies;
typedef policy<
  domain_error<ignore_error>,
  pole_error<ignore_error>,
  overflow_error<ignore_error>,
  evaluation_error<ignore_error>
> my_policy;

uintmax_t maxiters = 100;*/

QList<sArista2D> MarchingSquares::calcular_cortes(sMarching_Square cubo) {
    QList<sArista2D> aristas;
    sArista2D temp;
//  -----
//  |1|3|
//  -----
//  |0|2|
//  -----

    double x = cubo.centro.x();
    double y = cubo.centro.y();
    double hedge = cubo.medio_lado;

    QPointF v0 = QPointF(x-hedge, y-hedge);
    QPointF v1 = QPointF(x-hedge, y+hedge);
    QPointF v2 = QPointF(x+hedge, y-hedge);
    QPointF v3 = QPointF(x+hedge, y+hedge);

    //0-1
//     fixed_x = v0.x();
//     std::pair<double, double> r = bisect(boost::bind(&MarchingSquares::fy,this, _1), v0.y(), v1.y(), TerminationCondition(),maxiters, my_policy());
//     if (fy(r.first) * fy(r.second) <= 0) //corte
//     {
//         temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado,
//                              cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[1]));
//         temp.vertices[0] = 0;
//         temp.vertices[1] = 1;
//         aristas.append(temp);        
//     }

    //0-1
    if(signo_opuesto(cubo.vertices[0],cubo.vertices[1])) {
        //al primero luego sumale
        temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado,
                             cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[1]));
        temp.vertices[0] = 0;
        temp.vertices[1] = 1;
        aristas.append(temp);
    }

    //1-3
//     fixed_y = v1.y();
//     r = bisect(boost::bind(&MarchingSquares::fx,this, _1), v1.x(), v3.x(), TerminationCondition(),maxiters, my_policy());
//     if (fx(r.first) * fx(r.second) <= 0) //corte
//     {
//         temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[1],cubo.vertices[3]),
//                              cubo.centro.y()+cubo.medio_lado);
//         temp.vertices[0] = 1;
//         temp.vertices[1] = 3;
//         aristas.append(temp);     
//     }
    
    //1-3
    if(signo_opuesto(cubo.vertices[1],cubo.vertices[3])) {
        temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[1],cubo.vertices[3]),
                             cubo.centro.y()+cubo.medio_lado);
        temp.vertices[0] = 1;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }

    //2-3
//     fixed_x = v2.x();
//     r = bisect(boost::bind(&MarchingSquares::fy,this, _1), v2.y(), v3.y(), TerminationCondition(),maxiters, my_policy());
//     if (fy(r.first) * fy(r.second) <= 0) //corte
//     {
//         temp.corte = QPointF(cubo.centro.x()+cubo.medio_lado,
//                              cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[2],cubo.vertices[3]));
//         temp.vertices[0] = 2;
//         temp.vertices[1] = 3;
//         aristas.append(temp);      
//     }
    
    //2-3
    if(signo_opuesto(cubo.vertices[2],cubo.vertices[3])) {
        temp.corte = QPointF(cubo.centro.x()+cubo.medio_lado,
                             cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[2],cubo.vertices[3]));
        temp.vertices[0] = 2;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }

//  -----
//  |1|3|
//  -----
//  |0|2|
//  -----

    //0-2
//     fixed_y = v0.y();
//     r = bisect(boost::bind(&MarchingSquares::fx,this, _1), v0.x(), v2.x(), TerminationCondition(),maxiters, my_policy());
//     if (fx(r.first) * fx(r.second) <= 0) //corte
//     {
//         temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[2]),
//                              cubo.centro.y()-cubo.medio_lado);
//         temp.vertices[0] = 0;
//         temp.vertices[1] = 2;
//         aristas.append(temp);    
//     }
    
    //0-2
    if(signo_opuesto(cubo.vertices[0],cubo.vertices[2])) {
        temp.corte = QPointF(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[2]),
                             cubo.centro.y()-cubo.medio_lado);
        temp.vertices[0] = 0;
        temp.vertices[1] = 2;
        aristas.append(temp);
    }

//     if (cubo.tipo == 5)
//         qDebug() << "ASIZE" <<aristas.size();

    return aristas;
}

bool MarchingSquares::signo_opuesto(double a, double b) {
    return ((a > 0 && b <= 0) || (a <= 0 && b > 0));
}

double MarchingSquares::lineal(double vert_1, double vert_2) {
    //Posicion de 0 a 1
    return qAbs(vert_1/(vert_1 - vert_2));
}

void MarchingSquares::agregar_triangulos(QList<QPointF> &lista_triangulos) {

    for(int i=0; i<lista_triangulos.count(); i+=2) {

        if (lista_triangulos.size()-2 < i)
            continue;

        _addTri(lista_triangulos.at(i),lista_triangulos.at(i+1));

    }
}

//Tipos:
void MarchingSquares::identificar_tipo(sMarching_Square cubo) {

    QList<sArista2D> aristas;
    QList<unsigned int> vertices;
    unsigned int it;

    //Conseguir aristas y vertices
    aristas = calcular_cortes(cubo);

    unsigned short int type = cubo.tipo;

//     if (type == 1)
//         qDebug( ) << aristas.size();

    switch (type)
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
        tipo01(aristas);

        break;
    }
    case 5:
    case 10:
    {
        tipo05(aristas, cubo);

        break;
    }
    }
}

void MarchingSquares::tipo01(QList<sArista2D> aristas)
{
    
    if (aristas.isEmpty()) return;

    QList<QPointF> triangulos;

    //en este tipo hay 2 aristas

    triangulos << aristas[0].corte << aristas[1].corte;

    agregar_triangulos(triangulos);
}

void MarchingSquares::tipo05(QList<sArista2D> aristas,sMarching_Square cubo)
{
    if (aristas.isEmpty()) return;
    
    // en los tipos 10 y 5 hay 4 aristas
    QList<QPointF> triangulos;

//  -----
//  |1|3|
//  -----
//  |0|2|
//  -----

//     qDebug() << "singular510";
    

    if (cubo.tipo == 5) 
        triangulos << aristas[0].corte << aristas[2].corte;
    else if (cubo.tipo == 10)
        triangulos << aristas[1].corte << aristas[3].corte;

    agregar_triangulos(triangulos);
}
void MarchingSquares::buildGeometry()
{
    _faces_.clear();

    QList<sMarching_Square> cubos;
    sMarching_Square cubo;

    cubos = ejecutar();
//     printf("Cubos: %d\n",cubos.count());

    foreach(cubo,cubos) {
        //Puede que ahora sea innecesario cambiar el tipo...
//         if(cubo.tipo > 127){
//             cubo.tipo = 255 - cubo.tipo;
//         }
        identificar_tipo(cubo);
    }

}
void MarchingSquares::setWorld(double minx, double maxx, double miny, double maxy)
{
        sLimitesEspacio2D _esp;

        double a = 4;

        _esp.minX = minx;
        _esp.maxX = maxx;
        _esp.minY = miny;
        _esp.maxY = maxy;

//         qDebug() << _esp.minX << _esp.maxX << _esp.minY << _esp.maxY;
        
        largo_mundo = 1;

//         qDebug() << largo_mundo;
    //a mas pequenio el size se detectan las singularidades
    min_grid = qMin(fabs(maxx-minx), fabs(maxy-miny))/256;
    
    if (min_grid>0.05 && min_grid < 1)
        min_grid = 0.05; // 0.05 es el minimo valor para la presicion
            
        mundo = _esp;

///
/*
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
    mundo = _esp;*/
}
