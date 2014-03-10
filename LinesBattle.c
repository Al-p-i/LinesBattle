#include<stdio.h>
#include<time.h>
#include<windows.h>

/**ÊÎÍÑÒÀÍÒÛ*/
//ÖÂÅÒÀ
const WORD WHITE  = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
const WORD RED    = FOREGROUND_INTENSITY | FOREGROUND_RED;
const WORD GREEN  = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
const WORD BLUE   = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
const WORD YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
const WORD PURPLE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
const WORD AQUA   = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;

//ÐÅÆÈÌ Ñ ÎÒÊÐÛÒÎÉ ÊÎËÎÄÎÉ (âèäíû âñå êàðòû)
const bool IS_OPEN_PACK=false;

/**ÏÅÐÅÌÅÍÍÛÅ È ÑÒÐÓÊÒÓÐÛ*/
//ÈÃÐÎÂÎÅ ÏÎËÅ
struct Field{
	int width, height, size;
};

//ÈÃÐÎÊ(ïðèíàäëåæíîñòü ÿ÷åéêè îäíîìó äðóãîìó èëè íèêîìó)
enum Player{blue,red,none};

//ß×ÅÉÊÀ
struct Cells{
	int value;
	Player player;
};

//ÍÎÌÅÐ ÕÎÄÀ
int turnNumber=0;

//ÈÃÐÎÂÎÅ ÑÎÑÒÎßÍÈÅ
enum GameState{blueTurn,redTurn,blueWin,redWin};

//ÂÛÁÐÀÍÍÀß ÏÎÇÈÖÈß
int position=0;//TODO global

//ÂÛÁÐÀÍÍÀß ÊÀÐÒÀ
int card=0;//TODO global

//ÏÅÐÅÌÅÍÍÛÅ ÄËß ÊÓÐÑÎÐÀ
HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
COORD cursorPos;
COORD prevCursorPos;

/**ÍÀ×ÀËÍÛÅ ÄÅÉÑÒÂÈß*/
int fillPack(Field,int*, int*);		//íà÷àëüíîå çàïîëíåíèå êîëîäû

/**ÎÒÐÈÑÎÂÊÀ*/
void drawPack(Field, int*,int);			//êîëîäû
int drawField(Field, Cells*);			//ïîëÿ
int drawHands(int*, Field, int, BYTE);	//êàðòû èãðîêîâ
int drawArrow(GameState, Field, Cells*);//ñòðåëêà
void drawStartFieldElements(Field);		//ñòàðòîâûå ýëåìåíòû (çâ¸çäî÷êè)

/**ÊÎËÎÄÀ*/
int depack(Field,int*,int);			//èçâëå÷ü ñ âåðøèíû êîëîäû
void enpack(int,Field, int*, int);	//äîáàâèòü âíèç êîëîäû
bool isPackEmpty(int*);				//ïóñòà ëè êîëîäà

/**ÈÃÐÎÂÀß ÌÅÕÀÍÈÊÀ*/
bool isOwned(int, Player, Field, Cells*);	//åñòü ëè ó èãðîêà çàïðàøèâàåìàÿ êàðòà
int getCardNumber(int,int*,int);			//íàéòè íîìåð êàðòû â ðóêå\
bool isPlayerSumMoreThanEnemySum(GameState,int,Field,Cells*);//ñðàâíèòü ñóììû
int turn(GameState*,Field, int, int*, int*, Cells*, int);//ÎÒÂÅ×ÀÅÒ ÇÀ ÏÎÑËÅÄÎÂÀÒÅËÜÍÎÑÒÜ ÄÅÉÑÒÂÈÉ ÂÎ ÂÐÅÌß ÕÎÄÀ

/**ÂÑÏÎÌÎÃÀÒÅËÜÍÛÅ*/
void clearLine(Field);


