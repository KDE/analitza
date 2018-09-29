// Aqsis
// Copyright (C) 2006, Paul C. Gregory
//
// Contact: pgregory@aqsis.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

/** \file
    \brief MarchingCubes Algorithm
    \author Thomas Lewiner <thomas.lewiner@polytechnique.org>
    \author Math Dept, PUC-Rio
    \version 0.2
    \date    12/08/2002
*/

// Minor modifications for KDE-Edu/Analitza Library: Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>

#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <QVector>

struct SpaceLimits {
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
};






// Compute normals
#define COMPUTE_NORMALS 1

#include <QDebug>
// types
//-----------------------------------------------------------------------------
// Vertex structure
/** \struct Vertex "MarchingCubes.h" MarchingCubes
 * Position and normal of a vertex
 * \brief vertex structure
 * \param x X coordinate
 * \param y Y coordinate
 * \param z Z coordinate
 * \param nx X component of the normal
 * \param ny Y component of the normal
 * \param nz Z component of the normal
 */
typedef struct
{
  double  x,  y,  z ;  /**< Vertex coordinates */
#ifdef COMPUTE_NORMALS
  double nx, ny, nz ;  /**< Vertex normal */
#endif
} Vertex ;

//-----------------------------------------------------------------------------
// Triangle structure
/** \struct Triangle "MarchingCubes.h" MarchingCubes
 * Indices of the oriented triange vertices
 * \brief triangle structure
 * \param v1 First vertex index
 * \param v2 Second vertex index
 * \param v3 Third vertex index
 */
typedef struct
{
  int v1,v2,v3 ;  /**< Triangle vertices */
} Triangle ;
//_____________________________________________________________________________



