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

#define MAP_H 19
#define MAP_W 21
#define NUM_INIMIGOS 3

#define EMOJI_JOGADOR u8"🗿"
#define EMOJI_INIMIGO u8"💀"
#define EMOJI_PAREDE_SOLIDA u8"🧱"
#define EMOJI_PAREDE_FRAGIL u8"❎"
#define EMOJI_BOMBA u8"💣"
#define EMOJI_EXPLOSAO u8"💥"
#define EMOJI_VAZIO "  "

void limparAreaExplosao(bool areaExplosao[MAP_H][MAP_W])
{
    // Transforma a matriz inteira do mapa em false.
    for (int i = 0; i < MAP_H; i++)
    {
        for (int j = 0; j < MAP_W; j++)
        {
            areaExplosao[i][j] = false;
        }
    }
}

void criarMapa(int matriz[MAP_H][MAP_W], int altura, int largura)
{
    for (int i = 0; i < altura; i++)
    {
        for (int j = 0; j < largura; j++)
        {
            if (i == 0 || i == altura - 1 || j == 0 || j == largura - 1)
            {
                matriz[i][j] = 1; // borda solida
            }
            else if (i % 2 == 0 && j % 2 == 0)
            {
                matriz[i][j] = 1; // blocos internos solidos
            }
            else
            {
                // 35% de chance de ser parede fragil, 65% de chance de ser vazio.
                int sorteio = rand() % 100;
                if (sorteio < 35)
                {
                    matriz[i][j] = 2; // parede fragil aleatoria
                }
                else
                {
                    matriz[i][j] = 0; // espaco vazio
                }
            }
        }
    }

    // Mantem áreas iniciais livres para jogador e inimigos
    // esquerda superior (personagem)
    matriz[1][1] = 0;
    matriz[1][2] = 0;
    matriz[2][1] = 0;

    // direita superior (inimigo)
    matriz[1][largura - 2] = 0;
    matriz[1][largura - 3] = 0;
    matriz[2][largura - 2] = 0;

    // direita inferior (inimigo)
    matriz[altura - 2][largura - 2] = 0;
    matriz[altura - 2][largura - 3] = 0;
    matriz[altura - 3][largura - 2] = 0;

    // esquerda inferior (inimigo)
    matriz[altura - 2][1] = 0;
    matriz[altura - 2][2] = 0;
    matriz[altura - 3][1] = 0;
}

void processarBombaEExplosao(int matriz[MAP_H][MAP_W], bool &bombaAtiva, int &bombaX, int &bombaY, int &contadorBomba, bool &explosaoAtiva, int &contadorExplosao, bool areaExplosao[MAP_H][MAP_W])
{
    if (!bombaAtiva)
    {
        return;
    }

    contadorBomba--;
    if (contadorBomba > 0)
    {
        return;
    }

    bombaAtiva = false;
    explosaoAtiva = true;
    contadorExplosao = 8;
    limparAreaExplosao(areaExplosao);
    areaExplosao[bombaX][bombaY] = true;

    // Verifica as 4 direções ao redor da bomba para marcar a área de explosão, respeitando os limites do mapa e as paredes sólidas.
    int direcoes[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}};
    for (int k = 0; k < 4; k++)
    {
        int nx = bombaX + direcoes[k][0];
        int ny = bombaY + direcoes[k][1];

        if (nx < 0 || nx >= MAP_H || ny < 0 || ny >= MAP_W)
            continue;

        if (matriz[nx][ny] == 1)
            continue;

        areaExplosao[nx][ny] = true;

        if (matriz[nx][ny] == 2)
        {
            matriz[nx][ny] = 0;
        }
    }
}

void atualizarDuracaoExplosao(bool &explosaoAtiva, int &contadorExplosao, bool areaExplosao[MAP_H][MAP_W])
{
    if (!explosaoAtiva)
    {
        return;
    }

    contadorExplosao--;
    if (contadorExplosao <= 0)
    {
        explosaoAtiva = false;
        limparAreaExplosao(areaExplosao);
    }
}

