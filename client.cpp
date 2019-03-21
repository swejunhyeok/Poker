#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>
//------------------------------------------------------//
#define D_Select_X 22
#define D_Select_Y 8
#define D_Show_X 28
#define D_Show_Y 8
#define D_Sever_X 4
#define D_Sever_Y 3
#define D_Clnt_X 4
#define D_Clnt_Y 16
#define Max 8
#define RN_Max 14
#define RT_Max 4
#define Entry 5

#pragma warning (disable: 4996)
#pragma warning (disable: 4244)

typedef struct Card {
	int nType[Max];
	int nNum[Max];
	int nArray[Max];
}	Card, *CardPtr;

typedef struct Result{
	int nNum[RN_Max];
	int nType[RT_Max];
	int nArray[RT_Max][RN_Max];
	int Rank;
}	Result, *ResultPtr;
//------------------------------------------------------//
void ErrorHandling(char* message);
void gotoxy(int x,int y);
void removeCusor();
void consol_size2();
void Delete_Interface();
void Select_Interface();
void Bet_Interface();

void Poker(SOCKET hSocket,int *Money);
void print_card(int startX,int startY, CardPtr Card, int nNdx);
void NotIsInArrayNum(CardPtr Card,CardPtr Other_Card,int My_Delete,int Other_Delete,int nNdx);
int Delete_Card(CardPtr Card, int Delete);
void Make_Result_Zero(ResultPtr Result);
void Add_Result(ResultPtr Result, CardPtr Card, int nNdx);

void Rank(ResultPtr Result);
int Is_Over_Five_Color(ResultPtr Result);
int Is_Pair(ResultPtr Result);
int Num_One_Pair(ResultPtr Result);
int Num_Triple(ResultPtr Result);
int Is_Four_Card(ResultPtr Result);

int Is_Mountain(ResultPtr Result);
int Is_Back_Straight(ResultPtr Result);
int Is_Straight(ResultPtr Result);

int Is_Fullsh_Mountain(ResultPtr Result);
int Is_Fullsh_Back_Straight(ResultPtr Result);
int Is_Fullsh_Straight(ResultPtr Result);

int Find_Bigger_Rank(ResultPtr Result, ResultPtr Other_Result);
int Betting(int Input,int* Money,int* Total,int* Bet,int* Other_Bet,int Turn);
void End(ResultPtr Result, ResultPtr Other_Result, int* Money, int Total);
void Result_Interface(int R);

//------------------------------------------------------//
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[30];
	int strLen=0;
	int idx=0, readLen=0;

	
	if(argc!=3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");  

	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("hSocket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(argv[1]);
	servAddr.sin_port=htons(atoi(argv[2]));

	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	//------------------------------------------------------//
	//------------------------------------------------------//
	//------------------------------------------------------//
	//------------------------------------------------------//
	//------------------------------------------------------//
	int M = 5000;
	Poker(hSocket, &M);
//------------------------------------------------------//
	closesocket(hSocket);
	WSACleanup();
	return 0;
}
//------------------------------------------------------//
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void gotoxy(int x, int y)  
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void consol_size2()
{
	system("mode con:cols=130 lines=26");
}

void removeCusor()
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible=0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void Delete_Interface(){
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++)
		printf("▣\n");
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++){
		gotoxy(128,i+1);
		printf("▣");
		gotoxy(102,i+1);
		printf("▣");
	}
	gotoxy(34,3);
	for(int i = 0; i<16;i++){
		printf("▒");
	}
	gotoxy(34,4);
	printf("▒");
	gotoxy(64,4);
	printf("▒");
	gotoxy(34,5);
	printf("▒");
	gotoxy(64,5);
	printf("▒");
	gotoxy(34,6);
	for(int i = 0; i<16;i++){
		printf("▒");
	}
	gotoxy(36,4);
	printf("※삭제할 카드를 선택하세요※");
	gotoxy(42,5);
	printf("(1~4 까지 입력)");

	gotoxy(42,20);
	printf("입력: _");

	gotoxy(108,3);
	printf("※Poker Rule※");
	gotoxy(104,4);
	printf("① 4장의 카드를 받는다.");
	gotoxy(104,5);
	printf("② 한장의 카드를 버린다.");
	gotoxy(104,6);
	printf("③ 3장의 카드가 남는다.");
	gotoxy(104,7);
	printf("④ 표시할 카드 한장 선택");
	gotoxy(104,8);
	printf("⑤ 한장씩 카드를 받는다.");
	gotoxy(104,9);
	printf("⑤ 카드를 받을때 배팅");
	gotoxy(104,10);
	printf("⑥ 7장이 되면 승자 결정");
	gotoxy(108,12);
	printf("※Betting Rule※");
	gotoxy(104,13);
	printf("① 카드의 족보로 비교");
	gotoxy(104,14);
	printf("② 카드의 우선순위 결정");
	gotoxy(104,15);
	printf("③ 높은사람먼저 배팅");
	gotoxy(104,16);
	printf("④ 배팅은 최대 2턴까지");
	gotoxy(104,17);
	printf("※ 삥, 체크 선만가능");
	gotoxy(104,18);
	printf("※ 마지막은 콜,다이 만");
}

