/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef ANALITZAPLOT_FUNCTIONUTILS__G
#define ANALITZAPLOT_FUNCTIONUTILS__G

#include <cmath>
#include <limits>


#include "analitza/expression.h"
#include "analitza/value.h"
#include "analitza/analyzer.h"


#include <QPair>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QLineF>

using std::acos;
using std::atan;
using std::fabs;
using std::cos;
using std::sin;
using std::sqrt;

const double PI = std::acos(-1.0);

namespace Analitza
{
class Expression;
}

enum CoordinateSystem { Cartesian = 1, Polar, Cylindrical, Spherical };
enum CartesianAxis { XAxis = 1, YAxis, ZAxis };
enum PolarAxis { R = 1, p };

//math utils

//  default cotrs AxisAlignedBoundingBox 

// an   Oriented Bounding Box class 


class Face
{
public:
    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
    QVector3D normal;
};

//T u,v,w son vectores ortonormales ... verificar eso en los asserts
//TODO completar muchos metodos y conceptos relacionados a esta clase ma adelante
// por el momento es posible usarla como un aligned bounding box o como un cubo
class Box
{
public:
    Box(const QVector3D &cent = QVector3D(0,0,0), 
        double hw = 1, double hh = 1, double hd = 1, 
        const QVector3D &bu = QVector3D(1,0,0), 
        const QVector3D &bv = QVector3D(0,1,0), const QVector3D &bw = QVector3D(0,0,1));
    Box(const Box &other);

    QVector3D center() const { return m_center; }
    void setCenter(const QVector3D &c) { m_center = c; }

    //TODO
    void setHalfWidth(double hw) { m_halfHeight = hw; }
    void setHalfDepth(double hd) { m_halfDepth = hd; }
    
    double halfWidth() const { return m_halfWidth; }
    double halfHeight() const { return m_halfHeight; }
    double halfDepth() const { return m_halfDepth; }

    QVector3D u() const { return m_u; }
    QVector3D v() const { return m_v; }
    QVector3D w() const { return m_w; }

    bool operator == (const Box &other) const;
    Box operator=(const Box& other);

private:
    QVector3D m_center;
    QVector3D m_u;
    QVector3D m_v;
    QVector3D m_w;
    double m_halfWidth;
    double m_halfHeight;
    double m_halfDepth;
};

// class Cube : protected Box
// {
// public:
//     Cube(const QVector3D &cent = QVector3D(0,0,0), double he = 1); // with u,v,w = x,y,z => cube ctr
//     Cube(const Cube &other);
// 
//     QVector3D center() const { return Box::center(); }
//     void setCenter(double x, double y, double z) { Box::setCenter(QVector3D(x,y,z)); }
//     void setCenter(const QVector3D &c) { Box::setCenter(c); }
// 
//     double halfEdge() const { return Box::halfWidth(); }
//     void setHalfEdge(double he) { Box::setHalfWidth(he); }
//     
//     //TODO
// //     QVector3D u() const { return m_u; }
// //     QVector3D v() const { return m_v; }
// //     QVector3D w() const { return m_w; }
//     
//     
//     
// };

struct Cube
{
    Cube(const QVector3D cent = QVector3D(0,0,0), double halfe = 1) {}
    Cube(const Cube &other) :c(other.c), he(other.he) {}

    QVector3D c;
    double he;
    
    QVector3D center() const { return c; }
    double halfEdge() const { return he; }
    void setCenter(double x, double y, double z) { c = QVector3D(x,y,z); }
    void setCenter(const QVector3D &cent) { c = cent; }
    void setHalfEdge(double halfe) { he = halfe; }

};

static bool isSimilar(double a, double b, double diff = .0000001)
{
    return fabs(a-b) < diff;
}

static bool traverse(double p1, double p2, double next)
{
    static const double delta=3;
    double diff=p2-p1, diff2=next-p2;
    
    bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);
    
    return ret;
}

static QLineF slopeToLine(const double &der)
{
    double arcder = atan(der);
    const double len=6.*der;
    QPointF from, to;
    from.setX(len*cos(arcder));
    from.setY(len*sin(arcder));

    to.setX(-len*cos(arcder));
    to.setY(-len*sin(arcder));
    return QLineF(from, to);
}

static QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}

//NOTE
//qvector3d/qpointf no soporta doble precision, es mejor trabajar los calculos en double y para mostrarlos usar recien qvector3d
//por esa razon no pongo un return qvector3d y pongo las nuevas coords by ref

static QPointF polarToCartesian(double radial, double polar)
{
    return QPointF(radial*cos(polar), radial*sin(polar)); 
}

static QVector3D cylindricalToCartesian(double radial, double polar, double height)
{
    return QVector3D(radial*cos(polar), radial*sin(polar), height);
}