void moverInimigos(int matriz[MAP_H][MAP_W], bool inimigoVivo[NUM_INIMIGOS], int inimigoX[NUM_INIMIGOS],
                   int inimigoY[NUM_INIMIGOS], int contadorMovInimigo[NUM_INIMIGOS], bool bombaAtiva, int bombaX,
                   int bombaY)
{
    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        if (!inimigoVivo[i])
        {
            continue;
        }

        contadorMovInimigo[i]++;
        if (contadorMovInimigo[i] < 3)
        {
            continue;
        }

        contadorMovInimigo[i] = 0;

        int dir = rand() % 4;
        int dx = 0, dy = 0;

        if (dir == 0)
            dx = -1;
        if (dir == 1)
            dx = 1;
        if (dir == 2)
            dy = -1;
        if (dir == 3)
            dy = 1;

        int nx = inimigoX[i] + dx;
        int ny = inimigoY[i] + dy;

        if (nx >= 0 && nx < MAP_H && ny >= 0 && ny < MAP_W && matriz[nx][ny] != 1 && matriz[nx][ny] != 2)
        {
            if (!(bombaAtiva && nx == bombaX && ny == bombaY))
            {
                inimigoX[i] = nx;
                inimigoY[i] = ny;
            }
        }
    }
}

void aplicarDanoExplosao(bool explosaoAtiva, bool areaExplosao[MAP_H][MAP_W], int jogadorX, int jogadorY, bool &jogadorVivo, bool inimigoVivo[NUM_INIMIGOS], int inimigoX[NUM_INIMIGOS], int inimigoY[NUM_INIMIGOS])
{
    if (!explosaoAtiva)
    {
        return;
    }

    if (areaExplosao[jogadorX][jogadorY])
    {
        jogadorVivo = false;
    }

    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        if (inimigoVivo[i] && areaExplosao[inimigoX[i]][inimigoY[i]])
        {
            inimigoVivo[i] = false;
        }
    }
}

bool temInimigoNaPosicao(bool inimigoVivo[NUM_INIMIGOS], int inimigoX[NUM_INIMIGOS], int inimigoY[NUM_INIMIGOS], int linha, int coluna)
{
    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        if (inimigoVivo[i] && inimigoX[i] == linha && inimigoY[i] == coluna)
        {
            return true;
        }
    }
    return false;
}

void desenharMapaBase(int matriz[MAP_H][MAP_W], int jogadorX, int jogadorY, bool inimigoVivo[NUM_INIMIGOS], int inimigoX[NUM_INIMIGOS], int inimigoY[NUM_INIMIGOS], bool bombaAtiva, int bombaX, int bombaY, bool explosaoAtiva, bool areaExplosao[MAP_H][MAP_W])
{
    for (int i = 0; i < MAP_H; i++)
    {
        for (int j = 0; j < MAP_W; j++)
        {
            if (explosaoAtiva && areaExplosao[i][j])
            {
                cout << EMOJI_EXPLOSAO;
                continue;
            }

            if (i == jogadorX && j == jogadorY)
            {
                cout << EMOJI_JOGADOR;
            }
            else if (temInimigoNaPosicao(inimigoVivo, inimigoX, inimigoY, i, j))
            {
                cout << EMOJI_INIMIGO;
            }
            else if (bombaAtiva && i == bombaX && j == bombaY)
            {
                cout << EMOJI_BOMBA;
            }
            else if (matriz[i][j] == 0)
            {
                cout << EMOJI_VAZIO;
            }
            else if (matriz[i][j] == 1)
            {
                cout << EMOJI_PAREDE_SOLIDA;
            }
            else
            {
                cout << EMOJI_PAREDE_FRAGIL;
            }
        }
        cout << "\n";
    }
}

// void desenharInimigoBombaExplosao(bool inimigoVivo, int inimigoX, int inimigoY, bool bombaAtiva, int bombaX, int bombaY, bool explosaoAtiva, bool areaExplosao[MAP_H][MAP_W], COORD &cursorAux)
// {
//     (void)inimigoVivo;
//     (void)inimigoX;
//     (void)inimigoY;
//     (void)bombaAtiva;
//     (void)bombaX;
//     (void)bombaY;
//     (void)explosaoAtiva;
//     (void)areaExplosao;
//     (void)cursorAux;
// }

void desenharHUD(COORD &cursorAux)
{
    cursorAux.X = 0;
    cursorAux.Y = MAP_H + 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
    cout << "WASD/Setas: mover | ESPACO: bomba | ESC: sair      ";
}