void Select_Interface(){
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++)
		printf("▣\n");
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++){
		gotoxy(128,i+1);
		printf("▣");
		gotoxy(102,i+1);
		printf("▣");
	}
	gotoxy(34,3);
	for(int i = 0; i<16;i++){
		printf("▒");
	}
	gotoxy(34,4);
	printf("▒");
	gotoxy(64,4);
	printf("▒");
	gotoxy(34,5);
	printf("▒");
	gotoxy(64,5);
	printf("▒");
	gotoxy(34,6);
	for(int i = 0; i<16;i++){
		printf("▒");
	}
	gotoxy(36,4);
	printf("※표시할 카드를 선택하세요※");
	gotoxy(42,5);
	printf("(1~3 까지 입력)");

	gotoxy(42,20);
	printf("입력: _");

	gotoxy(108,3);
	printf("※Poker Rule※");
	gotoxy(104,4);
	printf("① 4장의 카드를 받는다.");
	gotoxy(104,5);
	printf("② 한장의 카드를 버린다.");
	gotoxy(104,6);
	printf("③ 3장의 카드가 남는다.");
	gotoxy(104,7);
	printf("④ 표시할 카드 한장 선택");
	gotoxy(104,8);
	printf("⑤ 한장씩 카드를 받는다.");
	gotoxy(104,9);
	printf("⑤ 카드를 받을때 배팅");
	gotoxy(104,10);
	printf("⑥ 7장이 되면 승자 결정");
	gotoxy(108,12);
	printf("※Betting Rule※");
	gotoxy(104,13);
	printf("① 카드의 족보로 비교");
	gotoxy(104,14);
	printf("② 카드의 우선순위 결정");
	gotoxy(104,15);
	printf("③ 높은사람먼저 배팅");
	gotoxy(104,16);
	printf("④ 배팅은 최대 2턴까지");
	gotoxy(104,17);
	printf("※ 삥, 체크 선만가능");
	gotoxy(104,18);
	printf("※ 마지막은 콜,다이 만");
}

void Bet_Interface(){
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++)
		printf("▣\n");
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++){
		gotoxy(128,i+1);
		printf("▣");
		gotoxy(102,i+1);
		printf("▣");
	}
	gotoxy(2,10);
	for(int i = 0; i < 50; i++)
		printf("▒");
	gotoxy(22,11);
	printf("① 콜 ② 삥(선) ③ 따당 ④ 쿼터 ⑤ 하프 ⑥ 풀 ⑦ 체크(선) ⑧ 다이");
	gotoxy(2,14);
	for(int i = 0; i < 50; i++)
		printf("▒");

	gotoxy(48,13);
	printf("입력: _");

	gotoxy(108,3);
	printf("※Poker Rule※");
	gotoxy(104,4);
	printf("① 4장의 카드를 받는다.");
	gotoxy(104,5);
	printf("② 한장의 카드를 버린다.");
	gotoxy(104,6);
	printf("③ 3장의 카드가 남는다.");
	gotoxy(104,7);
	printf("④ 표시할 카드 한장 선택");
	gotoxy(104,8);
	printf("⑤ 한장씩 카드를 받는다.");
	gotoxy(104,9);
	printf("⑤ 카드를 받을때 배팅");
	gotoxy(104,10);
	printf("⑥ 7장이 되면 승자 결정");
	gotoxy(108,12);
	printf("※Betting Rule※");
	gotoxy(104,13);
	printf("① 카드의 족보로 비교");
	gotoxy(104,14);
	printf("② 카드의 우선순위 결정");
	gotoxy(104,15);
	printf("③ 높은사람먼저 배팅");
	gotoxy(104,16);
	printf("④ 배팅은 최대 2턴까지");
	gotoxy(104,17);
	printf("※ 삥, 체크 선만가능");
	gotoxy(104,18);
	printf("※ 마지막은 콜,다이 만");
}

