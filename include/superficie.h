#ifndef SUPERFICIE_H
#define SUPERFICIE_H


class superficie
{
    private:
        int    largura;
        int    profundidade;                 // altura na imagem, profundidade no terreno
        int    **vertices;                   // matriz com todos os vertices [0]->coord. x, [1]-> coord. z, valor na matriz -> coord. y
        //int    *faces[ 3 ];                // faces triangulares. não utilizei neste projeto
    public:
        void   cria_vertices( void );                           // cria a matriz vertices
        void   destroi_vertices( void );                        // libera a memoria utiizada pela matriz vertices
        void   inclui_coordenada( int x , int y , int valor );  // inclui valor na matriz
        int    valor( int x , int y );                          // retorna o valor de uma determinada posicao da matriz
        void   seta_tamanho( int l , int a );                   // seta o tamanho da superficie
        int    largura_superficie( void );                      // retorna a largura da superficie (eixo x)
        int    profundidade_superficie( void );                 // retorna a profundidade da superficie (eixo y)
};

#endif // SUPERFICIE_H