void drawPack(Field field, int *pack,int pack_size){
	cursorPos.X = 1;
	cursorPos.Y = 5+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	int lastCard=-1;
	for(int i=0;i<pack_size;i++){
		if(pack[i]>=0){
			printf("[");
		}else{
			if(lastCard<0){
				lastCard=i;
			}
			printf(" ");
		}
		if(IS_OPEN_PACK){
			printf("%d",pack[i]);
		}
	}
	if(pack[0]>=0&&!IS_OPEN_PACK){
		cursorPos.X = lastCard+1;
		cursorPos.Y = 5+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		printf("*]");
	}
}

void clearLine(Field field){
	cursorPos.X = 0;
	cursorPos.Y = 12+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("                                                  ");
	//TODO çàìåíèòü ñèìâîëîì ïåðåõîäà â íà÷àëî ñòðîêè
	cursorPos.X = 0;
	cursorPos.Y = 12+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
}

bool isPackEmpty(int *pack){
	if(pack[0]==-1){
		return true;
	}
	else{
		return false;
	}
}

int depack(Field field,int *pack,int pack_size){
	int var=0;
	for(int i=pack_size-1;i>=0;i--){
		if(pack[i]>=0){
			var=pack[i];
			pack[i]=-1;
			return var;
		}
	}
	return -1;
}

void enpack(int cardToEnpack,Field field, int *pack, int pack_size){
	for(int i=pack_size-1;i>0;i--){
		pack[i]=pack[i-1];
	}
	pack[0]=cardToEnpack;
}

void drawStartFieldElements(Field field){
	//ÎÒÐÈÑÎÂÊÀ ÇÂ¨ÇÄÎ×ÅÊ
	//TODO äîáàâèòü çàêðàøèâàíèå íà÷àëüíûõ ïîçèöèé
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
	cursorPos.X = 2;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("*");
	/*
	printf("*%3d",2*field.height+2*field.width);
	cursorPos.X = 2;
	cursorPos.Y = 3;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("1");
	*/

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
	cursorPos.X = 5+field.width*3;
	cursorPos.Y = 3+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("*");
	//TODO íà÷àëüíàÿ ðàñêðàñêà äîñòóïíûõ ïîçèöèé
	/*
	printf("%3d  *",field.height+field.width);
	cursorPos.X = 3+field.width*3;
	cursorPos.Y = 2+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("%3d",field.height+field.width+1);
	*/

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}