bool processarEntrada(int &x, int &y, int &jogadorXAnterior, int &jogadorYAnterior, bool &bombaAtiva, int &bombaX, int &bombaY, int &contadorBomba)
{
    if (!_kbhit())
    {
        return false;
    }

    char tecla = getch();
    if (tecla == 0 || tecla == (char)224)
    {
        tecla = getch();
    }

    jogadorXAnterior = x;
    jogadorYAnterior = y;

    switch (tecla)
    {
    case 72:
    case 'w':
    case 'W':
        x--;
        break;
    case 80:
    case 's':
    case 'S':
        x++;
        break;
    case 75:
    case 'a':
    case 'A':
        y--;
        break;
    case 77:
    case 'd':
    case 'D':
        y++;
        break;
    case ' ':
        if (!bombaAtiva)
        {
            bombaAtiva = true;
            bombaX = x;
            bombaY = y;
            contadorBomba = 20; // tempo para a bomba explodir (20 ciclos de jogo)
        }
        break;
    case 27:
        return true;
    }

    return false;
}

void validarMovimentoJogador(int matriz[MAP_H][MAP_W], int &x, int &y, int jogadorXAnterior, int jogadorYAnterior, bool bombaAtiva, int bombaX, int bombaY)
{
    if (x < 0 || x >= MAP_H || y < 0 || y >= MAP_W || matriz[x][y] == 1 || matriz[x][y] == 2 || (bombaAtiva && x == bombaX && y == bombaY))
    {
        x = jogadorXAnterior;
        y = jogadorYAnterior;
    }
}

void verificarColisaoComInimigos(bool inimigoVivo[NUM_INIMIGOS], int jogadorX, int jogadorY, int inimigoX[NUM_INIMIGOS], int inimigoY[NUM_INIMIGOS], bool &jogadorVivo)
{
    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        if (inimigoVivo[i] && jogadorX == inimigoX[i] && jogadorY == inimigoY[i])
        {
            jogadorVivo = false;
            return;
        }
    }
}

bool todosInimigosMortos(bool inimigoVivo[NUM_INIMIGOS])
{
    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        if (inimigoVivo[i])
        {
            return false;
        }
    }
    return true;
}

void mostrarMensagemResultado(bool jogadorVivo, bool todosMortos, COORD &cursorAux)
{
    cursorAux.X = 0;
    cursorAux.Y = MAP_H + 2;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);

    if (!jogadorVivo)
    {
        cout << "DERROTA: você morreu." << endl;
    }
    else if (jogadorVivo && todosMortos)
    {
        cout << "VITÓRIA: todos os inimigos eliminados." << endl;
    }
}

void reiniciarJogo(int matriz[MAP_H][MAP_W], int &x, int &y, int &jogadorXAnterior, int &jogadorYAnterior, bool &jogadorVivo, bool inimigoVivo[NUM_INIMIGOS], int inimigoX[NUM_INIMIGOS], int inimigoY[NUM_INIMIGOS], int contadorMovInimigo[NUM_INIMIGOS], bool &bombaAtiva, int &bombaX, int &bombaY, int &contadorBomba, bool &explosaoAtiva, int &contadorExplosao, bool areaExplosao[MAP_H][MAP_W])
{
    criarMapa(matriz, MAP_H, MAP_W);

    x = 1;
    y = 1;
    jogadorXAnterior = x;
    jogadorYAnterior = y;
    jogadorVivo = true;

    inimigoVivo[0] = true;
    inimigoVivo[1] = true;
    inimigoVivo[2] = true;

    inimigoX[0] = 1;
    inimigoY[0] = MAP_W - 2;

    inimigoX[1] = MAP_H - 2;
    inimigoY[1] = MAP_W - 2;

    inimigoX[2] = MAP_H - 2;
    inimigoY[2] = 1;

    contadorMovInimigo[0] = 0;
    contadorMovInimigo[1] = 0;
    contadorMovInimigo[2] = 0;

    bombaAtiva = false;
    bombaX = -1;
    bombaY = -1;
    contadorBomba = 0;

    explosaoAtiva = false;
    contadorExplosao = 0;
    limparAreaExplosao(areaExplosao);
}

