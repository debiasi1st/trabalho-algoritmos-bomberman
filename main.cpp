/**
    Estrutura inicial para um jogo
    versão: 0.1 (Prof. Alex,  Adaptado Prof. Felski)
*/
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime> // lib para usar time() e srand()
#include <cstdlib> // lib para usar rand() e srand()

using namespace std;

// Equipe: Gabriel Debiasi Meurer, Pedro Henrique Rondon, Victor Alexandre Andrade 
int main()
{
    srand((unsigned)time(NULL));

    ///ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, A SEGUIR.
        //INICIO: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO     cursorInfo;
        GetConsoleCursorInfo(out, &cursorInfo);
        cursorInfo.bVisible = false; // set the cursor visibility
        SetConsoleCursorInfo(out, &cursorInfo);
        //FIM: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
        //INICIO: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
        short int CX=0, CY=0;
        COORD coord;
        coord.X = CX;
        coord.Y = CY;
        //FIM: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
    ///ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, ACIMA.

    int m[7][7]={   1,1,1,1,1,1,1,
                    1,0,0,0,0,0,1,
                    1,0,0,0,1,1,1,
                    1,0,0,0,1,0,1,
                    1,1,0,0,1,0,1,
                    1,0,0,0,0,0,1,
                    1,1,1,1,1,1,1};

    // Adições: paredes frageis no mapa (valor 2)
    m[1][2]=2;
    m[2][3]=2;
    m[4][3]=2;
    m[5][2]=2;

    //Posicao inicial do personagem no console
    int x=5, y=5;
    //Variavel para tecla precionada
    char tecla;


    // controle de movimento
    // salva a posição anterior do jogador, caso ele tente se mover para uma posição invalida (parede ou fora do mapa)
    int jogadorXAnterior = x, jogadorYAnterior = y;

    // estado do jogo
    bool jogadorVivo = true;
    bool inimigoVivo = true;
    int inimigoX = 1, inimigoY = 1;
    int contadorMovInimigo = 0;

    // bomba unica
    bool bombaAtiva = false;
    int bombaX = -1, bombaY = -1; // posição da bomba, -1 indica que nao tem bomba ativa ("vazio")
    int contadorBomba = 0;

    // explosao
    bool explosaoAtiva = false;
    int contadorExplosao = 0;
    bool areaExplosao[7][7] = {false};

    COORD cursorAux; // coordenada auxiliar para reposicionar o cursor para desenhar inimigo, bomba e explosao por cima do mapa.

    while(true){
        ///Posiciona a escrita no iicio do console
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        // Atualiza bomba
        if (bombaAtiva) {
            contadorBomba--;
            if (contadorBomba <= 0) {
                bombaAtiva = false;
                explosaoAtiva = true;
                contadorExplosao = 8; // duração da explosão em ciclos do jogo

                // limpa matriz da explosao
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        areaExplosao[i][j] = false;
                    }
                }

                // centro da explosao
                areaExplosao[bombaX][bombaY] = true;

                // cruz (raio 1) -> formato da area da explosão.
                int nx, ny;

                // calculo da area da explosão, verifica se tem parede sólida ou frágil, além de definir o formato de cruz da explosão.
                nx = bombaX - 1; ny = bombaY; // pega a posição acima da bomba
                if (nx >= 0 && m[nx][ny] != 1) {
                    areaExplosao[nx][ny] = true;
                    if (m[nx][ny] == 2) m[nx][ny] = 0; // destroi se tiver parede frágil
                }

                nx = bombaX + 1; ny = bombaY; // pega a posição abaixo da bomba
                if (nx < 7 && m[nx][ny] != 1) {
                    areaExplosao[nx][ny] = true;
                    if (m[nx][ny] == 2) m[nx][ny] = 0; // destroi se tiver parede frágil
                }

                nx = bombaX; ny = bombaY - 1; // pega a posição à esquerda da bomba
                if (ny >= 0 && m[nx][ny] != 1) {
                    areaExplosao[nx][ny] = true;
                    if (m[nx][ny] == 2) m[nx][ny] = 0; // destroi se tiver parede frágil
                }

                nx = bombaX; ny = bombaY + 1; // pega a posição à direita da bomba
                if (ny < 7 && m[nx][ny] != 1) {
                    areaExplosao[nx][ny] = true;
                    if (m[nx][ny] == 2) m[nx][ny] = 0; // destroi se tiver parede frágil
                }
            }
        }

        // Atualiza dano da explosao
        if (explosaoAtiva) {
            if (areaExplosao[x][y]) { // verifica se a posição do jogador está na área da explosão
                jogadorVivo = false;
            }
            if (inimigoVivo && areaExplosao[inimigoX][inimigoY]) { // verifica se a posição do inimigo está na área da explosão
                inimigoVivo = false;
            }

            contadorExplosao--;
            if (contadorExplosao <= 0) { // quando termina o tempo da explosão, desativa e limpa a área
                explosaoAtiva = false;
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        areaExplosao[i][j] = false;
                    }
                }
            }
        }

        // Movimento do inimigo por tempo
        if (inimigoVivo) {
            contadorMovInimigo++;
            if (contadorMovInimigo >= 10) {
                contadorMovInimigo = 0;

                int dir = rand() % 4; // sorteia uma das 4 direções: 0=cima, 1=baixo, 2=esquerda, 3=direita.
                int passos = rand() % 3 + 1;
                int dx = 0, dy = 0; // variaveis de direção para calcular o movimento do inimigo.

                // aplica a direção sorteada.
                if (dir == 0) dx = -1; 
                if (dir == 1) dx = 1;
                if (dir == 2) dy = -1;
                if (dir == 3) dy = 1;

                for (int p = 0; p < passos; p++) {
                    int nx = inimigoX + dx;
                    int ny = inimigoY + dy;

                    if (nx < 0 || nx >= 7 || ny < 0 || ny >= 7) break;
                    if (m[nx][ny] == 1 || m[nx][ny] == 2) break;

                    inimigoX = nx;
                    inimigoY = ny;

                    if (bombaAtiva && inimigoX == bombaX && inimigoY == bombaY) {
                        inimigoVivo = false;
                        break;
                    }
                }
            }
        }

        if (inimigoVivo && x == inimigoX && y == inimigoY) { // se o jogador encosta no inimigo, perde.
            jogadorVivo = false;
        }

        ///Imprime o jogo: mapa e personagem.
        for(int i=0;i<7;i++){
            for(int j=0;j<7;j++){
                if(i==x && j==y){
                    cout<<char(36); //personagem
                } else {
                    switch (m[i][j]){
                        case 0: cout<<" "; break; //caminho
                        case 1: cout<<char(219); break; //parede solida
                        case 2: cout<<char(178); break; //parede fragil
                    } //fim switch
                }
            }
            cout<<"\n";
        } //fim for mapa

        // Desenha inimigo, bomba e explosao por cima do mapa
        if (inimigoVivo) {
            cursorAux.X = inimigoY; // define onde a posição do cursor deve se posicionar para desenhar o inimigo.
            cursorAux.Y = inimigoX; // define onde a posição do cursor deve se posicionar para desenhar o inimigo.
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
            cout << "&";
        }

        if (bombaAtiva) {
            cursorAux.X = bombaY; // define onde a posição do cursor deve se posicionar para desenhar a bomba.
            cursorAux.Y = bombaX; // define onde a posição do cursor deve se posicionar para desenhar a bomba.
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
            cout << "o";
        }

        if (explosaoAtiva) {
            for (int i = 0; i < 7; i++) {
                for (int j = 0; j < 7; j++) {
                    if (areaExplosao[i][j]) { // para cada posição da área da explosão, desenha um asterisco. (calculo a partir da linha 100)
                        cursorAux.X = j;
                        cursorAux.Y = i;
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
                        cout << "*";
                    }
                }
            }
        }

        cursorAux.X = 0;
        cursorAux.Y = 8;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux);
        cout << "WASD/Setas: mover | ESPACO: bomba | ESC: sair      ";

        ///executa os movimentos
         if ( _kbhit() ){ // verifica se uma tecla foi pressionada, se sim, executa o movimento correspondente.
            tecla = getch(); // pega o código da tecla pressionada.

            // tratamento de setas
            if (tecla == 0 || tecla == (char)224) { // se for seta, o getch() precisa ser chamado duas vezes para pegar o código completo
                tecla = getch();
            }

            jogadorXAnterior = x; // salva posição anterior do jogador antes de tentar mover
            jogadorYAnterior = y;

            switch(tecla)
            {
                case 72: case 'w': case 'W': ///cima
                    x--;
                break;
                case 80: case 's': case 'S': ///baixo
                    x++;
                break;
                case 75:case 'a': case 'A': ///esquerda
                    y--;
                break;
                case 77: case 'd': case 'D': ///direita
                    y++;
                break;
                case ' ': // toda vez que o jogador apertar espaço, tenta colocar uma bomba.
                    if (!bombaAtiva) { // se nao tiver bomba ativa, pode colocar uma.
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
        if (x < 0 || x >= 7 || y < 0 || y >= 7 || m[x][y] == 1 || m[x][y] == 2) {
            x = jogadorXAnterior;
            y = jogadorYAnterior;
        }

        // checagens finais de derrota/vitoria
        if (inimigoVivo && x == inimigoX && y == inimigoY){ // se o jogador encosta no inimigo, perde.
            jogadorVivo = false;
        }
        if (explosaoAtiva && areaExplosao[x][y]){ // se o jogador estiver na área da explosão, perde.
            jogadorVivo = false;
        }


        if (!jogadorVivo) {
            cursorAux.X = 0;
            cursorAux.Y = 9;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de derrota/vitoria para aparecer no lugar correto.
            cout << "DERROTA: voce morreu.                             ";
            break;
        }

        if (jogadorVivo && !inimigoVivo) {
            cursorAux.X = 0;
            cursorAux.Y = 9;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de derrota/vitoria para aparecer no lugar correto.
            cout << "VITORIA: inimigo eliminado.                       ";
            break;
        }

        Sleep(80);

    } //fim do laco do jogo

    cursorAux.X = 0;
    cursorAux.Y = 10;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorAux); // organiza a mensagem de saída para aparecer no lugar correto, abaixo da mensagem de derrota/vitoria.
    cout << "Pressione qualquer tecla para sair...";
    getch();

    return 0;
} //fim main
