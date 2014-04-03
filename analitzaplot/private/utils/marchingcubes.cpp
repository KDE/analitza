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

#include "marchingcubes.h"

#include <QDebug>

//
//@percy spec/impl details
//
// vertex | moves from center (C)
// 0 | (-, -, -)
// 1 | (-, -, +)
// 2 | (-, +, -)
// 3 | (-, +, +)
// 4 | (+, -, -)
// 5 | (+, -, +)
// 6 | (+, +, -)
// 7 | (+, +, +)
// Notes: moves from center means the offsets (offsetfactor x,offsetfactor y,offsetfactor z) 
// where offsetfactor is + or - and the factor is the half-edge
// 
// edge | vertex from -> vertex to | axis (or paralel axis)
// 0  | 0 -> 1 | z
// 1  | 0 -> 2 | y
// 2  | 0 -> 4 | x
// 3  | 1 -> 3 | y
// 4  | 1 -> 5 | x
// 5  | 2 -> 3 | z
// 6  | 2 -> 6 | x
// 7  | 3 -> 7 | x
// 8  | 4 -> 5 | z
// 9  | 4 -> 6 | y
// 10 | 5 -> 7 | y
// 11 | 6 -> 7 | z
// 
// Notes: (->) means to low value from high value (in the direction of its axis)
// see the next ref system:
//        
//            +Z
//            |
//            |
//            |
//            |
//    /|      |________>+Y
// EYE |     /
//    \|    /
//         /
//        -X
// ... and the ref cube is:
//
//             3
//        1---------3
//      4/:        /|
//      / : 10    / |5
//     5---------7  |
//     |  :  C   |  |
//    8|  0......|..2
//     |2.    1  | /
//     |.      11|/6
//     4---------6
//         9

sMarching_Cube MarchingCubes::evalCube(const Cube& cubo)
{
    sMarching_Cube res;
    QVector3D punto;
    unsigned short int val;


    //basic data
    res.center = cubo.center();
    res.half_size = cubo.halfEdge();

    //fill vertices
    double x = res.center.x();
    double y = res.center.y();
    double z = res.center.z();
    double hedge = res.half_size;
    
    
    res.vertices[0] = evalScalarField(x-hedge, y-hedge, z-hedge);
    res.vertices[1] = evalScalarField(x-hedge, y-hedge, z+hedge);
    res.vertices[2] = evalScalarField(x-hedge, y+hedge, z-hedge);
    res.vertices[3] = evalScalarField(x-hedge, y+hedge, z+hedge);
    res.vertices[4] = evalScalarField(x+hedge, y-hedge, z-hedge);
    res.vertices[5] = evalScalarField(x+hedge, y-hedge, z+hedge);
    res.vertices[6] = evalScalarField(x+hedge, y+hedge, z-hedge);
    res.vertices[7] = evalScalarField(x+hedge, y+hedge, z+hedge);

    //define topology type
    res.type = 0;
    val=1;
    
    //sum each vertex based on position
    for(unsigned int i=0;i<8;i++){
        if(res.vertices[i] > 0){
            res.type += val;
        }
        val*=2;
    }
    
    return res;
}

QList<Cube> MarchingCubes::breadthSearch(int cubos_lado){
    Cube cubo;
    sMarching_Cube m_cubo;
    bool salir = false;
    QList<Cube> cubos;
    cubo.setHalfEdge(m_worldLength/(2*cubos_lado));

    double x = 0;
    double y = 0;
    double z = 0;
    
// static const double iteration_square_val = 0.5;

    for(int i=m_worldLimits.minX;i<=m_worldLimits.maxX;i++){
//         cubo.centro.x() = (2*i+1)*cubo.medio_lado;
        x = (2*i+1)*cubo.halfEdge();
        
        for(int j=m_worldLimits.minY;j<=m_worldLimits.maxY;j++){
            y = (2*j+1)*cubo.halfEdge();
            for(int k=m_worldLimits.minZ;k<=m_worldLimits.maxZ;k++){
                z = (2*k+1)*cubo.halfEdge();
                cubo.setCenter(x,y,z);
                m_cubo = evalCube(cubo);
                if(m_cubo.type != 0 && m_cubo.type != 255){
                    //if is inside the cube, stop the search ...
                    salir = true;
                    cubos.append(cubo);
                }
            }
        }
    }
    if(!salir && 2*cubo.halfEdge() > m_minCubeSize){
        cubos.append(breadthSearch(cubos_lado*2));
        //mundo.maxX*=2; mundo.maxY*=2; mundo.maxZ*=2;
    }
    return cubos;
}

