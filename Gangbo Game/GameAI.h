#pragma once
#include "ChessBoard.h"
#include <vector>

class GameAI
{
public:
	GameAI();
	// AI 思考主函数
	void calculateBestMove(const ChessBoard& board, int& outRow, int& outCol);

private:
	int aiColor;
	int playerColor;

	// 局势评估函数
	int evaluateMove(const ChessBoard& board, int r, int c, int color);

	// 模型预测函数
	int countContinuous(const ChessBoard& board, int r, int c, int dr, int dc, int color);
};

