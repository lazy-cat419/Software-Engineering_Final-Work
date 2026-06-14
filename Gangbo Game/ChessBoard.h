#pragma once
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <iostream>

//  全局常量定义
const int WINDOW_W = 800;      // 窗口总宽度
const int WINDOW_H = 600;      // 窗口总高度
const int BOARD_SIZE = 15;     // 15x15 棋盘大小
const int MARGIN = 40;         // 棋盘留白边距
const int GRID_SIZE = 30;      // 每个格子的像素大小 (30px)

// 数据结构定义
enum PointState {
	EMPTY = 0,
	CHESS_BLACK = 1, // 玩家黑子
	CHESS_WHITE = 2  // AI白子
};

//棋盘类定义
class ChessBoard
{
private:
	int boardData[BOARD_SIZE][BOARD_SIZE]; // 核心棋盘数据数组
	void drawPiece(int bx, int by, int color); // 私有绘制单个棋子函数

public:
	ChessBoard();       // 构造函数
	void initBoard();   // 初始化棋盘
	void render();      // 渲染整个棋盘与所有棋子

	// 坐标转换与状态控制接口
	bool convertCoordinates(int mx, int my, int& bx, int& by);
	bool isValidMove(int bx, int by);
	void setPiece(int bx, int by, int color);
	int getPiece(int bx, int by) const;

	// 判断胜负
	bool checkWin(int bx, int by, int color) const;
};

