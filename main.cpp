///*******************************************************
///
/// Superficie
///
/// Mapeamento de terreno a partir de uma imagem
///
/// Inicio             : 06/09/2022
/// Ultima modificacao : 12/09/2022 23:09
///
///*******************************************************

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <string.h>

#include <gl/glut.h>

using namespace std;

#define JANELAS         2

#define LAR_MAIN        1120
#define ALT_MAIN        630

#define LAR_HELP        600
#define ALT_HELP        225

#define SENS_ROT        1.0
#define SENS_OBS        100.0
#define SENS_TRANSL     100.0

///===============================================================================================================
/// PROTOTIPAGEM

void mostra_texto_bitmap( GLfloat x , GLfloat y , string texto );       // mostra texto na janela
void desenha_help( void );                                              // mostra a telazinha de help
void gerencia_mouse( GLint button, GLint state, GLint x, GLint y );     // tratamento dos cliques do mouse
void gerencia_movimento( GLint x , GLint y );                           // tratamento do movimento do mouse
void altera_tamanho_janela( GLsizei largura , GLsizei altura );         // reorganiza a janela principal quando modificada seu tamanho
void altera_tamanho_janela_help( GLsizei largura , GLsizei altura );    // reorganiza a janela de help quando modificada seu tamanho
void teclado( GLubyte key , GLint x , GLint y );                        // tratamento do teclado
void teclas_especiais( GLint key , GLint x , GLint y );                 // tratamento das teclas especiais
void atualiza_posicao_camera( void );                                   // atualiza a posicao z da camera em relacao ao tamanho do terreno
void especifica_parametros_visualizacao_help( void );                   // especifica a projecao ortogonal do help
void define_iluminacao( void );                                         // define os parametros de iluminacao
void especifica_parametros_visualizacao( void );                        // configura a perspectiva e posiciona a camera
void desenha_face( GLint indice );                                      // desenha uma determinada face
void desenha( void );                                                   // funcao responsavel pelo desenho da cena
void inicializa( void );                                                // inicializa todas as variaveis e estados
int  LoadBMP( void );                                                   // le um arquivo bmp e aoca um vetor com todos os valores dos pixels
void calcula_normal(  GLfloat v1[3] , GLfloat v2[3] , GLfloat v3[3] );  // calcula o vetor normal do triangulo
void unitario( void );                                                  // Funcao que torna o vetor normal unitario
int  cria_terreno( void );                                              // cria a matriz terreno com os valores de cada pixel
///===============================================================================================================


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

struct tipo_camera {                // estrutura com os dados da camera
    GLfloat posx;                   // posicao x da camera
    GLfloat posy;                   // posicao y da camera
    GLfloat posz;                   // posicao z da camera
    GLfloat alvox;                  // alvo x da visualizacao
    GLfloat alvoy;                  // alvo y da visualizacao
    GLfloat alvoz;                  // alvo z da visualizacao
    GLfloat ang;                    // abertura da 'lente' - efeito de zoom
    GLfloat inicio;                 // inicio da area de visualizacao em profundidade
    GLfloat fim;                    // fim da area de visualizacao em profundidade
};

struct tipo_transformacao{          // estrutura com os dados das transformacoes
    GLfloat dx, dy, dz;             // paramatros de translacao
    GLfloat sx, sy, sz;             // parametros de escala
    GLfloat angx , angy , angz;     // parametros de rotacao
};

struct tipo_janela{                 // estrutura com os dados das janelas
    GLfloat largura;                // largura da janela
    GLfloat altura;                 // altura da jaanela
    GLfloat aspecto;                // aspecto da janela (relacao entre largura e altura)
};

struct tipo_luz{                    // estrutura com os dados da iluminacao
    GLfloat difusa[ 4 ];            // cor e intensidade da luz difusa
    GLfloat especular[ 4 ];         // cor e intensidade da luz especular
    GLfloat posicao[ 4 ];           // posicao da fonte de luz
};

tipo_camera        camera;          // define a perspectiva da camera
tipo_transformacao transf;          // definicao dos valores de transformacao
tipo_janela        janela;          // definicao dos janela principal
tipo_luz           luz;             // definicao de uma fonte de luz

GLfloat especularidade[ 4 ];        // especularidade e brilho do material
GLfloat ambiente[ 4 ];              // luz ambiente (quando a iluminacao estiver desligada)
GLint   espec_material;             // brilho
GLubyte tonalizacao;                // tipo de tonalizacao dos triangulos (flat ou smooth)
GLint   modo;                       // modo de visualizacao dos objetos (pontos, wireframe ou solido)

