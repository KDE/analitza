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
    m_root = new sNode;

    m_root->cube.setCenter(largo_mundo/2,largo_mundo/2,largo_mundo/2);
    
    m_root->cube.setHalfEdge(largo_mundo/2);
    for(unsigned int i=0;i<8;i++){
        m_root->nodes[i]=NULL;
    }
}

Octree::Octree(Cube cubo){
    m_root = new sNode;
    m_root->cube = cubo;
    for(unsigned int i=0;i<8;i++){
        m_root->nodes[i]=NULL;
    }
}
Octree::~Octree(){
    recursiveDelete(m_root);
}

void Octree::initNodes(sNode* padre){
    for(unsigned int i=0;i<8;i++){
        padre->nodes[i] = new sNode;
        padre->nodes[i]->cube.setHalfEdge(padre->cube.halfEdge()/2);
        for(unsigned int j=0;j<8;j++){
            padre->nodes[i]->nodes[j]=NULL;
        }
    }
    
    double x = padre->cube.center().x();
    double y = padre->cube.center().y();
    double z = padre->cube.center().z();
    double hhedge = padre->cube.halfEdge()/2;
    
    
    padre->nodes[0]->cube.setCenter(x-hhedge, y-hhedge, z-hhedge);
    padre->nodes[1]->cube.setCenter(x-hhedge, y-hhedge, z+hhedge);
    padre->nodes[2]->cube.setCenter(x-hhedge, y+hhedge, z-hhedge);
    padre->nodes[3]->cube.setCenter(x-hhedge, y+hhedge, z+hhedge);
    padre->nodes[4]->cube.setCenter(x+hhedge, y-hhedge, z-hhedge);
    padre->nodes[5]->cube.setCenter(x+hhedge, y-hhedge, z+hhedge);
    padre->nodes[6]->cube.setCenter(x+hhedge, y+hhedge, z-hhedge);
    padre->nodes[7]->cube.setCenter(x+hhedge, y+hhedge, z+hhedge);

}

void Octree::recursiveDelete(sNode* nodo){
    if(nodo == NULL){
        return;
    }
    for(unsigned int i=0;i<8;i++){
        recursiveDelete(nodo->nodes[i]);
    }
    delete nodo;
}


void Octree::recursiveCreate(sNode* nodo, unsigned int nivel_actual, unsigned int nivel_max){
    if(nivel_actual > nivel_max){
        return;
    }
    initNodes(nodo);
    for(unsigned int i=0;i<8;i++){
        recursiveCreate(nodo->nodes[i],nivel_actual+1,nivel_max);
    }
}

sNode* Octree::getRoot(){
    return m_root;
}

void Octree::createLevel(unsigned int nivel){
    recursiveCreate(m_root,0,nivel);
}

void Octree::downLevel(sNode* nodo){
    initNodes(nodo);
}

void Octree::deleteChildren(sNode* padre){
    for(unsigned int i=0;i<8;i++){
        recursiveDelete(padre->nodes[i]);
        padre->nodes[i] = NULL;
    }
}

