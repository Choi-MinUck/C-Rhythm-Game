#define _CRT_SECURE_NO_WARNINGS

//*********************************************//
//**		  RhyThmRinMin Generator		 **//
//**		     ���븰�� ������			 **//
//**	  	       1307 �迵��			     **//
//*********************************************//

// 77�� ��, ����ο� �׽�Ʈ ���� �־�����ϴ�
// �� �ٲ㰡�� �׽�Ʈ ���ּ���!

#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include "MapReader.h"
#include "resource.h"
#include "Digitalv.h"
#include "Pan.h"
#include "networking.h"

#pragma comment(lib,"Winmm.lib")

#define TRUE 1
#define FALSE 0
#define OEM 2

// �Է� ���� Ű ����
#define KEY_A 0x41
#define KEY_B 0x53
#define KEY_C 0x4C
#define KEY_D 0xBA

// ���� ��Ʈ ���� (ms)
#define READ_NOTE_MIL 580

// ���� �Ķ���� ����
MCI_OPEN_PARMS m_mciOpenParms;
MCI_PLAY_PARMS m_mciPlayParms;
DWORD m_dwDeviceID;
MCI_OPEN_PARMS mciOpen;
MCI_PLAY_PARMS mciPlay;
int dwID;

// ����� ��Ʈ
HFONT font;
HFONT font_combo;
HFONT font_score;
HFONT font_acc;
RECT rt;

// ��Ʈ ����
int note_width = 0;
int note_height = 0;

// Ÿ�̸� ����
int PlayTimer = 0;
int map_playing = FALSE;
int beg_time = 0;

// ���� �̹��� ��ġ ����
int Start_Pos = 150;

// �Ͻ����� üũ ����
int IsPause = FALSE;

// Ű �Է� üũ ����
int KeyDown[4] = { 0 };
int KeyLight[4] = { 0 };

// �ܼ� ������ ������ ���� ���� ����
static HWND hWnd;
static HINSTANCE hInst;

// �� �̸�
// ���� ����ּ���!
//char* NoteMapName = "Jinjin - pi (Jinjin) [4K Beginner].map";          // 00:33 ���̵� ����
//char* NoteMapName = "Halozy - K.O.K.O.R.O (Nepuri) [EZ].map";			// 01:17 ���̵� ��
//char* NoteMapName = "banshi - Fading Star(banshi-Remix) (erlinadewi-) [Beginner].map"; // 02:33 ���̵� ��
//char* NoteMapName = "sakuzyo - Scattered Rose (Starry-) [Normal].map"; // 00:57 ���̵� ��
char* NoteMapName = "sakuzyo - Scattered Rose (Starry-) [Another].map";
//char* NoteMapName = "gems - Gems Pack 13 - LN Master 5th (gemboyong) [69 ^ - Randy Mortimer, Penguin (Pinnacle Remix)].map"; 
																		 // 01:35 ���̵� �ֻ�, ������ �׽�Ʈ�� ��. �� �����ּ���!

// ������ �ڵ� ����
HANDLE TimerHandle;
HANDLE KeyPressHandle;
HANDLE PanTimeHandle;
HANDLE GameHandle;

//*/////////////////////////////////// = �Լ� ���� = ///////////////////////////////////*//

//// �⺻ �Լ� ////
int UserDataLoad();						// �ְ� ���� �ҷ�����
void UserDataWrite(int score);			// �ְ� ���� ���
void ClearCursor();						// Ŀ�� �ȶ߰�
void* M_Timer(void* a);					// ���� Ÿ�̸� (������ �Լ�)
void KeyDownProcess(int k);				// Ű �Է� ó��. k : Ű ��ġ (0,1,2,3)
void HitNote();							// ��Ʈ Ŭ�� üũ
void* CheckKeyPress(void* a);			// Ű �������� üũ (������ �Լ�)
void* Pan_Timer(void* a);				// ���� ǥ�� �ð� Ÿ�̸�
void GameOver();						// ���� ���� �Լ� (���� ������ ����)
void* GameStatusCheck(void* a);			// ���� ���� üũ (������ �Լ�)
void RT_Change(RECT* rts, int a, int b, int c, int d); // ���� ��ġ ����
inline void Render();					// ���� �÷��̽� ������ �Լ�
void CheckPlayStatus();					//�÷��� ���� üũ
void LoadMap(char* MapName);			// �� �ҷ�����

//// ���� �Լ� ////
void MusicLoad();						// ���� �ҷ�����
void MusicOpen(char* FileName);			// ���ϴ� ���� �ҷ�����
void MusicPlay(int ID, int repeat);		// ���� ���
void MusicPause(int ID);				// ���� �Ͻ�����
void MusicResume(int ID);				// ���� �ٽ� ���
void MusicStop(int ID);					// ���� ������ ����
void SetMusicPosition(DWORD dwPosition);// ���� �÷��� ��ġ ����

//// ���� ���� �Լ� ////
void PlayMap();							// ���� �÷���
void ReadyRender();						// �غ�ȭ�� ������
void ReadyView();						// �غ� ��
void PauseView();						// �Ͻ����� ��
void Restart();							// ����� �Լ�
void GameOverView();					// ���� ���� ��
void* ScoreIncVisual(int* Score_VS);	// ���ھ� �ε巴�� �ö󰡰� �ϱ� (������ �Լ�)
void Result();							// ��� ���

//*/////////////////////////////////// = ����� ���� = ///////////////////////////////////*//

int UserDataLoad() //�ְ� ���� �ҷ�����
{
	FILE* fp;
	int index = 0;
	int data;
	
	fp = fopen("score.txt", "r");
	while (fscanf(fp, "%d", &data) != EOF) {
		index = (int)data;
	}
	fclose(fp);
	return index; // �ְ����� ����
}

void UserDataWrite(int score) // �ְ� ���� ���
{
	FILE* fp;

	fp = fopen("score.txt", "w");
	fprintf(fp, "%d\n", score); // ���ھ� ���
	fclose(fp);
}

void ClearCursor() { // Ŀ�� �ȶ߰�
	CONSOLE_CURSOR_INFO c = { 0 };
	c.dwSize = 1;
	c.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
}

void *M_Timer(void* a) // ���� Ÿ�̸� (������ �Լ�)
{
	beg_time = GetTickCount64() + 80; // ���� �ð�
	while (map_playing) {
		PlayTimer = GetTickCount64() - beg_time; // �÷��� Ÿ�� = ����ð� - ���۽ð�
		if (IsPause) {  // �Ͻ����� ���� ���
			int now_time = GetTickCount64(); // ������ �ð� ���
			while (IsPause)
			{
				continue;
			}
			now_time = GetTickCount64() - now_time;
			beg_time = beg_time + now_time; // �÷��� Ÿ�� ����� ���� ���� �ð� ����
		}
	}
	return;
}

void KeyDownProcess(int k) // Ű �Է� ó��. k : Ű ��ġ (0,1,2,3)
{
	for (int i = -140; i <= 140; i++) {
		if (PlayTimer + i < 0) continue; //�迭 ���� ������ ���� �ڵ�

		if (NotePoints[PlayTimer + i][k] == 1) //��Ʈ�� ���� ���
		{
			int abs_v = abs(i);						//������ ���ϱ� ���� ������ ���밪

			if (abs_v <= 16) IncKool();				//���� 16ms �̳� : Kool
			else if (abs_v <= 35) IncCool();		//���� 35ms �̳� : Cool
			else if (abs_v <= 85) IncGood();		//���� 85ms �̳� : Good
			else if (abs_v <= 140) IncBad();		//���� 140ms �̳� : Bad
			NotePoints[PlayTimer + i][k] = 0;		//��Ʈ ģ������ ǥ��
			ImagePoints[PlayTimer + i][k] = 0;		//Render �Լ����� �ش� ��Ʈ ǥ�� ����
			break;
		}
	}
}

void HitNote() // ��Ʈ Ŭ�� üũ
{
	if (KeyDown[0]) {    // ���� �ش� Ű�� ������ ���
		KeyDownProcess(0);   // ���̶�� KeyDownProcess�Լ� ����
		KeyDown[0] = FALSE;     //  KeyDown ������ �������� �ǵ���
								//  �� �����°��� �����ϱ� ���ؼ�
	}
	if (KeyDown[1]) { // ���� ������
		KeyDownProcess(1);
		KeyDown[1] = FALSE;
	}
	if (KeyDown[2]) {
		KeyDownProcess(2);
		KeyDown[2] = FALSE;
	}
	if (KeyDown[3]) {
		KeyDownProcess(3);
		KeyDown[3] = FALSE;
	}

	Rate = (floor(Combo / 10) * 0.85) + 1; // �޺� ���ʽ� ����
}