bool color , mostra_eixos , ligada; // variaveis que ligam/desligam coloracao do terreno, visualziacao dos eixos x,y,z e iluminacao ligada ou desligada

GLfloat rotX , rotY  , rotX_ini , rotY_ini; // controle do mouse
GLint   x_ini , y_ini , bot;                // controle do mouse

GLint jan[ JANELAS ], passo_camera , passo_objeto , primitiva; // controle das janelas, passo de atualizacao dos parametros das transformacoes e qual objeto mostrar

/// =======================================================================

FILE       *fp_arquivo;                     // ponteiro para o arquivo
GLubyte    *nome_arquivo;                   // nome do arquivo da imagem
GLubyte    *image;                          // vetor que recebera os dados de cores da imagem
GLint      imageSize , profundidade_cores;  // tamanho da imagem e a profuncidade de cores (bits)
GLfloat    normal[ 3 ];                     // vetor normal da obheto triangulo
superficie terreno;                         // criacao do objeto terreno

GLfloat coloracao[ 12 ][ 3 ] = {            // cria uma escala de cores a ser usada no eixo z
     0.14 , 0.40 , 0.00  , // verde
     0.21 , 0.65 , 0.00  , // verde
     0.30 , 0.87 , 0.01  , // verde
     0.30 , 0.92 , 0.00  , // verde
     1.00 , 1.00 , 0.00  , // amarelo
     0.90 , 0.89 , 0.00  , // amarelo
     0.95 , 0.77 , 0.02  , // laranja
     1.00 , 0.38 , 0.00  , // laranja
     0.90 , 0.58 , 0.00  , // laranja
     1.00 , 0.00 , 0.00  , // vermelho
     0.66 , 0.23 , 0.12  , // marrom
     0.34 , 0.00 , 0.01  };// marrom

/// =======================================================================
// Funcao que torna o vetor normal unitario
void unitario( void )
{
    GLfloat length;

    // Calcula o comprimento do vetor
    length = (GLfloat) sqrt( ( normal[ 0 ] * normal[ 0 ] ) +
                             ( normal[ 1 ] * normal[ 1 ] ) +
                             ( normal[ 2 ] * normal[ 2 ] ) );

    if ( length == 0.0 )
       length =  1.0 ;

    // tornar o vetor unitario
    normal[ 0 ] = normal[ 0 ] / length;
    normal[ 1 ] = normal[ 1 ] / length;
    normal[ 2 ] = normal[ 2 ] / length;
}

/// =======================================================================
// =====================================================
// Calcula o betor norma da face
// Assume que v1, v2 e v3 estão especificados em
// sentido anti-horario. Se voce passar os parametros em
// outra ordem, o vetor vai apontar para o lado errado.
void calcula_normal(  GLfloat v1[ 3 ] , GLfloat v2[ 3 ] , GLfloat v3[ 3 ] )
{
    GLfloat aux1[ 3 ] , aux2[ 3 ];

    // Calcula dois vetores a partir dos tres pontos.
    aux1[ 0 ] = v1[ 0 ] - v2[ 0 ];
    aux1[ 1 ] = v1[ 1 ] - v2[ 1 ];
    aux1[ 2 ] = v1[ 2 ] - v2[ 2 ];

    aux2[ 0 ] = v2[ 0 ] - v3[ 0 ];
    aux2[ 1 ] = v2[ 1 ] - v3[ 1 ];
    aux2[ 2 ] = v2[ 2 ] - v3[ 2 ];

    normal[ 0 ] = aux1[ 1 ] * aux2[ 2 ] - aux1[ 2 ] * aux2[ 1 ];
    normal[ 1 ] = aux1[ 2 ] * aux2[ 0 ] - aux1[ 0 ] * aux2[ 2 ];
    normal[ 2 ] = aux1[ 0 ] * aux2[ 1 ] - aux1[ 1 ] * aux2[ 0 ];

    // Normaliza o vector, reduzindo comprimento para um
    unitario();
}