int drawField(Field field, Cells *cell){
	cursorPos.X = 3;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	for(int i=0;i<field.width;i++){
		switch(cell[i].player){
		case blue:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
			break;
		case red:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			break;
		case none:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
			break;
		}
		printf("%3d",2*field.width+2*field.height-i);
	}
	for(int i=0;i<field.height;i++){
		switch(cell[i*field.width].player){
		case blue:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
			break;
		case red:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			break;
		case none:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
			break;
		}
		printf("\n%3d",i+1);
		//printf("\n%3d",++i);
		for(int j=0;j<field.width;j++){
			switch(cell[i*field.width+j].player){
			case blue:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
				break;
			case red:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
				break;
			case none:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
				break;
			}
			printf("%3d",cell[i*field.width+j].value);
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		switch(cell[i*field.width+field.width-1].player){
		case blue:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
			break;
		case red:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			break;
		case none:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
			break;
		}
		printf("%3d",2*field.height+field.width-i);
	}
	printf("\n   ");
	for(int i=0;i<field.width;i++){
		switch(cell[(field.height-1)*field.width+i].player){
			case blue:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
				break;
			case red:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
				break;
			case none:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
				break;
			}
		printf("%3d",field.height+i+1);
	}
	printf("\n\n");
	return 0;
}

int drawHands(int *hand, Field field, int hand_size, BYTE COLOR){
	//TODO äîáàâèòü èìåíà èãðîêîâ ïåðåä êàðòàìè, ðàçäåëèòü êàðòû

	cursorPos.X = 0;
	if(COLOR == BLUE){
		cursorPos.Y = field.height+7;
	}else if(COLOR == RED){
		cursorPos.Y = field.height+9;
	}
	SetConsoleCursorPosition(consoleOutput, cursorPos);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR);
	for(int i=0;i<hand_size;i++){
		if(hand[i]>=0){
			printf("%3d",hand[i]);
		}else{
			printf("   ");
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	return 0;
}

bool isOwned(int pos, Player player, Field field, Cells *cell){
	//ÏÐÎÂÅÐÊÀ, ÂÎÇÌÎÆÅÍ ËÈ ÕÎÄ Ñ ÏÎÇÈÖÈÈ pos
	if(turnNumber==1&&((player==blue&&(pos==1||pos==2*field.height+2*field.width))||(player==red&&(pos==field.width+field.height||pos==field.width+field.height+1)))){
		return true;
	}
	//Player color=player;
	if(pos<=field.height){
		if(cell[(pos-1)*field.width].player==player){
			return true;
		}else{
			return false;
		}
	}else if(pos<=field.width+field.height){
		if(cell[(field.height-1)*field.width+pos-field.height-1].player==player){
			return true;
		}else{
			return false;
		}
	}else if(pos<=field.width+2*field.height){
		if(cell[(2*field.height+field.width-pos)*field.width+field.width-1].player==player){
			return true;
		}else{
			return false;
		}
	}else if(pos<=2*field.width+2*field.height){
		if(cell[2*field.width+2*field.height-pos].player==player){
			return true;
		}else{
			return false;
		}
	}else
		printf("invalid position(isOwned)");
		return false;
}

int drawArrow(GameState gameState, Field field, Cells *cell){
	//ÎÒÐÈÑÎÂÊÀ ÑÒÐÅËÎÊ ÏÐÈ ÂÛÁÎÐÅ
	cursorPos.X = prevCursorPos.X;
	cursorPos.Y = prevCursorPos.Y;
	SetConsoleCursorPosition(consoleOutput, prevCursorPos);
	printf(" ");
	Player player=blue;
	if(gameState==blueTurn){
		player=blue;
	}else if(gameState==redTurn){
		player=red;
	}
	if(position>0&&position<=field.height&&isOwned(position,player,field,cell)){
		cursorPos.X = 0;
		cursorPos.Y = position+2;
		prevCursorPos.X=cursorPos.X;
		prevCursorPos.Y=cursorPos.Y;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		if(gameState==blueTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
		}else if(gameState==redTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		}
		printf(">");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
	}else if(position>0&&position<=field.height+field.width&&isOwned(position,player,field,cell)){
		cursorPos.X = 2+3*(position-field.height);
		cursorPos.Y = field.height+4;
		prevCursorPos.X=cursorPos.X;
		prevCursorPos.Y=cursorPos.Y;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		if(gameState==blueTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
		}else if(gameState==redTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		}
		printf("^");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
	}else if(position>0&&position<=2*field.height+field.width&&isOwned(position,player,field,cell)){
		cursorPos.X = 6+3*field.width;
		cursorPos.Y = 3+2*field.height+field.width-position;
		prevCursorPos.X=cursorPos.X;
		prevCursorPos.Y=cursorPos.Y;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		if(gameState==blueTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
		}else if(gameState==redTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		}
		printf("<");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
	}else if(position>0&&position<=2*field.height+2*field.width&&isOwned(position,player,field,cell)){
		cursorPos.X = 5+3*field.width-3*(position-2*field.height-field.width);
		cursorPos.Y = 1;
		prevCursorPos.X=cursorPos.X;
		prevCursorPos.Y=cursorPos.Y;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		if(gameState==blueTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
		}else if(gameState==redTurn){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		}
		printf("v");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
	}else{
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		printf("choose another position: \n");
	}

	return 0;
}

int getCardNumber(int card,int *hand,int hand_size){
	//ÏÐÎÂÅÐÊÀ, ÅÑÒÜ ËÈ ÇÀÏÐÀØÈÂÀÅÌÀß ÊÀÐÒÀ Â ÐÓÊÅ È ÂÎÇÂÐÀÙÅÍÈÅ Å¨ ÈÍÄÅÊÑÀ Â ÐÓÊÅ
	for(int i=0;i<hand_size;i++){
		if((hand[i]>=0)&&(hand[i]==card)){
			return i;
		}
	}
	return -1;
}

bool isPlayerSumMoreThanEnemySum(GameState gameState,int card,Field field,Cells *cell){
	int playerSum=0,enemySum=0;
	bool isStackFinished=false;
	Player player, enemy;
	if(gameState==blueTurn){
		player=blue;
		enemy=red;
	}else if(gameState==redTurn){
		player=red;
		enemy=blue;
	}

	if(position<=field.height){
		for(int i=0;i<field.width;i++){
			if(cell[(position-1)*field.width+i].player==player&&!isStackFinished){
				playerSum+=cell[(position-1)*field.width+i].value;
			}else if(cell[(position-1)*field.width+i].player==enemy){
				enemySum+=cell[(position-1)*field.width+i].value;
				isStackFinished=true;
			}else break;
		}
	}else if(position<=field.height+field.width){
		for(int i=0;i<field.height;i++){
			if(cell[(field.height-1-i)*field.width+position-field.height-1].player==player&&!isStackFinished){
				playerSum+=cell[(field.height-1-i)*field.width+position-field.height-1].value;
			}else if(cell[(field.height-1-i)*field.width+position-field.height-1].player==enemy){
				enemySum+=cell[(field.height-1-i)*field.width+position-field.height-1].value;
				isStackFinished=true;
			}else break;
		}
	}else if(position<=2*field.height+field.width){
		for(int i=0;i<field.width;i++){
			if(cell[(2*field.height-position+field.width)*field.width+field.width-1-i].player==player&&!isStackFinished){
				playerSum+=cell[(2*field.height-position+field.width)*field.width+field.width-1-i].value;
			}else if(cell[(2*field.height-position+field.width)*field.width+field.width-1-i].player==enemy){
				enemySum+=cell[(2*field.height-position+field.width)*field.width+field.width-1-i].value;
				isStackFinished=true;
			}else break;
		}
	}else if(position<=2*field.height+2*field.width){
		for(int i=0;i<field.height;i++){
			if(cell[i*field.width+2*field.width-position+2*field.height].player==player&&!isStackFinished){
				playerSum+=cell[i*field.width+2*field.width-position+2*field.height].value;
			}else if(cell[i*field.width+2*field.width-position+2*field.height].player==enemy){
				enemySum+=cell[i*field.width+2*field.width-position+2*field.height].value;
				isStackFinished=true;
			}else break;
		}
	}else{
			printf("wrong position (getSumInLine)");
	}

	cursorPos.X = field.width*3+10;
	cursorPos.Y = 3;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("%s\t%2d","playerSum",playerSum+card);
	cursorPos.X = field.width*3+10;
	cursorPos.Y = 4;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("%s\t%2d","enemySum",enemySum);
	cursorPos.X = 0;
	cursorPos.Y = field.height+12;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	return (playerSum+card>enemySum);
}

int turn(GameState *gameState,Field field, int hand_size, int *hand, int *pack, Cells *cell, int pack_size){
	//ÃÐÎÌÎÇÄÊÀß ÔÓÍÊÖÈß, ÎÒÂÅ×ÀÞÙÀß ÇÀ ÕÎÄ
	if(*gameState==blueTurn){
		turnNumber++;
	}

	WORD COLOR;
	char *TAG;
	Player player;
	Player enemy;
	//int *hand = &hand[0];
	GameState playerWin;
	GameState nextTurn;
	int ENEMY_BASE_X, ENEMY_BASE_Y;

	if(*gameState==blueTurn){
		COLOR = BLUE;
		TAG = "BLUE";
		player = blue;
		enemy = red;
		playerWin = blueWin;
		nextTurn = redTurn;
		ENEMY_BASE_X = field.width - 1;
		ENEMY_BASE_Y = field.height - 1;

	}else if(*gameState==redTurn){
		COLOR = RED;
		TAG = "RED";
		player = red;
		enemy = blue;
		playerWin = redWin;
		nextTurn = blueTurn;
		ENEMY_BASE_X = 0;
		ENEMY_BASE_Y = 0;
	}

	cursorPos.X = 0;
	cursorPos.Y = 11+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR);
	printf(TAG);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf(" choses position:    \n");
	do{
		if(!scanf("%d",&position)){
			//TODO îáðàáîòêà ââîäà
			printf("invalid input");
		}
		cursorPos.X = 0;
		cursorPos.Y = 12+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		printf("                                                  ");
		//TODO çà÷èùåíèå ýêðàíà ïîñëå ââîäà áîëüøîãî êîëè÷åñòâà ñèìâîëîâ
		if(drawArrow(*gameState, field, cell)){
			printf("error: can not draw arrow");
		}
	}while(position<=0||(position>(2*field.height+2*field.width))||!isOwned(position,player,field,cell));

	cursorPos.X = 0;
	cursorPos.Y = 11+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR);
	printf(TAG);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf(" choses card:         \n");
	do{
		//TODO îòìåíà âûáîðà ïîçèöèè
		if(!scanf("%d",&card)){
			//TODO îáðàáîòêà ââîäà
			printf("invalid input");
		}
		clearLine(field);
	}while(getCardNumber(card,hand,hand_size)==-1||!isPlayerSumMoreThanEnemySum(*gameState, card, field, cell));
	//TODO äâàæäû èñïîëíÿåòñÿ getCardNumber

	int cardNumber=getCardNumber(card,hand,hand_size);
	//bool isStackFinished=false;
	int finishIndex=0;
	bool isEnemyHere=false;
	//int numOfEnemiesInLine=0;
	if(position<=field.height){
		for(finishIndex=0;finishIndex<field.width-1;finishIndex++){
			if(cell[(position-1)*field.width+finishIndex].player==player){
				if(/*numOfEnemiesInLine*/isEnemyHere){
					finishIndex--;
					break;
				}

			}else if(cell[(position-1)*field.width+finishIndex].player==enemy){
				//numOfEnemiesInLine++;
				isEnemyHere=true;
			}
			else if(cell[(position-1)*field.width+finishIndex].player==none){
				break;
			}
		}
		if(cell[(position-1)*field.width+finishIndex].player!=none){
			enpack(cell[(position-1)*field.width+finishIndex].value,field,pack,pack_size);
		}
		for(int i=finishIndex;i>0;i--){
			cell[(position-1)*field.width+i].value=cell[(position-1)*field.width+i-1].value;
			cell[(position-1)*field.width+i].player=cell[(position-1)*field.width+i-1].player;
		}
		cell[(position-1)*field.width].player=player;
		cell[(position-1)*field.width].value=hand[cardNumber];
	}else if(position<=field.height+field.width){
		for(finishIndex=0;finishIndex<field.height-1;finishIndex++){
			if(cell[(field.height-1-finishIndex)*field.width+position-field.height-1].player==player){
				if(isEnemyHere){
					finishIndex--;
					break;
				}
			}else if(cell[(field.height-1-finishIndex)*field.width+position-field.height-1].player==enemy){
				isEnemyHere=true;
			}
			else if(cell[(field.height-1-finishIndex)*field.width+position-field.height-1].player==none){
				break;
			}
		}
		if(cell[(field.height-1-finishIndex)*field.width+position-field.height-1].player!=none){
			enpack(cell[(field.height-1-finishIndex)*field.width+position-field.height-1].value,field,pack,pack_size);
		}
		for(int i=finishIndex;i>0;i--){
			cell[(field.height-1-i)*field.width+position-field.height-1].value=cell[(field.height-i)*field.width+position-field.height-1].value;
			cell[(field.height-1-i)*field.width+position-field.height-1].player=cell[(field.height-i)*field.width+position-field.height-1].player;
		}
		cell[(field.height-1)*field.width+position-field.height-1].player=player;
		cell[(field.height-1)*field.width+position-field.height-1].value=hand[cardNumber];
	}else if(position<=2*field.height+field.width){
		for(finishIndex=0;finishIndex<field.width-1;finishIndex++){
			if(cell[(2*field.height-position+field.width)*field.width+field.width-1-finishIndex].player==player){
				if(isEnemyHere){
					finishIndex--;
					break;
				}
			}else if(cell[(2*field.height-position+field.width)*field.width+field.width-1-finishIndex].player==enemy){
				isEnemyHere=true;
			}
			else if(cell[(2*field.height-position+field.width)*field.width+field.width-1-finishIndex].player==none){
				break;
			}
		}
		if(cell[(2*field.height-position+field.width)*field.width+field.width-1-finishIndex].player!=none){
			enpack(cell[(2*field.height-position+field.width)*field.width+field.width-1-finishIndex].value,field,pack,pack_size);
		}
		for(int i=finishIndex;i>0;i--){
			cell[(2*field.height-position+field.width)*field.width+field.width-1-i].value=cell[(2*field.height-position+field.width)*field.width+field.width-i].value;
			cell[(2*field.height-position+field.width)*field.width+field.width-1-i].player=cell[(2*field.height-position+field.width)*field.width+field.width-i].player;
		}
		cell[(2*field.height-position+field.width)*field.width+field.width-1].player=player;
		cell[(2*field.height-position+field.width)*field.width+field.width-1].value=hand[cardNumber];
	}else if(position<=2*field.height+2*field.width){
		for(finishIndex=0;finishIndex<field.width-1;finishIndex++){
			if(cell[finishIndex*field.width+2*field.width-position+2*field.height].player==player){
				if(isEnemyHere){
					finishIndex--;
					break;
				}
			}else if(cell[finishIndex*field.width+2*field.width-position+2*field.height].player==enemy){
				isEnemyHere=true;
			}
			else if(cell[finishIndex*field.width+2*field.width-position+2*field.height].player==none){
				break;
			}
		}
		if(cell[finishIndex*field.width+2*field.width-position+2*field.height].player!=none){
			enpack(cell[finishIndex*field.width+2*field.width-position+2*field.height].value,field,pack,pack_size);
		}
		for(int i=finishIndex;i>0;i--){
			cell[i*field.width+2*field.width-position+2*field.height].value=cell[(i-1)*field.width+2*field.width-position+2*field.height].value;
			cell[i*field.width+2*field.width-position+2*field.height].player=cell[(i-1)*field.width+2*field.width-position+2*field.height].player;
		}
		cell[2*field.width-position+2*field.height].player=player;
		cell[2*field.width-position+2*field.height].value=hand[cardNumber];
	}else{
		printf("wrong position");
	}

	hand[cardNumber]=-1;

	for(int i=0;i<hand_size;i++){
		if(hand[i]==-1){
			if(!isPackEmpty(pack)){
				hand[i]=depack(field,pack,pack_size);
			}
		}
	}
//TODO çà÷èùåíèå ýêðàíà ïîñëå ââîäà áîëüøîãî êîëè÷åñòâà ñèìâîëîâ

	if(cell[ENEMY_BASE_Y*field.width+ENEMY_BASE_X].player==player){
		*gameState = playerWin;
	}else{
		*gameState = nextTurn;
	}

	drawField(field,cell);
	drawPack(field,pack,pack_size);
	drawHands(hand,field,hand_size,COLOR);
	return 0;
}