void* CheckKeyPress(void* a) // Ű �������� üũ (������ �Լ�)
{
	while (map_playing)
	{
	  //0x0000 : ������ ���� ���� ���� ȣ�� �������� �������� ���� ����
	  //0x0001 : ������ ���� ���� �ְ� ȣ�� �������� �������� ���� ����
	  //0x8000 : ������ ���� ���� ���� ȣ�� �������� �����ִ� ����
	  //0x8001 : ������ ���� ���� �ְ� ȣ�� �������� �����ִ� ����

		if (GetAsyncKeyState(KEY_A) & 0x0001) //Key A�� ������ ���
			KeyDown[0] = TRUE;
		if (GetAsyncKeyState(KEY_B) & 0x0001)
			KeyDown[1] = TRUE;
		if (GetAsyncKeyState(KEY_C) & 0x0001)
			KeyDown[2] = TRUE;
		if (GetAsyncKeyState(KEY_D) & 0x0001)
			KeyDown[3] = TRUE;
		HitNote();
	}
	return;
}

void* Pan_Timer(void* a) //���� ǥ�� �ð� Ÿ�̸�
{
	while (map_playing) {
		if (PanVisualTimer > 0) // ���� Ÿ�̸Ӱ� 0�̻� (300ms �̳� Ű�� ������ ���)
		{
			PanVisualTimer--; //Ÿ�̸Ӹ� 1 ���δ�
		}
		Sleep(1); //1ms ����
		while (IsPause) continue;
	}
}

void GameOver() // ���� ���� �Լ� (���� ������ ����)
{
	map_playing = FALSE; //������ ������Ŵ
}

void* GameStatusCheck(void* a) // ���� ���� üũ (������ �Լ�)
{
	ClearCursor();
	while (map_playing) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) //[ESC] Ű�� ���� ���
			PauseView(); // ���� �Ͻ�����

		if ((HP <= 0)&& (M_NoFail == 0))  //HP�� 0�� �ɰ�� (HP�� 0���� �������)
			GameOver(); // ���ӿ��� (���� ����)

		for (int i = PlayTimer - 130; i >= PlayTimer - 230; i--) // Miss ����
		{
			if (i < 0) continue; // �迭�� ���� �Լ� (�迭 �ε����� -�� ���°� ����)
			for (int j = 0; j < 4; j++)
			{
				if (NotePoints[i][j] == 1) // ġ�� ���� ��Ʈ�� ���� ���
				{
					ImagePoints[i][j] = -1; // ��Ʈ ġ�� ���������� ǥ��
					NotePoints[i][j] = -1; // Render���� ǥ�õ��� �ʰ� ��ũ
					IncMiss(); //Miss ���� (Pan.c �Լ�)
				}
			}
		}

		while (IsPause) continue;
	}
	return;
}

void RT_Change(RECT* rts, int a, int b, int c, int d) // ���� ��ġ ����
{
	// Render �Լ����� ���� ǥ�ýÿ� ��ġ �����ϴ� �Լ�
	rts->left = a;
	rts->top = b;

	rts->right = c;
	rts->bottom = d;
}

