#include "GameAI.h"

GameAI::GameAI() {
	aiColor = CHESS_WHITE;
	playerColor = CHESS_BLACK;
}

// 双方局势评分
void GameAI::calculateBestMove(const ChessBoard& board, int& outRow, int& outCol) {
	int maxScore = -1;
	outRow = 7; outCol = 7; // 默认下中心

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.getPiece(i, j) == EMPTY) {
				// 计算AI得分
				int aiScore = evaluateMove(board, i, j, aiColor);
				// 计算玩家得分
				int playerScore = evaluateMove(board, i, j, playerColor);

				// 综合评分
				int totalScore = aiScore + playerScore;

				if (totalScore > maxScore) {
					maxScore = totalScore;
					outRow = i;
					outCol = j;
				}
			}

		}
	}
}

// 棋局逻辑
int GameAI::evaluateMove(const ChessBoard& board, int r, int c, int color) {
	int totalScore = 0;
	int dx[4] = { 0,1,1,1 };
	int dy[4] = { 1,0,-1,1 };

	for (int i = 0; i < 4; i++) {
		int count = countContinuous(board, r, c, dx[i], dy[i], color);

		// 评分表
		if (count >= 5) totalScore += 100000; // 连五
		else if (count == 4) totalScore += 10000; // 活四/冲四
		else if (count == 3) totalScore += 1000; // 活三
		else if (count == 2) totalScore += 100; // 活二
		else totalScore += 10;
	}
	return totalScore;
}

// 计算连子数量
int GameAI::countContinuous(const ChessBoard& board, int r, int c, int dr, int dc, int color) {
	int count = 1;
	// 正向数
	int x = r + dr, y = c + dc;
	while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board.getPiece(x, y) == color) {
		count++; x += dr; y += dc;
	}
	// 反向数
	x = r - dr; y = c - dc;
	while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board.getPiece(x, y) == color) {
		count++; x -= dr; y -=dc;
	}
	return count;
}