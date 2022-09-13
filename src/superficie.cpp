#include "superficie.h"

/// -------------------------------------------------------------------------------------
void superficie::cria_faces( void )
{
    faces = new int * [ (largura-1) * (profundidade-1) * 2 ];

    for( int i = 0 ; i < ( (largura-1) * (profundidade-1) * 2 ) ; i++ )
        faces[ i ] = new int[ 3 ];

    qtde_faces = ( (largura-1) * (profundidade-1) * 2 );
}

/// -------------------------------------------------------------------------------------
void superficie::cria_vertices( void )
{
    vertices = new int *[ largura*profundidade ];

    for( int i = 0 ; i < (largura*profundidade) ; i++ )
        vertices[ i ] = new int[ 3 ];

    qtde_vertices = ( largura * profundidade );
}
/// -------------------------------------------------------------------------------------

void superficie::inclui_faces( int indice , int v1 , int v2 , int v3 )
{
    faces[ indice ][ 0 ] = v1;
    faces[ indice ][ 1 ] = v2;
    faces[ indice ][ 2 ] = v3;
}

/// -------------------------------------------------------------------------------------
void  superficie::inclui_vertices( int indice , int x , int y , int z )
{
    vertices[ indice ][ 0 ] = x;
    vertices[ indice ][ 1 ] = y;
    vertices[ indice ][ 2 ] = z;
}

/// -------------------------------------------------------------------------------------
void superficie::destroi_vertices( void )
{
    delete vertices;
    qtde_vertices = 0;
}

/// -------------------------------------------------------------------------------------
void superficie::destroi_faces( void )
{
    delete faces;
    qtde_faces = 0;
}

/// -------------------------------------------------------------------------------------
int  superficie::valor( int x , int y )
{
    return vertices[ x ][ y ];
}

/// -------------------------------------------------------------------------------------
void superficie::seta_tamanho( int larg , int prof )
{
    largura       = larg;
    profundidade  = prof;
}

/// -------------------------------------------------------------------------------------
int superficie::largura_superficie( void )
{
    return largura;
}

/// -------------------------------------------------------------------------------------
int superficie::profundidade_superficie( void )
{
    return profundidade;
}

/// -------------------------------------------------------------------------------------
int superficie::quantidade_faces( void )
{
    return qtde_faces;
}

/// -------------------------------------------------------------------------------------
int superficie::quantidade_vertices( void )
{
    return qtde_vertices;
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_faces_v1( int indice )
{
    return faces[ indice ][ 0 ];
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_faces_v2( int indice )
{
    return faces[ indice ][ 1 ];
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_faces_v3( int indice )
{
    return faces[ indice ][ 2 ];
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_vertices_x( int indice )
{
    return vertices[ indice ][ 0 ];
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_vertices_y( int indice )
{
    return vertices[ indice ][ 1 ];
}

/// -------------------------------------------------------------------------------------
int superficie::retorna_vertices_z( int indice )
{
    return vertices[ indice ][ 2 ];
}
