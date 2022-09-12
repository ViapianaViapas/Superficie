#include <iostream>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <string.h>

#include<superficie.h>

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

    mostra_texto_bitmap( 0 , 200 , "ESC           : finaliza o programa" );
    mostra_texto_bitmap( 0 , 182 , "L             : liga/desliga iluminacao" );
    mostra_texto_bitmap( 0 , 164 , "X x Y y Z z   : rotaciona a cena" );
    mostra_texto_bitmap( 0 , 146 , "C             : liga/desliga coloracao" );
    mostra_texto_bitmap( 0 , 128 , "E             : exibe/oculta eixos" );
    mostra_texto_bitmap( 0 , 110 , "F             : alterna tonalizacao entre flat e smooth" );
    mostra_texto_bitmap( 0 ,  93 , "ALT +         : aumenta o passo de movimento da camera" );
    mostra_texto_bitmap( 0 ,  74 , "ALT -         : diminui o passo de movimento da camera" );
    mostra_texto_bitmap( 0 ,  56 , "I             : reinicializa" );
    mostra_texto_bitmap( 0 ,  38 , "M             : alterna entre pontos, wireframe e solido" );
    mostra_texto_bitmap( 0 ,  20 , "TECLAS NAVEG. : movimenta a fonte de luz" );
    mostra_texto_bitmap( 0 ,   2 , "W A S D Q \\   : movimenta a camera" );

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
    glMaterialfv( GL_FRONT , GL_SPECULAR  , especularidade );

    // Define a concentração do brilho
    glMateriali(  GL_FRONT , GL_SHININESS , espec_material );

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


