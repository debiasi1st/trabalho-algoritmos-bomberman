/**
    Estrutura inicial para um jogo
    versão: 0.1 (Prof. Alex,  Adaptado Prof. Felski)
*/
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>   // lib para usar time() e srand()
#include <cstdlib> // lib para usar rand() e srand()
using namespace std;

#define MAP_H 11
#define MAP_W 13

// Equipe: Gabriel Debiasi Meurer, Pedro Henrique Rondon, Victor Alexandre Andrade
int main()
{
    srand((unsigned)time(NULL));
    SetConsoleOutputCP(437);

    /// ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, A SEGUIR.
    // INICIO: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
    // FIM: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
    // INICIO: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
    short int CX = 0, CY = 0;
    COORD coord;
    coord.X = CX;
    coord.Y = CY;
    // FIM: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
    /// ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, ACIMA.

    int matriz[MAP_H][MAP_W] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 2, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 2, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

    int x = 1, y = 1; // posição inicial do jogador
    char tecla;

    int jogadorXAnterior = x, jogadorYAnterior = y;

    bool jogadorVivo = true;
    bool inimigoVivo = true;
    int inimigoX = MAP_H - 2, inimigoY = MAP_W - 2; // posição inicial do inimigo
    int contadorMovInimigo = 0;

    bool bombaAtiva = false;
    int bombaX = -1, bombaY = -1; // posição da bomba, -1 indica que nao tem bomba ativa ("vazio")
    int contadorBomba = 0;

    bool explosaoAtiva = false;
    int contadorExplosao = 0;
    bool areaExplosao[MAP_H][MAP_W] = {false};

    const char BLOCO = char(219);

    COORD cursorAux;

    while (true)
    {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        if (bombaAtiva)
        {
            contadorBomba--;
            if (contadorBomba <= 0)
            {
                bombaAtiva = false;
                explosaoAtiva = true;
                contadorExplosao = 8; // duração da explosão em ciclos do jogo

                for (int i = 0; i < MAP_H; i++)
                {
                    for (int j = 0; j < MAP_W; j++)
                    {
                        areaExplosao[i][j] = false;
                    }
                }

                // centro da explosao
                areaExplosao[bombaX][bombaY] = true;

                // direcoes: cima, baixo, esquerda, direita (forma a acruz da explosão.)
                int direcoes[4][2] = {
                    {-1, 0},
                    {1, 0},
                    {0, -1},
                    {0, 1}};

                for (int k = 0; k < 4; k++)
                { // forma a cruz da explosão, verifica se tem parede sólida ou frágil, além de definir o formato de cruz da explosão.
                    int nx = bombaX + direcoes[k][0];
                    int ny = bombaY + direcoes[k][1];

                    // limite do mapa
                    if (nx < 0 || nx >= MAP_H || ny < 0 || ny >= MAP_W)
                        continue;

                    // parede solida bloqueia
                    if (matriz[nx][ny] == 1)
                        continue;

                    // entra na area da explosao
                    areaExplosao[nx][ny] = true;

                    // parede fragil eh destruida
                    if (matriz[nx][ny] == 2)
                    {
                        matriz[nx][ny] = 0;
                    }
                }
            }
        }

        if (explosaoAtiva)
        {
            if (areaExplosao[x][y])
            { // verifica se a posição do jogador está na área da explosão
                jogadorVivo = false;
            }
            if (inimigoVivo && areaExplosao[inimigoX][inimigoY])
            { // verifica se a posição do inimigo está na área da explosão
                inimigoVivo = false;
            }

            contadorExplosao--;
            if (contadorExplosao <= 0)
            { // quando termina o tempo da explosão, desativa e limpa a área
                explosaoAtiva = false;
                for (int i = 0; i < MAP_H; i++)
                {
                    for (int j = 0; j < MAP_W; j++)
                    {
                        areaExplosao[i][j] = false;
                    }
                }
            }
        }

        if (inimigoVivo)
        {
            contadorMovInimigo++;

            // menor valor = inimigo atualiza mais vezes (mais fluido)
            if (contadorMovInimigo >= 3)
            {
                contadorMovInimigo = 0;

                int dir = rand() % 4; // 0=cima, 1=baixo, 2=esquerda, 3=direita
                int dx = 0, dy = 0;

                if (dir == 0)
                    dx = -1;
                if (dir == 1)
                    dx = 1;
                if (dir == 2)
                    dy = -1;
                if (dir == 3)
                    dy = 1;

                // anda somente 1 casa por atualização (sem "teleporte")
                int nx = inimigoX + dx;
                int ny = inimigoY + dy;

                // verifica se o inimigo não vai atravessar paredes ou sair do mapa, se estiver tudo ok, move o inimigo.
                if (nx >= 0 && nx < MAP_H && ny >= 0 && ny < MAP_W && matriz[nx][ny] != 1 && matriz[nx][ny] != 2)
                {
                    inimigoX = nx;
                    inimigoY = ny;
                }

                // se o inimigo tiver na posição da área da bomba, ele morre. (isso pode acontecer se o inimigo andar para cima da bomba.)
                if (bombaAtiva && inimigoX == bombaX && inimigoY == bombaY)
                {
                    inimigoVivo = false;
                }
            }
        }

        if (inimigoVivo && x == inimigoX && y == inimigoY)
        { // se o jogador encosta no inimigo, perde.
            jogadorVivo = false;
        }

        if (inimigoVivo && inimigoX == x && inimigoY == y)
        { // se o jogador encosta no inimigo, perde.
            jogadorVivo = false;
        }

        // Desenha mapa e jogador
        for (int i = 0; i < MAP_H; i++)
        {
            for (int j = 0; j < MAP_W; j++)
            {
                if (i == x && j == y)
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
                    cout << BLOCO; // jogador
                }
                else
                {
                    if (matriz[i][j] == 0)
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
                        cout << BLOCO; // caminho vazio
                    }
                    else if (matriz[i][j] == 1)
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                        cout << BLOCO; // parede solida
                    }
                    else
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                        cout << BLOCO; // parede fragil
                    }
                }
            }
            cout << "\n";
        }

        // Desenha inimigo, bomba e explosao por cima do mapa
        if (inimigoVivo)
        {
            cursorAux.X = inimigoY; // define onde a posição do cursor deve se posicionar para desenhar o inimigo.
            cursorAux.Y = inimigoX; // define onde a posição do cursor deve se posicionar para desenhar o inimigo.
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            cout << BLOCO;
        }

        if (bombaAtiva)
        {
            cursorAux.X = bombaY; // define onde a posição do cursor deve se posicionar para desenhar a bomba.
            cursorAux.Y = bombaX; // define onde a posição do cursor deve se posicionar para desenhar a bomba.
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
            cout << BLOCO;
        }

        if (explosaoAtiva)
        {
            for (int i = 0; i < MAP_H; i++)
            {
                for (int j = 0; j < MAP_W; j++)
                {
                    if (areaExplosao[i][j])
                    { // para cada posição da área da explosão, desenha um asterisco. (calculo a partir da linha 100)
                        cursorAux.X = j;
                        cursorAux.Y = i;
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
                        cout << BLOCO;
                    }
                }
            }
        }

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cursorAux.X = 0;
        cursorAux.Y = MAP_H + 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
        cout << "WASD/Setas: mover | ESPACO: bomba | ESC: sair      ";

        /// executa os movimentos
        if (_kbhit())
        {                    // verifica se uma tecla foi pressionada, se sim, executa o movimento correspondente.
            tecla = getch(); // pega o código da tecla pressionada.

            // tratamento de setas
            if (tecla == 0 || tecla == (char)224)
            { // se for seta, F1, etc, o getch() precisa ser chamado duas vezes para pegar o código completo
                tecla = getch();
            }

            jogadorXAnterior = x; // salva posição anterior do jogador antes de tentar mover
            jogadorYAnterior = y;

            switch (tecla)
            {
            case 72:
            case 'w':
            case 'W': /// cima
                x--;
                break;
            case 80:
            case 's':
            case 'S': /// baixo
                x++;
                break;
            case 75:
            case 'a':
            case 'A': /// esquerda
                y--;
                break;
            case 77:
            case 'd':
            case 'D': /// direita
                y++;
                break;
            case ' ': // toda vez que o jogador apertar espaço, tenta colocar uma bomba.
                if (!bombaAtiva)
                { // se nao tiver bomba ativa, pode colocar uma.
                    bombaAtiva = true;
                    bombaX = x; // bomba na posição atual do jogador
                    bombaY = y;
                    contadorBomba = 30; // tempo para a bomba explodir, em ciclos do jogo
                }
                break;
            case 27:
                return 0;
            }
        }

        // jogador bloqueado por parede e limites
        if (x < 0 || x >= MAP_H || y < 0 || y >= MAP_W || matriz[x][y] == 1 || matriz[x][y] == 2)
        {
            x = jogadorXAnterior;
            y = jogadorYAnterior;
        }

        // checagens finais de derrota/vitoria
        if (inimigoVivo && x == inimigoX && y == inimigoY)
        { // se o jogador encosta no inimigo, perde.
            jogadorVivo = false;
        }
        if (explosaoAtiva && areaExplosao[x][y])
        { // se o jogador estiver na área da explosão, perde.
            jogadorVivo = false;
        }

        if (!jogadorVivo)
        {
            cursorAux.X = 0;
            cursorAux.Y = MAP_H + 2;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de derrota/vitoria para aparecer no lugar correto.
            cout << "DERROTA: voce morreu.                             ";
            break;
        }

        if (jogadorVivo && !inimigoVivo)
        {
            cursorAux.X = 0;
            cursorAux.Y = MAP_H + 2;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de derrota/vitoria para aparecer no lugar correto.
            cout << "VITORIA: inimigo eliminado.                       ";
            break;
        }

        Sleep(80);

    } // fim do laco do jogo

    cursorAux.X = 0;
    cursorAux.Y = MAP_H + 3;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de saída para aparecer no lugar correto, abaixo da mensagem de derrota/vitoria.
    cout << "Pressione qualquer tecla para sair...";
    getch();

    return 0;
}