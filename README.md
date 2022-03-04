# BatalhaNaval

Uma pequena batalha naval feita inteiramente em C.
Para que o jogo funcione, é necessário que existam dois arquivos de texto de inicialização de tabuleiros,
chamados de 'tabu1.txt' e 'tabu2.txt'. Nesses arquivos, os navios do jogador devem ser inicializados da seguinte forma:

Tipo de navio;id do navio;posicao;coordenada inicial

Sendo o tipo do navio com a primeira letra maiúscula, a posição sendo 0 para vertical e 1 para horizontal, e a coordenada inicial
no formato linha + coluna. A linha varia de 'a' até 'j' e a coluna de '1' até '10'

Exemplo:

$tabu1.txt

Carrier;1;0;e
Battleship;2;1;a1
Battleship;3;0;b10
Submarine;4;1;e5
Cruiser;6;0;a8
Destroyer;7;1;c2
Destroyer;8;0;i5
Destroyer;9;0;a6
Destroyer;10;1;g6

O diretório em questão deve ser informado ao rodar o programa

A entrada e a saída padrão são pelo terminal, mas podem ser redirecionadas.
