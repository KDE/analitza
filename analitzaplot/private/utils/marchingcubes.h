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


#ifndef FUNCTIONGRAPH2_H_mcub
#define FUNCTIONGRAPH2_H_mcub

#include "octree.h"
///

#include "triangle3d.h"

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

//TODO very bad implementation ... we need to use interval arithmetic plus root finding 
//to know if a 0 belongs to f(square)


//TODO esta clase se debe parametrizar para que pueda ser usada por implictcurves:
//MarchingSquares ... en general debe recibir como parametro el tree (octree,kdtree...)
//y las subclases deben implementar evalScalarfield ...pues esta clase sirve para eso:
//para convertir un campo K escalar en poligonos.
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

    QVector<Triangle3D> _faces_;

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

#endif