/// =======================================================================
// cria a matriz terreno com os valores de cada pixel
int cria_terreno()
{
    GLsizei step = 0 , x = 0 , z = 0 , indice = 0 , pulo , deltax , deltaz;

    if( LoadBMP() == -1)  // carrega a imagem e verifica se deu certo o carregamento
        return -1;

    terreno.cria_vertices(); // cria a a matriz de vertices dentro do objeto

    // para centralizar o terreno no SRU
    deltax = terreno.profundidade_superficie() / 2;
    deltaz = terreno.largura_superficie() / 2;

    // adiciona todos os pontos da imagem no vetor de vertices do objeto. o y do vertice eh a cor do ponto na imagem
    for ( GLint i = 0 ; i < terreno.quantidade_vertices() ; i++ )
    {
        terreno.inclui_vertices( i , x-deltax , (GLint)image[ step ] , z-deltaz );

        step += (profundidade_cores/8);
        z++;
        if( ( z % terreno.largura_superficie() ) == 0)
        {
            x++;
            z = 0;
        }
    }

    terreno.cria_faces(); // cria a matriz de faces dentro do objeto

    indice = 0;
    pulo = terreno.largura_superficie()-1;

    // adiciona as faces triangulares
    // cada 4 pontos são dois triangulos. aqui adiciona o "inferior"
    for( int i = 0 ; i < terreno.quantidade_vertices()-terreno.largura_superficie() ; i++ )
    {
        if( i == pulo )
        {
            pulo += terreno.largura_superficie();
            continue;
        }
        terreno.inclui_faces( indice , i , i+1 , i+terreno.largura_superficie() );

        indice++;
    }

    pulo = terreno.largura_superficie()-1;

    // cada 4 pontos são dois triangulos. aqui adiciona o "superior"
    for( int i = 1 ; i <= terreno.quantidade_vertices()-terreno.largura_superficie() ; i++ )
    {
        if( i == pulo+1 )
        {
            pulo += terreno.largura_superficie();
            continue;
        }
        terreno.inclui_faces( indice , i , i+terreno.largura_superficie() , i+terreno.largura_superficie()-1 );

        indice++;
    }

    atualiza_posicao_camera();
    free( image );

    return 1;
}
/// =======================================================================
// desenha um determinado vertice com sua respectiva cor
void desenha_face( GLint indice  )
{
    // melhorar a codificacao. retornar vetor de inteiros da classe
    // MELHORAR MUITO ESTE TRECHO DO CÓDIGO

    GLfloat v1[ 3 ] , v2[ 3 ] , v3[ 3 ];

    v1[0] = terreno.retorna_vertices_x( terreno.retorna_faces_v1( indice ) );
    v1[1] = terreno.retorna_vertices_y( terreno.retorna_faces_v1( indice ) );
    v1[2] = terreno.retorna_vertices_z( terreno.retorna_faces_v1( indice ) );

    v2[0] = terreno.retorna_vertices_x( terreno.retorna_faces_v2( indice ) );
    v2[1] = terreno.retorna_vertices_y( terreno.retorna_faces_v2( indice ) );
    v2[2] = terreno.retorna_vertices_z( terreno.retorna_faces_v2( indice ) );

    v3[0] = terreno.retorna_vertices_x( terreno.retorna_faces_v3( indice ) );
    v3[1] = terreno.retorna_vertices_y( terreno.retorna_faces_v3( indice ) );
    v3[2] = terreno.retorna_vertices_z( terreno.retorna_faces_v3( indice ) );


    calcula_normal( v1 , v2 , v3 );
    glNormal3fv( normal );


    if ( color )
        glColor3fv( coloracao[ ( 12 * terreno.retorna_vertices_y( terreno.retorna_faces_v1 ( indice ) ) / 256 ) ] );
    glVertex3fv( v1 );

    if ( color )
        glColor3fv( coloracao[ ( 12 * terreno.retorna_vertices_y( terreno.retorna_faces_v2 ( indice ) ) / 256 ) ] );
    glVertex3fv( v2 );

    if ( color )
        glColor3fv( coloracao[ ( 12 * terreno.retorna_vertices_y( terreno.retorna_faces_v3 ( indice ) ) / 256 ) ] );
    glVertex3fv( v3 );

}
/// =======================================================================
// funcao responsavel pelo desenho da cena
void desenha(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glTranslatef( transf.dx , transf.dy , transf.dz );
    glRotatef( transf.angx , 1 , 0 , 0 );
    glRotatef( transf.angy , 0 , 1 , 0 );
    glRotatef( transf.angz , 0 , 0 , 1 );
    glScalef( transf.sx+20 , transf.sy+2 , transf.sz+20 ); // para aumentar a visualizacao na tela

    define_iluminacao();

    // se a iluminaca esta ligada ou nao
    (ligada)?  glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

    if( mostra_eixos )
    {
        glLineWidth( 5 );
        glBegin( GL_LINES );

            glColor4f( 1.0 , 0.0 , 0.0 , 1.0 );
            glVertex3f( -1000 , 0 , 0 );
            glVertex3f(  1000 , 0 , 0 );

            glColor4f( 0.0 , 0.0 , 1.0 , 1.0 );
            glVertex3f( 0 , -1000 , 0 );
            glVertex3f( 0 ,  1000 , 0 );

            glColor4f( 0.0 , 1.0 , 0.0 , 1.0 );
            glVertex3f( 0 , 0 , -1000 );
            glVertex3f( 0 , 0 ,  1000 );

        glEnd();
    }

    glLineWidth( 1 );

    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );

    // modo de visualizacao: pontos, arestas ou solido
    if ( modo == 0 )
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT );
    else
        if ( modo == 1 )
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );

    // desenha as faces
    for( int i = 0 ; i < terreno.quantidade_faces() ; i++)
    {
        glBegin( GL_TRIANGLES );
            desenha_face( i );
        glEnd();
    }

    glPopMatrix();

    glutSwapBuffers();
}
/// =======================================================================
// inicializa todas as variaveis e estados
void inicializa( void )
{
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    transf.dx     = 0.0;
    transf.dy     = 0.0;
    transf.dz     = 0.0;
    transf.sx     = 1.0;
    transf.sy     = 1.0;
    transf.sz     = 1.0;
    transf.angx   = 0.0;
    transf.angy   = 0.0;
    transf.angz   = 0.0;

    camera.posx   = 0;
    camera.posy   = 1000;
    camera.posz   = terreno.largura_superficie()*10;
    camera.alvox  = 0;
    camera.alvoy  = 128;
    camera.alvoz  = 0;
    camera.inicio = 0.1;
    camera.fim    = 100000.0;
    camera.ang    = 45;

    tonalizacao   = 'S';
    modo          = 1;
    color         = true;
    mostra_eixos  = false;
    ligada        = true;

    passo_camera  = 200;
    passo_objeto  = 18;

    ambiente[ 0 ] = 0.4;
    ambiente[ 1 ] = 0.4;
    ambiente[ 2 ] = 0.4;
    ambiente[ 3 ]  = 1.0;

    // cor
    luz.difusa[ 0 ] = 0.7;
    luz.difusa[ 1 ] = 0.7;
    luz.difusa[ 2 ] = 0.7;
    luz.difusa[ 3 ] = 1.0;

    // brilho
    luz.especular[ 0 ] = 0.8;
    luz.especular[ 1 ] = 0.8;
    luz.especular[ 2 ] = 0.8;
    luz.especular[ 3 ] = 1.0;

    // posicao
    luz.posicao[ 0 ] =      0;
    luz.posicao[ 1 ] =    300;
    luz.posicao[ 2 ] =      0;
    luz.posicao[ 3 ] =    1.0;

    // capacidade de brilho do material
    especularidade[ 0 ] = 0.7;
    especularidade[ 1 ] = 0.7;
    especularidade[ 2 ] = 0.7;
    especularidade[ 3 ] = 1.0;

    espec_material = 300;

    // habilita a transparenica
    glEnable( GL_BLEND );

    // define a forma de calculo da transparencia
    //glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // Habilita antialiasing
    glEnable( GL_LINE_SMOOTH );

    // Solicita melhor qualidade
    //glHint( GL_LINE_SMOOTH_HINT , GL_NICEST );

    // sentido de criacao da face - frente da face - default: GL_CCW
    //glFrontFace( GL_CW );

    // Habilita a remocao de faces
    glEnable( GL_CULL_FACE );

    // remove faces traseiras - default: traseiras
    //glCullFace( GL_BACK );

    // habilita o z-buffer
    glEnable( GL_DEPTH_TEST );

    glutSetWindow( jan[ 0 ] );
    glutReshapeWindow( LAR_HELP , ALT_HELP );
    glutPositionWindow( 10 , 10 );

    glutSetWindow( jan[ 1 ] );
    glutReshapeWindow( LAR_MAIN , ALT_MAIN );
    glutPositionWindow( ( glutGet( GLUT_SCREEN_WIDTH  ) - LAR_MAIN ) / 2 ,
                        ( glutGet( GLUT_SCREEN_HEIGHT ) - ALT_MAIN ) / 2 );
}
/// =======================================================================
// le um arquivo bmp e aoca um vetor com todos os valores dos pixels
int  LoadBMP()
{
    #define SAIR        {fclose(fp_arquivo); return -1;}
    #define CTOI(C)     (*(int*)&C)

    GLubyte Header[0x54];
    GLint retorno , temp , largura_imagem , altura_imagem;

    // testes para verificar o arquivo
    if (fread(Header,1,0x36,fp_arquivo)!=0x36) // color table
    {
        cout << "ERRO A imagem especificada possui Color Table?" << endl;
        SAIR;
    }
    if (Header[0]!='B' || Header[1]!='M')
    {
        cout << "ERRO A imagem especificada nao eh do tipo BMP" << endl;
        SAIR;
    }
    if (CTOI(Header[0x1E])!=0) // 0 = sem compressão
    {
        cout << "ERRO A imagem especificada possui compressao" << endl;
        SAIR;
    }

    largura_imagem = CTOI(Header[0x12]);
    altura_imagem  = CTOI(Header[0x16]);

    //terreno.seta_tamanho( altura_imagem , largura_imagem  );    //seta o tamanho do terreno (altura da imagem  = largura da superficie)
    terreno.seta_tamanho( largura_imagem , altura_imagem  );    //seta o tamanho do terreno (altura da imagem  = largura da superficie)
                                                                //                          (largura da imagem = largura da superficie)

    //Verifica a profundidade de cores
    switch( CTOI(Header[0x1C]) )
    {
    case 8 :
        imageSize = largura_imagem * altura_imagem;
        profundidade_cores = 8;
        break;
    case 24 :
        imageSize = largura_imagem * altura_imagem * 3;
        profundidade_cores = 24;
        break;
    case 32 :
        imageSize = largura_imagem * altura_imagem * 4;
        profundidade_cores = 32;
        break;
    default:
        cout << "ERRO A imagem especificada possui profundidade de cores diferente de 8 16 32" << endl;
        SAIR;
    }

    // Efetura a Carga da Imagem
    image = ( GLubyte *) malloc ( imageSize );

    retorno = fread( image , 1 , imageSize , fp_arquivo );

    if ( retorno != (GLint)( imageSize ))
    {
        free (image);
        cout << "ERRO Erro na leitura da quantidade de bytes da imagem especificada" << endl;
        SAIR;
    }

    // Inverte os valores de R e B
    for ( GLint i = 0; i < (GLint)(imageSize); i += (profundidade_cores/8) )
    {
        temp       = image[i];
        image[i]   = image[i+2];
        image[i+2] = temp;
    }

    fclose (fp_arquivo);
    return 1;
}