QList<sMarching_Cube> MarchingCubes::depthSearch(Octree *arbol, sNode *nodo){
    QList<sMarching_Cube> cubos;
    sMarching_Cube m_cubo;

    //test if surface "cut" the cube
    m_cubo = evalCube(nodo->cube);

    if(m_cubo.type != 0 && m_cubo.type != 255){
        //if intersection
        if(m_cubo.half_size*2 > m_minCubeSize){ 
            //Seguir bajando
            arbol->downLevel(nodo);
            for(unsigned int i=0; i<8; i++){
                cubos.append(depthSearch(arbol,nodo->nodes[i]));
            }
        } else {
            //stop condition
            cubos.append(m_cubo);
        }
    }
    return cubos;
}

MarchingCubes::MarchingCubes()
{

}

void MarchingCubes::setupSpace(const SpaceLimits &spaceLimits)
{
    //TODO no magic numbers
    m_minCubeSize = 0.2;
    m_worldLength = 1;
    m_worldLimits = spaceLimits;    
}

MarchingCubes::~MarchingCubes()
{
}

QList<sMarching_Cube> MarchingCubes::getCubes()
{
    QList<sMarching_Cube> cubos;
    QList<Cube> found = breadthSearch(m_worldLength);

    foreach(const Cube& iterador, found){
        Octree* arbol = new Octree(iterador);
        cubos.append(depthSearch(arbol, arbol->getRoot()));
        delete arbol;
    }

    return cubos;
}

void MarchingCubes::apendTriangle(const QVector3D& a, const QVector3D& b, const QVector3D& c)
{
    QVector3D n = QVector3D::normal(a, b, c);

    _vertices << a.x() << a.y() << a.z() <<
                b.x() << b.y() << b.z() <<
                c.x() << c.y() << c.z();
                
    _normals << n.x() << n.y() << n.z(); 

    _indexes.append(_indexes.size());
    _indexes.append(_indexes.size());
    _indexes.append(_indexes.size());
}

QList<Edge> MarchingCubes::computeIntersections(const sMarching_Cube& cubo)
{
    QList<Edge> aristas;
    Edge temp;
    //0-1
    if(oppositeSign(cubo.vertices[0],cubo.vertices[1])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size,
                               cubo.center.y()-cubo.half_size,
                               cubo.center.z()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[0],cubo.vertices[1]));
        temp.vertices[0] = 0;
        temp.vertices[1] = 1;
        aristas.append(temp);
    }
    //0-2
    if(oppositeSign(cubo.vertices[0],cubo.vertices[2])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size,
                               cubo.center.y()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[0],cubo.vertices[2]),
                               cubo.center.z()-cubo.half_size);
        temp.vertices[0] = 0;
        temp.vertices[1] = 2;
        aristas.append(temp);
    }
    //0-4
    if(oppositeSign(cubo.vertices[0],cubo.vertices[4])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[0],cubo.vertices[4]),
                               cubo.center.y()-cubo.half_size,
                               cubo.center.z()-cubo.half_size);
        temp.vertices[0] = 0;
        temp.vertices[1] = 4;
        aristas.append(temp);
    }
    //1-3
    if(oppositeSign(cubo.vertices[1],cubo.vertices[3])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size,
                               cubo.center.y()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[1],cubo.vertices[3]),
                               cubo.center.z()+cubo.half_size);
        temp.vertices[0] = 1;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }
    //1-5
    if(oppositeSign(cubo.vertices[1],cubo.vertices[5])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[1],cubo.vertices[5]),
                           cubo.center.y()-cubo.half_size,
                           cubo.center.z()+cubo.half_size);
        temp.vertices[0] = 1;
        temp.vertices[1] = 5;
        aristas.append(temp);
    }
    //2-3
    if(oppositeSign(cubo.vertices[2],cubo.vertices[3])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size,
                               cubo.center.y()+cubo.half_size,
                               cubo.center.z()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[2],cubo.vertices[3]));
        temp.vertices[0] = 2;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }
    //2-6
    if(oppositeSign(cubo.vertices[2],cubo.vertices[6])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[2],cubo.vertices[6]),
                           cubo.center.y()+cubo.half_size,
                           cubo.center.z()-cubo.half_size);
        temp.vertices[0] = 2;
        temp.vertices[1] = 6;
        aristas.append(temp);
    }
    //3-7
    if(oppositeSign(cubo.vertices[3],cubo.vertices[7])){
        temp.cut = QVector3D(cubo.center.x()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[3],cubo.vertices[7]),
                           cubo.center.y()+cubo.half_size,
                           cubo.center.z()+cubo.half_size);
        temp.vertices[0] = 3;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    //4-5
    if(oppositeSign(cubo.vertices[4],cubo.vertices[5])){
        temp.cut = QVector3D(cubo.center.x()+cubo.half_size,
                               cubo.center.y()-cubo.half_size,
                               cubo.center.z()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[4],cubo.vertices[5]));
        temp.vertices[0] = 4;
        temp.vertices[1] = 5;
        aristas.append(temp);
    }
    //4-6
    if(oppositeSign(cubo.vertices[4],cubo.vertices[6])){
        temp.cut = QVector3D(cubo.center.x()+cubo.half_size,
                               cubo.center.y()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[4],cubo.vertices[6]),
                               cubo.center.z()-cubo.half_size);
        temp.vertices[0] = 4;
        temp.vertices[1] = 6;
        aristas.append(temp);
    }
    //5-7
    if(oppositeSign(cubo.vertices[5],cubo.vertices[7])){
        temp.cut = QVector3D(cubo.center.x()+cubo.half_size,
                               cubo.center.y()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[5],cubo.vertices[7]),
                               cubo.center.z()+cubo.half_size);
        temp.vertices[0] = 5;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    //6-7
    if(oppositeSign(cubo.vertices[6],cubo.vertices[7])){
        temp.cut = QVector3D(cubo.center.x()+cubo.half_size,
                               cubo.center.y()+cubo.half_size,
                               cubo.center.z()-cubo.half_size+2*cubo.half_size*linearInterpolation(cubo.vertices[6],cubo.vertices[7]));
        temp.vertices[0] = 6;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    return aristas;
}