//------------------------------------------------------//
void Poker(SOCKET hSocket,int *Money){
	char message[30];
	int strLen=0;
	int idx=0, readLen=0;
	consol_size2();
	removeCusor();
	int SEED = time(NULL);
	int Total_Money = 0, Sever_Money = 5000;
	int nClnt_Delete_Card = -1, nSever_Delete_Card = -1;
	srand(SEED);

	recv(hSocket,(char *)&Sever_Money,sizeof(int),0);
	send(hSocket,(char *)Money,sizeof(int),0);

	Total_Money += Entry;
	*Money -= Entry;
	Total_Money += Entry;
	Sever_Money -= Entry;
	
	system("cls");
	Delete_Interface();
	Card Sever_Card,Clnt_Card;

	//------------------------수신1
	for(int i = 0;i < 4;i++){
		recv(hSocket,(char *)&Sever_Card.nArray[i],sizeof(int),0);
		Sever_Card.nArray[i + 1] = -1;
		Sever_Card.nType[i] = Sever_Card.nArray[i] / 13;
		Sever_Card.nType[i + 1] = -1;
		Sever_Card.nNum[i] = Sever_Card.nArray[i]  % 13;
		Sever_Card.nNum[i + 1] = -1;
	}

	for(int i = 0;i < 4;i++){ //0 12 , 13 25 , 26 38 , 39 51  
		NotIsInArrayNum(&Clnt_Card, &Sever_Card,nClnt_Delete_Card,nSever_Delete_Card,i); 
		Clnt_Card.nType[i] = Clnt_Card.nArray[i] / 13;
		Clnt_Card.nType[i + 1] = -1;
		Clnt_Card.nNum[i] = Clnt_Card.nArray[i]  % 13;
		Clnt_Card.nNum[i + 1] = -1; 	
	}

	//------------------------클라이언트 카드 삭제 선택 출력
	print_card(D_Select_X, D_Select_Y, &Clnt_Card, 0);
	print_card(D_Select_X + 14, D_Select_Y, &Clnt_Card, 1);
	print_card(D_Select_X + 28, D_Select_Y, &Clnt_Card, 2);
	print_card(D_Select_X + 42, D_Select_Y, &Clnt_Card, 3);

	//------------------------클라이언트 카드 제거
	int nDelete_Card;
	do{
		//------------------------입력1 / 버퍼링 1
		gotoxy(48,20);
		fflush(stdin);
		scanf("%d",&nDelete_Card);
	}while(nDelete_Card != 1 && nDelete_Card != 2 && nDelete_Card != 3 && nDelete_Card != 4);
	nClnt_Delete_Card = Clnt_Card.nArray[nDelete_Card - 1];
	gotoxy(0,0);
	if(!Delete_Card(&Clnt_Card, nDelete_Card)){
		printf("Delete Card Error\n");
		closesocket(hSocket);
		WSACleanup();
		exit(1);
	}
	gotoxy(52,20);
	printf("전송되었습니다. 상대편을 기다리세요..");

	int nS_Delete_Card;
	//------------------------수신2
	recv(hSocket,(char *)&nS_Delete_Card,sizeof(int),0);
	
	//------------------------전송3
	for(int i = 0;i < 3;i++)send(hSocket,(char *)&Clnt_Card.nArray[i],sizeof(int),0);

	//------------------------전송4
	send(hSocket,(char *)&nClnt_Delete_Card,sizeof(int),0);
	

	//------------------------서버 카드 삭제
	nSever_Delete_Card = Sever_Card.nArray[nS_Delete_Card - 1];
	gotoxy(0,0);
	if(!Delete_Card(&Sever_Card, nS_Delete_Card)){
		printf("Delete Card Error\n");
		closesocket(hSocket);
		WSACleanup();	
		exit(1);
	}

	//------------------------클라이언트 쇼 선택 출력
	system("cls");
	Select_Interface();
	print_card(D_Show_X, D_Show_Y, &Clnt_Card, 0);
	print_card(D_Show_X + 14, D_Show_Y, &Clnt_Card, 1);
	print_card(D_Show_X + 28, D_Show_Y, &Clnt_Card, 2);
	
	//------------------------클라이언트 쇼 선택
	int nClnt_Show_Card;
	do{
		//------------------------입력2 / 버터링 2
		gotoxy(48,20);
		fflush(stdin);
		scanf("%d",&nClnt_Show_Card);
	}while(nClnt_Show_Card != 1 && nClnt_Show_Card != 2 && nClnt_Show_Card != 3);

	gotoxy(52,20);
	printf("전송되었습니다. 상대편을 기다리세요..");

	//------------------------수신5
	int nSever_Show_Card;
	recv(hSocket,(char *)&nSever_Show_Card,sizeof(int),0);
	
	//------------------------전송6
	send(hSocket,(char *)&nClnt_Show_Card,sizeof(int),0);

	//------------------------서버 카드 출력
	nSever_Show_Card--;
	system("cls");
	Bet_Interface();
	gotoxy(42,12);
	printf("Total : %d만원",Total_Money);
	gotoxy(2,1);
	printf("잔액 : %d만원",Sever_Money);
	gotoxy(2,23);
	printf("잔액 : %d만원",*Money);
	print_card(D_Sever_X, D_Sever_Y, &Sever_Card, -1);
	print_card(D_Sever_X+14, D_Sever_Y, &Sever_Card, -1);
	print_card(D_Sever_X+28, D_Sever_Y, &Sever_Card, nSever_Show_Card);

	//------------------------클라이언트 카드 출력
	nClnt_Show_Card--;
	print_card(D_Clnt_X, D_Clnt_Y, &Clnt_Card, (nClnt_Show_Card + 1) % 3);
	print_card(D_Clnt_X+14, D_Clnt_Y, &Clnt_Card, (nClnt_Show_Card + 2) % 3);
	print_card(D_Clnt_X+28, D_Clnt_Y, &Clnt_Card, nClnt_Show_Card);

	//------------------------클라이언트 결과 변수 초기화(0)
	Result Clnt_Result;
	Make_Result_Zero(&Clnt_Result);
	Add_Result(&Clnt_Result, &Clnt_Card, nClnt_Show_Card);

	//------------------------서버 결과 변수 초기화(0)
	Result Sever_Result;
	Make_Result_Zero(&Sever_Result);
	Add_Result(&Sever_Result, &Sever_Card, nSever_Show_Card);
	
	int Is_Ok = 0;

	//------------------------클라이언트 카드 출력
	for(int i = 3; i <  7 || Clnt_Card.nArray[i] != -1; i++){
		int Turn = 0;
		int Sever_Bet = 0;
		int Clnt_Bet = 0;

		//------------------------서버 카드 수신
		recv(hSocket,(char *)&Sever_Card.nArray[i], sizeof(int),0);
		Sever_Card.nType[i] = Sever_Card.nArray[i] / 13;
		Sever_Card.nNum[i] = Sever_Card.nArray[i]  % 13;
		Sever_Card.nNum[i + 1] = Sever_Card.nArray[i + 1] = Sever_Card.nType[i + 1] = -1;
		
		if(i != 6)
			Add_Result(&Sever_Result, &Sever_Card, i);
		
		//------------------------클라이언트 카드 생성
		NotIsInArrayNum(&Clnt_Card, &Sever_Card,nClnt_Delete_Card,nSever_Delete_Card,i);
		Clnt_Card.nType[i] = Clnt_Card.nArray[i] / 13;
		Clnt_Card.nNum[i] = Clnt_Card.nArray[i]  % 13;
		Clnt_Card.nNum[i + 1] = Clnt_Card.nType[i + 1] = -1;

		if(i != 6)
			Add_Result(&Clnt_Result, &Clnt_Card, i);

		//------------------------클라이언트 카드 전송
		send(hSocket,(char *)&Clnt_Card.nArray[i], sizeof(int),0);

		print_card(D_Clnt_X + (14 * i), D_Clnt_Y, &Clnt_Card, i);
		if(i==6)
			print_card(D_Sever_X + (14 * i), D_Sever_Y, &Sever_Card, -1);
		else
			print_card(D_Sever_X + (14 * i), D_Sever_Y, &Sever_Card, i);
		
		//------------------------우선 순위 결정
		Rank(&Clnt_Result);
		Rank(&Sever_Result);

		if(Sever_Result.Rank > Clnt_Result.Rank){
			int nClnt_Input;

			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);


			Turn ++;
			int nSever_Input;
			gotoxy(58,13);
			printf("상대편이 선입니다. 기다려주세요..");
			recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
			Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);

			gotoxy(58,13);
			printf("                                     ");
			if(Is_Ok == 2)
				break;

			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);

			Turn ++;
			Is_Ok = 0;
			gotoxy(58,13);
			printf("수신되었습니다. 입력해주세요..");
			while(!Is_Ok){
				gotoxy(54,13);
				scanf("%d",&nClnt_Input);
				Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
			}
			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(58,13);
			printf("전송되었습니다. 상대편을 기다리세요..");
			send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
			if(Is_Ok == 2)
				break;
			
				if(Is_Ok == 3)
				continue;

			Turn ++;
			recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(54,13);
			printf("_");
			Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);

			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);

			if(Is_Ok == 2)
				break;

			gotoxy(58,13);
			printf("수신되었습니다. 입력해주세요..");

			Turn ++;
			Is_Ok = 0;
			while(!Is_Ok){
				gotoxy(54,13);
				scanf("%d",&nClnt_Input);
				Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
			}
			send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(54,13);
			printf("_");
			if(Is_Ok == 2)
				break;
		}
		else if(Sever_Result.Rank < Clnt_Result.Rank){
			int nClnt_Input;
			int nSever_Input;

			Turn ++;
			Is_Ok = 0;

			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);

			gotoxy(58,13);
			printf("당신이 선입니다. 배팅해주세요..");

			while(!Is_Ok){
				gotoxy(54,13);
				scanf("%d",&nClnt_Input);
				Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
			}
			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(58,13);
			printf("전송되었습니다. 상대편을 기다리세요..");
			send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
			if(Is_Ok == 2)
				break;

			Turn ++;
			recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(54,13);
			printf("_");
			Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);
			if(Is_Ok == 2)
				break;
			
			if(Is_Ok == 3)
				continue;

			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);

			Turn ++;
			Is_Ok = 0;
			gotoxy(58,13);
			printf("수신되었습니다. 입력해주세요..");
			while(!Is_Ok){
				gotoxy(54,13);
				scanf("%d",&nClnt_Input);
				Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
			}
			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(58,13);
			printf("전송되었습니다. 상대편을 기다리세요..");
			send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
			if(Is_Ok == 2)
				break;

			Turn ++;
			recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
			gotoxy(58,13);
			printf("                                     ");
			gotoxy(54,13);
			printf("_");
			Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);
			gotoxy(50,12);
			printf("               ");
			gotoxy(42,12);
			printf("Total : %d만원",Total_Money);
			gotoxy(8,1);
			printf("         ");
			gotoxy(8,23);
			printf("         ");
			gotoxy(2,1);
			printf("잔액 : %d만원",Sever_Money);
			gotoxy(2,23);
			printf("잔액 : %d만원",*Money);
			if(Is_Ok == 2)
				break;
		}
		else{
			if(Find_Bigger_Rank(&Clnt_Result,&Sever_Result)){
				int nClnt_Input;
				int nSever_Input;

				Turn ++;
				Is_Ok = 0;

				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);

				gotoxy(58,13);
				printf("당신이 선입니다. 배팅해주세요..");

				while(!Is_Ok){
					gotoxy(54,13);
					scanf("%d",&nClnt_Input);
					Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
				}
				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(58,13);
				printf("전송되었습니다. 상대편을 기다리세요..");
				send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
				if(Is_Ok == 2)
					break;

				Turn ++;
				recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(54,13);
				printf("_");
				Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);
				if(Is_Ok == 2)
					break;
			
				if(Is_Ok == 3)
					continue;

				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);

				Turn ++;
				Is_Ok = 0;
				gotoxy(58,13);
				printf("수신되었습니다. 입력해주세요..");
				while(!Is_Ok){
					gotoxy(54,13);
					scanf("%d",&nClnt_Input);
					Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
				}
				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(58,13);
				printf("전송되었습니다. 상대편을 기다리세요..");
				send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
				if(Is_Ok == 2)
					break;

				Turn ++;
				recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(54,13);
				printf("_");
				Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);
				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);
				if(Is_Ok == 2)
					break;
			}
			else{
				int nClnt_Input;

				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);


				Turn ++;
				int nSever_Input;
				gotoxy(58,13);
				printf("상대편이 선입니다. 기다려주세요..");
				recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
				Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);

				gotoxy(58,13);
				printf("                                     ");
				if(Is_Ok == 2)
					break;

				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);

				Turn ++;
				Is_Ok = 0;
				gotoxy(58,13);
				printf("수신되었습니다. 입력해주세요..");
				while(!Is_Ok){
					gotoxy(54,13);
					scanf("%d",&nClnt_Input);
					Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
				}
				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(58,13);
				printf("전송되었습니다. 상대편을 기다리세요..");
				send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
				if(Is_Ok == 2)
					break;
			
					if(Is_Ok == 3)
					continue;

				Turn ++;
				recv(hSocket,(char *)&nSever_Input, sizeof(int),0);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(54,13);
				printf("_");
				Is_Ok = Betting(nSever_Input,&Sever_Money,&Total_Money,&Sever_Bet,&Clnt_Bet,Turn);

				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);

				if(Is_Ok == 2)
					break;

				gotoxy(58,13);
				printf("수신되었습니다. 입력해주세요..");

				Turn ++;
				Is_Ok = 0;
				while(!Is_Ok){
					gotoxy(54,13);
					scanf("%d",&nClnt_Input);
					Is_Ok = Betting(nClnt_Input,Money,&Total_Money, &Clnt_Bet, &Sever_Bet, Turn);
				}
				send(hSocket,(char *)&nClnt_Input, sizeof(int),0);
				gotoxy(50,12);
				printf("               ");
				gotoxy(42,12);
				printf("Total : %d만원",Total_Money);
				gotoxy(8,1);
				printf("         ");
				gotoxy(8,23);
				printf("         ");
				gotoxy(2,1);
				printf("잔액 : %d만원",Sever_Money);
				gotoxy(2,23);
				printf("잔액 : %d만원",*Money);
				gotoxy(58,13);
				printf("                                     ");
				gotoxy(54,13);
				printf("_");
				if(Is_Ok == 2)
					break;
			}
		}
	}


	int tmp = 0;
	while(tmp != -1){
		gotoxy(54,13);
		printf("_");
		gotoxy(58,13);
		printf("                                     ");
		gotoxy(58,13);
		printf("종료되었습니다. -1 을 입력해주세요..");
		gotoxy(54,13);
		scanf("%d",&tmp);
	}

	//------------------------결과값 결정 및 출력
	if(Is_Ok != 2){
		//------------------------결과값 결정 및 출력

		Add_Result(&Clnt_Result, &Clnt_Card, (nClnt_Show_Card + 1) % 3);
		Add_Result(&Clnt_Result, &Clnt_Card, (nClnt_Show_Card + 2) % 3);
		Add_Result(&Clnt_Result, &Clnt_Card, 6);

		Add_Result(&Sever_Result, &Sever_Card, (nSever_Show_Card + 1) % 3);
		Add_Result(&Sever_Result, &Sever_Card, (nSever_Show_Card + 2) % 3);
		Add_Result(&Sever_Result, &Sever_Card, 6);

		//------------------------우선 순위 결정
		Rank(&Clnt_Result);
		Rank(&Sever_Result);

		system("cls");
		End(&Sever_Result, &Clnt_Result,Money,Total_Money);
	}



	//------------------------수신7
	readLen = recv(hSocket, message, sizeof(message)-1, 0);
	message[readLen]='\0';
}

