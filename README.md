# Trabalho de Algoritmos - Bomberman (Versão Final/M1)

## Descrição

Este projeto implementa uma versão jogável de Bomberman em C++, desenvolvida como trabalho prático da disciplina de Algoritmos II.
O jogo roda no terminal do Windows, com renderização em grade e uso de emojis para representar os elementos do mapa.

## Tecnologias Utilizadas

- C++
- Bibliotecas padrão: `iostream`, `ctime`, `cstdlib`
- Bibliotecas do Windows/console: `windows.h`, `conio.h`

## Funcionalidades Implementadas

- Menu inicial com opções:
  - Jogar
  - Sobre
  - Sair
- Mapa com dimensões fixas (`19 x 21`)
- Geração procedural do cenário:
  - Paredes sólidas nas bordas e em blocos internos
  - Paredes frágeis distribuídas aleatoriamente
- Áreas iniciais livres para jogador e inimigos
- Jogador com movimentação por teclado (`WASD` e setas)
- Posicionamento de bomba (uma bomba ativa por vez)
- Sistema de contagem regressiva da bomba
- Explosão em cruz (centro + 4 direções)
- Destruição de paredes frágeis pela explosão
- Inimigos com movimentação automática aleatória
- Eliminação de inimigos pela explosão
- Condições de fim de jogo:
  - Vitória: todos os inimigos eliminados
  - Derrota: jogador atingido por inimigo ou explosão
- Tela de informações (HUD) com controles

## Controles

- `W`, `A`, `S`, `D` ou setas: movimentar o jogador
- `Espaço`: plantar bomba
- `Esc`: sair do jogo

## Regras Principais

- O jogador não atravessa paredes sólidas, paredes frágeis nem bomba ativa.
- A explosão não atravessa paredes sólidas.
- Ao atingir parede frágil, a explosão a destrói.
- Inimigos não atravessam paredes e não entram na posição da bomba ativa.

## Como Compilar e Executar (Windows)

### Usando g++ (MinGW)

1. Abra o terminal na pasta do projeto.
2. Compile:

```bash
g++ cod2.cpp -o bomberman.exe
```

3. Execute:

```bash
./bomberman.exe
```

## Estrutura do Projeto

- `cod2.cpp`: código-fonte principal do jogo
- `README.md`: documentação do projeto

## Observações

- O jogo foi desenvolvido para ambiente Windows devido ao uso de `windows.h` e `conio.h`.
- A saída do console utiliza UTF-8 para exibir emojis dos elementos do jogo.

## Licença

Projeto acadêmico para fins educacionais.