int fillPack(Field field,int *pack, int *pack_size){
	//TODO DEBUG
	cursorPos.X = 0;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	printf("Fill the hand: (values will be shuffled)\n");

	for(int i=0;i<*pack_size;i++){
		printf("%d: ",i+1);
		scanf("%d",&pack[i]);

		cursorPos.X = 0;
		cursorPos.Y = 3;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		printf("\t");
		cursorPos.X = 0;
		cursorPos.Y = 3;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
	}

	cursorPos.X = 0;
	cursorPos.Y = 15+field.height;
	SetConsoleCursorPosition(consoleOutput, cursorPos);

	int packShadow[*pack_size];

	srand(time(NULL));

	for(int i=0;i<*pack_size;i++){
		packShadow[i]=rand();
	}
	//qsort(packShadow, field.size, sizeof(int), comp);
	bool isSorted=false;
	do{
		isSorted=true;
		for(int i=0;i<*pack_size;i++){
			if(packShadow[i]>packShadow[i+1]){
				packShadow[i]=packShadow[i] xor packShadow[i+1];
				packShadow[i+1]=packShadow[i] xor packShadow[i+1];
				packShadow[i]=packShadow[i] xor packShadow[i+1];

				pack[i]=pack[i] xor pack[i+1];
				pack[i+1]=pack[i] xor pack[i+1];
				pack[i]=pack[i] xor pack[i+1];

				isSorted=false;
			}
		}
	}while(!isSorted);

	return 0;
}