/// =======================================================================
// Programa Principal
int main( int argc , char *argv[] )
{
    if ( argc != 2 )
    {
        cout << "Faltou nome do arquivo a ser aberto" << endl;
        return 0;
    }
    fp_arquivo = fopen(argv[1],"rb");
    if ( !fp_arquivo )
    {
        cout << endl << "Nao pode abrir o arquivo " << argv[1] << endl;
        return -1;
    }
    else
        cout << endl << "Sucesso na abertura do arquivo " << argv[1] << endl;

    glutInit( &argc , argv );

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // janela do help
    glutInitWindowSize( LAR_HELP , ALT_HELP );
    glutInitWindowPosition( 300 , 300 );
    jan[ 0 ] = glutCreateWindow( "HELP" );

    // janela principal
    glutInitWindowSize( LAR_MAIN , ALT_MAIN );
    glutInitWindowPosition( ( glutGet( GLUT_SCREEN_WIDTH  ) - LAR_MAIN ) / 2 ,
                           ( glutGet( GLUT_SCREEN_HEIGHT )  - ALT_MAIN  ) / 2 );
    jan[ 1 ] = glutCreateWindow( "TERRENO" );

    // callbacks da janela de help
    glutSetWindow( jan[ 0 ] );
    glutDisplayFunc( desenha_help );
    glutReshapeFunc( altera_tamanho_janela_help );

    // callbacks da janela principal
    glutSetWindow( jan[ 1 ] );
    glutDisplayFunc( desenha );
    glutKeyboardFunc( teclado );
    glutSpecialFunc( teclas_especiais );
    glutReshapeFunc( altera_tamanho_janela );

    // Funcao callback para eventos de botoes do mouse
    glutMouseFunc( gerencia_mouse );

    // Funcao callback para eventos de movimento do mouse
    glutMotionFunc( gerencia_movimento );

    // funcao que tem as inicializacoes de variaveis e estados do OpenGL
    inicializa();

    if( cria_terreno() == -1)
        return 0;

    glutMainLoop();

    return 0;
}
/// =======================================================================
void superficie::cria_faces( void )
{
    faces = new int * [ (largura-1) * (profundidade-1) * 2 ];

    for( int i = 0 ; i < ( (largura-1) * (profundidade-1) * 2 ) ; i++ )
        faces[ i ] = new int[ 3 ];

    qtde_faces = ( (largura-1) * (profundidade-1) * 2 );
}