bool MarchingCubes::oppositeSign(double a, double b){
    return ((a > 0 && b <= 0) || (a <= 0 && b > 0));
}

double MarchingCubes::linearInterpolation(double vert_1, double vert_2){
    //Posicion de 0 a 1
    return qAbs(vert_1/(vert_1 - vert_2));
}

void MarchingCubes::appendTriangles(QList<QVector3D> &lista_triangulos){
    
    for(int i=0; i<lista_triangulos.count();i+=3){
        
        if (lista_triangulos.size()-3 < i)
            continue;

        apendTriangle(lista_triangulos.at(i),lista_triangulos.at(i+1),lista_triangulos.at(i+2));

    }

}

void MarchingCubes::computeTopologyType(const sMarching_Cube& cubo){
    QList<Edge> aristas;
    QList<unsigned int> vertices;
    unsigned int it;

    //Conseguir aristas y vertices
    aristas = computeIntersections(cubo);
    it=0;
    for(unsigned int i=1; i<129; i*=2){
        if((cubo.type & i) == i){
            vertices.append(it);
        }
        it++;
    }
    if(vertices.count() > 4){
        it=0;
        vertices.clear();
        for(unsigned int i=1; i<129; i*=2){
            if((cubo.type & i) != i){
                vertices.append(it);
            }
            it++;
        }
    }

    //get type
    switch(aristas.count()){
    case 3:
        //type 1
        type01(aristas, vertices);
        return ;
    case 4:
        //types 2, 5
        if(vertices.count() == 2){
            //type 2
            type02(aristas);
            return ;
        } else {
            //type 5
            type05(aristas, vertices);
            return ;
        }
    case 5:
    {
        //type 4
        type04(aristas, vertices);
        return ;
    }
    case 6:
        //types 3, 8, 9, 10, 14
        if(vertices.count() == 2){
            //types 3 or 10 -> type01 can draw this cases too
             type01(aristas, vertices); return ;
        } else {
            for(int i=0; i<vertices.count(); i++){
                bool tiene_arista = false;
                for(int j=0; j<aristas.count(); j++){
                    if(aristas.at(j).vertices[0] == vertices.at(i) || aristas.at(j).vertices[1] == vertices.at(i)){
                        tiene_arista = true;
                        break;
                    }
                }
                if(!tiene_arista){
                    //Tipo 8
                    type08(aristas, vertices, i);
                    return ;
                }
            }
            //types 9 or 14 (are the same)
            type09(aristas,vertices);
            return ;
        }
    case 7:
        //type 11
        {
            type11(aristas, vertices);
            return ;
        }
    case 8:
    {
        //types 6, 13
        bool encontrado;
        for(int i=0; i<vertices.count()-1; i++){
            encontrado = true;
            for(int j=i+1;j<vertices.count();j++){
                if(vertices.at(j) - vertices.at(i) == 1 ||
                   vertices.at(j) - vertices.at(i) == 2 ||
                   vertices.at(j) - vertices.at(i) == 4 ){
                    encontrado = false;
                    break;
                    
                }
            }
            if(encontrado){
                //type 6
                type06(aristas, vertices, i);
                return ;
            }
        }
        //type 13
        type13(aristas, vertices);
        return ;
    }
    case 9:
        //type 12 -> type01 can draw this case
        {
        type01(aristas, vertices);
        return ;
        }
    case 12:
        //type 7 -> type01 can draw this case
        {
                    type01(aristas, vertices);
            return ;
            
        }
    default: qDebug() << "Can't compute the surface type"; break;
    }
}

