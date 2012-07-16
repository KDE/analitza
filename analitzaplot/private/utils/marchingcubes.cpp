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


///
#include "marchingcubes.h"



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


sMarching_Cube MarchingCubes::evaluar_cubo(Cube cubo){
    sMarching_Cube res;
    QVector3D punto;
    unsigned short int val;


    //Datos generales
    res.centro = cubo.center();
    res.medio_lado = cubo.halfEdge();

    //Llenar vertices
    double x = res.centro.x();
    double y = res.centro.y();
    double z = res.centro.z();
    double hedge = res.medio_lado;
    
    
    res.vertices[0] = evalScalarField(x-hedge, y-hedge, z-hedge);
    res.vertices[1] = evalScalarField(x-hedge, y-hedge, z+hedge);
    res.vertices[2] = evalScalarField(x-hedge, y+hedge, z-hedge);
    res.vertices[3] = evalScalarField(x-hedge, y+hedge, z+hedge);
    res.vertices[4] = evalScalarField(x+hedge, y-hedge, z-hedge);
    res.vertices[5] = evalScalarField(x+hedge, y-hedge, z+hedge);
    res.vertices[6] = evalScalarField(x+hedge, y+hedge, z-hedge);
    res.vertices[7] = evalScalarField(x+hedge, y+hedge, z+hedge);

    //Definir tipo
    res.tipo = 0;
    val=1;
    //Sumar cada vertice segun su posicion
    for(unsigned int i=0;i<8;i++){
        if(res.vertices[i] > 0){
            res.tipo += val;
        }
        val*=2;
    }
    return res;
}

QList<Cube> MarchingCubes::breadth_rec(int cubos_lado){
    Cube cubo;
    sMarching_Cube m_cubo;
    bool salir = false;
    QList<Cube> cubos;
    cubo.setHalfEdge(largo_mundo/(2*cubos_lado));

    double x = 0;
    double y = 0;
    double z = 0;
    
    for(int i=mundo.minX;i<=mundo.maxX;i++){
//         cubo.centro.x() = (2*i+1)*cubo.medio_lado;
        x = (2*i+1)*cubo.halfEdge();
        
        for(int j=mundo.minY;j<=mundo.maxY;j++){
            y = (2*j+1)*cubo.halfEdge();
            for(int k=mundo.minZ;k<=mundo.maxZ;k++){
                z = (2*k+1)*cubo.halfEdge();
                cubo.setCenter(x,y,z);
                m_cubo = evaluar_cubo(cubo);
                if(m_cubo.tipo != 0 && m_cubo.tipo != 255){
                    //Esta dentro del cubo. Detener busqueda
                    salir = true;
                    cubos.append(cubo);
                }
            }
        }
    }
    if(!salir && 2*cubo.halfEdge() > min_grid){
        cubos.append(breadth_rec(cubos_lado*2));
        //mundo.maxX*=2; mundo.maxY*=2; mundo.maxZ*=2;
    }
    return cubos;
}