inline void Render() // ���� �÷��̽� ������ �Լ�
{
	hWnd = GetConsoleWindow();		// �ڽ��� �ܼ� �����츦 ������
	hInst = GetModuleHandle(NULL);	// �ܼ� �ڵ鷯 ������
	HDC hDC, hMemDC;		//ǥ���� �޸� �Ҵ�
	static HDC hBackDC;		//ǥ���� �������� �Լ� �Ҵ� (���� ���۸�)
	HBITMAP hBackBitmap, hOldBitmap, hNewBitmap; //ǥ���� ��Ʈ��
	BITMAP Bitmap;	//��Ʈ�� ����

	hDC = GetDC(hWnd);

	hMemDC = CreateCompatibleDC(hDC);
	hBackDC = CreateCompatibleDC(hDC);

	hBackBitmap = CreateCompatibleBitmap(hDC, 1000, 500);		//������ �� �ȷ�Ʈ ũ�� ����
	hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);	//ǥ�õǴ� ��Ʈ��

	hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1)); //��Ʈ �̹��� �ε�
	GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
	SelectObject(hMemDC, hNewBitmap);

	note_width = Bitmap.bmWidth;
	note_height = Bitmap.bmHeight;
	for (int i = PlayTimer; i < PlayTimer + READ_NOTE_MIL; i++) // ȭ�鿡 �׸� ��Ʈ ����
	{
		if (PlayTimer < 0) //Ÿ�̸Ӱ� �۵����� ������ ������
			break;
		for (int j = 0; j < 4; j++) // �� Ű���� Ȯ��
		{
			if (ImagePoints[i][j] == 1) // ��ġ�� ��Ʈ�� �������
			{
				/*if (i == TimingPoints[0].time + (int)(floor(TimingPoints[0].beatLength / 4.0 * k))) k++; // ����ȭ �ڵ忴���� ���� ����������..
				else continue;*/
				GdiTransparentBlt(hBackDC, j* Bitmap.bmWidth + Start_Pos, (i - PlayTimer - 500)*(-0.9), Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
				// �ܳ�Ʈ ǥ��
			}
			else if (ImagePoints[i][j] == 2) {
				GdiTransparentBlt(hBackDC, j * Bitmap.bmWidth + Start_Pos, (i - PlayTimer - 500) * (-0.9), Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
				// �ճ�Ʈ ǥ��
			}
		}
	}
	DeleteObject(hNewBitmap); // ��Ʈ ������Ʈ ����

	hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3)); // ������ �ε�
	GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
	SelectObject(hMemDC, hNewBitmap);
	GdiTransparentBlt(hBackDC, Start_Pos, 450, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228)); // �׸���
	DeleteObject(hNewBitmap);

	hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4)); // ��Ʈ ���� ���м� �ε�
	GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
	SelectObject(hMemDC, hNewBitmap);
	GdiTransparentBlt(hBackDC, Start_Pos, 0, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228)); // ���� �׸���
	GdiTransparentBlt(hBackDC, Start_Pos+(note_width*4), 0, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228)); // ������ �׸���
	DeleteObject(hNewBitmap);

	hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_HP_BAR)); // HP�� �ε�
	GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
	SelectObject(hMemDC, hNewBitmap);
	GdiTransparentBlt(hBackDC, Start_Pos + (note_width * 4) + 10, 450-floor((double)((__int64)Bitmap.bmHeight * (HP)) / 1.5), Bitmap.bmWidth, floor(((__int64)Bitmap.bmHeight*(HP))/1.5), hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
			// 5*5 ��Ʈ���� HP�� ���� �ø��� ���δ�.
			// ���� ������ �̹��� ���̸� �����ؼ� �ִ��� �ڿ������� �ٹδ�
	DeleteObject(hNewBitmap);

	if (PanVisualTimer > 0) { // ���� �̹����� ǥ���ؾ� �Ұ�� (300ms �̳��� ��Ʈ�� �������)
		if (LastPan == KOOL) { // �ֱٿ� ģ ��Ʈ�� Kool �ΰ��
			hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_KOOL)); // Kool �̹��� �ε�
			GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
			SelectObject(hMemDC, hNewBitmap);
			GdiTransparentBlt(hBackDC, 235, 350, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228)); // �׸���
			DeleteObject(hNewBitmap);
		}
		else if (LastPan == COOL) { // ���� ����
			hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_COOL));
			GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
			SelectObject(hMemDC, hNewBitmap);
			GdiTransparentBlt(hBackDC, 235, 350, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
			DeleteObject(hNewBitmap);
		}
		else if (LastPan == GOOD) {
			hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_GOOD));
			GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
			SelectObject(hMemDC, hNewBitmap);
			GdiTransparentBlt(hBackDC, 235, 350, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
			DeleteObject(hNewBitmap);
		}
		else if (LastPan == BAD) {
			hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_BAD));
			GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
			SelectObject(hMemDC, hNewBitmap);
			GdiTransparentBlt(hBackDC, 235, 350, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
			DeleteObject(hNewBitmap);
		}
		else if (LastPan == MISS) {
			hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_MISS));
			GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
			SelectObject(hMemDC, hNewBitmap);
			GdiTransparentBlt(hBackDC, 235, 350, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228));
			DeleteObject(hNewBitmap);
		}
	}

	char buf[100]; // ǥ���� ���� ����
	SetTextColor(hBackDC, RGB(255, 255, 255)); // �ؽ�Ʈ ���� ����
	SetBkMode(hBackDC, TRANSPARENT); // ���� ���� ����

	if (Combo > 0) { // �޺��� 0 �̻��ϰ�� (�޺��� 0�� ��� ǥ�� ���ϱ� ����)
		(HFONT)SelectObject(hBackDC, font_combo); // ��Ʈ�� font_combo�� ����

		sprintf(buf, "%d", Combo); //�޺� buffer�� ����
		RT_Change(&rt, 10, 110, 500, 160); // ���� ��ġ ����
		DrawTextA(hBackDC, buf, -1, &rt, DT_CENTER); // ���� ��ο�
	}

	(HFONT)SelectObject(hBackDC, font_score); // ���ھ� ��Ʈ�� ����

	//���� ����
	sprintf(buf, "%08d", Score); // ���� ǥ��
	RT_Change(&rt, 10, 10, 950, 160);
	DrawTextA(hBackDC, buf, -1, &rt, DT_RIGHT);

	(HFONT)SelectObject(hBackDC, font_acc);

	sprintf(buf, "%.2lf%%\n", Accurate); // ��Ȯ�� ǥ��
	RT_Change(&rt, 10, 40, 950, 160);
	DrawTextA(hBackDC, buf, -1, &rt, DT_RIGHT);

	BitBlt(hDC, 0, 0, 1000, 500, hBackDC, 0, 0, SRCCOPY); // ��׶��忡�� �׸� �̹��� �ֿܼ� �׸���

	DeleteObject(SelectObject(hBackDC, hBackBitmap)); // ����� ������Ʈ ����
	DeleteObject(hNewBitmap);
	DeleteDC(hBackDC);
	DeleteDC(hMemDC);

	ReleaseDC(hWnd, hDC); // ������ �Ҵ� ����
}