void MarchingCubes::type01(QList<Edge> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;

    for(int i=0; i<vertices.count(); i++){
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0]==vertices[i] || aristas.at(j).vertices[1]==vertices[i]){
                triangulos << aristas.at(j).cut;
            }
        }
    }
    appendTriangles(triangulos);
}
void MarchingCubes::type02(QList<Edge> aristas){
    QList<QVector3D> triangulos;
    triangulos << aristas.at(0).cut;
    triangulos << aristas.at(1).cut;
    triangulos << aristas.at(2).cut;
    triangulos << aristas.at(1).cut;
    triangulos << aristas.at(2).cut;
    triangulos << aristas.at(3).cut;
    appendTriangles(triangulos);
}

void MarchingCubes::type04(QList<Edge> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    unsigned int encontrado, sentido, pos_arista;
    QList< QList<unsigned int> > pos;

    //find the vertex with just one cut
    for(int i=0;i<vertices.count();i++){
        encontrado = 0;
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0]==vertices[i] || aristas.at(j).vertices[1]==vertices[i]){
                encontrado++;
                pos_arista = j;
                if(encontrado == 2){
                    break;
                }
            }
        }
        if(encontrado == 1){
            //we have the singleton vertex in i, so we can get first triangle
            sentido = aristas.at(pos_arista).vertices[1]-aristas.at(pos_arista).vertices[0];
            for(int j=0; j<aristas.count(); j++){
                if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
                    triangulos << aristas.at(j).cut;
                }
            }
            aristas.removeAt(pos_arista);

            //group by common vertex
            for(int k=0;k<vertices.count();k++){
                if(i==k){
                    continue;
                }
                pos.append(QList<unsigned int>());
                for(int j=0; j<aristas.count(); j++){
                    if(aristas.at(j).vertices[0] == vertices[k] || aristas.at(j).vertices[1] == vertices[k]){
                        pos.back().append(j);
                    }
                }
            }

            //first triangle
            triangulos << aristas.at(pos.at(0).at(0)).cut;
            triangulos << aristas.at(pos.at(0).at(1)).cut;
            for(int j=0; j<pos.at(1).count(); j++){
                if(aristas.at(pos.at(1).at(j)).vertices[1] - aristas.at(pos.at(1).at(j)).vertices[0] == sentido){
                    triangulos << aristas.at(pos.at(1).at(j)).cut;
                }
            }

            //second triangle
            triangulos << aristas.at(pos.at(1).at(0)).cut;
            triangulos << aristas.at(pos.at(1).at(1)).cut;
            for(int j=0; j<pos.at(0).count(); j++){
                if(aristas.at(pos.at(0).at(j)).vertices[1] - aristas.at(pos.at(0).at(j)).vertices[0] != sentido){
                    triangulos << aristas.at(pos.at(0).at(j)).cut;
                }
            }
            break;
        }
    }
    appendTriangles(triangulos);
}
void MarchingCubes::type05(QList<Edge> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    int vertice_arista[4];
    //indentify each vertex with its edges (vertices are sort from low to high)
    for(int i=0; i<vertices.count(); i++){
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0] == vertices.at(i) || aristas.at(j).vertices[1] == vertices.at(i)){
                vertice_arista[i] = j;
                break;
            }
        }
    }
    //Pintar triangulos
    triangulos << aristas.at(vertice_arista[0]).cut;
    triangulos << aristas.at(vertice_arista[1]).cut;
    triangulos << aristas.at(vertice_arista[2]).cut;
    triangulos << triangulos.at(1);
    triangulos << triangulos.at(2);
    triangulos << aristas.at(vertice_arista[3]).cut;

    appendTriangles(triangulos);
}
void MarchingCubes::type06(QList<Edge> aristas, QList<unsigned int> vertices, int ind_vertice_solitario){
    //type 1 + 4
    QList<Edge> aristas2;
    QList<unsigned int> vertices2;
    //generate edge2
    for(int i=0; i<aristas.count();i++){
        if(aristas.at(i).vertices[0] != vertices.at(ind_vertice_solitario)
           && aristas.at(i).vertices[1] != vertices.at(ind_vertice_solitario)){
            aristas2.append(aristas.at(i));
            aristas.removeAt(i);
            i--;
        }
    }
    vertices2.append(vertices.at(ind_vertice_solitario));
    vertices.removeAt(ind_vertice_solitario);
}

