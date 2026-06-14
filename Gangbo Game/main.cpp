#include "ChessBoard.h"
#include "main.h"
#include "GameAI.h"
#include <vector>

//定义结构体
struct ChessStep {
	int row;
	int col;
	int color;
};

std::vector<ChessStep> gameHistory;

// 分配内存的全局变量定义
int playerScore = 0;
int aiScore = 0;
int undoCount = 0;
bool isGameOver = false;
GameStatus g_gameStatus = STATUS_PLAYING; // 默认设为进行中

// 放大到全屏
bool isFullScreen = false;
float g_zoomScale = 1.0f; // 缩放比例默认1.0
int g_offsetX = 0; // X 不偏移
int g_offsetY = 0;// Y 不偏移

// 绘制右侧控制面板与得分
void drawControlPanel() {
	// 绘制右侧背景的分割线
	setlinecolor(RGB(150, 150, 150));
	setlinestyle(PS_SOLID, 1);
	line(500, 0, 500, WINDOW_H);

	// 设置文字基本样式
	settextstyle(22, 0, _T("Arial"), 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE);
	setbkmode(TRANSPARENT);

	// 顶部得分与状态卡片
	int scoreX1 = 525, scoreY1 = 25;
	int scoreX2 = 775, scoreY2 = 125;
	int scoreW = scoreX2 - scoreX1; 

	setfillcolor(RGB(60, 64, 67)); 
	setlinecolor(RGB(80, 84, 87));
	fillroundrect(scoreX1, scoreY1, scoreX2, scoreY2, 8, 8);
	settextcolor(WHITE); 

	// 渲染得分比分
	TCHAR scoreStr[100];
	_stprintf_s(scoreStr, _T("玩家 (黑) %d  VS  AI (白) %d"), playerScore, aiScore);
	int strW1 = textwidth(scoreStr);
	int strH1 = textheight(scoreStr);
	outtextxy(scoreX1 + (scoreW - strW1) / 2, 45, scoreStr); // 计算水平居中坐标

	// 渲染游戏状态
	TCHAR statusStr[50] = _T("");
	switch (g_gameStatus) {
	case STATUS_WAITING:    _tcscpy_s(statusStr, _T("游戏状态: 等待开局")); break;
	case STATUS_PLAYING:    _tcscpy_s(statusStr, _T("游戏状态: 你的回合")); break;
	case STATUS_AI_THINKING:_tcscpy_s(statusStr, _T("游戏状态: AI思考中...")); break;
	case STATUS_PAUSED:     _tcscpy_s(statusStr, _T("游戏状态: 游戏已暂停")); break;
	default:                _tcscpy_s(statusStr, _T("游戏状态: 对局结束")); break;
	}
	int strW2 = textwidth(statusStr);
	outtextxy(scoreX1 + (scoreW - strW2) / 2, 85, statusStr); // 计算水平居中坐标

	// [悔棋] 按钮
	int hX1 = 530, hY1 = 160;
	int hX2 = 750, hY2 = 210;
	setfillcolor(RGB(235, 235, 235));
	setlinecolor(RGB(180, 180, 180));
	fillroundrect(hX1, hY1, hX2, hY2, 5, 5);
	settextcolor(RGB(50, 50, 50)); 

	TCHAR hText[] = _T("悔棋");
	int hTextW = textwidth(hText);
	int hTextH = textheight(hText);
	outtextxy(hX1 + (220 - hTextW) / 2, hY1 + (50 - hTextH) / 2, hText); // 居中

	// [开始] 按钮
	int bX1 = 530, bY1 = 250;
	int bX2 = 630, bY2 = 300;
	setfillcolor(RGB(235, 235, 235));
	setlinecolor(RGB(180, 180, 180));
	fillroundrect(bX1, bY1, bX2, bY2, 5, 5);

	TCHAR bText[] = _T("开始");
	int bTextW = textwidth(bText);
	int bTextH = textheight(bText);
	outtextxy(bX1 + (100 - bTextW) / 2, bY1 + (50 - bTextH) / 2, bText);

	// [暂停] 按钮
	int zX1 = 650, zY1 = 250;
	int zX2 = 750, zY2 = 300;
	fillroundrect(zX1, zY1, zX2, zY2, 5, 5); 

	TCHAR zText[] = _T("暂停");
	int zTextW = textwidth(zText);
	int zTextH = textheight(zText);
	outtextxy(zX1 + (100 - zTextW) / 2, zY1 + (50 - zTextH) / 2, zText);

	// [结束当前对局] 按钮
	int eX1 = 530, eY1 = 340;
	int eX2 = 750, eY2 = 390;
	setfillcolor(RGB(180, 50, 50)); 
	setlinecolor(RGB(140, 30, 30));
	fillroundrect(eX1, eY1, eX2, eY2, 5, 5);
	settextcolor(WHITE);

	TCHAR eText[] = _T("结束当前对局");
	int eTextW = textwidth(eText);
	int eTextH = textheight(eText);
	outtextxy(eX1 + (220 - eTextW) / 2, eY1 + (50 - eTextH) / 2, eText);
}

