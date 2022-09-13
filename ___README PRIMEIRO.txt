Este projeto foi feito utilizando o CodeBlocks.

Programado em C++ com utilização da OpenGL (GLUT).

Composto por um arquivo pincipal (main.cpp), um arquivo de funções (funcoes.cpp) e os 
arquivos da classe superficie (superficie.cpp e superficie.h).

Não foi configurado CMkae para criar o projeto.

Infelizmente, esta versão do programa não está lendo corretamente imagens BMP de 8 bits
de profundidade de cores (imagens monogrcomática - gray scale).

Para contornar este problema, cria-se imagens de BMP de 24 bits com escala de cinza (gray scale).
Assim, o programa pega o valro do canal R como cor (uma vez que os três canais - RGB -
possuem o mesmo valor em uma imagem de gray sacle).

Por fim, não foi utilizado foi utilizada uma segunda imagem como textura, nem utilizado
shader GLSL me, aplicado tesselation.

Há um o arquivo de video Exemplo Uitilização.mpg que demonstra o uso do programa.
 
Abraço e bom trabalho