void CheckPlayStatus() //�÷��� ���� üũ
{
	if (PlayTimer >= Last_Note_pos + 5000) { // ��Ʈ�� ������ ���
		map_playing = FALSE; // ���� ������
	}
}

void LoadMap(char * MapName) // �� �ҷ�����
{
	char buf[500] = { NULL }; // ���� ����
	if (LoadMapFile(MapName) == 0)
	{
		printf("Load Failed.");
	}
	sprintf(buf, "%s - %s (%s) [%s]", M_MetaData.Artist, M_MetaData.Title, M_MetaData.Creator, M_MetaData.Version); 
			// ���� ������, �� �̸�, �� ������, ���� buffer�� ����
	SetConsoleTitle(buf); //���� ���� ����
}

void MusicLoad() // ���� �ҷ�����
{
	mciOpen.lpstrElementName = M_General.AudioFilename; // �ʿ��� ������ ����� ���� �ҷ�����
	mciOpen.lpstrDeviceType = "mpegvideo";

	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
		(DWORD)(LPVOID)&mciOpen); // ��� Ŀ�ǵ� ������

	dwID = mciOpen.wDeviceID; // ��� ����̽� ID ����
}

void MusicOpen(char * FileName) // ���ϴ� ���� �ҷ�����
{
	mciOpen.lpstrElementName = FileName; // ���� ���
	mciOpen.lpstrDeviceType = "mpegvideo";

	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
		(DWORD)(LPVOID)&mciOpen); // ��� Ŀ�ǵ� ������

	dwID = mciOpen.wDeviceID;
}

void MusicPlay(int ID, int repeat) // ���� ���
{
	if(repeat == 0) // �ݺ����� �������
		mciSendCommand(ID, MCI_PLAY, NULL, (DWORD)(LPVOID)&m_mciPlayParms); // �ݺ����� �ʰ� ���
	else
		mciSendCommand(ID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&m_mciPlayParms); // ������ ������ �ݺ���
}

void MusicPause(int ID) // ���� �Ͻ�����
{
	mciSendCommand(ID, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay);
}

void MusicResume(int ID) // ���� �ٽ� ���
{
	mciSendCommandW(ID, MCI_RESUME, 0, NULL);
}

void MusicStop(int ID) // ���� ������ ����
{
	mciSendCommandW(ID, MCI_CLOSE, 0, NULL);
}

void SetMusicPosition(DWORD dwPosition) // ���� �÷��� ��ġ ����
{
	MCI_SEEK_PARMS mciSeekParams;
	mciSeekParams.dwTo = dwPosition;
	mciSendCommand(dwID, MCI_SEEK, MCI_TO, (DWORD)(LPVOID)&mciSeekParams); // ���� ��ġ ����
}

void PlayMap() // ���� �÷���
{
	// ��Ʈ ����
	font = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Aller");
	font_combo = CreateFont(48, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Slant");
	font_score = CreateFont(36, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Slant");
	font_acc = CreateFont(32, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Cunia");

	char buf[500] = { NULL };

	// ������ ����
	TimerHandle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)M_Timer, 0, 0, NULL); // Ÿ�̸� ������
	KeyPressHandle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)CheckKeyPress, 0, 0, NULL); // Ű�Է� ������
	PanTimeHandle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Pan_Timer, 0, 0, NULL); // ���� ǥ�� Ÿ�̸� ������
	GameHandle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)GameStatusCheck, 0, 0, NULL); // ������ ���� ���� ������

	map_playing = TRUE; // ���� �÷��� ����
	while (map_playing) {
		CheckPlayStatus(); // ���� ���� üũ
		Render(); // ���� ȭ�� ������ ����

		// ������ �Ͻ����� �������
		if (IsPause) MusicPause(dwID); //���� �Ͻ�����
		else MusicResume(dwID);        //���� ���
		while (IsPause) continue;    // �Ͻ������� �Լ� �Ͻ�����
	}
}