QList<sMarching_Cube> MarchingCubes::depth_rec(Octree *arbol, sNodo *nodo){
    QList<sMarching_Cube> cubos;
    sMarching_Cube m_cubo;

    //Calcular si la superfice lo corta
    m_cubo = evaluar_cubo(nodo->cubo);

    if(m_cubo.tipo != 0 && m_cubo.tipo != 255){
        //Superfice corta
        if(m_cubo.medio_lado*2 > min_grid){
            //Seguir bajando
            arbol->bajarNivel(nodo);
            for(unsigned int i=0; i<8; i++){
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

MarchingCubes::MarchingCubes(/*double min_grid, double arista_mundo, sLimitesEspacio limites*/){
    //TODO enlazar con arg interval
        sLimitesEspacio _esp;

            _esp.minX = -6; _esp.maxX = 6;
        _esp.minY = -6; _esp.maxY = 6;
        _esp.minZ = -6; _esp.maxZ = 6;

    
    
    this->min_grid = 0.4;
    largo_mundo = 5;
    mundo = _esp;
}

MarchingCubes::~MarchingCubes(){
}

QList<sMarching_Cube> MarchingCubes::ejecutar(){
    QList<sMarching_Cube> cubos;
    QList<Cube> encontrados;
    Cube iterador;
    Octree *arbol;

    //Buscar la superficie (breadth search octree)
    encontrados = breadth_rec(largo_mundo);
    //Si no encuentra la superficie, retorna lista vacia
    if(encontrados.isEmpty()){
        return cubos;
    }
//     printf("Encontrados: %d\n",encontrados.count());

    //Ubicar los cubos (depth search octree)
    foreach(iterador, encontrados){
        arbol = new Octree(iterador);
        cubos.append(depth_rec(arbol, arbol->get_raiz()));
        delete arbol;
    }

    //Devolver los cubos
    return cubos;
}


void MarchingCubes::_addTri(const QVector3D& a, const QVector3D& b, const QVector3D& c)
{
        Triangle3D _f(a,b,c);
    _faces_.append(_f);

}


QList<sArista> MarchingCubes::calcular_cortes(sMarching_Cube cubo){
    QList<sArista> aristas;
    sArista temp;
    //0-1
    if(signo_opuesto(cubo.vertices[0],cubo.vertices[1])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado,
                               cubo.centro.z()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[1]));
        temp.vertices[0] = 0;
        temp.vertices[1] = 1;
        aristas.append(temp);
    }
    //0-2
    if(signo_opuesto(cubo.vertices[0],cubo.vertices[2])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[2]),
                               cubo.centro.z()-cubo.medio_lado);
        temp.vertices[0] = 0;
        temp.vertices[1] = 2;
        aristas.append(temp);
    }
    //0-4
    if(signo_opuesto(cubo.vertices[0],cubo.vertices[4])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[0],cubo.vertices[4]),
                               cubo.centro.y()-cubo.medio_lado,
                               cubo.centro.z()-cubo.medio_lado);
        temp.vertices[0] = 0;
        temp.vertices[1] = 4;
        aristas.append(temp);
    }
    //1-3
    if(signo_opuesto(cubo.vertices[1],cubo.vertices[3])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[1],cubo.vertices[3]),
                               cubo.centro.z()+cubo.medio_lado);
        temp.vertices[0] = 1;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }
    //1-5
    if(signo_opuesto(cubo.vertices[1],cubo.vertices[5])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[1],cubo.vertices[5]),
                           cubo.centro.y()-cubo.medio_lado,
                           cubo.centro.z()+cubo.medio_lado);
        temp.vertices[0] = 1;
        temp.vertices[1] = 5;
        aristas.append(temp);
    }
    //2-3
    if(signo_opuesto(cubo.vertices[2],cubo.vertices[3])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado,
                               cubo.centro.y()+cubo.medio_lado,
                               cubo.centro.z()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[2],cubo.vertices[3]));
        temp.vertices[0] = 2;
        temp.vertices[1] = 3;
        aristas.append(temp);
    }
    //2-6
    if(signo_opuesto(cubo.vertices[2],cubo.vertices[6])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[2],cubo.vertices[6]),
                           cubo.centro.y()+cubo.medio_lado,
                           cubo.centro.z()-cubo.medio_lado);
        temp.vertices[0] = 2;
        temp.vertices[1] = 6;
        aristas.append(temp);
    }
    //3-7
    if(signo_opuesto(cubo.vertices[3],cubo.vertices[7])){
        temp.corte = QVector3D(cubo.centro.x()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[3],cubo.vertices[7]),
                           cubo.centro.y()+cubo.medio_lado,
                           cubo.centro.z()+cubo.medio_lado);
        temp.vertices[0] = 3;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    //4-5
    if(signo_opuesto(cubo.vertices[4],cubo.vertices[5])){
        temp.corte = QVector3D(cubo.centro.x()+cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado,
                               cubo.centro.z()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[4],cubo.vertices[5]));
        temp.vertices[0] = 4;
        temp.vertices[1] = 5;
        aristas.append(temp);
    }
    //4-6
    if(signo_opuesto(cubo.vertices[4],cubo.vertices[6])){
        temp.corte = QVector3D(cubo.centro.x()+cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[4],cubo.vertices[6]),
                               cubo.centro.z()-cubo.medio_lado);
        temp.vertices[0] = 4;
        temp.vertices[1] = 6;
        aristas.append(temp);
    }
    //5-7
    if(signo_opuesto(cubo.vertices[5],cubo.vertices[7])){
        temp.corte = QVector3D(cubo.centro.x()+cubo.medio_lado,
                               cubo.centro.y()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[5],cubo.vertices[7]),
                               cubo.centro.z()+cubo.medio_lado);
        temp.vertices[0] = 5;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    //6-7
    if(signo_opuesto(cubo.vertices[6],cubo.vertices[7])){
        temp.corte = QVector3D(cubo.centro.x()+cubo.medio_lado,
                               cubo.centro.y()+cubo.medio_lado,
                               cubo.centro.z()-cubo.medio_lado+2*cubo.medio_lado*lineal(cubo.vertices[6],cubo.vertices[7]));
        temp.vertices[0] = 6;
        temp.vertices[1] = 7;
        aristas.append(temp);
    }
    return aristas;
}