/// =======================================================================
void superficie::cria_vertices( void )
{
    vertices = new int *[ largura*profundidade ];

    for( int i = 0 ; i < (largura*profundidade) ; i++ )
        vertices[ i ] = new int[ 3 ];

    qtde_vertices = ( largura * profundidade );
}
/// =======================================================================

void superficie::inclui_faces( int indice , int v1 , int v2 , int v3 )
{
    faces[ indice ][ 0 ] = v1;
    faces[ indice ][ 1 ] = v2;
    faces[ indice ][ 2 ] = v3;
}

/// =======================================================================
void  superficie::inclui_vertices( int indice , int x , int y , int z )
{
    vertices[ indice ][ 0 ] = x;
    vertices[ indice ][ 1 ] = y;
    vertices[ indice ][ 2 ] = z;
}

/// =======================================================================
void superficie::destroi_vertices( void )
{
    delete vertices;
    qtde_vertices = 0;
}

/// =======================================================================
void superficie::destroi_faces( void )
{
    delete faces;
    qtde_faces = 0;
}

/// =======================================================================
int  superficie::valor( int x , int y )
{
    return vertices[ x ][ y ];
}

/// =======================================================================
void superficie::seta_tamanho( int larg , int prof )
{
    largura       = larg;
    profundidade  = prof;
}