void ReadyRender() // �غ�ȭ�� ������
{
	// Render �Լ��� �ߺ��Ǵ� �ּ��� ���� ����
	hWnd = GetConsoleWindow();
	hInst = GetModuleHandle(NULL);
	HDC hDC, hMemDC;
	static HDC hBackDC;
	HBITMAP hBackBitmap, hOldBitmap, hNewBitmap;
	BITMAP Bitmap;

	hDC = GetDC(hWnd);

	hMemDC = CreateCompatibleDC(hDC);
	hBackDC = CreateCompatibleDC(hDC);

	hBackBitmap = CreateCompatibleBitmap(hDC, 1280, 720);
	hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

	char buf[200];
	(HFONT)SelectObject(hBackDC, font_score); // ��Ʈ ����

	SetTextColor(hBackDC, RGB(255, 255, 255)); // �� ����
	SetBkMode(hBackDC, TRANSPARENT);

	sprintf(buf, "Map: %s [%s]", M_MetaData.Title, M_MetaData.Version); // �� �̸�, ���� ���
	RT_Change(&rt, 20, 30, 900, 90);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	sprintf(buf, "Artist: %s", M_MetaData.Artist); // ���� ������ ���
	RT_Change(&rt, 20, 90, 700, 120);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	sprintf(buf, "Press [ENTER] To Start"); // ���ʹ����� ���� ���
	RT_Change(&rt, 20, 180, 700, 240);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	sprintf(buf, "Your Best Score : %d", UserDataLoad()); // ���� �ְ� ���ھ� ���
	RT_Change(&rt, 20, 400, 700, 430);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	BitBlt(hDC, 0, 0, 1280, 720, hBackDC, 0, 0, SRCCOPY); // �̹��� �ֿܼ� �׸���

	DeleteObject(SelectObject(hBackDC, hBackBitmap));
	DeleteDC(hBackDC);
	DeleteDC(hMemDC);

	ReleaseDC(hWnd, hDC);
}

void ReadyView() // �غ� ��
{
	// ��Ʈ ����
	font_score = CreateFont(36, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Slant");

	SetMusicPosition(M_General.PreviewTime); //���� ���۽ð��� ������ Ÿ�ӿ� ����
	MusicPlay(dwID, 1); // ���� ���, ����
	ReadyRender(); // �غ� ������ ǥ��
	while (1)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x0001) { // ����Ű�� ������ ������
			break;
		}
	}

	MusicPause(dwID); // ������ ���� ��� �Ͻ�����
	// �� �Լ� ���� �� ���� ������ ����
}

void PauseView() // �Ͻ����� ��
{
	IsPause = 1; // �Ͻ����� Ȱ��ȭ
	// Render �Լ��� �ߺ��Ǵ� �ּ��� ���� ����

	hWnd = GetConsoleWindow();
	hInst = GetModuleHandle(NULL);
	HDC hDC, hMemDC;
	static HDC hBackDC;
	HBITMAP hBackBitmap, hOldBitmap, hNewBitmap;
	BITMAP Bitmap;

	while (1) {
		hDC = GetDC(hWnd);

		hMemDC = CreateCompatibleDC(hDC);
		hBackDC = CreateCompatibleDC(hDC);

		hBackBitmap = CreateCompatibleBitmap(hDC, 1280, 720);
		hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

		char buf[200];
		(HFONT)SelectObject(hBackDC, font_score);

		SetTextColor(hBackDC, RGB(255, 255, 255));
		SetBkMode(hBackDC, TRANSPARENT);

		sprintf(buf, "Pause..");
		RT_Change(&rt, 20, 30, 900, 90);
		DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

		sprintf(buf, "Press [ESC] to Resume");
		RT_Change(&rt, 20, 90, 900, 150);
		DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

		BitBlt(hDC, 0, 0, 1280, 720, hBackDC, 0, 0, SRCCOPY);

		DeleteObject(SelectObject(hBackDC, hBackBitmap));
		DeleteDC(hBackDC);
		DeleteDC(hMemDC);

		ReleaseDC(hWnd, hDC);

		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) { // [ESC] Ű�� ������ ���
			IsPause = 0; // �Ͻ����� ����
			break; //�Լ� ������
		}
	}
	return; // �ٽ� ���� �Լ��� ���ư�
}