// Equipe: Gabriel Debiasi Meurer, Pedro Henrique Rondon, Victor Alexandre Andrade
int main()
{
    srand((unsigned)time(NULL));
    SetConsoleOutputCP(CP_UTF8);

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

    int matriz[MAP_H][MAP_W];
    criarMapa(matriz, MAP_H, MAP_W);

    int x = 1, y = 1; // posição inicial do jogador
    int jogadorXAnterior = x, jogadorYAnterior = y;

    bool jogadorVivo = true;
    bool inimigoVivo[NUM_INIMIGOS] = {true, true, true};
    int inimigoX[NUM_INIMIGOS] = {1, MAP_H - 2, MAP_H - 2};
    int inimigoY[NUM_INIMIGOS] = {MAP_W - 2, MAP_W - 2, 1};
    int contadorMovInimigo[NUM_INIMIGOS] = {0, 0, 0};

    bool bombaAtiva = false;
    int bombaX = -1, bombaY = -1; // posição da bomba, -1 indica que nao tem bomba ativa ("vazio")
    int contadorBomba = 0;

    bool explosaoAtiva = false;
    int contadorExplosao = 0;
    bool areaExplosao[MAP_H][MAP_W] = {false};

    COORD cursorAux;
    int menu;
    do
    {
        system("cls");
        cout << "====== BOMBERMAN ======" << endl;
        cout << "1 - Jogar" << endl;
        cout << "2 - Sobre" << endl;
        cout << "3 - Sair" << endl;
        cout << "Escolha uma opção: ";
        cin >> menu;

        switch (menu)
        {
        case 1:
            reiniciarJogo(matriz, x, y, jogadorXAnterior, jogadorYAnterior,
                          jogadorVivo, inimigoVivo, inimigoX, inimigoY, contadorMovInimigo,
                          bombaAtiva, bombaX, bombaY, contadorBomba,
                          explosaoAtiva, contadorExplosao, areaExplosao);
            while (true)
            {
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

                processarBombaEExplosao(matriz, bombaAtiva, bombaX, bombaY, contadorBomba, explosaoAtiva, contadorExplosao, areaExplosao);
                aplicarDanoExplosao(explosaoAtiva, areaExplosao, x, y, jogadorVivo, inimigoVivo, inimigoX, inimigoY);
                atualizarDuracaoExplosao(explosaoAtiva, contadorExplosao, areaExplosao);

                moverInimigos(matriz, inimigoVivo, inimigoX, inimigoY, contadorMovInimigo, bombaAtiva, bombaX, bombaY);
                verificarColisaoComInimigos(inimigoVivo, x, y, inimigoX, inimigoY, jogadorVivo);

                desenharMapaBase(matriz, x, y, inimigoVivo, inimigoX, inimigoY, bombaAtiva, bombaX, bombaY, explosaoAtiva, areaExplosao);
                desenharHUD(cursorAux);

                if (processarEntrada(x, y, jogadorXAnterior, jogadorYAnterior, bombaAtiva, bombaX, bombaY, contadorBomba))
                {
                    return 0;
                }

                validarMovimentoJogador(matriz, x, y, jogadorXAnterior, jogadorYAnterior, bombaAtiva, bombaX, bombaY);

                verificarColisaoComInimigos(inimigoVivo, x, y, inimigoX, inimigoY, jogadorVivo);
                aplicarDanoExplosao(explosaoAtiva, areaExplosao, x, y, jogadorVivo, inimigoVivo, inimigoX, inimigoY);

                bool todosMortos = todosInimigosMortos(inimigoVivo);

                if (!jogadorVivo)
                {
                    mostrarMensagemResultado(jogadorVivo, todosMortos, cursorAux);
                    break;
                }

                if (jogadorVivo && todosMortos)
                {
                    mostrarMensagemResultado(jogadorVivo, todosMortos, cursorAux);
                    break;
                }

                Sleep(80); // pausa o jogo por 80 milissegundos a cada ciclo para controlar a velocidade do jogo.

            } // fim do laço do jogo

            cursorAux.X = 0;
            cursorAux.Y = MAP_H + 3;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de saída para aparecer no lugar correto, abaixo da mensagem de derrota/vitoria.
            cout << "Pressione qualquer tecla para sair...";
            cin.ignore();
            getch();
            break;
        case 2:
            system("cls");
            cout << "Trabalho de Algoritmos e Programação II - Bomberman" << endl;
            cout << "Equipe: Gabriel Debiasi Meurer, Pedro Henrique Rondon, Victor Alexandre Andrade" << endl;
            cout << "Pressione qualquer tecla para voltar ao menu..." << endl;
            cin.ignore();
            getch();
            break;
        case 3:
            cout << "Saindo do jogo..." << endl;
            return 0;
        default:
            cout << "Opção inválida. Tente novamente." << endl;
        }
    } while (menu != 3);

    return 0;
}