void MarchingCubes::type08(QList<Edge> aristas, QList<unsigned int> vertices, unsigned int ind_vertice_solitario){
    QList<QVector3D> triangulos;
    unsigned int ind_vert = 0, sentido;
    QList<int> orden;
    if(ind_vertice_solitario == 0){
        ind_vert = 1;
    }

    //join 2 points associated with first vertex
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[0] == vertices.at(ind_vert) || aristas.at(j).vertices[1] == vertices.at(ind_vert)){
            triangulos << aristas.at(j).cut;
            orden.append(j);
        }
    }
    //join with points associated with 2nd vertex
    sentido = aristas.at(orden.at(0)).vertices[1] - aristas.at(orden.at(0)).vertices[0];
    for(int j=0; j<aristas.count(); j++){
        if(orden.at(0) == j || orden.at(1) == j){
            continue;
        }
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).cut;
            orden.append(j);
            for(int k=0; k<vertices.count(); k++){
                if(aristas.at(j).vertices[0] == vertices.at(k) || aristas.at(j).vertices[1] == vertices.at(k)){
                    ind_vert = k;
                    break;
                }
            }
            break;
        }
    }
    triangulos << triangulos.at(1);
    triangulos << triangulos.at(2);
    for(int j=0; j<aristas.count(); j++){
        if((aristas.at(j).vertices[0] == vertices.at(ind_vert) || aristas.at(j).vertices[1] == vertices.at(ind_vert))
            && (aristas.at(j).vertices[1] - aristas.at(j).vertices[0] != sentido)){
            triangulos << aristas.at(j).cut;
            orden.append(j);
            break;
        }
    }
    //2 triangles, we need 2 more ...
    sentido = aristas.at(orden.at(1)).vertices[1] - aristas.at(orden.at(1)).vertices[0];
    triangulos << triangulos.at(0);
    triangulos << triangulos.at(2);

    for(int j=0; j<aristas.count(); j++){
        if(orden.at(0) == j || orden.at(1) == j || orden.at(2) == j || orden.at(3) == j){
            continue;
        }
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).cut;
            orden.append(j);
            for(int k=0; k<vertices.count(); k++){
                if(aristas.at(j).vertices[0] == vertices.at(k) || aristas.at(j).vertices[1] == vertices.at(k)){
                    ind_vert = k;
                    break;
                }
            }
            break;
        }
    }
    triangulos << triangulos.at(0);
    triangulos << triangulos.at(8);
    for(int j=0; j<aristas.count(); j++){
        if((aristas.at(j).vertices[0] == vertices.at(ind_vert) || aristas.at(j).vertices[1] == vertices.at(ind_vert))
            && (aristas.at(j).vertices[1] - aristas.at(j).vertices[0] != sentido)){
            triangulos << aristas.at(j).cut;
            orden.append(j);
            break;
        }
    }

    appendTriangles(triangulos);
}
void MarchingCubes::type09(QList<Edge> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    QList<int> vertices_doble;
    QList< QList<int> > aristas_doble;
    bool doble;
    unsigned int sentido, eje_compartido;

    //find double vertices
    for(int i=0; i<vertices.count(); i++){
        doble = false;
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0] == vertices.at(i) || aristas.at(j).vertices[1] == vertices.at(i)){
                if(doble){
                    vertices_doble.append(i);
                    break;
                } else {
                    doble = true;
                }
            }
        }
    }
    //find vertices of double vertices
    aristas_doble.append(QList<int>());
    aristas_doble.append(QList<int>());
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[0] == vertices.at(vertices_doble.at(0)) || aristas.at(j).vertices[1] == vertices.at(vertices_doble.at(0))){
            aristas_doble.front().append(j);
        } else if(aristas.at(j).vertices[0] == vertices.at(vertices_doble.at(1)) || aristas.at(j).vertices[1] == vertices.at(vertices_doble.at(1))){
            aristas_doble.back().append(j);
        }
    }
    //find shared axis
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++){
            if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] ==
               aristas.at(aristas_doble.at(1).at(j)).vertices[1] - aristas.at(aristas_doble.at(1).at(j)).vertices[0]){
                eje_compartido = aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0];
                break;
            }
        }
    }
    //first triangle
    for(int i=0; i<2; i++){
        triangulos << aristas.at(aristas_doble.at(0).at(i)).cut;
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).cut;
            break;
       }
    }
    //second triangle
    for(int i=0; i<2; i++){
        triangulos << aristas.at(aristas_doble.at(1).at(i)).cut;
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).cut;
            break;
       }
    }
    //third triangle
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).cut;
            break;
       }
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] != eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).cut;
            sentido = aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0];
            break;
       }
    }
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).cut;
        }
    }
    //four triangle
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] != eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).cut;
            sentido = aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0];
            break;
       }
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).cut;
            break;
       }
    }
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).cut;
        }
    }

    appendTriangles(triangulos);
}