bool MarchingCubes::signo_opuesto(double a, double b){
    return ((a > 0 && b <= 0) || (a <= 0 && b > 0));
}

double MarchingCubes::lineal(double vert_1, double vert_2){
    //Posicion de 0 a 1
    return qAbs(vert_1/(vert_1 - vert_2));
}

void MarchingCubes::agregar_triangulos(QList<QVector3D> &lista_triangulos){
    
    for(int i=0; i<lista_triangulos.count();i+=3){
        
        if (lista_triangulos.size()-3 < i)
            continue;

        _addTri(lista_triangulos.at(i),lista_triangulos.at(i+1),lista_triangulos.at(i+2));

    }

}

//Tipos:
void MarchingCubes::identificar_tipo(sMarching_Cube cubo){
    QList<sArista> aristas;
    QList<unsigned int> vertices;
    unsigned int it;

    //Conseguir aristas y vertices
    aristas = calcular_cortes(cubo);
    it=0;
    for(unsigned int i=1; i<129; i*=2){
        if((cubo.tipo & i) == i){
            vertices.append(it);
        }
        it++;
    }
    if(vertices.count() > 4){
        it=0;
        vertices.clear();
        for(unsigned int i=1; i<129; i*=2){
            if((cubo.tipo & i) != i){
                vertices.append(it);
            }
            it++;
        }
    }

    //Identificar tipo
    switch(aristas.count()){
    case 3:
        //Tipo 1
        tipo01(aristas, vertices);
        return ;
    case 4:
        //Tipo 2, 5
        if(vertices.count() == 2){
            //Tipo 2
            tipo02(aristas);
            return ;
        } else {
            //Tipo 5
            tipo05(aristas, vertices);
            return ;
        }
    case 5:
    {
        //Tipo 4
        tipo04(aristas, vertices);
        return ;
    }
    case 6:
        //Tipo 3, 8, 9, 10, 14
        if(vertices.count() == 2){
            //Tipo 3 o 10 -> El tipo01 es capaz de dibujar estos casos
            return tipo01(aristas, vertices);
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
                    tipo08(aristas, vertices, i);
                    return ;
                }
            }
            //Tipo 9 o 14 (son iguales)
            tipo09(aristas,vertices);
            return ;
        }
    case 7:
        //Tipo 11
        {
            tipo11(aristas, vertices);
            return ;
        }
    case 8:
    {
        //Tipo 6, 13
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
                //Tipo 6
                tipo06(aristas, vertices, i);
                return ;
            }
        }
        //Tipo 13
        tipo13(aristas, vertices);
        return ;
    }
    case 9:
        //Tipo 12 -> El tipo01 es capaz de dibujar este caso
        {
        tipo01(aristas, vertices);
        return ;
        }
    case 12:
        //Tipo 7 -> El tipo01 es capaz de dibujar este caso
        {
                    tipo01(aristas, vertices);
return ;
            
        }
//     default:
//         printf("Error al calcular tipo\n");
    }
}

void MarchingCubes::tipo01(QList<sArista> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    //Pintar los triangulos
    for(int i=0; i<vertices.count(); i++){
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0]==vertices[i] || aristas.at(j).vertices[1]==vertices[i]){
                triangulos << aristas.at(j).corte;
            }
        }
    }
    agregar_triangulos(triangulos);
}
void MarchingCubes::tipo02(QList<sArista> aristas){
    QList<QVector3D> triangulos;
    triangulos << aristas.at(0).corte;
    triangulos << aristas.at(1).corte;
    triangulos << aristas.at(2).corte;
    triangulos << aristas.at(1).corte;
    triangulos << aristas.at(2).corte;
    triangulos << aristas.at(3).corte;
    agregar_triangulos(triangulos);
}

