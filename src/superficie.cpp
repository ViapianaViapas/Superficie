#include "superficie.h"
/// -------------------------------------------------------------------------------------
void superficie::destroi_vertices( void )
{
    delete vertices;
}
/// -------------------------------------------------------------------------------------
void superficie::cria_vertices( void )
{
    vertices = new int *[ largura ];

    for( int i = 0 ; i < largura ; i++ )
        vertices[ i ] = new int[ profundidade ];
}
/// -------------------------------------------------------------------------------------
void  superficie::inclui_coordenada( int x , int y , int valor)
{
    vertices[ x ][ y ] = valor;
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