void Restart() // ����� �Լ�
{
	// ���� �ʱ�ȭ
	HP = 100;
	PlayTimer = 0;
	Score = 0;
	PanVisualTimer = 0;
	LastPan = -1;
	Combo = 0;
	MaxCombo = 0;
	Rate = 1.0;
	HPUpRate = 1.0;
	HPDownRate = 1.0;
	Accurate = 100.0;

	Start_Pos = 150;

	for (int i = 0; i < 5; i++)
		TargetPass[i] = 0;

	ClearCursor();
	LoadMap(NoteMapName);   // �� ��ε�
	MusicLoad();			// ���� ��ε�

	SetMusicPosition(0);	// ���� ��ġ ��������
	MusicPlay(dwID, 0);     // ���� ���� ���� ���
	map_playing = TRUE;		// �÷��� �� �Լ� Ȱ��ȭ
	PlayMap();				// �� �÷���
	Result();				// ��� ���
}

void GameOverView() // ���� ���� ��
{
	// Render �Լ��� �ߺ��Ǵ� �ּ��� ���� ����
	hWnd = GetConsoleWindow();
	hInst = GetModuleHandle(NULL);
	HDC hDC, hMemDC;
	static HDC hBackDC;
	HBITMAP hBackBitmap, hOldBitmap, hNewBitmap;
	BITMAP Bitmap;

	hDC = GetDC(hWnd);

	hMemDC = CreateCompatibleDC(hDC);
	hBackDC = CreateCompatibleDC(hDC);

	hBackBitmap = CreateCompatibleBitmap(hDC, 1280, 720);
	hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

	char buf[200];
	(HFONT)SelectObject(hBackDC, font_score);

	SetTextColor(hBackDC, RGB(255, 255, 255));
	SetBkMode(hBackDC, TRANSPARENT);

	sprintf(buf, "You are Dead..");
	RT_Change(&rt, 20, 30, 900, 90);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	sprintf(buf, "Press [R] to retry");
	RT_Change(&rt, 20, 90, 900, 150);
	DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

	BitBlt(hDC, 0, 0, 1280, 720, hBackDC, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hBackDC, hBackBitmap));
	DeleteDC(hBackDC);
	DeleteDC(hMemDC);

	ReleaseDC(hWnd, hDC);
	
	while (1) {
		if (GetAsyncKeyState(0x52) & 0x0001) {  // [R] Ű�� �������
			Restart();							// �����
			break;
		}
	}
}

void* ScoreIncVisual(int* Score_VS) // ���ھ� �ε巴�� �ö󰡰� �ϱ� (������ �Լ�)
{
	for (int i = 0; i < Score; i++) // ���ھ� ��ŭ �ݺ�
	{
		(*Score_VS)++;	// ���ھ� ++
		if(i%((Score/1400)+1) == 0) Sleep(2); //���� �ʿ��� Score_VS ���
			// ��, ���� �ʰ� �Ǿ߸� ���ڰ� �ݿ��� �Ǿ� �ε巴�� �ö󰡴� ������ �ȴ�
	}
}