//_____________________________________________________________________________
/** Marching Cubes algorithm wrapper */
class MarchingCubes
//-----------------------------------------------------------------------------
{
// Constructors
public :
  /**
   * Main and default constructor
   * \brief constructor
   * \param size_x width  of the grid
   * \param size_y depth  of the grid
   * \param size_z height of the grid
   */
  MarchingCubes () ;
  /** Destructor */
  virtual ~MarchingCubes() ;

    virtual double evalScalarField(double x, double y, double z) = 0;
    void setupSpace(const SpaceLimits &spaceLimits);
    void buildGeometry() {run();}

//-----------------------------------------------------------------------------
// Accessors

  /** accesses the number of vertices of the generated mesh */
  int nverts() const { return i_nverts ; }
  /** accesses the number of triangles of the generated mesh */
  int ntrigs() const { return i_ntrigs ; }
  /** accesses a specific vertex of the generated mesh */
  Vertex   * vert( const int i ) const { if( i < 0  || i >= i_nverts ) return ( Vertex *)nullptr ; return i_vertices  + i ; }
  /** accesses a specific triangle of the generated mesh */
  Triangle * trig( const int i ) const { if( i < 0  || i >= i_ntrigs ) return (Triangle*)nullptr ; return i_triangles + i ; }

  /** accesses the vertex buffer of the generated mesh */
  Vertex   *mc_vertices () { return i_vertices  ; }
  /** accesses the triangle buffer of the generated mesh */
  Triangle *triangles() { return i_triangles ; }

  /**  accesses the width  of the grid */
  int size_x() const { return i_size_x ; }
  /**  accesses the depth  of the grid */
  int size_y() const { return i_size_y ; }
  /**  accesses the height of the grid */
  int size_z() const { return i_size_z ; }

  /**
   * changes the size of the grid
   * \param size_x width  of the grid
   * \param size_y depth  of the grid
   * \param size_z height of the grid
   * se encesita llamar a setup space y buildgeometry para que se tome efecto ... esto solo cambia los attr de la clase
   */
  void set_resolution( const int size_x, const int size_y, const int size_z ) { i_size_x = size_x ;  i_size_y = size_y ;  i_size_z = size_z ; }
  
  

  // Data access
  /**
   * accesses a specific cube of the grid
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  double get_data  ( const int i, const int j, const int k ) const { return i_data[ i + j*i_size_x + k*i_size_x*i_size_y] ; }

//eval de analitza es muy costoso, asi que este aproach esta deprecated... es mejor el triple bucle para llenar la data
//   const double get_data  ( const int i, const int j, const int k )  
//   {
//       double x = xmin+hx*i;
//       double y = ymin+hy*j;
//       double z = zmin+hz*k;
// 
// //       qDebug() << x << y << z;
//       
//       return evalScalarField(x,y,z);
//   }


  /**
   * sets a specific cube of the grid
   * \param val new value for the cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  void  set_data  ( const double val, const int i, const int j, const int k ) { i_data[ i + j*i_size_x + k*i_size_x*i_size_y] = val ; }

private:
  /**
   * selects whether the algorithm will use the enhanced topologically controlled lookup table or the original MarchingCubes
   * \param originalMC true for the original Marching Cubes
   * DEPRECATED el metodo original no tiene consistencia topologica
   */
  void set_method    ( const bool originalMC = false ) { i_originalMC = originalMC ; }

  // Data initialization
  /** inits temporary structures (must set sizes before call) : the grid and the vertex index per cube */
  void init_temps () ;
  /** inits all structures (must set sizes before call) : the temporary structures and the mesh buffers */
  void init_all   () ;
  /** clears temporary structures : the grid and the main */
  void clean_temps() ;
  /** clears all structures : the temporary structures and the mesh buffers */
  void clean_all  () ;


//-----------------------------------------------------------------------------
// Exportation
public :
  /**
   * GTS exportation of the generated mesh
   * \param fn  name of the GTS file to create
   * \param bin if true, the GTS will be written in binary mode
   */
  void write( const char *fn, bool bin = false ) ;


//-----------------------------------------------------------------------------
// Algorithm
private :
  /** Main algorithm : must be called after init_all */
  void run() ;

private :
  /** tesselates one cube */
  void process_cube ()             ;
  /** tests if the components of the tesselation of the cube should be connected by the interior of an ambiguous face */
  bool test_face    ( int face ) ;
  /** tests if the components of the tesselation of the cube should be connected through the interior of the cube */
  bool test_interior( int s )    ;


//-----------------------------------------------------------------------------
// Operations
private :
  /** computes almost all the vertices of the mesh by interpolation along the cubes edges */
  void compute_intersection_points() ;

  /**
   * routine to add a triangle to the mesh
   * \param trig the code for the triangle as a sequence of edges index
   * \param n    the number of triangles to produce
   * \param v12  the index of the interior vertex to use, if necessary
   */
  void add_triangle ( const int* trig, char n, int v12 = -1 ) ;

  /** tests and eventually doubles the vertex buffer capacity for a new vertex insertion */
  void test_vertex_addition() ;
  /** adds a vertex on the current horizontal edge */
  int add_x_vertex() ;
  /** adds a vertex on the current longitudinal edge */
  int add_y_vertex() ;
  /** adds a vertex on the current vertical edge */
  int add_z_vertex() ;
  /** adds a vertex inside the current cube */
  int add_c_vertex() ;

  /**
   * interpolates the horizontal gradient of the implicit function at the lower vertex of the specified cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  double get_x_grad( const int i, const int j, const int k )  ;
  /**
   * interpolates the longitudinal gradient of the implicit function at the lower vertex of the specified cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  double get_y_grad( const int i, const int j, const int k )  ;
  /**
   * interpolates the vertical gradient of the implicit function at the lower vertex of the specified cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  double get_z_grad( const int i, const int j, const int k )  ;

  /**
   * accesses the pre-computed vertex index on the lower horizontal edge of a specific cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  int   get_x_vert( const int i, const int j, const int k ) const { return i_x_verts[ i + j*i_size_x + k*i_size_x*i_size_y] ; }
  /**
   * accesses the pre-computed vertex index on the lower longitudinal edge of a specific cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  int   get_y_vert( const int i, const int j, const int k ) const { return i_y_verts[ i + j*i_size_x + k*i_size_x*i_size_y] ; }
  /**
   * accesses the pre-computed vertex index on the lower vertical edge of a specific cube
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  int   get_z_vert( const int i, const int j, const int k ) const { return i_z_verts[ i + j*i_size_x + k*i_size_x*i_size_y] ; }

  /**
   * sets the pre-computed vertex index on the lower horizontal edge of a specific cube
   * \param val the index of the new vertex
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  void  set_x_vert( const int val, const int i, const int j, const int k ) { i_x_verts[ i + j*i_size_x + k*i_size_x*i_size_y] = val ; }
  /**
   * sets the pre-computed vertex index on the lower longitudinal edge of a specific cube
   * \param val the index of the new vertex
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  void  set_y_vert( const int val, const int i, const int j, const int k ) { i_y_verts[ i + j*i_size_x + k*i_size_x*i_size_y] = val ; }
  /**
   * sets the pre-computed vertex index on the lower vertical edge of a specific cube
   * \param val the index of the new vertex
   * \param i abscisse of the cube
   * \param j ordinate of the cube
   * \param k height of the cube
   */
  void  set_z_vert( const int val, const int i, const int j, const int k ) { i_z_verts[ i + j*i_size_x + k*i_size_x*i_size_y] = val ; }

  /** prints cube for debug */
  void print_cube() ;

//-----------------------------------------------------------------------------
// Elements
private :
  int       i_originalMC ;   /**< selects whether the algorithm will use the enhanced topologically controlled lookup table or the original MarchingCubes */
  int       i_N[15]      ;   /**< counts the occurrence of each case for debug */

  int       i_size_x     ;  /**< width  of the grid */
  int       i_size_y     ;  /**< depth  of the grid */
  int       i_size_z     ;  /**< height of the grid */
  double    *i_data       ;  /**< implicit function values sampled on the grid */

  //BEGIN percy addons
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
  double hx;
  double hy;
  double hz;
  //END

  int      *i_x_verts    ;  /**< pre-computed vertex indices on the lower horizontal   edge of each cube */
  int      *i_y_verts    ;  /**< pre-computed vertex indices on the lower longitudinal edge of each cube */
  int      *i_z_verts    ;  /**< pre-computed vertex indices on the lower vertical     edge of each cube */

  int       i_nverts     ;  /**< number of allocated vertices  in the vertex   buffer */
  int       i_ntrigs     ;  /**< number of allocated triangles in the triangle buffer */
  int       i_Nverts     ;  /**< size of the vertex   buffer */
  int       i_Ntrigs     ;  /**< size of the triangle buffer */
  Vertex     *i_vertices   ;  /**< vertex   buffer */
  Triangle   *i_triangles  ;  /**< triangle buffer */

  int       i_i          ;  /**< abscisse of the active cube */
  int       i_j          ;  /**< height of the active cube */
  int       i_k          ;  /**< ordinate of the active cube */

  double     i_cube[8]    ;  /**< values of the implicit function on the active cube */
  unsigned char     i_lut_entry  ;  /**< cube sign representation in [0..255] */
  unsigned char     i_case       ;  /**< case of the active cube in [0..15] */
  unsigned char     i_config     ;  /**< configuration of the active cube */
  unsigned char     i_subconfig  ;  /**< subconfiguration of the active cube */
};
//_____________________________________________________________________________




#endif