void MarchingCubes::tipo04(QList<sArista> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    unsigned int encontrado, sentido, pos_arista;
    QList< QList<unsigned int> > pos;

    //Encontrar el vertice con solo un corte asociado
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
            //Tengo el vertice solitario en ´i´, pintar primer triangulo
            sentido = aristas.at(pos_arista).vertices[1]-aristas.at(pos_arista).vertices[0];
            for(int j=0; j<aristas.count(); j++){
                if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
                    triangulos << aristas.at(j).corte;
                }
            }
            aristas.removeAt(pos_arista);

            //Agrupar por vertice comun
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

            //Primer triangulo
            triangulos << aristas.at(pos.at(0).at(0)).corte;
            triangulos << aristas.at(pos.at(0).at(1)).corte;
            for(int j=0; j<pos.at(1).count(); j++){
                if(aristas.at(pos.at(1).at(j)).vertices[1] - aristas.at(pos.at(1).at(j)).vertices[0] == sentido){
                    triangulos << aristas.at(pos.at(1).at(j)).corte;
                }
            }

            //Segundo triangulo
            triangulos << aristas.at(pos.at(1).at(0)).corte;
            triangulos << aristas.at(pos.at(1).at(1)).corte;
            for(int j=0; j<pos.at(0).count(); j++){
                if(aristas.at(pos.at(0).at(j)).vertices[1] - aristas.at(pos.at(0).at(j)).vertices[0] != sentido){
                    triangulos << aristas.at(pos.at(0).at(j)).corte;
                }
            }
            break;
        }
    }
    agregar_triangulos(triangulos);
}
void MarchingCubes::tipo05(QList<sArista> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    int vertice_arista[4];
    //Identificar cada vertice con su arista (los vertices estan ordenados de menor a mayor)
    for(int i=0; i<vertices.count(); i++){
        for(int j=0; j<aristas.count(); j++){
            if(aristas.at(j).vertices[0] == vertices.at(i) || aristas.at(j).vertices[1] == vertices.at(i)){
                vertice_arista[i] = j;
                break;
            }
        }
    }
    //Pintar triangulos
    triangulos << aristas.at(vertice_arista[0]).corte;
    triangulos << aristas.at(vertice_arista[1]).corte;
    triangulos << aristas.at(vertice_arista[2]).corte;
    triangulos << triangulos.at(1);
    triangulos << triangulos.at(2);
    triangulos << aristas.at(vertice_arista[3]).corte;

    agregar_triangulos(triangulos);
}
void MarchingCubes::tipo06(QList<sArista> aristas, QList<unsigned int> vertices, int ind_vertice_solitario){
    //Tipo 1 + 4
    QList<sArista> aristas2;
    QList<unsigned int> vertices2;
    //Generar el aristas2
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

void MarchingCubes::tipo08(QList<sArista> aristas, QList<unsigned int> vertices, unsigned int ind_vertice_solitario){
    QList<QVector3D> triangulos;
    unsigned int ind_vert = 0, sentido;
    QList<int> orden;
    if(ind_vertice_solitario == 0){
        ind_vert = 1;
    }

    //Unir dos puntos asociados al primer vertice
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[0] == vertices.at(ind_vert) || aristas.at(j).vertices[1] == vertices.at(ind_vert)){
            triangulos << aristas.at(j).corte;
            orden.append(j);
        }
    }
    //Unir con los puntos asociados al 2do vertice
    sentido = aristas.at(orden.at(0)).vertices[1] - aristas.at(orden.at(0)).vertices[0];
    for(int j=0; j<aristas.count(); j++){
        if(orden.at(0) == j || orden.at(1) == j){
            continue;
        }
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).corte;
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
            triangulos << aristas.at(j).corte;
            orden.append(j);
            break;
        }
    }
    //2 triangulos pintados, faltan 2
    sentido = aristas.at(orden.at(1)).vertices[1] - aristas.at(orden.at(1)).vertices[0];
    triangulos << triangulos.at(0);
    triangulos << triangulos.at(2);

    for(int j=0; j<aristas.count(); j++){
        if(orden.at(0) == j || orden.at(1) == j || orden.at(2) == j || orden.at(3) == j){
            continue;
        }
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).corte;
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
            triangulos << aristas.at(j).corte;
            orden.append(j);
            break;
        }
    }

    agregar_triangulos(triangulos);
}
void MarchingCubes::tipo09(QList<sArista> aristas, QList<unsigned int> vertices){
    QList<QVector3D> triangulos;
    QList<int> vertices_doble;
    QList< QList<int> > aristas_doble;
    bool doble;
    unsigned int sentido, eje_compartido;

    //Encontrar los vertices dobles
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
    //Encontrar aristas de los vertices dobles
    aristas_doble.append(QList<int>());
    aristas_doble.append(QList<int>());
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[0] == vertices.at(vertices_doble.at(0)) || aristas.at(j).vertices[1] == vertices.at(vertices_doble.at(0))){
            aristas_doble.front().append(j);
        } else if(aristas.at(j).vertices[0] == vertices.at(vertices_doble.at(1)) || aristas.at(j).vertices[1] == vertices.at(vertices_doble.at(1))){
            aristas_doble.back().append(j);
        }
    }
    //Encontrar eje compartido
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++){
            if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] ==
               aristas.at(aristas_doble.at(1).at(j)).vertices[1] - aristas.at(aristas_doble.at(1).at(j)).vertices[0]){
                eje_compartido = aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0];
                break;
            }
        }
    }
    //Primer triangulo
    for(int i=0; i<2; i++){
        triangulos << aristas.at(aristas_doble.at(0).at(i)).corte;
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).corte;
            break;
       }
    }
    //Segundo triangulo
    for(int i=0; i<2; i++){
        triangulos << aristas.at(aristas_doble.at(1).at(i)).corte;
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).corte;
            break;
       }
    }
    //Tercer triangulo
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).corte;
            break;
       }
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] != eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).corte;
            sentido = aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0];
            break;
       }
    }
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).corte;
        }
    }
    //Cuarto triangulo
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0] != eje_compartido){
            triangulos << aristas.at(aristas_doble.at(0).at(i)).corte;
            sentido = aristas.at(aristas_doble.at(0).at(i)).vertices[1] - aristas.at(aristas_doble.at(0).at(i)).vertices[0];
            break;
       }
    }
    for(int i=0; i<2; i++){
        if(aristas.at(aristas_doble.at(1).at(i)).vertices[1] - aristas.at(aristas_doble.at(1).at(i)).vertices[0] == eje_compartido){
            triangulos << aristas.at(aristas_doble.at(1).at(i)).corte;
            break;
       }
    }
    for(int j=0; j<aristas.count(); j++){
        if(aristas.at(j).vertices[1] - aristas.at(j).vertices[0] == sentido){
            triangulos << aristas.at(j).corte;
        }
    }

    agregar_triangulos(triangulos);
}