void MarchingCubes::type11(QList<Edge> aristas, QList<unsigned int> vertices){
    //type 1 + 2
    unsigned int vert_solitario;
    bool encontrado;
    QList<Edge> aristas2;

    //find singleton vertex
    for(int i=0; i<vertices.count()-1; i++){
        encontrado = true;
        for(int j=i+1;j<vertices.count();j++){
            if(vertices.at(j) - vertices.at(i) == 1 ||
               vertices.at(j) - vertices.at(i) == 2 ||
               vertices.at(j) - vertices.at(i) == 4 ){
                //Son contiguos
                encontrado = false;
                break;
            }
        }
        if(encontrado){
            vert_solitario = vertices.at(i);
            break;
        }
    }
    //generate edge2
    for(int i=0; i<aristas.count();i++){
        if(aristas.at(i).vertices[0] != vert_solitario
           && aristas.at(i).vertices[1] != vert_solitario){
            aristas2.append(aristas.at(i));
            aristas.removeAt(i);
            i--;
        }
    }
    vertices.clear();
    vertices.append(vert_solitario);
}

void MarchingCubes::type13(QList<Edge> aristas, QList<unsigned int> vertices){
    //type 2 + 2
    unsigned int verts[2];
    QList<Edge> aristas2;

    //find couple of vertices
    verts[0] = vertices.at(0);
    for(int i=1; i<vertices.count(); i++){
        if(vertices.at(i) - verts[0] == 1 ||
               vertices.at(i) - verts[0] == 2 ||
               vertices.at(i) - verts[0] == 4 ){
            verts[1] = vertices.at(i);
            break;
        }
    }
    //generate edge2
    for(int i=0; i<aristas.count();i++){
        if(aristas.at(i).vertices[0] != verts[0]
           && aristas.at(i).vertices[1] != verts[0]
           && aristas.at(i).vertices[0] != verts[1]
           && aristas.at(i).vertices[1] != verts[1]){
            aristas2.append(aristas.at(i));
            aristas.removeAt(i);
            i--;
        }
    }
}

void MarchingCubes::buildGeometry()
{
    _vertices.clear();
    _normals.clear();
    _indexes.clear();
    
    QList<sMarching_Cube> cubos = getCubes();

    sMarching_Cube cubo;
    foreach(cubo, cubos) {
        //we can change the type now ... 
        if(cubo.type > 127){
            cubo.type = 255 - cubo.type;
        }
        computeTopologyType(cubo);
     }
}
