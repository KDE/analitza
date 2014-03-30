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

#include <QVector>

struct SpaceLimits {
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
};

struct sMarching_Cube{
    QVector3D center;
    double half_size;
    unsigned short int type;
    double vertices[8];
};

struct Edge{
    QVector3D cut;
    unsigned int vertices[2];
};

//TODO very bad implementation ... we need to use interval arithmetic plus root finding 
//to know if a 0 belongs to f(square)

class MarchingCubes
{
friend class ImplicitSurf;
    
public:
    virtual double evalScalarField(double x, double y, double z) = 0;

    MarchingCubes();
    
    //here we put the size of intervals for x,z,and z ... call setupSpace before buildGeometry
    void setupSpace(const SpaceLimits &spaceLimits);

    ~MarchingCubes();

    void buildGeometry();

    QVector<double> _vertices;
    QVector<double> _normals;
    QVector<unsigned int> _indexes;

private:
    QList<sMarching_Cube> getCubes();
    sMarching_Cube evalCube(Cube cubo);
    QList<Cube> breadthSearch(int cubos_lado);
    QList<sMarching_Cube> depthSearch(Octree *arbol, sNode *nodo);
    QList<Edge> computeIntersections(sMarching_Cube cubo);
    bool oppositeSign(double a, double b);
    double linearInterpolation(double vert_1, double vert_2);
    void apendTriangle(const QVector3D &a, const QVector3D &b, const QVector3D &c);
    void appendTriangles(QList<QVector3D> &lista_triangulos);
    void computeTopologyType(const sMarching_Cube& cubo);
    void type01(QList<Edge> aristas, QList<unsigned int> vertices);
    void type02(QList<Edge> aristas);
    void type04(QList<Edge> aristas, QList<unsigned int> vertices);
    void type05(QList<Edge> aristas, QList<unsigned int> vertices);
    void type06(QList<Edge> aristas, QList<unsigned int> vertices, int ind_vertice_solitario);
    void type08(QList<Edge> aristas, QList<unsigned int> vertices, unsigned int ind_vertice_solitario);
    void type09(QList<Edge> aristas, QList<unsigned int> vertices);
    void type11(QList<Edge> aristas, QList<unsigned int> vertices);
    void type13(QList<Edge> aristas, QList<unsigned int> vertices);
    
private:
    double m_worldLength;
    double m_minCubeSize;
    SpaceLimits m_worldLimits;
};

#endif
