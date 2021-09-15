#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Tabuleiro
#define TAM_TABULEIRO 10 // Tamanho pode ser alterado
#define MAPA_VAZIO '.'
#define MAPA_AGUA 'o'
#define MAPA_NAVIO 'X'

// Peças
#define CARRIER_SIZE 5
#define BATTLESHIP_SIZE 4
#define CRUISER_SIZE 3
#define SUBMARINE_SIZE 3
#define DESTROYER_SIZE 2

// Orientação
#define VERTICAL 0
#define HORIZONTAL 1

typedef struct{
	char simbolo;					// Tipo de estrutura de dado que 
	int EhNavio;					// se refere a cada cada do tabuleiro 
	int FoiAlvejado;				// individualmente.
	int IdDoNavio;					//
	int tipoDeNavio;				//
}tCasa;
typedef struct{
	int id, orientacao, coluna;		// Posicao dos navios na inicializacao
	char linha;
}tPosicao;
typedef struct{
	tCasa posicao[TAM_TABULEIRO][TAM_TABULEIRO]; // Caracteristicas do tabuleiro
	int EhValido;
	int numCarrier;
	int numBattleship;		//Numero de cada tipo de navio
	int numCruiser;
	int numSubmarine;
	int numDestroyer;
}tTabuleiro;
typedef struct{
	int id;
	tPosicao posicaoInicial;
	char enderecoTabuleiro[1001];
	FILE * arquivoTabuleiro;
	char nome[1001];
	char jogadasLinha[101];		//Armazenam as jogadas
	int jogadasColuna[101];		//
	int JogadorVenceu;
	tTabuleiro campoJogador;
}tJogador;
tTabuleiro InicializaMapa(tTabuleiro mapa);
tTabuleiro InicializaTabuleiro(tJogador jogador);
tJogador InicializaJogador(tJogador jogador);
int reconhecePeca(char * peca);
int EhPossivelcolocarPeca(int peca, tJogador jogador);
int VaiExtrapolarLimites(tPosicao posicao, int tamanhoPeca);
int TemNavioProx(tTabuleiro mapa, tPosicao posicao, int tamanhoPeca);
tTabuleiro ColocaPeca(int peca, tPosicao posicao, tTabuleiro mapa);
void printaTabuleiro(tTabuleiro mapa);
int criaArquivoValidacao(char * endereco, tTabuleiro campo1, tTabuleiro campo2);
int saoCompativeis(tTabuleiro campo1, tTabuleiro campo2);
void criaArquivoInicializacao(char * endereco, tJogador jogador1, tJogador jogador2);
void printaTabuleiroNoArquivo(FILE * arquivoInicializacao, tTabuleiro mapa);
void realizaJogo(tJogador jogador1, tJogador jogador2, char * endereco);
int todosOsNaviosAfundaram(tTabuleiro mapa);
int EhJogadaValida(int linha, int coluna, tTabuleiro mapa);
int destruiuPeca(tTabuleiro mapa, int id);
tTabuleiro realizaJogada(int linha, int coluna, tTabuleiro mapa, char * nome);
int CodigoValido(char * jogada);
int TransformaJogadaColuna(char * jogada);
tTabuleiro jogaJogador(char * nomeJogadorAtacante, tTabuleiro campoJogadorAtacado, char * JogadaLinha, int * JogadaColuna);
int CasaExtrapolouTabuleiro(int linha, int coluna);
void salvarJogada(char linha, int coluna, char * jogadaLinha, int * jogadaColuna);
void CriaArquivoResultado(tJogador jogador1, tJogador jogador2, char * endereco, tTabuleiro campoJogador1, tTabuleiro campoJogador2);
int main(int argc, char * argv[]){
	tJogador jogador1; tJogador jogador2;
	jogador1.id=1; jogador2.id=2;
	if(argc<=1){
		printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
		return 0;
	}
	sprintf(jogador1.enderecoTabuleiro, "%s/tabu_1.txt", argv[1]);
	sprintf(jogador2.enderecoTabuleiro, "%s/tabu_2.txt", argv[1]);
	jogador1.campoJogador = InicializaTabuleiro(jogador1);
	jogador2.campoJogador = InicializaTabuleiro(jogador2);
	if(criaArquivoValidacao(argv[1], jogador1.campoJogador, jogador2.campoJogador)){  //Retorna 1 caso as condicoes estejam aptas para o prosseguimento do jogo
		jogador1=InicializaJogador(jogador1);
		jogador2=InicializaJogador(jogador2);
		printf("\n");
		criaArquivoInicializacao(argv[1], jogador1, jogador2);
	}
	else{
		return 0;
	}
	realizaJogo(jogador1, jogador2, argv[1]);
	return 0;
}
tTabuleiro InicializaMapa(tTabuleiro mapa){
	int i, j;
	for(i=0;i<=TAM_TABULEIRO;i++){
		for(j=0;j<=TAM_TABULEIRO;j++){
			mapa.posicao[i][j].simbolo=MAPA_AGUA;
			mapa.posicao[i][j].EhNavio=0;
			mapa.posicao[i][j].FoiAlvejado=0;
			mapa.posicao[i][j].IdDoNavio=0;
			mapa.posicao[i][j].tipoDeNavio=0;
		}
	}
	mapa.numBattleship=0; mapa.numCarrier=0; mapa.numCruiser=0; mapa.numDestroyer=0; mapa.numSubmarine=0;
	return mapa;
}
tTabuleiro InicializaTabuleiro(tJogador jogador){
	jogador.campoJogador=InicializaMapa(jogador.campoJogador);
	int ErroDeInicializacao = 0, contNavios=0;
	char tipoDePeca [11]="\0"; // nenhuma pe�a tem mais que 10 caracteres
	int peca=0;
	jogador.arquivoTabuleiro = fopen(jogador.enderecoTabuleiro, "r");
	if(!jogador.arquivoTabuleiro){
		printf("Nao foi possivel encontrar o arquivo %s\n", jogador.enderecoTabuleiro);
		exit(0);
	}
	while(!feof(jogador.arquivoTabuleiro)){
		fscanf(jogador.arquivoTabuleiro, "%[a-zA-Z];%d;%d;%c%d", &tipoDePeca, &jogador.posicaoInicial.id
										, &jogador.posicaoInicial.orientacao, &jogador.posicaoInicial.linha
										, &jogador.posicaoInicial.coluna);
		jogador.posicaoInicial.coluna-=1;	//para adequar o valor da coluna em relação à matriz (comeca do 0,e  nao do 1)
		fscanf(jogador.arquivoTabuleiro, "%*[\r\n]");
		peca=reconhecePeca(tipoDePeca); 
		switch(peca){
			case 1:
				jogador.campoJogador.numCarrier++;
				break;
			case 2:
				jogador.campoJogador.numBattleship++;
				break;
			case 3:
				jogador.campoJogador.numCruiser++;
				break;
			case 4:
				jogador.campoJogador.numSubmarine++;
				break;
			case 5:
				jogador.campoJogador.numDestroyer++;
				break;
			default:
				ErroDeInicializacao=1;
				break;
		}
		if(EhPossivelcolocarPeca(peca, jogador)){	//retorna zero caso nao seja poss�vel colocar a pe�a
			jogador.campoJogador=ColocaPeca(peca, jogador.posicaoInicial, jogador.campoJogador);
		}
		else{
			ErroDeInicializacao=1;
		}
		contNavios++;
	}
	if(contNavios==0){
		ErroDeInicializacao=1;
	}
	if(ErroDeInicializacao){
		jogador.campoJogador.EhValido=0;
	}
	else{
		jogador.campoJogador.EhValido=1;
	}
	fclose(jogador.arquivoTabuleiro);
	return jogador.campoJogador;
}
tJogador InicializaJogador(tJogador jogador){
	int i;
	printf("Nome do Jogador %d:\n", jogador.id);
	for(i=0;i<101;i++){
		jogador.jogadasColuna[i]=0;
		jogador.jogadasLinha[i]='\0';
	}
	jogador.JogadorVenceu=0;
	scanf("%s", &jogador.nome);
	return jogador;
}
int reconhecePeca(char * peca){
	if(!strcmp(peca, "Carrier")){
		return 1;
	}
	else if(!strcmp(peca, "Battleship")){
		return 2;
	}
	else if(!strcmp(peca,"Cruiser")){
		return 3;
	}
	else if(!strcmp(peca, "Submarine")){
		return 4;
	}
	else if(!strcmp(peca,"Destroyer")){
		return 5;
	}
	else{
		return -1;
	}
}
int VaiExtrapolarLimites(tPosicao posicao, int tamanhoPeca){
	if(posicao.orientacao==HORIZONTAL){
		if(posicao.linha-'a'>=0 && posicao.linha-'a'<TAM_TABULEIRO){					//checa se ambas as extremidades est�o
			if(posicao.coluna>=0 && posicao.coluna+tamanhoPeca<=TAM_TABULEIRO){ 			//nos limites do tabuleiro. Se extiverem,
				return 0;																//consequentemente todo o navio est�.
			}																			
			else{
				return 1;
			}
		}
		else{
			return 1;
		}
	}
	else if(posicao.orientacao==VERTICAL){
		if(posicao.coluna>=0 && posicao.coluna<TAM_TABULEIRO){
			if(posicao.linha-'a'>=0 && posicao.linha-'a'+tamanhoPeca<=TAM_TABULEIRO){
				return 0;
			}
			else{
				return 1;
			}
		}
		else{
			return 1;
		}
	}
}
int TemNavioProx(tTabuleiro mapa, tPosicao posicao, int tamanhoPeca){
	int i;
	if(posicao.orientacao==VERTICAL){
		for(i=0;i<tamanhoPeca;i++){
			if( (mapa.posicao[posicao.linha-'a'+i][posicao.coluna].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a'+i, posicao.coluna)  ==0) ||		// Caso a posicao em questao extrapole os limites da matriz, entao estariamos analisando numeros que nao foram inicializados. 
				(mapa.posicao[posicao.linha-'a'+i][posicao.coluna+1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+i, posicao.coluna+1)==0)||		// Por isso, devemos checar se cada posicao nao foge os limites. 
				(mapa.posicao[posicao.linha-'a'+i][posicao.coluna-1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+i, posicao.coluna-1)==0) )  { 	// Se fugir, entao logicamente nao tem navio proximo naquela posicao, afinal, ela nem faz parte do tabuleiro
																																							// Checa se as posiçoes vizinhas 
				return 1;																																	// de cada parte do navio a ser inserido
			}																																				// nao esta colada em nenhum navio
			if(i==0){
				if( (mapa.posicao[posicao.linha-'a'-1][posicao.coluna].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna)  ==0) ||	
					(mapa.posicao[posicao.linha-'a'-1][posicao.coluna-1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna-1)==0) ||	
					(mapa.posicao[posicao.linha-'a'-1][posicao.coluna+1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna+1)==0)  )	{	
					return 1;
				}
			}
			else if(i==tamanhoPeca-1){
				if( (mapa.posicao[posicao.linha-'a'+i+1][posicao.coluna].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a'+i+1, posicao.coluna)  ==0) ||		
					(mapa.posicao[posicao.linha-'a'+i+1][posicao.coluna-1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+i+1, posicao.coluna-1)==0) ||	
					(mapa.posicao[posicao.linha-'a'+i+1][posicao.coluna+1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+i+1, posicao.coluna+1)==0)  )	{	
					return 1;
				}
			}
		}
	}
	else if(posicao.orientacao==HORIZONTAL){
		for(i=0;i<tamanhoPeca;i++){
			if( (mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a', posicao.coluna+i)  ==0) ||
				(mapa.posicao[posicao.linha-'a'+1][posicao.coluna+i].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+1, posicao.coluna+i)==0) ||	
				(mapa.posicao[posicao.linha-'a'-1][posicao.coluna+i].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna+i)==0)  )	{	
																				
				return 1;
			}
			if(i==0){
				if( (mapa.posicao[posicao.linha-'a'][posicao.coluna-1].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a', posicao.coluna-1)  ==0) ||	
					(mapa.posicao[posicao.linha-'a'+1][posicao.coluna-1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+1, posicao.coluna-1)==0) ||	
					(mapa.posicao[posicao.linha-'a'-1][posicao.coluna-1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna-1)==0)  )	{
					return 1;
				}
			}
			else if(i==tamanhoPeca-1){
				if( (mapa.posicao[posicao.linha-'a'][posicao.coluna+i+1].EhNavio==1   && CasaExtrapolouTabuleiro(posicao.linha-'a', posicao.coluna+i+1)  ==0) ||		
					(mapa.posicao[posicao.linha-1-'a'][posicao.coluna+i+1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'-1, posicao.coluna+i+1)==0) ||	
					(mapa.posicao[posicao.linha+1-'a'][posicao.coluna+i+1].EhNavio==1 && CasaExtrapolouTabuleiro(posicao.linha-'a'+1, posicao.coluna+i+1)==0)  )	{	
					return 1;
				}
			}
		}
	}
	return 0;
}
int EhPossivelcolocarPeca(int peca, tJogador jogador){
	switch(peca){
		case 1:
			if(!(VaiExtrapolarLimites(jogador.posicaoInicial, CARRIER_SIZE))){
				if(!(TemNavioProx(jogador.campoJogador, jogador.posicaoInicial, CARRIER_SIZE))){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
			break;
		case 2:
			if(!(VaiExtrapolarLimites(jogador.posicaoInicial, BATTLESHIP_SIZE))){
				if(!(TemNavioProx(jogador.campoJogador, jogador.posicaoInicial, BATTLESHIP_SIZE))){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
			break;
		case 3:
			if(!(VaiExtrapolarLimites(jogador.posicaoInicial, CRUISER_SIZE))){
				if(!(TemNavioProx(jogador.campoJogador, jogador.posicaoInicial, CRUISER_SIZE))){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
			break;
		case 4:
			if(!(VaiExtrapolarLimites(jogador.posicaoInicial, SUBMARINE_SIZE))){
				if(!(TemNavioProx(jogador.campoJogador, jogador.posicaoInicial, SUBMARINE_SIZE))){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
			break;
		case 5:
			if(!(VaiExtrapolarLimites(jogador.posicaoInicial, DESTROYER_SIZE))){
				if(!(TemNavioProx(jogador.campoJogador, jogador.posicaoInicial, DESTROYER_SIZE))){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
			break;
	}
}
tTabuleiro ColocaPeca(int peca, tPosicao posicao, tTabuleiro mapa){
	int i=0;
	switch (peca)
	{
		case 1:
			if(posicao.orientacao==VERTICAL){
				for(i=0;i<CARRIER_SIZE;i++){
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].EhNavio=1;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].tipoDeNavio=peca;
				}
			}
			if(posicao.orientacao==HORIZONTAL){
				for(i=0;i<CARRIER_SIZE;i++){
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio=1;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].tipoDeNavio=peca;
				}
			}
			break;
		case 2:
			if(posicao.orientacao==VERTICAL){
				for(i=0;i<BATTLESHIP_SIZE;i++){
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].EhNavio=1;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].tipoDeNavio=peca;
				}
			}
			if(posicao.orientacao==HORIZONTAL){
				for(i=0;i<BATTLESHIP_SIZE;i++){
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio=1;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].tipoDeNavio=peca;
				}
			}
			break;
		case 3:
			if(posicao.orientacao==VERTICAL){
				for(i=0;i<CRUISER_SIZE;i++){
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].EhNavio=1;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].tipoDeNavio=peca;
				}
			}
			if(posicao.orientacao==HORIZONTAL){
				for(i=0;i<CRUISER_SIZE;i++){
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio=1;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].tipoDeNavio=peca;
				}
			}
			break;
		case 4:
			if(posicao.orientacao==VERTICAL){
				for(i=0;i<SUBMARINE_SIZE;i++){
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].EhNavio=1;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'+i][posicao.coluna].tipoDeNavio=peca;
				}
			}
			if(posicao.orientacao==HORIZONTAL){
				for(i=0;i<SUBMARINE_SIZE;i++){
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio=1;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].tipoDeNavio=peca;
				}
			}
			break;
		case 5:
			if(posicao.orientacao==VERTICAL){
				for(i=0;i<DESTROYER_SIZE;i++){
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].EhNavio=1;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha+i-'a'][posicao.coluna].tipoDeNavio=peca;
				}
			}
			if(posicao.orientacao==HORIZONTAL){
				for(i=0;i<DESTROYER_SIZE;i++){
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].simbolo=MAPA_NAVIO;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].EhNavio=1;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].IdDoNavio=posicao.id;
					mapa.posicao[posicao.linha-'a'][posicao.coluna+i].tipoDeNavio=peca;
				}
			}
			break;
	}
	return mapa;
}
void printaTabuleiro(tTabuleiro mapa){
	int i, j;
	for(i=0;i<TAM_TABULEIRO;i++){
		for(j=0;j<TAM_TABULEIRO;j++){
			if(mapa.posicao[i][j].FoiAlvejado==1){
				printf("%c", mapa.posicao[i][j].simbolo);
				if(j!=TAM_TABULEIRO-1){
					printf(" ");
				}
			}
			else{
				printf("%c", MAPA_VAZIO);
				if(j!=TAM_TABULEIRO-1){
					printf(" ");
				}
			}
		}
		printf("\n");
	}
	printf("\n");
}
int criaArquivoValidacao(char * endereco, tTabuleiro campo1, tTabuleiro campo2){
	char enderecoValidacao [1001];
	sprintf(enderecoValidacao, "%s/saida/validacao_tabuleiros.txt", endereco);
	FILE * validacaoTabuleiros = fopen(enderecoValidacao, "w");
	fprintf(validacaoTabuleiros, "tabu_1.txt;");
	if(campo1.EhValido==1){
		fprintf(validacaoTabuleiros, "valido\n");
	}
	else{
		fprintf(validacaoTabuleiros, "invalido\n");
	}
	fprintf(validacaoTabuleiros, "tabu_2.txt;");
	if(campo2.EhValido==1){
		fprintf(validacaoTabuleiros, "valido");
	}
	else{
		fprintf(validacaoTabuleiros, "invalido");
	}
	if(campo1.EhValido==1 && campo2.EhValido==1){
		fprintf(validacaoTabuleiros, "\n");
		if(saoCompativeis(campo1, campo2)){
			fprintf(validacaoTabuleiros, "Tabuleiros compativeis entre si");
			fclose(validacaoTabuleiros);
			return 1;
		}
		else{
			fprintf(validacaoTabuleiros, "Tabuleiros incompativeis entre si");
			fclose(validacaoTabuleiros);
			return 0;
		}
	}
	else{
		fclose(validacaoTabuleiros);
		return 0;
	}
}
int saoCompativeis(tTabuleiro campo1, tTabuleiro campo2){
	if(campo1.numBattleship == campo2.numBattleship &&
	   campo1.numCarrier == campo2.numCarrier &&
	   campo1.numCruiser == campo2.numCruiser &&
	   campo1.numDestroyer == campo2.numDestroyer &&
	   campo1.numSubmarine == campo2.numSubmarine){
		return 1;   
	}
	else{
		return 0;
	}
}
void printaTabuleiroNoArquivo(FILE * arquivoInicializacao, tTabuleiro mapa){
	int i, j;
	for(i=0;i<TAM_TABULEIRO;i++){
		for(j=0;j<TAM_TABULEIRO;j++){
			fprintf(arquivoInicializacao, "%c", mapa.posicao[i][j].simbolo);
			if(j!=TAM_TABULEIRO-1){
				fprintf(arquivoInicializacao, " ");
			}
		}
		if(i!=TAM_TABULEIRO-1){
			fprintf(arquivoInicializacao, "\n");
		}
	}
}
void criaArquivoInicializacao(char * endereco, tJogador jogador1, tJogador jogador2){
	char enderecoInicializacao [1001];
	sprintf(enderecoInicializacao, "%s/saida/inicializacao.txt", endereco);
	FILE * arquivoInicializacao = fopen(enderecoInicializacao, "w");
	fprintf(arquivoInicializacao, "%s\n", &jogador1.nome);
	printaTabuleiroNoArquivo(arquivoInicializacao, jogador1.campoJogador);
	fprintf(arquivoInicializacao, "\n\n");
	fprintf(arquivoInicializacao, "%s\n", &jogador2.nome);
	printaTabuleiroNoArquivo(arquivoInicializacao, jogador2.campoJogador);
	fclose(arquivoInicializacao);
}
void realizaJogo(tJogador jogador1, tJogador jogador2, char * endereco){
	while(todosOsNaviosAfundaram(jogador1.campoJogador)==0 && todosOsNaviosAfundaram(jogador2.campoJogador)==0){
		jogador2.campoJogador=jogaJogador(jogador1.nome, jogador2.campoJogador, jogador1.jogadasLinha, jogador1.jogadasColuna);
		printf("Tabuleiro atual de %s apos a jogada de %s\n", &jogador2.nome, &jogador1.nome);
		printaTabuleiro(jogador2.campoJogador);
		jogador1.campoJogador=jogaJogador(jogador2.nome, jogador1.campoJogador, jogador2.jogadasLinha, jogador2.jogadasColuna);
		printf("Tabuleiro atual de %s apos a jogada de %s\n", &jogador1.nome, &jogador2.nome);
		printaTabuleiro(jogador1.campoJogador);
	}
	if(todosOsNaviosAfundaram(jogador1.campoJogador)==1 && todosOsNaviosAfundaram(jogador2.campoJogador)==1){
		printf("Empate\n");
	}
	else if(todosOsNaviosAfundaram(jogador1.campoJogador)==1){
		printf("Vencedor: %s\n", &jogador2.nome);
		jogador2.JogadorVenceu=1;
	}
	else{
		printf("Vencedor: %s\n", &jogador1.nome);
		jogador1.JogadorVenceu=1;
	}
	CriaArquivoResultado(jogador1, jogador2, endereco, jogador1.campoJogador, jogador2.campoJogador);
}
int todosOsNaviosAfundaram(tTabuleiro mapa){
	int i, j;
	for(i=0;i<TAM_TABULEIRO;i++){
		for(j=0;j<TAM_TABULEIRO;j++){
			if(mapa.posicao[i][j].EhNavio==1 && mapa.posicao[i][j].FoiAlvejado==0){
				return 0;
			}
		}
	}
	return 1;
}
int EhJogadaValida(int linha, int coluna, tTabuleiro mapa){
	if(linha>=0 && linha<TAM_TABULEIRO && coluna>=0 && coluna<TAM_TABULEIRO){	//Verifica se a jogada está nos limites do tabuleiro.
		if(mapa.posicao[linha][coluna].FoiAlvejado==0){ 							//Verifica se a jogada já foi feita antes.
			return 1;												   
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}
}
tTabuleiro realizaJogada(int linha, int coluna, tTabuleiro mapa, char * nome){
	char linhaChar = linha+'a';
	mapa.posicao[linha][coluna].FoiAlvejado=1;
	if(mapa.posicao[linha][coluna].EhNavio==1){
		mapa.posicao[linha][coluna].simbolo='X';
		if(destruiuPeca(mapa, mapa.posicao[linha][coluna].IdDoNavio)){
			switch (mapa.posicao[linha][coluna].tipoDeNavio)
			{
			case 1:
				printf("%c%d:Afundou Carrier\n", linhaChar, coluna+1);
				printf("\n");
				break;
			case 2:
				printf("%c%d:Afundou Battleship\n", linhaChar, coluna+1);
				printf("\n");
				break;
			case 3:
				printf("%c%d:Afundou Cruiser\n", linhaChar, coluna+1);
				printf("\n");
				break;
			case 4:
				printf("%c%d:Afundou Submarine\n", linhaChar, coluna+1);
				printf("\n");
				break;
			case 5:
				printf("%c%d:Afundou Destroyer\n", linhaChar, coluna+1);
				printf("\n");
				break;
			}
		}
		else{
			printf("%c%d:Tiro!\n", linhaChar, coluna+1);
			printf("\n");
		}
	}
	else{
		printf("%c%d:Agua\n", linhaChar, coluna+1);
		printf("\n");
		mapa.posicao[linha][coluna].simbolo='o';
	}
	return mapa;
}
int destruiuPeca(tTabuleiro mapa, int id){
	int i, j;
	for(i=0;i<TAM_TABULEIRO;i++){
		for(j=0;j<TAM_TABULEIRO;j++){
			if(mapa.posicao[i][j].IdDoNavio==id){
				if(mapa.posicao[i][j].FoiAlvejado==0){
					return 0;
				}
			}
		}
	}
	return 1;
}
int CodigoValido(char * jogada){
	int i;
	if(jogada[0]>='a' && jogada[0]<= 'z'){
		for(i=1;jogada[i]!='\0';i++){
			if(jogada[i]<'0' || jogada[i]>'9'){
				return 0;
			}
		}
		return 1;
	}
	else{
		return 0;
	}
}
int TransformaJogadaColuna(char * jogada){
	int coluna=0, i, num[10], j;
	for(i=1;jogada[i]!='\0';i++){
		num[i-1]=jogada[i]-'0';				// Transforma uma matriz do tipo:
	}										// [2, 3, 5, 1] 
	for(j=0;j<i-1;j++){						// em:
		coluna+=num[j]*pow(10, i-2-j);		// 2000 + 300 + 50 + 1 = 2351
	}
	return coluna;
}
tTabuleiro jogaJogador(char * nomeJogadorAtacante, tTabuleiro campoJogadorAtacado, char * JogadaLinha, int * JogadaColuna){
	int EhValida=1;
	char jogada[10];
	printf("Jogada de %s:\n", nomeJogadorAtacante);
		do{
			scanf("%s", &jogada);
			printf("\n");
			if(CodigoValido(jogada)){
				int linha = jogada[0]-'a';								//jogada[0]-'a' serve para transformar o valor ascii do caractere
				int coluna = TransformaJogadaColuna(jogada)-1;			//referente a linha lido no valor int correto.
				salvarJogada(jogada[0], coluna, JogadaLinha, JogadaColuna);
				if(EhJogadaValida(linha, coluna, campoJogadorAtacado)){
					campoJogadorAtacado=realizaJogada(linha, coluna, campoJogadorAtacado, nomeJogadorAtacante);
					EhValida=1;
				}
				else{
					printf("%s:Jogada invalida! Jogue novamente %s:\n", jogada, nomeJogadorAtacante);
					EhValida=0;
				}
			}
			else{
				printf("%s:Jogada invalida! Jogue novamente %s:\n", jogada, nomeJogadorAtacante);
				EhValida=0;
			}
		}while(EhValida==0);
	return campoJogadorAtacado; 
}
int CasaExtrapolouTabuleiro(int linha, int coluna){
	if(linha>=0 && linha<TAM_TABULEIRO){
		if(coluna>=0 && coluna<TAM_TABULEIRO){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 1;
	}
}
void salvarJogada(char linha, int coluna, char * jogadaLinha, int * jogadaColuna){
	int i;
	for(i=0;i<101;i++){
		if(jogadaLinha[i]=='\0'){
			jogadaLinha[i]=linha;
			break;
		}
	}
	for(i=0;i<101;i++){
		if(jogadaColuna[i]==0){
			jogadaColuna[i]=coluna+1;
			break;
		}
	}
}
void CriaArquivoResultado(tJogador jogador1, tJogador jogador2, char * endereco, tTabuleiro campoJogador1, tTabuleiro campoJogador2){
	int i, j;
	char enderecoResultado [1001];
	sprintf(enderecoResultado, "%s/saida/resultado.txt", endereco);
	FILE * resultado = fopen(enderecoResultado, "w");
	fprintf(resultado, "%s\n", jogador1.nome);
	for(i=0; jogador1.jogadasColuna[i]!=0;i++){
		fprintf(resultado, "%c%d - ", jogador1.jogadasLinha[i], jogador1.jogadasColuna[i]);
		if(campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].EhNavio==1){
			switch(campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].tipoDeNavio){
				case 1:
					fprintf(resultado, "Tiro - Carrier - ID %02d\n", campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].IdDoNavio);
					break;
				case 2:
					fprintf(resultado, "Tiro - Battleship - ID %02d\n", campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].IdDoNavio);
					break;
				case 3:
					fprintf(resultado, "Tiro - Cruiser - ID %02d\n", campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].IdDoNavio);
					break;
				case 4:
					fprintf(resultado, "Tiro - Submarine - ID %02d\n", campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].IdDoNavio);
					break;
				case 5:
					fprintf(resultado, "Tiro - Destroyer - ID %02d\n", campoJogador2.posicao[jogador1.jogadasLinha[i]-'a'][jogador1.jogadasColuna[i]-1].IdDoNavio);
					break;
			}
		}
		else{
			fprintf(resultado, "Agua\n");
		}
	}
	fprintf(resultado, "\n");
	fprintf(resultado, "%s\n", jogador2.nome);
	for(j=0; j<i;j++){
		fprintf(resultado, "%c%d - ", jogador2.jogadasLinha[j], jogador2.jogadasColuna[j]);
		if(campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].EhNavio==1){
			switch(campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].tipoDeNavio){
				case 1:
					fprintf(resultado, "Tiro - Carrier - ID %02d\n", campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].IdDoNavio);
					break;
				case 2:
					fprintf(resultado, "Tiro - Battleship - ID %02d\n", campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].IdDoNavio);
					break;
				case 3:
					fprintf(resultado, "Tiro - Cruiser - ID %02d\n", campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].IdDoNavio);
					break;
				case 4:
					fprintf(resultado, "Tiro - Submarine - ID %02d\n", campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].IdDoNavio);
					break;
				case 5:
					fprintf(resultado, "Tiro - Destroyer - ID %02d\n", campoJogador1.posicao[jogador2.jogadasLinha[j]-'a'][jogador2.jogadasColuna[j]-1].IdDoNavio);
					break;
			}
		}
		else{
			fprintf(resultado, "Agua\n");
		}
	}
	fprintf(resultado, "\n");
	if(jogador1.JogadorVenceu==1){
		fprintf(resultado, "Vencedor: %s", jogador1.nome);
	}
	else if(jogador2.JogadorVenceu==1){
		fprintf(resultado, "Vencedor: %s", jogador2.nome);
	}
	else{
		fprintf(resultado, "Empate");
	}
	fclose(resultado);
}