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

#include "octree.h"


Octree::Octree(double largo_mundo){
    raiz = new sNodo;

    raiz->cubo.setCenter(largo_mundo/2,largo_mundo/2,largo_mundo/2);
    
    raiz->cubo.setHalfEdge(largo_mundo/2);
    for(unsigned int i=0;i<8;i++){
        raiz->nodos[i]=NULL;
    }
}

Octree::Octree(Cube cubo){
    raiz = new sNodo;
    raiz->cubo = cubo;
    for(unsigned int i=0;i<8;i++){
        raiz->nodos[i]=NULL;
    }
}
Octree::~Octree(){
    borrar_rec(raiz);
}

void Octree::inicializar_nodos(sNodo* padre){
    for(unsigned int i=0;i<8;i++){
        padre->nodos[i] = new sNodo;
        padre->nodos[i]->cubo.setHalfEdge(padre->cubo.halfEdge()/2);
        for(unsigned int j=0;j<8;j++){
            padre->nodos[i]->nodos[j]=NULL;
        }
    }
    
    double x = padre->cubo.center().x();
    double y = padre->cubo.center().y();
    double z = padre->cubo.center().z();
    double hhedge = padre->cubo.halfEdge()/2;
    
    
    padre->nodos[0]->cubo.setCenter(x-hhedge, y-hhedge, z-hhedge);
    padre->nodos[1]->cubo.setCenter(x-hhedge, y-hhedge, z+hhedge);
    padre->nodos[2]->cubo.setCenter(x-hhedge, y+hhedge, z-hhedge);
    padre->nodos[3]->cubo.setCenter(x-hhedge, y+hhedge, z+hhedge);
    padre->nodos[4]->cubo.setCenter(x+hhedge, y-hhedge, z-hhedge);
    padre->nodos[5]->cubo.setCenter(x+hhedge, y-hhedge, z+hhedge);
    padre->nodos[6]->cubo.setCenter(x+hhedge, y+hhedge, z-hhedge);
    padre->nodos[7]->cubo.setCenter(x+hhedge, y+hhedge, z+hhedge);

}

void Octree::borrar_rec(sNodo* nodo){
    if(nodo == NULL){
        return;
    }
    for(unsigned int i=0;i<8;i++){
        borrar_rec(nodo->nodos[i]);
    }
    delete nodo;
}


void Octree::crear_rec(sNodo* nodo, unsigned int nivel_actual, unsigned int nivel_max){
    if(nivel_actual > nivel_max){
        return;
    }
    inicializar_nodos(nodo);
    for(unsigned int i=0;i<8;i++){
        crear_rec(nodo->nodos[i],nivel_actual+1,nivel_max);
    }
}

sNodo* Octree::get_raiz(){
    return raiz;
}

void Octree::crearNivel(unsigned int nivel){
    crear_rec(raiz,0,nivel);
}

void Octree::bajarNivel(sNodo* nodo){
    inicializar_nodos(nodo);
}

void Octree::borrarHijos(sNodo* padre){
    for(unsigned int i=0;i<8;i++){
        borrar_rec(padre->nodos[i]);
        padre->nodos[i] = NULL;
    }
}

