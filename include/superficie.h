#ifndef SUPERFICIE_H
#define SUPERFICIE_H


class superficie
{
    private:
        int    largura;
        int    profundidade;                 // altura na imagem, profundidade no terreno
        int    qtde_vertices;                // quantidade de vertices da superfice
        int    qtde_faces;                   // quantidade de faces triangulares da superficie
        int    **vertices;                   // matriz com todos os vertices [0]->coord. x, [1]-> coord. z, valor na matriz -> coord. y
        int    **faces;                      // faces triangulares - indices dos vertices que a compoem
    public:
        void   cria_vertices( void );                                     // cria a matriz vertices
        void   cria_faces( void );                                        // cria o vetor de faces
        void   inclui_faces( int indice , int v1 , int v2 , int v3 );     // inclui vertices na face
        void   destroi_vertices( void );                                  // libera a memoria utiizada pela matriz vertices
        void   destroi_faces( void );                                     // libera a memoria utiizada pela matriz faces
        void   inclui_vertices( int indice , int x , int y , int valor ); // inclui valor na matriz de vertices
        int    valor( int x , int y );                                    // retorna o valor de uma determinada posicao da matriz
        void   seta_tamanho( int l , int a );                             // seta o tamanho da superficie
        int    largura_superficie( void );                                // retorna a largura da superficie (eixo x)
        int    profundidade_superficie( void );                           // retorna a profundidade da superficie (eixo y)
        int    quantidade_faces( void );                                  // retorna a quantidade de faces da superficie
        int    quantidade_vertices( void );                               // retorna a quantidade de vertices da superficie
        int    retorna_faces_v1( int indice );                            // retorna v1 da face *MELHORAR*
        int    retorna_faces_v2( int indice );                            // retorna v2 da face *MELHORAR*
        int    retorna_faces_v3( int indice );                            // retorna v3 da face *MELHORAR*
        int    retorna_vertices_x( int indice );                          // retorna x da vertice *MELHORAR*
        int    retorna_vertices_y( int indice );                          // retorna y da vertice *MELHORAR*
        int    retorna_vertices_z( int indice );                          // retorna z da vertice *MELHORAR*
};

#endif // SUPERFICIE_H