void MarchingCubes::tipo11(QList<sArista> aristas, QList<unsigned int> vertices){
    //Tipo 1 + 2
    unsigned int vert_solitario;
    bool encontrado;
    QList<sArista> aristas2;

    //Buscar el vertice solitario
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
    //Generar el aristas2
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

void MarchingCubes::tipo13(QList<sArista> aristas, QList<unsigned int> vertices){
    //Tipo 2 + 2
    unsigned int verts[2];
    QList<sArista> aristas2;

    //Encontrar un par de vertices
    verts[0] = vertices.at(0);
    for(int i=1; i<vertices.count(); i++){
        if(vertices.at(i) - verts[0] == 1 ||
               vertices.at(i) - verts[0] == 2 ||
               vertices.at(i) - verts[0] == 4 ){
            verts[1] = vertices.at(i);
            break;
        }
    }
    //Generar el aristas2
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
        _faces_.clear();

        
    QList<sMarching_Cube> cubos;
    sMarching_Cube cubo;
    
    cubos = ejecutar();
//     printf("Cubos: %d\n",cubos.count());

    foreach(cubo,cubos){
        //Puede que ahora sea innecesario cambiar el tipo...
        if(cubo.tipo > 127){
            cubo.tipo = 255 - cubo.tipo;
        }
        identificar_tipo(cubo);
     }

}