int main() {
	// 初始化窗口
	initgraph(WINDOW_W, WINDOW_H);
	BeginBatchDraw();

	// 实例化棋盘类
	ChessBoard board;
	board.initBoard(); // 开局先清空棋盘

	// 实例化AI对象
	DWORD aiStartTime = 0;
	GameAI ai;

	while (!isGameOver) {
		// 刷底色
		setbkcolor(WHITE);
		cleardevice();

		// 渲染棋盘与右侧UI
		board.render();
		drawControlPanel();

		// AI 思考计时器
		if (g_gameStatus == STATUS_AI_THINKING) {
			if (GetTickCount() - aiStartTime >= 3000) {
				int aiRow, aiCol;
				ai.calculateBestMove(board, aiRow, aiCol);
				board.setPiece(aiRow, aiCol, CHESS_WHITE);
				gameHistory.push_back({ aiRow,aiCol, CHESS_WHITE }); // AI落子记录
				// 判定AI是否获胜
				if (board.checkWin(aiRow, aiCol, CHESS_WHITE)) {
					board.render();
					FlushBatchDraw();
					aiScore++;
					MessageBox(GetHWnd(), _T("AI获胜"), _T("对局结束"), MB_OK);
					board.initBoard();
					gameHistory.clear(); // 清空记录

					undoCount = 0;
					g_gameStatus = STATUS_WAITING;
				}
				else {
					g_gameStatus = STATUS_PLAYING; // 切换回玩家回合
				}
			}
		}

		FlushBatchDraw();

		// 消息处理
		ExMessage msg;
		if (peekmessage(&msg, EM_MOUSE)) {
			if (msg.message == WM_LBUTTONDOWN) {
				int mx = msg.x;
				int my = msg.y;

				// [开始] 按钮交互
				if (mx >= 530 && mx <= 630 && my >= 250 && my <= 300) {
					board.initBoard();// 清空键盘
					gameHistory.clear();
					undoCount = 0; // 新对局重置悔棋次数
					g_gameStatus = STATUS_PLAYING; // 切换游戏状态为进行中
					isGameOver = false; // 重置结束标志
					continue;
				}

				// [暂停] 按钮交互
				if (mx >= 650 && mx <= 750 && my >= 250 && my <= 300) {
					HWND hwnd = GetHWnd(); // 获取窗口句柄

					if (g_gameStatus == STATUS_PLAYING) {
						g_gameStatus = STATUS_PAUSED; // 正在玩则暂停
						//弹出提示框
						MessageBox(hwnd, _T("当前游戏已暂停！ 点击确认恢复对局。"), _T("暂停提示"), MB_OK | MB_ICONINFORMATION);
						g_gameStatus = STATUS_PLAYING;
					}
					else if (g_gameStatus == STATUS_PAUSED) {
						g_gameStatus = STATUS_PLAYING;
					}
					continue;
				}

				// [结束当前对局] 按钮
				if (mx >= 530 && mx <= 750 && my >= 340 && my <= 390) {
					HWND hwnd = GetHWnd(); // 获取窗口句柄用于弹出提示框
					int result = MessageBox(hwnd, _T("确定要认输并结束当前对局吗？")
						, _T("提示"), MB_YESNO | MB_ICONQUESTION);
					if (result == IDYES) {
						board.initBoard(); // 清空棋盘
						gameHistory.clear();
						undoCount = 0; //认输/重置时归零
						aiScore++; // AI得分
						g_gameStatus = STATUS_WAITING; // 玩家状态切换为等待中
					}
					continue;
				}

				// [悔棋] 按钮
				if (mx >= 530 && mx <= 750 && my >= 160 && my <= 210) {
					if (g_gameStatus == STATUS_PLAYING) {
						if (undoCount >= 1) {
							MessageBox(GetHWnd(), _T("本局对战悔棋次数已用完"), _T("悔棋限制"), MB_OK | MB_ICONINFORMATION);
							continue;
						}
						// 撤销玩家和AI的上一步棋
						if (gameHistory.size() >= 2) {
							// 弹出并抹除AI上一步
							ChessStep aiStep = gameHistory.back();
							board.setPiece(aiStep.row, aiStep.col, EMPTY);
							gameHistory.pop_back();

							// 弹出并抹除玩家上一步
							ChessStep playerStep = gameHistory.back();
							board.setPiece(playerStep.row, playerStep.col,  EMPTY);
							gameHistory.pop_back();
							// 悔棋成功后，计数器自增
							undoCount++;
						}
						else {
							MessageBox(GetHWnd(), _T("棋盘上还没有足够的棋子可以悔棋！"), _T("提示"), MB_OK | MB_ICONWARNING);
						}
					}
					else {
						MessageBox(GetHWnd(), _T("当前状态下无法悔棋！"), _T("提示"), MB_OK | MB_ICONWARNING);
					}
					continue;
				}

				// 棋盘落子逻辑：只有对局在 STATUS_PLAYING 状态下才允许落子
				if (g_gameStatus == STATUS_PLAYING) {
					int bx, by;
					if (board.convertCoordinates(mx, my, bx, by)) {
						if (board.isValidMove(bx, by)) {
							// 玩家落子
							board.setPiece(bx, by, CHESS_BLACK);
							gameHistory.push_back({ bx, by, CHESS_BLACK });// 记录玩家这步棋

							// 判定玩家是否获胜
							if (board.checkWin(bx, by, CHESS_BLACK)) {
								board.render(); FlushBatchDraw();
								playerScore++;
								MessageBox(GetHWnd(), _T("太强了，你赢了！"), _T("恭喜"), MB_OK);
								gameHistory.clear();
								board.initBoard();
								g_gameStatus = STATUS_WAITING;
								continue;
							}
							else {
								// 玩家没赢，开启 AI 思考模式
								aiStartTime = GetTickCount(); // 记录起始时间
								g_gameStatus = STATUS_AI_THINKING; // 切换状态
							}
						}
					}
				}
			}
		}
		Sleep(4);
	}

	EndBatchDraw();
	closegraph();
	return 0;
}