void NotIsInArrayNum(CardPtr Card,CardPtr Other_Card,int My_Delete,int Other_Delete,int nNdx){
	int nAtmp, Is_Ok;
	do{
		Is_Ok = 0;
		nAtmp=rand() % 52;
		for(int i=0;i <= nNdx || Card->nArray[i] != -1; i++)
			if(Card->nArray[i] == nAtmp)
				Is_Ok = 1;
			else if(Other_Card->nArray[i] == nAtmp)
				Is_Ok = 1;
			else if(My_Delete == nAtmp)
				Is_Ok = 1;
			else if(Other_Delete == nAtmp)
				Is_Ok = 1;
	}while(Is_Ok);
	Card->nArray[nNdx] = nAtmp;
	Card->nArray[nNdx + 1] = -1;
}

void print_card(int startX,int startY, CardPtr Card, int nNdx){
	//카드 틀
	for(int i=0;i<7;i++){
		for(int j=0;j<6;j++){
			if(i==0){
				if(j==0){
					gotoxy(startX+2*j,startY+i); printf("┏");
				}
				else if(j==5){
					gotoxy(startX+2*j,startY+i); printf("┓");
				}
				else{
					gotoxy(startX+2*j,startY+i); printf("━");
				}
			}
			else if(i==6){
				if(j==0){
					gotoxy(startX+2*j,startY+i); printf("┗");
				}
				else if(j==5){
					gotoxy(startX+2*j,startY+i); printf("┛");
				}
				else{
					gotoxy(startX+2*j,startY+i); printf("━");
				}
			}
			else
				if(j==0 || j==5){
					gotoxy(startX+2*j,startY+i); printf("┃");
				}
		}
	}
	//카드 문양 (스,다,하,클)
	if(nNdx == -1){
		gotoxy(startX+2,startY+1); printf("?");
		gotoxy(startX+8,startY+5); printf("?");
	}
	else{
		if(Card->nType[nNdx] == 0){
			gotoxy(startX+2,startY+1); printf("♠");
			gotoxy(startX+8,startY+5); printf("♠");
		}
		else if(Card->nType[nNdx] == 1){
			gotoxy(startX+2,startY+1); printf("◆");
			gotoxy(startX+8,startY+5); printf("◆");
		}
		else if(Card->nType[nNdx] == 2){
			gotoxy(startX+2,startY+1); printf("♥");
			gotoxy(startX+8,startY+5); printf("♥");
		}
		else{
			gotoxy(startX+2,startY+1); printf("♣");
			gotoxy(startX+8,startY+5); printf("♣");
		}
	}

	//카드번호
	if(nNdx == -1){
		gotoxy(startX+2,startY+1); printf("?");
		gotoxy(startX+8,startY+5); printf("?");
	}
	else{
		if(Card->nNum[nNdx] == 0){
			gotoxy(startX+2,startY+2); printf("A");
			gotoxy(startX+8,startY+4); printf("A");
		}
		for(int i = 1;i < 10; i++){
			if(Card->nNum[nNdx] == i){
				gotoxy(startX+2,startY+2); printf("%d",i+1);
				gotoxy(startX+8,startY+4); printf("%d",i+1);
			}
		}
		if(Card->nNum[nNdx] == 10){
			gotoxy(startX+2,startY+2); printf("J");
			gotoxy(startX+8,startY+4); printf("J");
		}
		if(Card->nNum[nNdx] == 11){
			gotoxy(startX+2,startY+2); printf("Q");
			gotoxy(startX+8,startY+4); printf("Q");
		}
		if(Card->nNum[nNdx] == 12){
			gotoxy(startX+2,startY+2); printf("K");
			gotoxy(startX+8,startY+4); printf("K");
		}
	}
}