main(){
	printf("  Lines Battle OPTIONS");
	int hand_size = 0;
	int pack_size = 0;

	Field field;

	cursorPos.X = 0;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	printf("CHOOSE FIELD WIDTH: ");
	scanf("%d",&field.width);

	printf("CHOOSE FIELD HEIGHT: ");
	scanf("%d",&field.height);

	field.size = field.width*field.height;

	printf("CHOOSE HAND SIZE: ");
	scanf("%d",&hand_size);

	printf("CHOOSE PACK SIZE: ");
	scanf("%d",&pack_size);

	cursorPos.X = 0;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("\t\t\t\t\t\n\t\t\t\t\t\n\t\t\t\t\t\n\t\t\t\t\t");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	int pack[pack_size];

	cursorPos.X = 0;
	cursorPos.Y = 11;
	SetConsoleCursorPosition(consoleOutput, cursorPos);

	if(fillPack(field,pack,&pack_size)){
		printf("Error: can not fill pack");
	}

	cursorPos.X = 0;
	cursorPos.Y = 2;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("\t\t\t\t\t\n\t\t\t\t\t\n\t\t\t\t\t\n\t\t\t\t\t");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	cursorPos.X = 0;
	cursorPos.Y = 0;
	SetConsoleCursorPosition(consoleOutput, cursorPos);
	printf("  Lines Battle %dx%d\t",field.width,field.height);

	int hand1[hand_size];
	int hand2[hand_size];

	Cells cell[field.size];

	GameState gameState = blueTurn;

	for(int i=0;i<field.height;i++){
		for(int j=0;j<field.width;j++){
			cell[i*field.width+j].value=0;
			cell[i*field.width+j].player=none;
		}
	}

	for(int i=0;i<hand_size;i++){
		hand1[i]=depack(field,pack,pack_size);
		hand2[i]=depack(field,pack,pack_size);
	}

	if(drawField(field, cell)){
		printf("error: can not draw field\n");
	}

	drawStartFieldElements(field);

	drawPack(field,pack,pack_size);

	if(drawHands(hand1,field,hand_size,BLUE)||drawHands(hand2,field,hand_size,RED)){
		printf("error: can not draw cards\n");
	}

	do{
		//TODO redo
		if(gameState==blueTurn)
			turn(&gameState,field,hand_size,hand1,pack,cell,pack_size);
		else if(gameState==redTurn)
			turn(&gameState,field,hand_size,hand2,pack,cell,pack_size);
	}while((gameState==blueTurn)||(gameState==redTurn));

	if(gameState==blueWin){
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);
		printf("BLUE WINS!");
	}else if(gameState==redWin){
		cursorPos.X = 0;
		cursorPos.Y = 11+field.height;
		SetConsoleCursorPosition(consoleOutput, cursorPos);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		printf("RED WINS!");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	return 0;
}