static QVector3D sphericalToCartesian(double radial, double azimuth, double polar)
{
    return QVector3D(radial*cos(azimuth)*sin(polar), radial*sin(azimuth)*sin(polar), radial*cos(polar));
}


//PRIVATE utils folder colocar estas clases


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
struct sNodo{
    Cube cubo;
    sNodo* nodos[8];
};

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

///

















///















///



struct sLimitesEspacio {
    int minX;
    int maxX;
    int minY;
    int maxY;
    int minZ;
    int maxZ;
};

struct sMarching_Cube{
    QVector3D centro;
    double medio_lado;
    unsigned short int tipo;
    double vertices[8];
};

struct sArista{
    QVector3D corte;
    unsigned int vertices[2];
};


class MarchingCubes
{
    
public:
    virtual double evalScalarField(double x, double y, double z) = 0;

public:
    
//     double funcion(double x, double y, double z)
//     {
// //     if(MarchingCubes::esSolido) {
// //         if(punto.x()==0 || punto.y()==0 || punto.z()==0)
// //             return qPow(punto.x()-MarchingCubes::constantes.at(0),2) + qPow(punto.y()-MarchingCubes::constantes.at(1),2) + qPow(punto.z()-MarchingCubes::constantes.at(2),2) - qPow(MarchingCubes::constantes.at(3),2);
// //         else
// //            return 1;
// //     }else {
// //             return qPow(punto.x()-MarchingCubes::constantes.at(0),2) + qPow(punto.y()-MarchingCubes::constantes.at(1),2) + qPow(punto.z()-MarchingCubes::constantes.at(2),2) - qPow(MarchingCubes::constantes.at(3),2);
// //     } return 0.0;
// 
// //     return punto.x()*punto.x() + punto.y()*punto.y() + punto.z()*punto.z() - 4;
// 
// 
// // return cos(x) + cos(y) + cos(z);
// 
// // return   3*(cos(x) + cos(y) + cos(z)) + 4* cos(x) * cos(y) * cos(z);
// 
//         float ret = 0;
//         if (x*x + y*y +z*z < 35)
//             ret = 2 - (cos(x + (1+sqrt(5))/2*y) + cos(x - (1+sqrt(5))/2*y) + cos(y +
//                        (1+sqrt(5))/2*z) + cos(y - (1+sqrt(5))/2*z) + cos(z - (1+sqrt(5))/2*x) + cos(z + (1+sqrt(5))/2*x));
//         else ret = 1;
// 
// 
//         return ret;
// 
// 
//     }
    
private:
    /*
     largo_mundo: arista del mundo
     min_grid: arista minima del cubo a evaluar
    */
    double largo_mundo;
    double min_grid;
    sLimitesEspacio mundo;
    //Evaluar un cubo
    sMarching_Cube evaluar_cubo(Cube cubo);

    //Busqueda recursiva (breadth search)
    QList<Cube> breadth_rec(int cubos_lado);

    //Busqueda recursiva (depth search)
    QList<sMarching_Cube> depth_rec(Octree *arbol, sNodo *nodo);

public:
    //Recibe el tamaño de grilla deseado y el largo del mundo
    //Produce un min_grid menor o igual al proporcionado
    MarchingCubes(/*double min_grid, double arista_mundo, sLimitesEspacio limites*/);

    //Destructor
    ~MarchingCubes();

    //Ejecutar
    QList<sMarching_Cube> ejecutar();
    
    friend class ImplicitSurf;
public:
    void buildGeometry();

    QVector<Face> _faces_;

    void _addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c);
    
private:


    //Calcular los cortes
    QList<sArista> calcular_cortes(sMarching_Cube cubo);
    bool signo_opuesto(double a, double b);

    //Calcular la posicion en la arista
    double lineal(double vert_1, double vert_2);

    //Agregar triangulos
    void agregar_triangulos(QList<QVector3D> &lista_triangulos);

    //Tipos:
    void identificar_tipo(sMarching_Cube cubo);
    void tipo01(QList<sArista> aristas, QList<unsigned int> vertices);
    void tipo02(QList<sArista> aristas);
    void tipo04(QList<sArista> aristas, QList<unsigned int> vertices);
    void tipo05(QList<sArista> aristas, QList<unsigned int> vertices);
    void tipo06(QList<sArista> aristas, QList<unsigned int> vertices, int ind_vertice_solitario);
    void tipo08(QList<sArista> aristas, QList<unsigned int> vertices, unsigned int ind_vertice_solitario);
    void tipo09(QList<sArista> aristas, QList<unsigned int> vertices);
    void tipo11(QList<sArista> aristas, QList<unsigned int> vertices);
    void tipo13(QList<sArista> aristas, QList<unsigned int> vertices);
};



#endif // ANALITZAPLOT_FUNCTIONUTILS