int Delete_Card(CardPtr Card, int Delete){
	if(Delete < 1 || Delete > 4)
		return false;
	Delete--;
	for(Delete; Delete < 4 || Card->nArray[Delete] != -1 ; Delete++){
		Card->nArray[Delete] = Card->nArray[Delete + 1]; 	
		Card->nNum[Delete] = Card->nNum[Delete + 1];
		Card->nType[Delete] = Card->nType[Delete + 1];
	}
	return true;
}

void Make_Result_Zero(ResultPtr Result){
	for(int i = 0; i < RT_Max; i++){
		Result->nType[i] = 0;
		for(int j = 0; j < RN_Max; j++){
			Result->nNum[j] = 0;
			Result->nArray[i][j]=0;
		}
	}
	Result->Rank = 0;
}

void Add_Result(ResultPtr Result, CardPtr Card, int nNdx){
	if(Card->nNum[nNdx] != 0){
		Result->nNum[Card->nNum[nNdx]]++;
		Result->nType[Card->nType[nNdx]]++;
		Result->nArray[Card->nType[nNdx]][Card->nNum[nNdx]]++;
	}
	else{
		Result->nNum[Card->nNum[nNdx]]++;
		Result->nNum[13]++;
		Result->nType[Card->nType[nNdx]]++;
		Result->nArray[Card->nType[nNdx]][Card->nNum[nNdx]]++;
		Result->nArray[Card->nType[nNdx]][13]++;	
	}
}

