#pragma once
#include <graphics.h>
#include <tchar.h>

// 游戏全局状态
enum GameStatus {
	STATUS_WAITING,    // 等待开始
	STATUS_PLAYING,    // 游戏进行中
	STATUS_PAUSED,     // 游戏暂停
	STATUS_PLAYER_WIN, // 玩家获胜
	STATUS_AI_WIN,      // AI获胜
	STATUS_AI_THINKING // AI正在思考中
};

// 全局控制变量声明
extern int playerScore;
extern int aiScore;
extern bool isGameOver;
extern GameStatus g_gameStatus;

// 缩放函数
void UpdateZoomScale(HWND hwnd);

// 绘制面板
void drawControlPanel();

