#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define LINHAS 30
#define COLUNAS 30
#define TAMANHO_QUADRADO 20

int direcao = 1; 
int comprimento_cobra = 5;
int cobraX[LINHAS * COLUNAS];
int cobraY[LINHAS * COLUNAS];
int comidaX, comidaY;
int jogo_terminado = 0;
int pausado = 0;
char mensagem[] = "¨:(   Pressione 'r' para reiniciar";

void inicializar() {
    glClearColor(0.0, 0.0, 1.0, 1.0); 
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, COLUNAS * TAMANHO_QUADRADO, 0, LINHAS * TAMANHO_QUADRADO);
    srand(time(NULL));
    cobraX[0] = COLUNAS / 2 * TAMANHO_QUADRADO;
    cobraY[0] = LINHAS / 2 * TAMANHO_QUADRADO;
    comidaX = rand() % COLUNAS * TAMANHO_QUADRADO;
    comidaY = rand() % LINHAS * TAMANHO_QUADRADO;
}

void desenharQuadrado(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + TAMANHO_QUADRADO, y);
    glVertex2i(x + TAMANHO_QUADRADO, y + TAMANHO_QUADRADO);
    glVertex2i(x, y + TAMANHO_QUADRADO);
    glEnd();
}

void desenharComida(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 30) {
        float angle = i * M_PI / 180;
        float xOffset = 0.5 * TAMANHO_QUADRADO * cos(angle);
        float yOffset = 0.5 * TAMANHO_QUADRADO * sin(angle);
        glVertex2f(x + xOffset, y + yOffset);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 180; i < 540; i += 30) {
        float angle = i * M_PI / 180;
        float xOffset = 0.5 * TAMANHO_QUADRADO * cos(angle);
        float yOffset = 0.5 * TAMANHO_QUADRADO * sin(angle);
        glVertex2f(x + xOffset, y + yOffset);
    }
    glEnd();
}

void desenharTexto(int x, int y, char *string) {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x, y);
    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}

void desenhar() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (pausado) {
        desenharQuadrado(0, 0, 0.0, 0.0, 1.0); 
        desenharTexto(100, LINHAS * TAMANHO_QUADRADO / 2, mensagem);
        glutSwapBuffers();
        return;
    }

    // Desenhar fundo xadrez 
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if ((i + j) % 2 == 0) {
                desenharQuadrado(j * TAMANHO_QUADRADO, i * TAMANHO_QUADRADO, 0.2, 0.5, 0.2); 
            } else {
                desenharQuadrado(j * TAMANHO_QUADRADO, i * TAMANHO_QUADRADO, 0.1, 0.4, 0.1); 
            }
        }
    }

    // Desenhar comida 
    desenharComida(comidaX + TAMANHO_QUADRADO / 2, comidaY + TAMANHO_QUADRADO / 2, 1.0, 0.8, 0.4); 

    // Desenhar cobra
    for (int i = 0; i < comprimento_cobra; i++) {
        float blue = (float)i / comprimento_cobra; 
        desenharQuadrado(cobraX[i], cobraY[i], 0.0, 1.0, 0.0); 
    }

    glutSwapBuffers();
}

void reiniciarJogo() {
    comprimento_cobra = 5;
    cobraX[0] = COLUNAS / 2 * TAMANHO_QUADRADO;
    cobraY[0] = LINHAS / 2 * TAMANHO_QUADRADO;
    comidaX = rand() % COLUNAS * TAMANHO_QUADRADO;
    comidaY = rand() % LINHAS * TAMANHO_QUADRADO;
    jogo_terminado = 0;
    pausado = 0;
}

void atualizar(int value) {
    if (!jogo_terminado && !pausado) {
        // Mover a cauda da cobra para frente
        for (int i = comprimento_cobra - 1; i > 0; i--) {
            cobraX[i] = cobraX[i - 1];
            cobraY[i] = cobraY[i - 1];
        }

        // Mover a cabeça da cobra na direção correta
        switch (direcao) {
            case 1:
                cobraY[0] += TAMANHO_QUADRADO;
                break;
            case 2:
                cobraX[0] += TAMANHO_QUADRADO;
                break;
            case 3:
                cobraY[0] -= TAMANHO_QUADRADO;
                break;
            case 4:
                cobraX[0] -= TAMANHO_QUADRADO;
                break;
        }

        // Checar colisão com parede
        if (cobraX[0] >= COLUNAS * TAMANHO_QUADRADO || cobraX[0] < 0 ||
            cobraY[0] >= LINHAS * TAMANHO_QUADRADO || cobraY[0] < 0) {
            pausado = 1;
        }

        // Checar colisão com o próprio corpo
        for (int i = 1; i < comprimento_cobra; i++) {
            if (cobraX[0] == cobraX[i] && cobraY[0] == cobraY[i]) {
                pausado = 1;
            }
        }

        // Checar colisão com comida
        if (cobraX[0] == comidaX && cobraY[0] == comidaY) {
            comprimento_cobra++;
            comidaX = rand() % COLUNAS * TAMANHO_QUADRADO;
            comidaY = rand() % LINHAS * TAMANHO_QUADRADO;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(180, atualizar, 0);
}

void teclasEspeciais(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (direcao != 3)
                direcao = 1;
            break;
        case GLUT_KEY_RIGHT:
            if (direcao != 4)
                direcao = 2;
            break;
        case GLUT_KEY_DOWN:
            if (direcao != 1)
                direcao = 3;
            break;
        case GLUT_KEY_LEFT:
            if (direcao != 2)
                direcao = 4;
            break;
    }
}

void teclado(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':
            if (pausado) {
                reiniciarJogo();
                
            }
            break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(COLUNAS * TAMANHO_QUADRADO, LINHAS * TAMANHO_QUADRADO);
    glutCreateWindow("Jogo da Cobrinha");
    inicializar();
    glutDisplayFunc(desenhar);
    glutTimerFunc(180, atualizar, 0);
    glutSpecialFunc(teclasEspeciais); 
    glutKeyboardFunc(teclado);
    glutMainLoop();
    return 0;
}