void Rank(ResultPtr Result){
	if(Is_Over_Five_Color(Result))
		if(Is_Fullsh_Mountain(Result))
			Result->Rank = 12;
		else if(Is_Fullsh_Back_Straight(Result))
			Result->Rank = 11;
		else if(Is_Fullsh_Straight(Result))
			Result->Rank = 10;
		else
			Result->Rank = 7;
	else
		if(Is_Mountain(Result))
			Result->Rank = 6;
		else if(Is_Back_Straight(Result))
			Result->Rank = 5;
		else if(Is_Straight(Result))
			Result->Rank = 4;
		else if(Is_Pair(Result))
			if(Is_Four_Card(Result))
				Result->Rank = 9;
			else if(Num_Triple(Result))
				if(Num_One_Pair(Result))
					Result->Rank = 8;
				else
					Result->Rank = 3;
			else if(1 < Num_One_Pair(Result))
				Result->Rank = 2;
			else
				Result->Rank = 1;
		else
			Result->Rank = 0;
		
}

/*
	0 노페어 
	1 원페어
	2 투페어
	3 트리플
	4 스트레이트
	5 백스트레이트(A~5)
	6 마운틴 (10~A)
	7 플러쉬
	8 풀하우스
	9 포카드
	10 스트레이트 플러쉬
	11 백스트레이트 플러쉬
	12 로얄 스트레이트 플러쉬
*/

int Is_Over_Five_Color(ResultPtr Result){
	for(int i = 0; i<RT_Max; i ++)
		if(Result->nType[i] >= 5)
			return true;
	return false;
}

int Is_Pair(ResultPtr Result){
	for(int i = 0; i<RN_Max - 1; i ++)
		if(Result->nNum[i] >= 2)
			return true;
	return false;
}

int Num_One_Pair(ResultPtr Result){
	int Count=0;
	for(int i = 0; i<RN_Max - 1; i ++)
		if(Result->nNum[i] == 2)
			Count++;
	return Count;
}

int Num_Triple(ResultPtr Result){
	int Count=0;
	for(int i = 0; i<RN_Max - 1; i ++)
		if(Result->nNum[i] == 3)
			Count++;
	return Count;
}

int Is_Four_Card(ResultPtr Result){
	for(int i = 0; i<RN_Max - 1; i ++)
		if(Result->nNum[i] == 4)
			return true;
	return false;
}

int Is_Mountain(ResultPtr Result){
	int Count = 0;
	for(int i = 0;i < 5; i++)
		if(Result->nNum[9+i] != 0)
			Count ++;
	return Count == 5;
}

int Is_Back_Straight(ResultPtr Result){
	int Count = 0;
	for(int i = 0;i < 5; i++)
		if(Result->nNum[0+i] != 0)
			Count ++;
	return Count == 5;
}

int Is_Straight(ResultPtr Result){
	for(int j = 1; j < 9;j++){
		int Count = 0;
		for(int i = 0;i < 5; i++)
			if(Result->nNum[j+i] != 0)
				Count ++;
		if(Count == 5)
			return true;
	}
	return false;
	
}

int Is_Fullsh_Mountain(ResultPtr Result){
	int type;
	for(int i = 0; i < RT_Max;i++){
		if(Result->nType[i] > 4){
			type = i;
			break;
		}
	}
	int Count = 0;
	for(int i = 0;i < 5; i++)
		if(Result->nArray[type][9+i] != 0)
			Count ++;
	return Count == 5;
}