/// =======================================================================
int superficie::largura_superficie( void )
{
    return largura;
}

/// =======================================================================
int superficie::profundidade_superficie( void )
{
    return profundidade;
}

/// =======================================================================
int superficie::quantidade_faces( void )
{
    return qtde_faces;
}

/// =======================================================================
int superficie::quantidade_vertices( void )
{
    return qtde_vertices;
}

/// =======================================================================
int superficie::retorna_faces_v1( int indice )
{
    return faces[ indice ][ 0 ];
}

/// =======================================================================
int superficie::retorna_faces_v2( int indice )
{
    return faces[ indice ][ 1 ];
}

/// =======================================================================
int superficie::retorna_faces_v3( int indice )
{
    return faces[ indice ][ 2 ];
}

/// =======================================================================
int superficie::retorna_vertices_x( int indice )
{
    return vertices[ indice ][ 0 ];
}

/// =======================================================================
int superficie::retorna_vertices_y( int indice )
{
    return vertices[ indice ][ 1 ];
}

/// =======================================================================
int superficie::retorna_vertices_z( int indice )
{
    return vertices[ indice ][ 2 ];
}

/// =======================================================================
// mostra texto na janela
void mostra_texto_bitmap( GLfloat x , GLfloat y , string texto )
{
    glRasterPos2f ( x , y );
    for( int i = 0 ; i < (int)(texto.length()) ; i++ )
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , texto[ i ] );
}
/// =======================================================================
// mostra a telazinha de help
void desenha_help(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glClear( GL_COLOR_BUFFER_BIT );
    glColor4f( 1.0 , 1.0 , 0.0 , 1.0 );

    mostra_texto_bitmap( 0 , 210 , "ESC           : finaliza o programa" );
    mostra_texto_bitmap( 0 , 190 , "M             : alterna entre pontos, wireframe e solido" );
    mostra_texto_bitmap( 0 , 170 , "L             : liga/desliga iluminacao" );
    mostra_texto_bitmap( 0 , 150 , "X x Y y Z z   : rotaciona a cena" );
    mostra_texto_bitmap( 0 , 130 , "C             : liga/desliga coloracao" );
    mostra_texto_bitmap( 0 , 110 , "E             : exibe/oculta eixos" );
    mostra_texto_bitmap( 0 ,  90 , "F             : alterna tonalizacao entre flat e smooth" );
    mostra_texto_bitmap( 0 ,  70 , "W A S D Q \\   : movimenta a camera" );
    mostra_texto_bitmap( 0 ,  50 , "TECLAS NAVEG. : movimenta a fonte de luz" );
    mostra_texto_bitmap( 0 ,  30 , "MOUSE         : rotaciona a camera" );
    mostra_texto_bitmap( 0 ,  10 , "I             : reinicializa" );

    glutSwapBuffers();
}
/// =======================================================================
// Funcao usada para especificar a projecao ortogonal do help
void especifica_parametros_visualizacao_help( void )
{
    // seleciona o tipo de matriz para a projecao
    glMatrixMode( GL_PROJECTION );
    // limpa (zera) as matrizes
    glLoadIdentity();
    glClearColor( 0.1 , 0.1 , 0.1 , 1.0 );
    gluOrtho2D(  0 , LAR_HELP , 0 , ALT_HELP );
}
/// =======================================================================
// reorganiza a janela de help quando modificada seu tamanho
void altera_tamanho_janela_help( GLsizei largura , GLsizei altura )
{
    glutInitWindowPosition( 300 , 300 );
    glutReshapeWindow( LAR_HELP , ALT_HELP );
    especifica_parametros_visualizacao_help();
}
/// =======================================================================
// define os parametros de iluminacao
void define_iluminacao( void )
{
    // habilita o uso da iluminacao
    glEnable( GL_LIGHTING );

    // Define a refletância do material
    glMaterialfv( GL_FRONT_AND_BACK , GL_SPECULAR  , especularidade );

    // Define a concentração do brilho
    glMateriali(  GL_FRONT_AND_BACK , GL_SHININESS , espec_material );

    // ativa o uso da luz ambiente - caso a luz 0 seja desligada
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , ambiente );

    glLightfv( GL_LIGHT0 , GL_AMBIENT  , ambiente );
    glLightfv( GL_LIGHT0 , GL_DIFFUSE  , luz.difusa );
    glLightfv( GL_LIGHT0 , GL_SPECULAR , luz.especular );
    glLightfv( GL_LIGHT0 , GL_POSITION , luz.posicao );

    // habilita a definicao da cor do material a partir da cor corrente
    glEnable( GL_COLOR_MATERIAL );

    glEnable(GL_AUTO_NORMAL);

    glEnable( GL_LIGHT0 );

    // normaliza os vetores normais
    glEnable( GL_NORMALIZE );

    // habilita o modelo de colorizacao de Gouraud
    if ( tonalizacao == 'F' )
        glShadeModel( GL_FLAT );
    else
        glShadeModel( GL_SMOOTH );
}
/// =======================================================================
// configura a perspectiva e posiciona a camera
void especifica_parametros_visualizacao( void )
{
    // seleciona o tipo de matriz para a projecao
    glMatrixMode( GL_PROJECTION );
    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a projecao perspectiva
    gluPerspective( camera.ang , janela.aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posicao da camera (o observador) e do alvo
    glRotatef( rotX/10.0 , 1 , 0 , 0 );
    glRotatef( rotY/10.0 , 0 , 1 , 0 );

    gluLookAt( camera.posx , camera.posy , camera.posz , camera.alvox , camera.alvoy , camera.alvoz , 0 , 1 , 0 );
}
/// =======================================================================
// tratamento dos cliques do mouse
void gerencia_mouse(GLint button, GLint state, GLint x, GLint y)
{
    if( state == GLUT_DOWN )
    {
        // Salva os parametros atuais
        x_ini = x;
        y_ini = y;
        rotX_ini = rotX;
        rotY_ini = rotY;
        bot = button;
    }
    else bot = -1;
}
/// =======================================================================
// tratamento do movimento do mouse
void gerencia_movimento( GLint x , GLint y )
{
    GLint deltax, deltay;

    // Botao esquerdo
    if( bot == GLUT_LEFT_BUTTON )
    {
        // Calcula diferencas
        deltax = x_ini - x;
        deltay = y_ini - y;
        // E modifica angulos
        rotY = rotY_ini - deltax / SENS_ROT;
        rotX = rotX_ini - deltay / SENS_ROT;
    }
    especifica_parametros_visualizacao();
    glutPostRedisplay();
}
/// =======================================================================
// reorganiza a janela principal quando modificada seu tamanho
void altera_tamanho_janela( GLsizei largura , GLsizei altura )
{
    janela.largura = largura;
    janela.altura  = altura;

    // Para previnir uma divisao por zero
    if ( janela.altura == 0 ) janela.altura = 1;

    // Especifica as dimensıes da viewport
    glViewport( 0 , 0 , janela.largura , janela.altura );

    // Calcula o aspecto
    janela.aspecto = janela.largura / janela.altura;

    especifica_parametros_visualizacao();
}

/// =======================================================================
// tratamento das teclas especiais
void teclas_especiais( GLint key , GLint x , GLint y )
{
    if ( key == GLUT_KEY_LEFT )
        luz.posicao[ 0 ] -= 50;

    if ( key == GLUT_KEY_RIGHT )
        luz.posicao[ 0 ] += 50;

    if ( key == GLUT_KEY_PAGE_DOWN )
        luz.posicao[ 1 ] -= 50;

    if ( key == GLUT_KEY_PAGE_UP )
        luz.posicao[ 1 ] += 50;

    if ( key == GLUT_KEY_UP )
        luz.posicao[ 2 ] -= 50;

    if ( key == GLUT_KEY_DOWN )
        luz.posicao[ 2 ] += 50;

    glutSetWindow( jan[ 1 ] );
    glutPostRedisplay();
}

/// =======================================================================
// atualiza a posicao z da camera em relacao ao tamanho do terreno
void atualiza_posicao_camera()
{
    //configurar a posicao inicial da camera levando em consideracao o tamanho do terreno
    camera.posz = terreno.largura_superficie()*25;
    especifica_parametros_visualizacao();
}

/// =======================================================================
// tratamento do teclado
void teclado( GLubyte key , GLint x , GLint y )
{
    GLint modificador = glutGetModifiers();

    if ( modificador & GLUT_ACTIVE_ALT)
    {
        // ALT pressionado
        if( key == '-' || key == '_' )
            if( passo_camera - 1 > 0 )
                passo_camera--;

        if( key == '+' || key == '=' )
            passo_camera++;
    }
    else
    {
        if ( key == 27 )
        {
            terreno.destroi_vertices();
            exit( 0 );
        }

        if ( toupper( key ) == 'F')
            ( tonalizacao == 'F' ) ? tonalizacao = 'S' : tonalizacao = 'F';

        if ( toupper( key ) == 'I' )
        {
            inicializa();
            atualiza_posicao_camera();
        }

        if ( toupper( key ) == 'M' )
            (modo == 2)? modo = 0 : modo++;


         if ( toupper( key ) == 'C' )
         {
            color = !color;
            if ( !color )
                modo = 1;
         }

        //optei por fazer com que o alvo siga a camera. Ou seja, a camera fica olhando reto para a frente quando se movimenta. paralela ao eixo z
        if ( toupper( key ) == 'A' )
        {
            camera.posx  -= passo_camera;
            camera.alvox -= passo_camera;
        }

        if ( toupper( key ) == 'D' )
        {
            camera.posx  += passo_camera;
            camera.alvox += passo_camera;
        }

        if ( toupper( key ) ==  'Q' )
        {
            camera.posy  += passo_camera;
            camera.alvoy += passo_camera;
        }

        if ( key ==  '\\' || key == '|' )
        {
            camera.posy  -= passo_camera;
            camera.alvoy -= passo_camera;
        }

        if ( toupper( key ) == 'W' )
        {
            camera.posz  -= passo_camera;
            camera.alvoz -= passo_camera;
        }

        if ( toupper( key ) == 'S' )
        {
            camera.posz  += passo_camera;
            camera.alvoz += passo_camera;
        }

        if ( toupper( key ) == 'E' )
            mostra_eixos = !mostra_eixos;

        if ( toupper( key ) == 'L' )
            ligada = !ligada;

        if ( key == 'X' )
            transf.angx -= 5;

        if ( key == 'x' )
            transf.angx += 5;

        if ( key == 'Y' )
            transf.angy -= 5;

        if ( key == 'y' )
            transf.angy += 5;

        if ( key == 'Z' )
            transf.angz -= 5;

        if ( key == 'z' )
            transf.angz += 5;
    }
    especifica_parametros_visualizacao();

    glutSetWindow( jan[ 1 ] );
    glutPostRedisplay();
}

