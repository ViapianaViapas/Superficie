//*******************************************************
//
// Superficie
//
// Mapeamento de terreno a partir de uma imagem
//
// Inicio : 06/09/2022
// Ultima modificacao : 12/09/2022 21:40
//
//*******************************************************

#include "funcoes.cpp"

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <string.h>

#include<superficie.h>

#include <gl/glut.h>

using namespace std;

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
void plota_vertice( GLint vertice_linha , GLint vertice_profundidade ); // desenha um determinado vertice com sua respectiva cor
void desenha( void );                                                   // funcao responsavel pelo desenho da cena
void inicializa( void );                                                // inicializa todas as variaveis e estados
int  LoadBMP( void );                                                   // le um arquivo bmp e aoca um vetor com todos os valores dos pixels
void calcula_normal(  GLfloat v1[3] , GLfloat v2[3] , GLfloat v3[3] );  // calcula o vetor normal do triangulo
void unitario( void );                                                  // Funcao que torna o vetor normal unitario
int  cria_terreno( void );                                              // cria a matriz terreno com os valores de cada pixel

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
// atualiza a posicao z da camera em relacao ao tamanho do terreno
void atualiza_posicao_camera()
{
    //configurar a posicao inicial da camera levando em consideracao o tamanho do terreno
    camera.posz = terreno.profundidade_superficie()*8;
    especifica_parametros_visualizacao();
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

    terreno.seta_tamanho( altura_imagem , largura_imagem  );    //seta o tamanho do terreno (altura da imagem  = largura da superficie)
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
    image = (GLubyte *) malloc ( imageSize );

    retorno = fread(image,1,imageSize,fp_arquivo);

    if (retorno != (GLint)(imageSize))
    {
        free (image);
        cout << "ERRO5 Erro na leitura da quantidade de bytes da imagem especificada" << endl;
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
// cria a matriz terreno com os valores de cada pixel
int cria_terreno()
{
    GLsizei step = 0;

    if( LoadBMP() == -1)  // carrega a imagem e verifica se deu certo o carregamento
        return -1;

    terreno.cria_vertices(); // cria a a matriz de vertices dentro do objeto

    for (GLint i = 0; i < terreno.largura_superficie() ; i++)           // percorre a largura da matriz imagem (linhas)
        for (GLint j = 0; j < terreno.profundidade_superficie() ; j++)  // percorre a altura  da matriz imagem (colunas)
        {
            terreno.inclui_coordenada( i , j , (GLint)image[ step ] );  // insere a cor de cada posicao da matriz (posicao x,y da matriz - posicao x,z do plano de coordenadas)
            step += (profundidade_cores/8);                             // pulo dentro do vetor image (vai depende da profundidade de cores da imagem)
        }                                                               // caso a imagem tiver mais que 8 bits (mono) converte em omo pegando o valor de red como a cor
    atualiza_posicao_camera();
    free( image );
    return 1;
}
/// =======================================================================
// desenha um determinado vertice com sua respectiva cor
void plota_vertice( GLint vertice_linha , GLint vertice_profundidade )
{
    GLint superficie_largura      = terreno.largura_superficie() ,
          superficie_profundidade = terreno.profundidade_superficie();

    if ( color )
        glColor3fv( coloracao[ ( 12 * terreno.valor( vertice_linha , vertice_profundidade ) / 256 ) ] );

    glVertex3d( vertice_linha-superficie_largura/2 , terreno.valor( vertice_linha , vertice_profundidade ) ,  vertice_profundidade-superficie_profundidade/2 );

}
/// =======================================================================
// funcao responsavel pelo desenho da cena
void desenha(void)
{
    GLfloat v[ 3 ][ 3 ];

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glTranslatef( transf.dx , transf.dy , transf.dz );
    glRotatef( transf.angx , 1 , 0 , 0 );
    glRotatef( transf.angy , 0 , 1 , 0 );
    glRotatef( transf.angz , 0 , 0 , 1 );
    glScalef( transf.sx+10 , transf.sy , transf.sz+10 );

    define_iluminacao();

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

    cout << "Modo : " << modo << endl;

    for( GLint l = 0 ; l <  terreno.largura_superficie() ; l++ )
    {
        for( GLint p = 0 ; p <  terreno.profundidade_superficie() ; p++ )
        {
            //sentido anti-horario
            // ultima linha e coluna de pontos : para não estourar o limite da matriz (tem que fazer linhas aqi - se der tempo)
            if ( (  modo != 0 ) && ( p == terreno.profundidade_superficie()-1 || l == terreno.largura_superficie()-1 ) )
                continue;

            if ( modo == 0 )
            {
                glBegin( GL_POINTS );
                    plota_vertice( l , p );
                glEnd();
            }
            else
            {
                if ( modo == 1 )
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
                /*
                v [ 0 ][ 0 ] = l   ; v[ 0 ][ 1 ] = terreno.valor( l   , p   ) ; v[ 0 ][ 2 ] = p;
                v [ 0 ][ 0 ] = l   ; v[ 0 ][ 1 ] = terreno.valor( l   , p+1 ) ; v[ 0 ][ 2 ] = p+1;
                v [ 0 ][ 0 ] = l+1 ; v[ 0 ][ 1 ] = terreno.valor( l+1 , p   ) ; v[ 0 ][ 2 ] = p;

                calcula_normal( v[ 0 ] , v[ 1 ] , v[ 2 ] );
                glNormal3fv( normal );
                */
                glBegin( GL_TRIANGLES );
                    plota_vertice( l   , p   );
                    plota_vertice( l   , p+1 );
                    plota_vertice( l+1 , p   );
                glEnd();
                /*
                v [ 0 ][ 0 ] = l   ; v[ 0 ][ 1 ] = terreno.valor( l   , p+1 ) ; v[ 0 ][ 2 ] = p+1;
                v [ 0 ][ 0 ] = l+1 ; v[ 0 ][ 1 ] = terreno.valor( l+1 , p+1 ) ; v[ 0 ][ 2 ] = p+1;
                v [ 0 ][ 0 ] = l+1 ; v[ 0 ][ 1 ] = terreno.valor( l+1 , p   ) ; v[ 0 ][ 2 ] = p;

                calcula_normal( v[ 0 ] , v[ 1 ] , v[ 2 ] );
                glNormal3fv( normal );
                */
                glBegin( GL_TRIANGLES );
                    plota_vertice( l   , p+1 );
                    plota_vertice( l+1 , p+1 );
                    plota_vertice( l+1 , p   );
                glEnd();
            }
        }
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
    camera.posy   = 128;
    camera.posz   = terreno.profundidade_superficie()*8;
    camera.alvox  = 0;
    camera.alvoy  = 128;
    camera.alvoz  = 0;
    camera.inicio = 0.1;
    camera.fim    = 100000.0;
    camera.ang    = 45;

    tonalizacao   = 'M';
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
    luz.posicao[ 0 ] =   1000;
    luz.posicao[ 1 ] =   1000;
    luz.posicao[ 2 ] =   1000;
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
            transf.angx += 5;

        if ( key == 'x' )
            transf.angx -= 5;

        if ( key == 'Y' )
            transf.angy += 5;

        if ( key == 'y' )
            transf.angy -= 5;

        if ( key == 'Z' )
            transf.angz += 5;

        if ( key == 'z' )
            transf.angz -= 5;
    }
    especifica_parametros_visualizacao();

    glutSetWindow( jan[ 1 ] );
    glutPostRedisplay();
}

/// =======================================================================
// Programa Principal
int main( int argc , char *argv[] )
{
    /*
    if ( argc != 2 )
    {
        cout << "faltou nome do arquivo a ser aberto" << endl;
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

*/
    fp_arquivo = fopen("imagens\\entrada24bits.bmp","rb");
    if ( !fp_arquivo )
    {
        cout << endl << "Nao pode abrir o arquivo " << endl;
        return -1;
    }
    else
        cout << endl << "Sucesso na abertura do arquivo " << endl;

    // ESTA24.BMP"
    // "circulos24bits.bmp" );
    // "listras24bits.bmp" );
    // "girassol24bits.bmp" );
    // "entrada24bits.bmp" );
    // "nova24bits.bmp" );

    glutInit( &argc , argv );

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // janela do help
    glutInitWindowSize( LAR_HELP , ALT_HELP );
    glutInitWindowPosition( 150 , 150 );
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