int Is_Fullsh_Back_Straight(ResultPtr Result){
	int type;
	for(int i = 0; i < RT_Max;i++){
		if(Result->nType[i] > 4){
			type = i;
			break;
		}
	}
	int Count = 0;
	for(int i = 0;i < 5; i++)
		if(Result->nArray[type][0+i] != 0)
			Count ++;
	return Count == 5;
}

int Is_Fullsh_Straight(ResultPtr Result){
	int type;
	for(int i = 0; i < RT_Max;i++){
		if(Result->nType[i] > 4){
			type = i;
			break;
		}
	}
	for(int j = 1; j < 9;j++){
		int Count = 0;
		for(int i = 0;i < 5; i++)
			if(Result->nArray[type][j+i] != 0)
				Count ++;
		if(Count == 5)
			return true;
	}
	return false;
}

int Find_Bigger_Rank(ResultPtr Result, ResultPtr Other_Result){
	if(Result->Rank == 0)
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				if(Result->nArray[i][j] + Other_Result->nArray[i][j])
					return Result->nArray[i][j];
			}
	else if(Result->Rank == 1){
		int tmp,Other_tmp;
		for(int i = 0; i<RN_Max; i++){
			if(Result->nNum[i] == 2)
				tmp = i;
			if(Other_Result->nNum[i] == 2)
				Other_tmp = i;
		}
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				if(Result->nArray[i][tmp] && tmp == j)
					return true;
				if(Other_Result->nArray[i][Other_tmp] && Other_tmp == j)
					return false;
			}
	}
	else if(Result->Rank == 2){
		int tmp_num,Other_tmp_num;
		tmp_num=Num_One_Pair(Result);
		Other_tmp_num=Num_One_Pair(Other_Result);
		int tmp[7],Other_tmp[7];
		for(int i = 0, a = 0, b= 0; i<RN_Max; i++){
			if(Result->nNum[i] == 2)
				tmp[a++] = i;
			if(Other_Result->nNum[i] == 2)
				Other_tmp[b++] = i;
		}
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				for(int k = 0; k < tmp_num ; k++)
					if(Result->nArray[i][tmp[k]] && tmp[k] == j)
						return true;
				for(int k = 0; k < Other_tmp_num ; k++)
					if(Other_Result->nArray[i][Other_tmp[k]] && Other_tmp[k] == j)
						return false;
			}
	}
	else if(Result->Rank == 3||Result->Rank == 8){
		int tmp,Other_tmp;
		for(int i = 0; i<RN_Max; i++){
			if(Result->nNum[i] == 3)
				tmp = i;
			if(Other_Result->nNum[i] == 3)
				Other_tmp = i;
		}
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				if(Result->nArray[i][tmp] && tmp == j)
					return true;
				if(Other_Result->nArray[i][Other_tmp] && Other_tmp == j)
					return false;
			}
	}
	else if(Result->Rank == 4 || Result->Rank == 10){
		int tmp[2],Other_tmp[2];
		for(int i = 0;i < RT_Max; i++)
			for(int j = 1;j < 9; j++){
				int count = 0;
				for(int k = 0; k < 5; k++)
					if(Result->nArray[i][j+k])
						count ++;
				if(count == 5){
					tmp[0] = i;
					tmp[1] = j;
				}
				count = 0;
				for(int k = 0; k < 5; k++)
					if(Other_Result->nArray[i][j+k])
						count ++;
				if(count == 5){
					Other_tmp[0] = i;
					Other_tmp[1] = j;
				}
			}
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				if(tmp[0] == i && tmp[1] == j)
					return true;
				if(Other_tmp[0] == i && Other_tmp[1] == j)
					return false;
			}
	}
	else if(Result->Rank == 5 || Result->Rank == 11){ //수정필요
		int tmp, Other_tmp;
		for(int i = 0;i < RT_Max; i++){
			int count = 0;
			for(int k = 0; k < 5; k++)
				if(Result->nArray[i][0+k])
					count ++;
			if(count == 5)
				tmp = i;
			count = 0;
			for(int k = 0; k < 5; k++)
				if(Other_Result->nArray[i][0+k])
					count ++;
			if(count == 5)
				Other_tmp = i;
		}
		return tmp<Other_tmp;
	}
	else if(Result->Rank == 6 || Result->Rank == 12){
		int tmp, Other_tmp;
		for(int i = 0;i < RT_Max; i++){
			int count = 0;
			for(int k = 0; k < 5; k++)
				if(Result->nArray[i][9+k])
					count ++;
			if(count == 5)
				tmp = i;
			count = 0;
			for(int k = 0; k < 5; k++)
				if(Other_Result->nArray[i][9+k])
					count ++;
			if(count == 5)
				Other_tmp = i;
		}
		return tmp<Other_tmp;
	}
	else if(Result->Rank == 7){
		int type;
		for(int i = 0; i < RT_Max;i++){
			if(Result->nType[i] > 4){
				type = i;
				break;
			}
		}
		for(int i =13; i>0; i++){
			if(Result->nArray[type][i])
				return true;
			if(Other_Result->nArray[type][i])
				return false;
		}
	}
	else if(Result->Rank == 9){
		int tmp,Other_tmp;
		for(int i = 0; i<RN_Max; i++){
			if(Result->nNum[i] == 4)
				tmp = i;
			if(Other_Result->nNum[i] == 4)
				Other_tmp = i;
		}
		for(int i = 0; i <4; i++)
			for(int j = 13; j>0; j--){
				if(Result->nArray[i][tmp] && tmp == j)
					return true;
				if(Other_Result->nArray[i][Other_tmp] && Other_tmp == j)
					return false;
			}
	}
}

