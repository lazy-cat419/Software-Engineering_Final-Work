/**
 * ================================================================================
 * @file        ChessBoard.cpp 
 * @brief       五子棋棋盘数据结构与胜负逻辑判定
 * @author      毛胤荃
 * @date        2026-06-15
 * * @details     本模块定义了 15x15 的棋盘数据矩阵。
 * - 核心职责：棋盘重置、落子状态更新、落子合法性检查（越界/重叠）。
 * - 核心算法：基于最后落子点 (x, y) 的【横、竖、撇、捺】八方向局部扫描算法，
 * 在 O(1) 时间复杂度内检测是否有同色五子连珠。
 * * @note        棋盘状态值定义：0 - 空白, 1 - 黑子, 2 - 白子
 * ================================================================================
 */

#include "ChessBoard.h"

// ChessBoard()实现
ChessBoard::ChessBoard() {
	initBoard();
}

// InitBoard()实现
void ChessBoard::initBoard() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			boardData[i][j] = EMPTY;
		}
	}
}

// render()实现
void ChessBoard::render() {
	// 绘制网格线(棕色木质感)
	setlinecolor(RGB(200, 150, 100));
	setlinestyle(PS_SOLID, 1);

	for (int i = 0; i < BOARD_SIZE; i++) {
		// 画横线
		line(MARGIN, MARGIN + i * GRID_SIZE, MARGIN + (BOARD_SIZE - 1) * GRID_SIZE, MARGIN + i * GRID_SIZE);
		// 画竖线
		line(MARGIN + i * GRID_SIZE, MARGIN, MARGIN + i * GRID_SIZE, MARGIN + (BOARD_SIZE - 1) * GRID_SIZE);
	}

	// 扫描整个棋盘数组，画出黑子白子
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (boardData[i][j] == CHESS_BLACK) {
				drawPiece(i, j, CHESS_BLACK); // 渲染黑子
			}
			else if (boardData[i][j] == CHESS_WHITE) {
				drawPiece(i, j, CHESS_WHITE); // 渲染白子
			}
		}
	}
}

// drawPiece()实现，绘制高光立体棋子
void ChessBoard::drawPiece(int bx, int by, int color) {
	// 根据数组下标计算绝对像素坐标
	int pixelX = MARGIN + by * GRID_SIZE;
	int pixelY = MARGIN + bx * GRID_SIZE;
	int radius = 13; // 棋子半径

	if (color == CHESS_BLACK) {
		// 绘制黑棋：深黑底色 + 左上角灰色微型高光圈
		setlinecolor(RGB(30, 30, 30));
		setfillcolor(RGB(15, 15, 15));
		fillcircle(pixelX, pixelY, radius);

		// 绘制高光
		setlinecolor(RGB(100, 100, 100));
		setfillcolor(RGB(90, 90, 90));
		fillcircle(pixelX - 3, pixelY - 3, 3);
	}
	else if (color == CHESS_WHITE) {
		// 绘制白棋：纯白底色 + 左上角纯白反光点
		setlinecolor(RGB(200, 200, 200));
		setfillcolor(RGB(245, 245, 245));
		fillcircle(pixelX, pixelY, radius);

		// 绘制高光
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		fillcircle(pixelX - 3, pixelY - 3, 2);
	}
}

bool ChessBoard::convertCoordinates(int mx, int my, int& bx, int& by) {
	// 计算鼠标点击位置的偏移
	float relX = (float)(mx - MARGIN);
	float relY = (float)(my - MARGIN);

	// 估算出最近的格子坐标 
	int col = (int)(relX / GRID_SIZE + 0.5f);
	int row = (int)(relY / GRID_SIZE + 0.5f);

	// 边界检查
	if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {

		// 计算鼠标点击点离最近交点的实际像素距离
		int targetPixelX = MARGIN + col * GRID_SIZE;
		int targetPixelY = MARGIN + row * GRID_SIZE;

		// 勾股定理算距离
		int dx = mx - targetPixelX;
		int dy = my - targetPixelY;
		float dist = sqrt((float)(dx * dx + dy * dy));

		// 棋子半径：12
		if (dist < 12.0f) {
			bx = row;
			by = col;
			return true;
		}
	}
	return false;
}

// 判断坐标合法性
bool ChessBoard::isValidMove(int bx, int by) {
	if (bx >= 0 && bx < BOARD_SIZE && by >= 0 && by < BOARD_SIZE) {
		return (boardData[bx][by] == EMPTY);
	}
	return false;
}

// 落子
void ChessBoard::setPiece(int bx, int by, int color) {
	if (bx >= 0 && bx < BOARD_SIZE && by >= 0 && by < BOARD_SIZE) {
		boardData[bx][by] = color;
	}
}

// 获取当前格子的棋子状态
int ChessBoard::getPiece(int bx, int by) const {
	if (bx >= 0 && bx < BOARD_SIZE && by >= 0 && by < BOARD_SIZE) {
		return boardData[bx][by];
	}
	return EMPTY;
}

// 胜负判定
bool ChessBoard::checkWin(int bx, int by, int color) const {
	// 4个方向的步进向量
	// dx/dy 代表：横向向右、 竖向向下、 斜向右下、 反斜向右上
	int dx[4] = { 0, 1, 1, 1 };
	int dy[4] = { 1, 0 , -1, 1 };

	// 循环检查4个方向
	for (int i = 0; i < 4; i++) {
		int count = 1; // 计数器

		// 沿正方向延伸扫描
		int x = bx + dx[i];
		int y = by + dy[i];
		while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && boardData[x][y] == color) {
			count++;
			x += dx[i];
			y += dy[i];
		}

		// 沿反方向延伸扫描
		x = bx - dx[i];
		y = by - dy[i];
		while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && boardData[x][y] == color) {
			count++;
			x -= dx[i];
			y -= dy[i];
		}

		// 核心判断逻辑：只要有任何一个方向连续同色棋子达成5个，直接判赢
		if (count >= 5) {
			return true;
		}
	}
	return false; // 四个方向都没连成5个，对局继续
}