void Result() // ��� ���
{
	if (HP <= 0) { // HP�� 0 ����, �׾ ������ ���
		MusicPause(dwID);	// ���� ����
		GameOverView();		// ���ӿ��� �� ���
	}
	else { // �����ؼ� ���� ���
		if(UserDataLoad() < Score) UserDataWrite(Score); // �ְ������� ���� ���
		char* buffs[200] = {NULL};
		sprintf(buffs, "{\"Kool\":%d,\"Cool\":%d,\"Good\":%d,\"Bad\":%d,\"Miss\":%d,\"MaxCombo\":%d,\"Acc\":%.2lf}"
			, TargetPass[KOOL], TargetPass[COOL], TargetPass[GOOD], TargetPass[BAD], TargetPass[MISS], MaxCombo, (double)Accurate);
		send_score("player2", Score, M_MetaData.BeatmapID, buffs);
		MusicOpen("applause.mp3");		// ���ȭ�� BGM �ε�
		MusicPlay(dwID, 0);		// ���� ���

		int Score_View = 0; // ���̴� ���ھ� ����
		HANDLE IncHandle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ScoreIncVisual, &Score_View, 0, NULL);
				// ���ھ� �ε巴�� �����ϱ� ���� ������ ����

		// ��Ʈ �ε�
		HFONT fnt_score = CreateFont(36, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Cunia");
		HFONT fnt_result = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Cunia");
		HFONT fnt_combo = CreateFont(36, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Cunia");

		while (1) {
			// Render �Լ��� �ߺ��Ǵ� �ּ��� ���� ����
			hWnd = GetConsoleWindow();
			hInst = GetModuleHandle(NULL);
			HDC hDC, hMemDC;
			static HDC hBackDC;
			HBITMAP hBackBitmap, hOldBitmap, hNewBitmap;
			BITMAP Bitmap;

			hDC = GetDC(hWnd);

			hMemDC = CreateCompatibleDC(hDC);
			hBackDC = CreateCompatibleDC(hDC);

			hBackBitmap = CreateCompatibleBitmap(hDC, 1280, 720);
			hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

			char buf[200];
			(HFONT)SelectObject(hBackDC, fnt_score);

			SetTextColor(hBackDC, RGB(174, 189, 242));
			SetBkMode(hBackDC, TRANSPARENT);

			if (Score == Score_View) {
				if(Accurate >= 80.0) hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SCORE_A)); // A ����
				else if (Accurate >= 70.0) hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SCORE_B)); // B ����
				else if (Accurate >= 60.0) hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SCORE_C)); // C ����
				else hNewBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SCORE_D)); // D ����
				GetObject(hNewBitmap, sizeof(BITMAP), &Bitmap);
				SelectObject(hMemDC, hNewBitmap);
				GdiTransparentBlt(hBackDC, 580, 0, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 0, 228)); // �׸���
				DeleteObject(hNewBitmap);
			}

			sprintf(buf, "Map: %s [%s]", M_MetaData.Title, M_MetaData.Version);
			RT_Change(&rt, 20, 30, 900, 90);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

			sprintf(buf, "Score: %07d", Score_View); // ���ھ� ��� (�ε巴��)
			RT_Change(&rt, 20, 90, 700, 120);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

			(HFONT)SelectObject(hBackDC, fnt_result);

			SetTextColor(hBackDC, RGB(255,255,255));
			SetBkMode(hBackDC, TRANSPARENT);

			sprintf(buf, "Kool: %d\n", TargetPass[KOOL]); //Kool, Cool... ���
			RT_Change(&rt, 75, 150, 500, 200);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);
			sprintf(buf, "Cool: %d\n", TargetPass[COOL]);
			RT_Change(&rt, 300, 150, 500, 200);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);
			sprintf(buf, "Good: %d\n", TargetPass[GOOD]);
			RT_Change(&rt, 75, 190, 500, 240);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);
			sprintf(buf, "Bad: %d\n", TargetPass[BAD]);
			RT_Change(&rt, 300, 190, 500, 240);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);
			sprintf(buf, "Miss: %d\n", TargetPass[MISS]);
			RT_Change(&rt, 75, 230, 500, 270);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

			(HFONT)SelectObject(hBackDC, fnt_combo);

			SetTextColor(hBackDC, RGB(255, 255, 255));
			SetBkMode(hBackDC, TRANSPARENT);

			sprintf(buf, "MaxCombo: %d\n", MaxCombo);	// �ִ� �޺�
			RT_Change(&rt, 75, 300, 500, 370);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

			sprintf(buf, "Accuracy: %.2lf%%\n", (double)Accurate); // ��Ȯ�� ���
			RT_Change(&rt, 75, 350, 500, 400);
			DrawTextA(hBackDC, buf, -1, &rt, DT_LEFT);

			BitBlt(hDC, 0, 0, 1280, 720, hBackDC, 0, 0, SRCCOPY); // �̹��� ǥ��

			DeleteObject(SelectObject(hBackDC, hBackBitmap));
			DeleteDC(hBackDC);
			DeleteDC(hMemDC);

			ReleaseDC(hWnd, hDC);
		}
	}
}

int main() {
	//* ���۾� *//
	ClearCursor();					// Ŀ�� �Ⱥ��̰�
	LoadMap(NoteMapName);			// �� �ε�
	MusicLoad();					// ���� �ε�
	ReadyView();					// �غ� ��
	
	//* ���� �÷��� *//
	SetMusicPosition(0);			// ���� ��ġ ó������
	MusicPlay(dwID, 0);				// ���� ���
	PlayMap();						// ���� �÷���

	//* ��� ��� *//
	Result();						// ��� ȭ��

	return 0;
}