int Betting(int Input,int* Money,int* Total,int* Bet,int* Other_Bet,int Turn){
	if(Input == 1 && Turn != 1 && (*Other_Bet >= *Bet)){
		*Money -= (*Other_Bet - *Bet);
		*Total += (*Other_Bet - *Bet);
		*Bet = *Other_Bet;
		if(Turn == 2){
			return 3;
		}
		return 1;
	}
	else if(Input == 2 && Turn == 1){
		*Money -= Entry;
		*Bet += Entry;
		*Total += Entry;
		return 1;
	}
	else if(Input == 3 && Turn != 1 && Turn != 4){
		*Money -= (*Other_Bet) * 2;
		*Total += (*Other_Bet) * 2;
		*Bet += (*Other_Bet) * 2;
		return 1;
	}
	else if(Input == 4 && Turn != 4){
		*Money -= *Total /4;
		*Bet += *Total /4;
		*Total += *Total /4;
		return 1;
	}
	else if(Input == 5 && Turn != 4){
		*Money -= *Total/2;
		*Bet += *Total/2;
		*Total += *Total/2;
		return 1;
	}
	else if(Input == 6 && Turn != 4){
		*Money -= *Total;
		*Bet += *Total;
		*Total += *Total;
		return 1;
	}
	else if(Input == 7 && Turn == 1){
		return 1;
	}
	else if(Input == 8){
		return 2;
	}
	return 0;
}

void End(ResultPtr Result, ResultPtr Other_Result, int* Money, int Total){
	if(Result->Rank > Other_Result->Rank){
		Result_Interface(0);
		*Money += Total;
		gotoxy(40,16);
		printf("%d만원 이득",Total);
	}
	else if(Result->Rank < Other_Result->Rank){
		Result_Interface(1);
		gotoxy(40,16);
		printf("%d만원 손해",Total);
	}
	else
		if(Find_Bigger_Rank(Result,Other_Result)){
			Result_Interface(1);
			gotoxy(40,16);
			printf("%d만원 손해",Total);
		}
		else{
			Result_Interface(0);
			*Money += Total;
			gotoxy(40,16);
			printf("%d만원 이득",Total);
		}
	gotoxy(40,20);
	printf("나의 카드 : ");
	switch(Result->Rank){
		case 0 : printf("노페어"); break;
		case 1 : printf("원페어"); break;
		case 2 : printf("투페어"); break;
		case 3 : printf("트리플"); break;
		case 4 : printf("스트레이트"); break;
		case 5 : printf("백스트레이트"); break;
		case 6 : printf("마운틴"); break;
		case 7 : printf("플러쉬"); break;
		case 8 : printf("풀하우스"); break;
		case 9 : printf("포카드"); break;
		case 10 : printf("스트레이트 플러쉬"); break;
		case 11 : printf("백스트레이트 플러쉬"); break;
		case 12 : printf("로얄 스트레이트 플러쉬"); break;
	}
	gotoxy(40,18);
	printf("상대방의 카드 : ");
	switch(Other_Result->Rank){
		case 0 : printf("노페어"); break;
		case 1 : printf("원페어"); break;
		case 2 : printf("투페어"); break;
		case 3 : printf("트리플"); break;
		case 4 : printf("스트레이트"); break;
		case 5 : printf("백스트레이트"); break;
		case 6 : printf("마운틴"); break;
		case 7 : printf("플러쉬"); break;
		case 8 : printf("풀하우스"); break;
		case 9 : printf("포카드"); break;
		case 10 : printf("스트레이트 플러쉬"); break;
		case 11 : printf("백스트레이트 플러쉬"); break;
		case 12 : printf("로얄 스트레이트 플러쉬"); break;
	}
}

void Result_Interface(int R){
	int Result[2][8][30]={
	{
		{1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,1},
		{0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,0,0,0,0,0,1},
		{0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,1},
		{0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,1},
		{0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,1,0,0,1},
		{0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,1},
		{0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,1,1}
	},
	{
		{1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0},
		{1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0},
		{1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
		{1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
		{1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	}
	};


	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++)
		printf("▣\n");
	for(int i = 0; i < 65; i++)
		printf("▣");
	for(int i = 0; i< 23; i++){
		gotoxy(128,i+1);
		printf("▣");
		gotoxy(102,i+1);
		printf("▣");
	}
	gotoxy(108,3);
	printf("※Poker Rule※");
	gotoxy(104,4);
	printf("① 4장의 카드를 받는다.");
	gotoxy(104,5);
	printf("② 한장의 카드를 버린다.");
	gotoxy(104,6);
	printf("③ 3장의 카드가 남는다.");
	gotoxy(104,7);
	printf("④ 표시할 카드 한장 선택");
	gotoxy(104,8);
	printf("⑤ 한장씩 카드를 받는다.");
	gotoxy(104,9);
	printf("⑤ 카드를 받을때 배팅");
	gotoxy(104,10);
	printf("⑥ 7장이 되면 승자 결정");
	gotoxy(108,12);
	printf("※Betting Rule※");
	gotoxy(104,13);
	printf("① 카드의 족보로 비교");
	gotoxy(104,14);
	printf("② 카드의 우선순위 결정");
	gotoxy(104,15);
	printf("③ 높은사람먼저 배팅");
	gotoxy(104,16);
	printf("④ 배팅은 최대 2턴까지");
	gotoxy(104,17);
	printf("※ 삥, 체크 선만가능");
	gotoxy(104,18);
	printf("※ 마지막은 콜,다이 만");
	for(int i=0;i<8;i++){
		gotoxy(20,3+i);
		for(int j=0;j<30;j++)
			if(Result[R][i][j])
				printf("■");
			else
				printf("  ");
	}

}
