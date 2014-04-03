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

#ifndef FUNCTIONGRAPH2_H__Q
#define FUNCTIONGRAPH2_H__Q

#include <QVector3D>

/*
 Node index

 layer z+
y+
 -----
 |3|7|
 -----
 |1|5|
 ----- x+

 layer z-
y+
 -----
 |2|6|
 -----
 |0|4|
 ----- x+
*/


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

struct sNode{
    Cube cube;
    sNode* nodes[8];
};

//TODO we can replace this by kdtree (ANN library)
class Octree
{
private:
    sNode* m_root;

    //create children with correct vertex values
    void initNodes(sNode* parent);

    void recursiveDelete(sNode* node);
    void recursiveCreate(sNode* node, unsigned int current_level, unsigned int max_level);
public:
    Octree(double world_size);
    Octree(Cube cubo);
    ~Octree();

    sNode* getRoot();
    void createLevel(unsigned int level);
    void downLevel(sNode* node);
    void deleteChildren(sNode* parent);

};

#endif

