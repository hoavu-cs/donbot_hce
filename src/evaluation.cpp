#include "chess.hpp"
#include "evaluation.hpp"
#include <tuple>
#include <unordered_map>
#include <cstdint>
#include <map>
#include <omp.h> 

using namespace chess; 

/*--------------------------------------------------------------------------
    Tables, Constants, and Global Variables
------------------------------------------------------------------------*/

// Constants for the evaluation function
const int PAWN_VALUE = 120;
const int KNIGHT_VALUE = 320; 
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 5000;

// Pawn hash table
std::unordered_map<std::uint64_t, std::unordered_map<std::uint64_t, int>> whitePawnHashTable;
std::unordered_map<std::uint64_t, std::unordered_map<std::uint64_t, int>> blackPawnHashTable;

// Knight piece-square tables
const int whiteKnightTableMid[64] = {
    -105, -30, -58, -33, -17, -28, -30,  -90,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  15,  -16,
     -13,   4,  16,  13,  20,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

const int blackKnightTableMid[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  20,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  15,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -30, -58, -33, -17, -28, -30,  -90,
};

const int whiteKnightTableEnd[64] = {
     -29, -51, -23, -15, -22, -18, -50,  -64,
     -42, -20, -10,  -5,  -2, -20, -23,  -44,
     -23,  -3,  -1,  15,  10,  -3, -20,  -22,
     -18,  -6,  16,  25,  16,  17,   4,  -18,
     -17,   3,  22,  22,  22,  11,   8,  -18,
     -24, -20,  10,   9,  -1,  -9, -19,  -41,
     -25,  -8, -25,  -2,  -9, -25, -24,  -52,
     -58, -38, -13, -28, -31, -27, -63,  -99,
};

const int blackKnightTableEnd[64] = {
     -58, -38, -13, -28, -31, -27, -63,  -99,
     -25,  -8, -25,  -2,  -9, -25, -24,  -52,
     -24, -20,  10,   9,  -1,  -9, -19,  -41,
     -17,   3,  22,  22,  22,  11,   8,  -18,
     -18,  -6,  16,  25,  16,  17,   4,  -18,
     -23,  -3,  -1,  15,  10,  -3, -20,  -22,
     -42, -20, -10,  -5,  -2, -20, -23,  -44,
     -29, -51, -23, -15, -22, -18, -50,  -64,
};

// Bishop piece-square tablesint 
const int whiteBishopTableMid[64] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  25,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  20,  26,  34,  20,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

const int blackBishopTableMid[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  20,  26,  34,  20,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  25,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

const int whiteBishopTableEnd[64] = {
    -23,  -9, -23,  -5,  -9, -16,  -5, -17,
    -14, -18,  -7,  -1,   4,  -9, -15, -27,
    -12,  -3,   8,  10,  13,   3,  -7, -15,
     -6,   3,  13,  19,   7,  10,  -3,  -9,
     -3,   9,  12,   9,  14,  10,   3,   2,
      2,  -8,   0,  -1,  -2,   6,   0,   4,
     -8,  -4,   7, -12,  -3, -13,  -4, -14,
    -14, -21, -11,  -8,  -7,  -9, -17, -24,
};

const int blackBishopTableEnd[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

// Pawn piece-square tables for White in the middle game
const int whitePawnTableMid[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    -35,  -1, -20, -35, -35,  24,  38, -22,
    -26,  -4,  3,  0,  0,   3,  33, -12,
    -27,  -2,  5,  25,  25,   5,  10, -25,
    -14,  13,   6,  21,  23,  12,  17, -23,
     -6,   7,  26,  31,  65,  56,  25, -20,
     98, 134,  61,  95,  68, 126,  34, -11,
      0,   0,   0,   0,   0,   0,   0,   0,
};

// Pawn piece-square tables for Black in the middle game
const int blackPawnTableMid[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     98, 134,  61,  95,  68, 126,  34, -11,
     -6,   7,  26,  31,  65,  56,  25, -20,
    -14,  13,   6,  21,  23,  12,  17, -23,
    -27,  -2,  5,  25,  25,   5,  10, -25,
    -26,  -4,  3,  0,  0,   3,  33, -12,
    -35,  -1, -20, -35, -35,  24,  38, -22,
      0,   0,   0,   0,   0,   0,   0,   0,
};

const int whitePawnTableEnd[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   8,   8,  10,  13,   0,   2,  -7,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
     32,  24,  13,   5,  -2,   4,  17,  17,
     94, 100,  85,  67,  56,  53,  82,  84,
    178, 173, 158, 134, 147, 132, 165, 187,
      0,   0,   0,   0,   0,   0,   0,   0,
};

const int blackPawnTableEnd[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int weakPawnPenaltyTable[64] = {
	 0,   0,   0,   0,   0,   0,   0,   0,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
	 0,   0,   0,   0,   0,   0,   0,   0
};

const int whitePassedPawnTable[64] = {
	 0,   0,   0,   0,   0,   0,   0,   0,
	20,  20,  20,  20,  20,  20,  20,  20,
	20,  20,  20,  20,  20,  20,  20,  20,
	32,  32,  32,  32,  32,  32,  32,  32,
	56,  56,  56,  56,  56,  56,  56,  56,
	92,  92,  92,  92,  92,  92,  92,  92,
   140, 140, 140, 140, 140, 140, 140, 140,
	 0,   0,   0,   0,   0,   0,   0,   0
};


const int blackPassedPawnTable[64] = {
	 0,   0,   0,   0,   0,   0,   0,   0,
   140, 140, 140, 140, 140, 140, 140, 140,
	92,  92,  92,  92,  92,  92,  92,  92,
	56,  56,  56,  56,  56,  56,  56,  56,
	32,  32,  32,  32,  32,  32,  32,  32,
	20,  20,  20,  20,  20,  20,  20,  20,
	20,  20,  20,  20,  20,  20,  20,  20,
	 0,   0,   0,   0,   0,   0,   0,   0
};


// Rook piece-square tables
const int whiteRookTableMid[64] = {
    -19, -13,   1,  17,  16,   7, -37, -26,
    -44, -16, -20,  -9,  -1,  11,  -6, -71,
    -45, -25, -16, -17,   3,   0,  -5, -33,
    -36, -26, -12,  -1,   9,  -7,   6, -23,
    -24, -11,   7,  26,  24,  35,  -8, -20,
     -5,  19,  26,  36,  17,  45,  61,  16,
     27,  32,  58,  62,  80,  67,  26,  44,
     32,  42,  32,  51,  63,   9,  31,  43,
};

const int blackRookTableMid[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

const int whiteRookTableEnd[64] = {
     -9,   2,   3,  -1,  -5, -13,   4, -20,
     -6,  -6,   0,   2,  -9,  -9, -11,  -3,
     -4,   0,  -5,  -1,  -7, -12,  -8, -16,
      3,   5,   8,   4,  -5,  -6,  -8, -11,
      4,   3,  13,   1,   2,   1,  -1,   2,
      7,   7,   7,   5,   4,  -3,  -5,  -3,
     11,  13,  13,  11,  -3,   3,   8,   3,
     13,  10,  18,  15,  12,  12,   8,   5,
};

const int blackRookTableEnd[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

// Queen piece-square tables
const int whiteQueenTableMid[64] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

const int blackQueenTableMid[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

const int whiteQueenTableEnd[64] = {
    -33, -28, -22, -43,  -5, -32, -20, -41,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -18,  28,  19,  47,  31,  34,  39,  23,
      3,  22,  24,  45,  57,  40,  57,  36,
    -20,   6,   9,  49,  47,  35,  19,   9,
    -17,  20,  32,  41,  58,  25,  30,   0,
     -9,  22,  22,  27,  27,  19,  10,  20,
};


const int blackQueenTableEnd[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

const int whiteKingTableMid[64] = {
    -15,  35,  25, -54,  -5, -28,  35,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};

const int blackKingTableMid[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  35,  25, -54, -5, -28,  35,   14,
};

const int whiteKingTableEnd[64] = {
    -53, -34, -21, -11, -28, -14, -24, -43,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -18,  -4,  21,  24,  27,  23,   9, -11,
     -8,  22,  24,  27,  26,  33,  26,   3,
     10,  17,  23,  15,  20,  45,  44,  13,
    -12,  17,  14,  17,  17,  38,  23,  11,
    -74, -35, -18, -18, -11,  15,   4, -17,
};

const int blackKingTableEnd[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

const std::unordered_map<int, std::vector<int>> adjSquares = {
    {0, {1, 8, 9}}, 
    {1, {0, 2, 8, 9, 10}}, 
    {2, {1, 3, 9, 10, 11}}, 
    {3, {2, 4, 10, 11, 12}}, 
    {4, {3, 5, 11, 12, 13}}, 
    {5, {4, 6, 12, 13, 14}}, 
    {6, {5, 7, 13, 14, 15}}, 
    {7, {6, 14, 15}}, 
    {8, {0, 1, 9, 16, 17}}, 
    {9, {0, 1, 2, 8, 10, 16, 17, 18}}, 
    {10, {1, 2, 3, 9, 11, 17, 18, 19}}, 
    {11, {2, 3, 4, 10, 12, 18, 19, 20}}, 
    {12, {3, 4, 5, 11, 13, 19, 20, 21}}, 
    {13, {4, 5, 6, 12, 14, 20, 21, 22}}, 
    {14, {5, 6, 7, 13, 15, 21, 22, 23}}, 
    {15, {6, 7, 14, 22, 23}}, 
    {16, {8, 9, 17, 24, 25}}, 
    {17, {8, 9, 10, 16, 18, 24, 25, 26}}, 
    {18, {9, 10, 11, 17, 19, 25, 26, 27}}, 
    {19, {10, 11, 12, 18, 20, 26, 27, 28}}, 
    {20, {11, 12, 13, 19, 21, 27, 28, 29}}, 
    {21, {12, 13, 14, 20, 22, 28, 29, 30}}, 
    {22, {13, 14, 15, 21, 23, 29, 30, 31}}, 
    {23, {14, 15, 22, 30, 31}}, 
    {24, {16, 17, 25, 32, 33}}, 
    {25, {16, 17, 18, 24, 26, 32, 33, 34}}, 
    {26, {17, 18, 19, 25, 27, 33, 34, 35}}, 
    {27, {18, 19, 20, 26, 28, 34, 35, 36}}, 
    {28, {19, 20, 21, 27, 29, 35, 36, 37}}, 
    {29, {20, 21, 22, 28, 30, 36, 37, 38}}, 
    {30, {21, 22, 23, 29, 31, 37, 38, 39}}, 
    {31, {22, 23, 30, 38, 39}}, 
    {32, {24, 25, 33, 40, 41}}, 
    {33, {24, 25, 26, 32, 34, 40, 41, 42}}, 
    {34, {25, 26, 27, 33, 35, 41, 42, 43}}, 
    {35, {26, 27, 28, 34, 36, 42, 43, 44}}, 
    {36, {27, 28, 29, 35, 37, 43, 44, 45}}, 
    {37, {28, 29, 30, 36, 38, 44, 45, 46}}, 
    {38, {29, 30, 31, 37, 39, 45, 46, 47}}, 
    {39, {30, 31, 38, 46, 47}}, 
    {40, {32, 33, 41, 48, 49}}, 
    {41, {32, 33, 34, 40, 42, 48, 49, 50}}, 
    {42, {33, 34, 35, 41, 43, 49, 50, 51}}, 
    {43, {34, 35, 36, 42, 44, 50, 51, 52}}, 
    {44, {35, 36, 37, 43, 45, 51, 52, 53}}, 
    {45, {36, 37, 38, 44, 46, 52, 53, 54}}, 
    {46, {37, 38, 39, 45, 47, 53, 54, 55}}, 
    {47, {38, 39, 46, 54, 55}}, 
    {48, {40, 41, 49, 56, 57}}, 
    {49, {40, 41, 42, 48, 50, 56, 57, 58}}, 
    {50, {41, 42, 43, 49, 51, 57, 58, 59}}, 
    {51, {42, 43, 44, 50, 52, 58, 59, 60}}, 
    {52, {43, 44, 45, 51, 53, 59, 60, 61}}, 
    {53, {44, 45, 46, 52, 54, 60, 61, 62}}, 
    {54, {45, 46, 47, 53, 55, 61, 62, 63}}, 
    {55, {46, 47, 54, 62, 63}}, 
    {56, {48, 49, 57}}, 
    {57, {48, 49, 50, 56, 58}}, 
    {58, {49, 50, 51, 57, 59}}, 
    {59, {50, 51, 52, 58, 60}}, 
    {60, {51, 52, 53, 59, 61}}, 
    {61, {52, 53, 54, 60, 62}}, 
    {62, {53, 54, 55, 61, 63}}, 
    {63, {54, 55, 62}}
};


const Bitboard a1 = Bitboard::fromSquare(0);
const Bitboard b1 = Bitboard::fromSquare(1);
const Bitboard c1 = Bitboard::fromSquare(2);
const Bitboard d1 = Bitboard::fromSquare(3);
const Bitboard e1 = Bitboard::fromSquare(4);
const Bitboard f1 = Bitboard::fromSquare(5);
const Bitboard g1 = Bitboard::fromSquare(6);
const Bitboard h1 = Bitboard::fromSquare(7);

const Bitboard a2 = Bitboard::fromSquare(8);
const Bitboard b2 = Bitboard::fromSquare(9);
const Bitboard c2 = Bitboard::fromSquare(10);
const Bitboard d2 = Bitboard::fromSquare(11);
const Bitboard e2 = Bitboard::fromSquare(12);
const Bitboard f2 = Bitboard::fromSquare(13);
const Bitboard g2 = Bitboard::fromSquare(14);
const Bitboard h2 = Bitboard::fromSquare(15);

const Bitboard a3 = Bitboard::fromSquare(16);
const Bitboard b3 = Bitboard::fromSquare(17);
const Bitboard c3 = Bitboard::fromSquare(18);
const Bitboard d3 = Bitboard::fromSquare(19);
const Bitboard e3 = Bitboard::fromSquare(20);
const Bitboard f3 = Bitboard::fromSquare(21);
const Bitboard g3 = Bitboard::fromSquare(22);
const Bitboard h3 = Bitboard::fromSquare(23);

const Bitboard a4 = Bitboard::fromSquare(24);
const Bitboard b4 = Bitboard::fromSquare(25);
const Bitboard c4 = Bitboard::fromSquare(26);
const Bitboard d4 = Bitboard::fromSquare(27);
const Bitboard e4 = Bitboard::fromSquare(28);
const Bitboard f4 = Bitboard::fromSquare(29);
const Bitboard g4 = Bitboard::fromSquare(30);
const Bitboard h4 = Bitboard::fromSquare(31);

const Bitboard a5 = Bitboard::fromSquare(32);
const Bitboard b5 = Bitboard::fromSquare(33);
const Bitboard c5 = Bitboard::fromSquare(34);
const Bitboard d5 = Bitboard::fromSquare(35);
const Bitboard e5 = Bitboard::fromSquare(36);
const Bitboard f5 = Bitboard::fromSquare(37);
const Bitboard g5 = Bitboard::fromSquare(38);
const Bitboard h5 = Bitboard::fromSquare(39);

const Bitboard a6 = Bitboard::fromSquare(40);
const Bitboard b6 = Bitboard::fromSquare(41);
const Bitboard c6 = Bitboard::fromSquare(42);
const Bitboard d6 = Bitboard::fromSquare(43);
const Bitboard e6 = Bitboard::fromSquare(44);
const Bitboard f6 = Bitboard::fromSquare(45);
const Bitboard g6 = Bitboard::fromSquare(46);
const Bitboard h6 = Bitboard::fromSquare(47);

const Bitboard a7 = Bitboard::fromSquare(48);
const Bitboard b7 = Bitboard::fromSquare(49);
const Bitboard c7 = Bitboard::fromSquare(50);
const Bitboard d7 = Bitboard::fromSquare(51);
const Bitboard e7 = Bitboard::fromSquare(52);
const Bitboard f7 = Bitboard::fromSquare(53);
const Bitboard g7 = Bitboard::fromSquare(54);
const Bitboard h7 = Bitboard::fromSquare(55);

const Bitboard a8 = Bitboard::fromSquare(56);
const Bitboard b8 = Bitboard::fromSquare(57);
const Bitboard c8 = Bitboard::fromSquare(58);
const Bitboard d8 = Bitboard::fromSquare(59);
const Bitboard e8 = Bitboard::fromSquare(60);
const Bitboard f8 = Bitboard::fromSquare(61);
const Bitboard g8 = Bitboard::fromSquare(62);
const Bitboard h8 = Bitboard::fromSquare(63);

/*------------------------------------------------------------------------
    Helper Functions
------------------------------------------------------------------------*/

// Calculate material imbalance
int materialImbalance(const Board& board) {
    Bitboard whitePawns = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard whiteKnights = board.pieces(PieceType::KNIGHT, Color::WHITE);
    Bitboard whiteBishops = board.pieces(PieceType::BISHOP, Color::WHITE);
    Bitboard whiteRooks = board.pieces(PieceType::ROOK, Color::WHITE);
    Bitboard whiteQueens = board.pieces(PieceType::QUEEN, Color::WHITE);

    Bitboard blackPawns = board.pieces(PieceType::PAWN, Color::BLACK);
    Bitboard blackKnights = board.pieces(PieceType::KNIGHT, Color::BLACK);
    Bitboard blackBishops = board.pieces(PieceType::BISHOP, Color::BLACK);
    Bitboard blackRooks = board.pieces(PieceType::ROOK, Color::BLACK);
    Bitboard blackQueens = board.pieces(PieceType::QUEEN, Color::BLACK);

    int whiteMaterial = whitePawns.count() * PAWN_VALUE +
                        whiteKnights.count() * KNIGHT_VALUE +
                        whiteBishops.count() * BISHOP_VALUE +
                        whiteRooks.count() * ROOK_VALUE +
                        whiteQueens.count() * QUEEN_VALUE;

    int blackMaterial = blackPawns.count() * PAWN_VALUE + 
                        blackKnights.count() * KNIGHT_VALUE +
                        blackBishops.count() * BISHOP_VALUE +
                        blackRooks.count() * ROOK_VALUE +
                        blackQueens.count() * QUEEN_VALUE;

    return whiteMaterial - blackMaterial;
}

// Clear the pawn hash table
void clearPawnHashTable() {
    whitePawnHashTable = {};
    blackPawnHashTable = {};
}

//End game special heuristics to avoid illusory material advantage.
bool knownDraw(const Board& board) {

    // Two kings are a draw
    if (board.us(Color::WHITE).count() == 1 && board.us(Color::BLACK).count() == 1) {
        return true;
    }

    int whitePawnCount = board.pieces(PieceType::PAWN, Color::WHITE).count();
    int whiteKnightCount = board.pieces(PieceType::KNIGHT, Color::WHITE).count();
    int whiteBishopCount = board.pieces(PieceType::BISHOP, Color::WHITE).count();
    int whiteRookCount = board.pieces(PieceType::ROOK, Color::WHITE).count();
    int whiteQueenCount = board.pieces(PieceType::QUEEN, Color::WHITE).count();

    int blackPawnCount = board.pieces(PieceType::PAWN, Color::BLACK).count();
    int blackKnightCount = board.pieces(PieceType::KNIGHT, Color::BLACK).count();
    int blackBishopCount = board.pieces(PieceType::BISHOP, Color::BLACK).count();
    int blackRookCount = board.pieces(PieceType::ROOK, Color::BLACK).count();
    int blackQueenCount = board.pieces(PieceType::QUEEN, Color::BLACK).count();

    // If there are pawns on the board, it is not a draw
    if (whitePawnCount > 0 || blackPawnCount > 0) {
        return false;
    }

    // Cannot checkmate with 2 knights or a knight 
    if (whitePawnCount == 0 && whiteKnightCount <= 2 && whiteBishopCount == 0 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount <= 0 && blackBishopCount == 0 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 0 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount <= 2 && blackBishopCount == 0 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    // Cannot checkmate with one bishop
    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 1 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 0 && blackBishopCount == 0 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 0 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 0 && blackBishopCount == 1 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    // A RK vs RB or a QK vs QB endgame is drawish
    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 0 && whiteRookCount == 1 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 1 && blackBishopCount == 0 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 0 && whiteRookCount == 1 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 0 && blackBishopCount == 1 && blackRookCount == 0 && blackQueenCount == 0) {
        return true;
    }

    if (whitePawnCount == 0 && whiteKnightCount == 1 && whiteBishopCount == 0 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 0 && blackBishopCount == 0 && blackRookCount == 1 && blackQueenCount == 0) {
        return true;
    }

    if (whitePawnCount == 0 && whiteKnightCount == 0 && whiteBishopCount == 1 && whiteRookCount == 0 && whiteQueenCount == 0 &&
        blackPawnCount == 0 && blackKnightCount == 0 && blackBishopCount == 0 && blackRookCount == 1 && blackQueenCount == 0) {
        return true;
    }

    return false;

}


// Return game phase 0-24 for endgame to opening
int gamePhase (const Board& board) {
    int phase = board.pieces(PieceType::KNIGHT, Color::WHITE).count() + board.pieces(PieceType::KNIGHT, Color::BLACK).count() +
                     board.pieces(PieceType::BISHOP, Color::WHITE).count() + board.pieces(PieceType::BISHOP, Color::BLACK).count() +
                     board.pieces(PieceType::ROOK, Color::WHITE).count() * 2 + board.pieces(PieceType::ROOK, Color::BLACK).count() * 2 +
                     board.pieces(PieceType::QUEEN, Color::WHITE).count() * 4 + board.pieces(PieceType::QUEEN, Color::BLACK).count() * 4;

    return phase;
}

// Function to visualize a bitboard for debugging.
void bitBoardVisualize(const Bitboard& board) {
    std::uint64_t boardInt = board.getBits();

    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << std::endl;
        }
        if (boardInt & (1ULL << i)) {
            std::cout << "1 ";
        } else {
            std::cout << "0 ";
        }
    }
    std::cout << std::endl;
}

// Generate a bitboard mask for the specified file.
Bitboard generateFileMask(int file) {
    constexpr Bitboard fileMasks[] = {
        0x0101010101010101ULL, // File A
        0x0202020202020202ULL, // File B
        0x0404040404040404ULL, // File C
        0x0808080808080808ULL, // File D
        0x1010101010101010ULL, // File E
        0x2020202020202020ULL, // File F
        0x4040404040404040ULL, // File G
        0x8080808080808080ULL  // File H
    };

    if (file >= 0 && file < 8) {
        return Bitboard(fileMasks[file]);
    }
    
    return Bitboard(0ULL);
}

// Generate a bitboard mask for the specified rank.
constexpr Bitboard generateHalfMask(int startRank, int endRank) {
    Bitboard mask = 0ULL;
    for (int rank = startRank; rank <= endRank; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int squareIndex = rank * 8 + file;
            mask |= (1ULL << squareIndex);
        }
    }
    return mask;
}


// Check if the given square is a passed pawn
bool isPassedPawn(int sqIndex, Color color, const Bitboard& theirPawns) {
    int file = sqIndex % 8;
    int rank = sqIndex / 8;

    Bitboard theirPawnsCopy = theirPawns;

    while (theirPawnsCopy) {
        int sqIndex2 = theirPawnsCopy.lsb();  
        int file2 = sqIndex2 % 8;
        int rank2 = sqIndex2 / 8;

        if (std::abs(file - file2) <= 1 && rank2 > rank && color == Color::WHITE) {
            return false; 
        }

        if (std::abs(file - file2) <= 1 && rank2 < rank && color == Color::BLACK) {
            return false; 
        }

        theirPawnsCopy.clear(sqIndex2);
    }

    return true;  
}

// Function to compute the Manhattan distance between two squares
int manhattanDistance(const Square& sq1, const Square& sq2) {
    return std::abs(sq1.file() - sq2.file()) + std::abs(sq1.rank() - sq2.rank());
}

// Min distance between a square and a set of squares
int minDistance(const Square& sq, const Square& sq2) {
    return std::min(std::abs(sq.file() - sq2.file()), std::abs(sq.rank() - sq2.rank()));
}

// Check if a square is an outpost
bool isOutpost(const Board& board, int sqIndex, Color color) {
    int file = sqIndex % 8, rank = sqIndex / 8;
    bool isWhite = color == Color::WHITE;

    // Outposts must be in the opponent's half of the board
    if ((isWhite && rank < 4) || (!isWhite && rank > 3)) {
        return false;
    }

    Bitboard ourPawns = board.pieces(PieceType::PAWN, color);
    Bitboard theirPawns = board.pieces(PieceType::PAWN, !color);

    // Check for support from our pawns
    int frontRank = (isWhite) ? rank - 1 : rank + 1;
    Bitboard supportMask = (file > 0 ? (1ULL << (frontRank * 8 + file - 1)) : 0) |
                           (file < 7 ? (1ULL << (frontRank * 8 + file + 1)) : 0);

    if (!(ourPawns & supportMask)) {
        return false; 
    }

    // Check for potential attack from opponent pawns
    for (int r = rank + 1; r < 8 && isWhite; ++r) {
        if (file > 0 && (theirPawns & (1ULL << (r * 8 + file - 1)))) {
            return false; 
        }
        if (file < 7 && (theirPawns & (1ULL << (r * 8 + file + 1)))) {
            return false; 
        }
    }
    for (int r = rank - 1; r >= 0 && !isWhite; --r) {
        if (file > 0 && (theirPawns & (1ULL << (r * 8 + file - 1)))) {
            return false; 
        }
        if (file < 7 && (theirPawns & (1ULL << (r * 8 + file + 1)))) {
            return false; 
        }
    }

    return true;
}

Bitboard allPieces(const Board& board, Color color) {
    // Return a bitboard with all pieces of the given color except kings
    return board.pieces(PieceType::PAWN, color) | board.pieces(PieceType::KNIGHT, color) 
           | board.pieces(PieceType::BISHOP, color) | board.pieces(PieceType::ROOK, color) 
           | board.pieces(PieceType::QUEEN, color) | board.pieces(PieceType::KING, color);
}

bool isOpenFile(const Board& board, int file) {
    // Get bitboards for white and black pawns
    Bitboard whitePawns = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard blackPawns = board.pieces(PieceType::PAWN, Color::BLACK);
    Bitboard mask = generateFileMask(File(file));

    return !(whitePawns & mask) && !(blackPawns & mask);
}

bool isSemiOpenFile(const Board& board, int file, Color color) {
    Bitboard ownPawns = board.pieces(PieceType::PAWN, color);
    Bitboard mask = generateFileMask(File(file));
    return !(ownPawns & mask);
}

bool isProtected(const Board& board, Color color, int sqIndex) {
    // Get the file and rank of the square
    Bitboard protectors = attacks::attackers(board, color, Square(sqIndex));
    if (protectors) {
        return true;
    } else {
        return false;
    }
}

bool isProtectedByPawn(int sqIndex, const Board& board, Color color) {
    // Get the file and rank of the square
    int file = sqIndex % 8;
    int rank = sqIndex / 8;

    if (color == Color::WHITE) {
        if (rank == 0) {
            return false;
        }
        if (file > 0 && board.at(Square((rank - 1) * 8 + (file - 1))) == PieceType::PAWN
            && board.at(Square((rank - 1) * 8 + (file - 1))).color() == Color::WHITE) {
            return true; 
        }
        if (file < 7 && board.at(Square((rank - 1) * 8 + (file + 1))) == PieceType::PAWN
            && board.at(Square((rank - 1) * 8 + (file + 1))).color() == Color::WHITE) {
            return true;
        }
    }
    else {
        if (rank == 7) {
            return false;
        }
        if (file > 0 && board.at(Square((rank + 1) * 8 + (file - 1))) == PieceType::PAWN
            && board.at(Square((rank + 1) * 8 + (file - 1))).color() == Color::BLACK) {
            return true; 
        }
        if (file < 7 && board.at(Square((rank + 1) * 8 + (file + 1))) == PieceType::PAWN
            && board.at(Square((rank + 1) * 8 + (file + 1))).color() == Color::BLACK) {
            return true;
        }
    }

    return false; // No protecting pawn found
}

// check if a squared is opposed by an enemy pawn
bool isOpposed(int sqIndex, const Board& board, Color color) {
    int file = sqIndex % 8;
    int rank = sqIndex / 8;

    if (color == Color::WHITE) {
        if (board.at(Square((rank + 1) * 8 + file)) == PieceType::PAWN
            && board.at(Square((rank + 1) * 8 + file)).color() == Color::BLACK) {
            return true;
        }
    }
    else {
        if (board.at(Square((rank - 1) * 8 + file)) == PieceType::PAWN
            && board.at(Square((rank - 1) * 8 + file)).color() == Color::WHITE) {
            return true;
        }
    }

    return false; // No opposing pawn found
}


/*------------------------------------------------------------------------
 Main Functions 
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
Compute the value of the pawns on the board. This is an expensive function
so we use a hash table to store the pawn structure values for each side 
along with the evaluation value.

This reduces the amount of computation by at least a half based on experiments.
------------------------------------------------------------------------*/
int pawnValue(const Board& board, int baseValue, Color color, Info& info) {

    Bitboard ourPawns = board.pieces(PieceType::PAWN, color);
    Bitboard theirPawns = board.pieces(PieceType::PAWN, !color);
    std::uint64_t ourPawnsBits = ourPawns.getBits();
    std::uint64_t theirPawnsBits = theirPawns.getBits();

    // Select the appropriate pawn hash table based on color
    auto& pawnHashTable = (color == Color::WHITE) ? whitePawnHashTable : blackPawnHashTable;
    bool found = false;
    int storedValue = 0;
    
    #pragma omp critical
    {
        // Check if the pawn structure value is already stored
        auto itOuter = pawnHashTable.find(ourPawnsBits);
        if (itOuter != pawnHashTable.end()) {
            auto itInner = itOuter->second.find(theirPawnsBits);
            if (itInner != itOuter->second.end()) {
                found = true;
                storedValue = itInner->second;
            }
        }
    }

    if (found) {
        return storedValue;
    }

    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;

    // constants
    const int passedPawnBonus = 35;
    const int protectedPassedPawnBonus = 45;
    const int centerBonus = 10;

    const int isolatedPawnPenalty = 20;
    const int unSupportedPenalty = 25;

    int files[8] = {0};
    int value = 0;

    // Interpolate the pawn advancement bonus based on the game phase
    int advancedPawnBonus = static_cast<int>(- (1.0 / 6.0) * info.gamePhase + 6.0);;

    Bitboard theirPieces = board.pieces(PieceType::BISHOP, !color) 
                            | board.pieces(PieceType::KNIGHT, !color) 
                            | board.pieces(PieceType::ROOK, !color) 
                            | board.pieces(PieceType::QUEEN, !color); 
                            
    Bitboard ourPawnsCopy = ourPawns;
    while (!ourPawns.empty()) {
        int sqIndex = ourPawns.lsb(); 
        int file = sqIndex % 8; // Get the file of the pawn
        files[file]++; // Increment the count of pawns on the file
        ourPawns.clear(sqIndex);
    }
    ourPawns = ourPawnsCopy;

    while (!ourPawns.empty()) {
        int sqIndex = ourPawns.lsb(); 
        bool isolated = false;

        value += baseValue; 
        if (color == Color::WHITE) {
            value += static_cast<int>(midGameWeight * whitePawnTableMid[sqIndex] + endGameWeight * whitePawnTableEnd[sqIndex]);
        } else {
            value += static_cast<int>(midGameWeight * blackPawnTableMid[sqIndex] + endGameWeight * blackPawnTableEnd[sqIndex]);
        }

        int file = sqIndex % 8;
        int rank = sqIndex / 8;

        if (file == 3 || file == 4) {
            value += centerBonus;
        }

        // If the pawn is isolated, add a penalty
        if ((file == 0 && files[1] == 0) || (file == 7 && files[6] == 0)) {
            isolated = true;
            value -= isolatedPawnPenalty;
        } else if (file > 0 && file < 7 && files[file - 1] == 0  && files[file + 1] == 0) {
            isolated = true;
            value -= isolatedPawnPenalty;
        }

        // Add bonus for passed pawns, especially if they are protected
        if (isPassedPawn(sqIndex, color, theirPawns)) {
            if (isProtectedByPawn(sqIndex, board, color)) {
                value += protectedPassedPawnBonus;
            } else {            
                value += passedPawnBonus;
            }

            if (color == Color::WHITE) {
                value += whitePassedPawnTable[sqIndex];
            } else {
                value += blackPassedPawnTable[sqIndex];
            }
        }  
        
        // Add penalty for unsupported pawns, more if they are on semi-open files or open files
        if (!isProtectedByPawn(sqIndex, board, color)) {
            if (color == Color::WHITE && info.semiOpenFilesBlack[file]) {
                value -= unSupportedPenalty;
            } else if (color == Color::BLACK && info.semiOpenFilesWhite[file]) {
                value -= unSupportedPenalty;
            } else  {
                value -= (unSupportedPenalty - 15); 
            }
        }

        // Bonus for advanced pawns, more if we are in the endgame
        if (color == Color::WHITE) {
            value += (rank - 1) * advancedPawnBonus;
        } else {
            value += (6 - rank) * advancedPawnBonus;
        }


        ourPawns.clear(sqIndex);
    }
    
    // Add penalty for doubled pawns
    const int doubledPawnPenalty = 30;
    const int doubledPawnPenaltyDE = 40;
    const int doubleIsolatedPenalty = 30;

    for (int i = 0; i < 8; i++) {
        if (i == 3 || i == 4) {
            value -= (files[i] - 1) * doubledPawnPenaltyDE;
        } else {
            value -= (files[i] - 1) * doubledPawnPenalty;
        }

        if (i > 0 && i < 7 && files[i] > 1 && files[i - 1] == 0 && files[i + 1] == 0) {
            value -= doubleIsolatedPenalty; // extra penalty for double & isolated pawns
        } else if (files[i] > 1 && (files[i - 1] == 0 || files[i + 1] == 0)) {
            value -= doubleIsolatedPenalty; // handling double isolated pawns at the edges
        } 
    }

    #pragma omp critical
    {
        pawnHashTable[ourPawnsBits][theirPawnsBits] = value;
    }

    return value;
}

// Compute the value of the knights on the board
int knightValue(const Board& board, int baseValue, Color color, Info& info) {

    // Constants
    const int outpostBonus = 30;
    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;

    int knightAdjust[9] = {-20, -16, -12, -8, -4,  0,  4,  8, 12}; // Adjust the value of the knight based on the number of pawns
    const int mobilityBonus = 3;

    int ourPawnCount = board.pieces(PieceType::PAWN, color).count();
    Bitboard knights = board.pieces(PieceType::KNIGHT, color);
    
    int value = 0;

    while (!knights.empty()) {
        value += baseValue + knightAdjust[ourPawnCount];
        int sqIndex = knights.lsb();
        
        if (color == Color::WHITE) {
            value += static_cast<int>(midGameWeight * whiteKnightTableMid[sqIndex] + endGameWeight * whiteKnightTableEnd[sqIndex]);
        } else {
            value += static_cast<int>(midGameWeight * blackKnightTableMid[sqIndex] + endGameWeight * blackKnightTableEnd[sqIndex]);
        }

        if (isOutpost(board, sqIndex, color)) {
            value += outpostBonus;
        }

        // Compute the mobility of the knight. 
        // We count the number of legal moves that don't land onto a pawn attack or blocked by our own pieces.
        Bitboard knightMoves = attacks::knight(Square(sqIndex));
        int mobility = 0;
        while (!knightMoves.empty()) {
            
            int sqIndexMove = knightMoves.lsb();
            Bitboard blocked = board.us(color) & Bitboard::fromSquare(sqIndexMove);
            
            if (!isProtectedByPawn(sqIndexMove, board, !color) && !blocked) {
                mobility++;
            }
            knightMoves.clear(sqIndexMove);
        }
        value +=  mobilityBonus * (mobility - 4);

        const int protectedBonus = 4;
        if (isProtected(board, color, sqIndex)) {
            value += protectedBonus;
        }


        knights.clear(sqIndex);
    }

    return value;
}

// Compute the value of the bishops on the board
int bishopValue(const Board& board, int baseValue, Color color, Info& info) {

    // Constants
    const int outpostBonus = 30;
    const int protectionBonus = 3;

    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;

    int bishopPairBonus = 30 * endGameWeight;
    int rookAdjust[9] = {15, 12, 9, 6, 3, 0, -3, -6, -9};
    int mobilityBonus = 2;

    Bitboard bishops = board.pieces(PieceType::BISHOP, color);
    Bitboard ourPawns = board.pieces(PieceType::PAWN, color);
    int value = 0;
    
    if (bishops.count() >= 2) {
        value += bishopPairBonus;
    }
 
    while (!bishops.empty()) {
        value += baseValue;
        int sqIndex = bishops.lsb();
        if (color == Color::WHITE) {
            value += static_cast<int>(midGameWeight * whiteBishopTableMid[sqIndex] + endGameWeight * whiteBishopTableEnd[sqIndex]);
        } else {
            value += static_cast<int>(midGameWeight * blackBishopTableMid[sqIndex] + endGameWeight * blackBishopTableEnd[sqIndex]);
        }

        Bitboard bishopMoves = attacks::bishop(Square(sqIndex), ourPawns);

        int mobility = std::min(bishopMoves.count(), 12);
        value += mobilityBonus * (mobility - 7);

        if (isOutpost(board, sqIndex, color)) {
            value += outpostBonus;
        }

        const int protectedBonus = 4;
        if (isProtected(board, color, sqIndex)) {
            value += protectedBonus;
        }

        bishops.clear(sqIndex);
    }

    return value;
}


// Compute the total value of the rooks on the board
int rookValue(const Board& board, int baseValue, Color color, Info& info) {

    // Constants
    const int semiOpenFileBonus = 10;
    const int openFileBonus = 15;
    
    

    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;
    int rookAdjust[9] = {15, 12, 9, 6, 3, 0, -3, -6, -9};

    // Give a bigger bonus for mobility near the endgame
    int mobilityBonus = info.gamePhase < 12 ? 3 : 2;

    Bitboard rooks = board.pieces(PieceType::ROOK, color);
    Bitboard ourPawns = board.pieces(PieceType::PAWN, color);
    int ourPawnCount = ourPawns.count();

    int value = 0;

    while (!rooks.empty()) {
        int sqIndex = rooks.lsb(); 
        int file = sqIndex % 8; 
        int rank = sqIndex / 8;

        value += baseValue + rookAdjust[ourPawnCount];

        if (color == Color::WHITE) {
            value += static_cast<int>(midGameWeight * whiteRookTableMid[sqIndex] + endGameWeight * whiteRookTableEnd[sqIndex]);
        } else {
            value += static_cast<int>(midGameWeight * blackRookTableMid[sqIndex] + endGameWeight * blackRookTableEnd[sqIndex]);
        }

        if (info.openFiles[file]) {
            value += openFileBonus;
        } else {
            if (info.semiOpenFilesWhite[file] && color == Color::WHITE) {
                value += semiOpenFileBonus;
            } else if (info.semiOpenFilesBlack[file] && color == Color::BLACK) {
                value += semiOpenFileBonus;
            }
        }
        
        Bitboard rookMoves = attacks::rook(Square(sqIndex), board.occ());
        int mobility = std::min(rookMoves.count(), 12);
        value += mobilityBonus * (mobility - 7);

        const int pawnBlockPenalty = 20;

        if ((color == Color::WHITE && rank == 0) || (color == Color::BLACK && rank == 7)) {
            int squareAbove = (color == Color::WHITE) ? sqIndex + 8 : sqIndex - 8;
    
            if (ourPawns & Bitboard::fromSquare(squareAbove)) {
                value -= pawnBlockPenalty;
            }
        }
        
        const int protectedBonus = 4;
        if (isProtected(board, color, sqIndex)) {
            value += protectedBonus;
        }

        rooks.clear(sqIndex);
    }
    
    return value;
}


// Compute the total value of the queens on the board
int queenValue(const Board& board, int baseValue, Color color, Info& info) {

    // Constants
    const int* queenTable;    
    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;

    // Give a bigger bonus for mobility near the endgame
    int mobilityBonus = info.gamePhase < 12 ? 2 : 1;
    
    Bitboard queens = board.pieces(PieceType::QUEEN, color);
    Bitboard theirKing = board.pieces(PieceType::KING, !color);

    Square theirKingSQ = Square(theirKing.lsb()); // Get the square of the their king
    int value = 0;

    Bitboard theirRooks = board.pieces(PieceType::ROOK, !color);
    Bitboard theirBishops = board.pieces(PieceType::BISHOP, !color);

    while (!queens.empty()) {
        int sqIndex = queens.lsb(); 

        int queenRank = sqIndex / 8, queenFile = sqIndex % 8;
        value += baseValue; 
        if (color == Color::WHITE) {
            value += static_cast<int>(midGameWeight * whiteQueenTableMid[sqIndex] + endGameWeight * whiteQueenTableEnd[sqIndex]);
        } else {
            value += static_cast<int>(midGameWeight * blackQueenTableMid[sqIndex] + endGameWeight * blackQueenTableEnd[sqIndex]);
        }

        Bitboard queenMoves = attacks::queen(Square(sqIndex), board.occ());
        int mobility = std::min(queenMoves.count(), 12);
        value += mobilityBonus * (mobility - 14);

        const int protectedBonus = 4;
        if (isProtected(board, color, sqIndex)) {
            value += protectedBonus;
        }

        queens.clear(sqIndex); 
    }
    return value;
}

int kingThreat(const Board& board, Color color) {

    Bitboard king = board.pieces(PieceType::KING, color);
    int sqIndex = king.lsb();
    int kingRank = sqIndex / 8, kingFile = sqIndex % 8;
    int threatScore = 0;

    // Penalty for being attacked
    Bitboard attackers;
    int numAttackers = 0;

    // Get the adjacent squares to the king
    Bitboard adjSq;
    for (const auto& adjSqIndex : adjSquares.at(sqIndex)) {
        adjSq = adjSq | Bitboard::fromSquare(adjSqIndex);
    }

    // A pawn is a threat if it is within Manhattan distance of 4
    Bitboard theirPawns = board.pieces(PieceType::PAWN, !color);
    while (theirPawns) {
        int pawnIndex = theirPawns.lsb();

        if (manhattanDistance(Square(pawnIndex), Square(sqIndex)) <= 4) {
            attackers.set(pawnIndex);
        }

        theirPawns.clear(pawnIndex);
    }

    theirPawns = board.pieces(PieceType::PAWN, !color);

    /*--------------------------------------------------------------
     Check for attacks from other pieces
     A piece is a threat if it attacks the adjacent squares to the king 
     given the presence of our pieces and their pawns
    --------------------------------------------------------------*/

    Bitboard theirQueens = board.pieces(PieceType::QUEEN, !color);
    Bitboard ourDefenders = board.us(color);

    while (theirQueens) {
        // Count the queen as a threat if it is close or attacking an adjacent square
        int queenIndex = theirQueens.lsb();

        Bitboard queenAttacks = attacks::queen(Square(queenIndex), ourDefenders | theirPawns);

        bool beingClose = (manhattanDistance(Square(queenIndex), Square(sqIndex)) <= 6);
        bool attackingAdj = (queenAttacks & adjSq).count() > 0;

        if (beingClose || attackingAdj) {
            attackers.set(queenIndex);
        }

        theirQueens.clear(queenIndex);
    }

    Bitboard theirRooks = board.pieces(PieceType::ROOK, !color);
    while (theirRooks) {
        // Count the rook as a threat if it is attacking an adjacent square 
        int rookIndex = theirRooks.lsb();
        
        Bitboard rookAttacks = attacks::rook(Square(rookIndex), ourDefenders | theirPawns);
        bool attackingAdj = (rookAttacks & adjSq).count() > 0;

        if (attackingAdj) {
            attackers.set(rookIndex);
        }

        theirRooks.clear(rookIndex);
    }

    Bitboard theirKnight = board.pieces(PieceType::KNIGHT, !color);
    while (theirKnight) {
        // Count the knight as a threat if it is close 
        int knightIndex = theirKnight.lsb();
        Bitboard knightAttacks = attacks::knight(Square(knightIndex));

        bool beingClose = (manhattanDistance(Square(knightIndex), Square(sqIndex)) <= 5);
        bool attackingAdj = (knightAttacks & adjSq).count() > 0;

        if (beingClose || attackingAdj) {
            attackers.set(knightIndex);
        }

        theirKnight.clear(knightIndex);
    }

    Bitboard theirBishop = board.pieces(PieceType::BISHOP, !color);
    while (theirBishop) {
        // Count the bishop as a threat if it is close or attacking an adjacent square
        int bishopIndex = theirBishop.lsb();
        Bitboard bishopAttacks = attacks::bishop(Square(bishopIndex), ourDefenders | theirPawns);

        bool beingClose = (manhattanDistance(Square(bishopIndex), Square(sqIndex)) <= 4);
        bool attackingAdj = (bishopAttacks & adjSq).count() > 0;

        if (beingClose || attackingAdj) {
            attackers.set(bishopIndex);
        }

        theirBishop.clear(bishopIndex);
    }

    // Scale the adjacent square attacks by the number of attackers
    numAttackers = attackers.count();

    // The more attackers, the higher the penalty
    int attackWeight = 0; 
    switch (numAttackers) {
        case 0: attackWeight = 0; break;
        case 1: attackWeight = 25; break;  
        case 2: attackWeight = 65; break;  
        case 3: attackWeight = 100; break; 
        case 4: attackWeight = 120; break; 
        case 5: attackWeight = 150; break; 
        case 6: attackWeight = 175; break; 
        case 7: attackWeight = 200; break; 
        case 8: attackWeight = 200; break; 
        default: break;
    }

    while (attackers) {
        int attackerIndex = attackers.lsb();
        Piece attacker = board.at(Square(attackerIndex));

        // Add scores as integers
        if (attacker.type() == PieceType::PAWN) {
            threatScore += attackWeight * 15; // Scaled integer math
        } else if (attacker.type() == PieceType::KNIGHT) {
            threatScore += attackWeight * 30;
        } else if (attacker.type() == PieceType::BISHOP) {
            threatScore += attackWeight * 30;
        } else if (attacker.type() == PieceType::ROOK) {
            threatScore += attackWeight * 50;
        } else if (attacker.type() == PieceType::QUEEN) {
            threatScore += attackWeight * 100;
        }

        attackers.clear(attackerIndex);
    }

    return threatScore / 100;

}

/*------------------------------------------------------------------------
Compute the value of the king on the board. This is an expensive function
since it involves a lot of computation checking for threats to the king.
------------------------------------------------------------------------*/
int kingValue(const Board& board, int baseValue, Color color, Info& info) {

    double midGameWeight = info.gamePhase / 24.0;
    double endGameWeight = 1.0 - midGameWeight;
    
    Bitboard king = board.pieces(PieceType::KING, color);
    const PieceType allPieceTypes[] = {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN};
    
    int value = baseValue;
    int sqIndex = king.lsb();
    int kingRank = sqIndex / 8, kingFile = sqIndex % 8;
    
    if (color == Color::WHITE) {
        value += static_cast<int>(midGameWeight * whiteKingTableMid[sqIndex] + endGameWeight * whiteKingTableEnd[sqIndex]);
    } else {
        value += static_cast<int>(midGameWeight * blackKingTableMid[sqIndex] + endGameWeight * blackKingTableEnd[sqIndex]);
    }

    int threatScore = kingThreat(board, color) * midGameWeight; // Scale the threat score by the game phase (heavy toward midgame)
    int originalThreatScore =  kingThreat(board, color);
    value -= threatScore;

    // King protection by pawns, scale the bonus by the game phase (heavy toward midgame)
    int pawnShieldBonus = 30 * midGameWeight;
    Bitboard ourPawns = board.pieces(PieceType::PAWN, color);
    Bitboard theirPawns = board.pieces(PieceType::PAWN, !color);

    while (!ourPawns.empty()) {
        int pawnIndex = ourPawns.lsb();
        int pawnRank = pawnIndex / 8, pawnFile = pawnIndex % 8;
        // if the pawn is in front of the king and on an adjacent file, add shield bonus
        if (color == Color::WHITE 
                    && pawnRank == kingRank + 1 && std::abs(pawnFile - kingFile) <= 1) {
            value += pawnShieldBonus;
        } else if (color == Color::BLACK 
                    && pawnRank == kingRank - 1 && std::abs(pawnFile - kingFile) <= 1) {
            value += pawnShieldBonus; 
        }

        ourPawns.clear(pawnIndex);
    }
    
    // King protection by pieces, scale the bonus by the game phase (heavy toward midgame)
    int pieceProtectionBonus = 30 * midGameWeight;
    for (const auto& type : allPieceTypes) {
        Bitboard pieces = board.pieces(type, color);
        
        while (!pieces.empty()) {
            int pieceSqIndex = pieces.lsb();
            if (color == Color::WHITE) {
                if (pieceSqIndex / 8 > sqIndex / 8 && manhattanDistance(Square(pieceSqIndex), Square(sqIndex)) <= 4) { 
                    value += pieceProtectionBonus; 
                }
            } else if (color == Color::BLACK) {
                if (pieceSqIndex / 8 < sqIndex / 8 && manhattanDistance(Square(pieceSqIndex), Square(sqIndex)) <= 4) {
                    value += pieceProtectionBonus; 
                }
            }
            pieces.clear(pieceSqIndex);
        }
    }

    // Penalty for being on or next to an open file (heavy toward midgame)
    int numAdjOpenFiles = 0;
    const int openFilePenalty[4] = {0, 20, 35, 60};

    if (info.openFiles[kingFile] || info.semiOpenFilesWhite[kingFile] || info.semiOpenFilesBlack[kingFile]) {
        numAdjOpenFiles++;
    }

    if (kingFile > 0 && (info.openFiles[kingFile - 1] 
        || info.semiOpenFilesWhite[kingFile - 1] 
        || info.semiOpenFilesBlack[kingFile - 1])) {
        numAdjOpenFiles++;
    }

    if (kingFile < 7 && (info.openFiles[kingFile + 1] 
        || info.semiOpenFilesWhite[kingFile + 1] 
        || info.semiOpenFilesBlack[kingFile + 1])) {
        numAdjOpenFiles++;
    }

    value -= openFilePenalty[numAdjOpenFiles] * midGameWeight;

    int kingDistancePenalty = 6;
    int pawnDistancePenalty = 3;
    int passedPawnDistancePenalty = 6;

    Bitboard theirKing = board.pieces(PieceType::KING, !color);
    int theirKingIndex = theirKing.lsb();

    int dist = manhattanDistance(Square(sqIndex), Square(theirKingIndex)); 
    value -= kingDistancePenalty * dist * endGameWeight; // Stronger penalty for being far away from the opponent king in the endgame

    ourPawns = board.pieces(PieceType::PAWN, color);
    theirPawns = board.pieces(PieceType::PAWN, !color);

    // Encourage the king to stay close to our own pawns & their pawns, more so if they are passed pawns
    while (ourPawns) {
        int pawnSqIndex = ourPawns.lsb();
        dist = manhattanDistance(Square(sqIndex), Square(pawnSqIndex));
        
        if (isPassedPawn(pawnSqIndex, color, theirPawns)) {
            value -= passedPawnDistancePenalty * dist * endGameWeight;
        } else {
            value -= pawnDistancePenalty * dist * endGameWeight;
        }
        
        ourPawns.clear(pawnSqIndex); 
    }

    while (theirPawns) {
        int pawnSqIndex = theirPawns.lsb();
        dist = manhattanDistance(Square(sqIndex), Square(pawnSqIndex));
        
        if (isPassedPawn(pawnSqIndex, !color, ourPawns)) {
            value -= passedPawnDistancePenalty * dist * endGameWeight;
        } else {
            value -= pawnDistancePenalty * dist * endGameWeight;
        }
        
        theirPawns.clear(pawnSqIndex); 
    }

    return value;
}

// Function to evaluate the board position
int evaluate(const Board& board) {
    // Constant
    const int tempoBonus = 10;

    // Initialize totals
    int whiteScore = 0;
    int blackScore = 0;

    if (knownDraw(board)) {
        return 0;
    }
    
    /*--------------------------------------------------------------------------
    Mop-up phase: if only their king is left without any other pieces.
    Aim to checkmate.
    --------------------------------------------------------------------------*/

    if (board.us(Color::WHITE).count() == 1 && board.us(Color::BLACK).count() == 1) {
        return 0; 
    }

    bool mopUp = board.us(Color::WHITE).count() == 1 || board.us(Color::BLACK).count() == 1;

    Color winningColor = board.us(Color::WHITE).count() > 1 ? Color::WHITE : Color::BLACK;

    Bitboard pieces = board.pieces(PieceType::PAWN, winningColor) | board.pieces(PieceType::KNIGHT, winningColor) | 
                    board.pieces(PieceType::BISHOP, winningColor) | board.pieces(PieceType::ROOK, winningColor) | 
                    board.pieces(PieceType::QUEEN, winningColor);

    if (mopUp) {
        Square winningKingSq = Square(board.pieces(PieceType::KING, winningColor).lsb());
        Square losingKingSq = Square(board.pieces(PieceType::KING, !winningColor).lsb());
        Square E4 = Square(28);

        int kingDist = manhattanDistance(winningKingSq, losingKingSq);
        int distToCenter = manhattanDistance(losingKingSq, E4);
        int winningMaterial = 900 * board.pieces(PieceType::QUEEN, winningColor).count() + 
                          500 * board.pieces(PieceType::ROOK, winningColor).count() + 
                          300 * board.pieces(PieceType::BISHOP, winningColor).count() + 
                          300 * board.pieces(PieceType::KNIGHT, winningColor).count() + 
                          100 * board.pieces(PieceType::PAWN, winningColor).count(); // avoid throwing away pieces
        int score = 5000 +  500 * distToCenter + 150 * (14 - kingDist);

        return winningColor == Color::WHITE ? score : -score;
    }

    /*--------------------------------------------------------------------------
    Standard evaluation phase
    --------------------------------------------------------------------------*/
    Info info;

    // Early queen development penalty
    Bitboard whiteKnights = board.pieces(PieceType::KNIGHT, Color::WHITE);
    Bitboard blackKnights = board.pieces(PieceType::KNIGHT, Color::BLACK);

    Bitboard whiteBishops = board.pieces(PieceType::BISHOP, Color::WHITE);
    Bitboard blackBishops = board.pieces(PieceType::BISHOP, Color::BLACK);

    Bitboard whiteRooks = board.pieces(PieceType::ROOK, Color::WHITE);
    Bitboard blackRooks = board.pieces(PieceType::ROOK, Color::BLACK);

    Bitboard whiteQueen = board.pieces(PieceType::QUEEN, Color::WHITE);
    Bitboard blackQueen = board.pieces(PieceType::QUEEN, Color::BLACK);

    Bitboard whitePawns = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard blackPawns = board.pieces(PieceType::PAWN, Color::BLACK);

    Bitboard whiteKing = board.pieces(PieceType::KING, Color::WHITE);
    Bitboard blackKing = board.pieces(PieceType::KING, Color::BLACK);

    // Tempo bonus
    if (board.sideToMove() == Color::WHITE) {
        whiteScore += tempoBonus;
    } else {
        blackScore += tempoBonus;
    }

    info.gamePhase = gamePhase(board);
    if (info.gamePhase > 24) {
        info.gamePhase = 24;
    }

    // Compute open files and semi-open files
    for (int i = 0; i < 8; i++) {
        info.openFiles[i] = isOpenFile(board, i);
        if (!info.openFiles[i]) {
            info.semiOpenFilesWhite[i] = isSemiOpenFile(board, i, Color::WHITE);
            info.semiOpenFilesBlack[i] = isSemiOpenFile(board, i, Color::BLACK);
        }
    }

    // Traverse each piece type
    const PieceType allPieceTypes[] = {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, 
                                           PieceType::ROOK, PieceType::QUEEN, PieceType::KING};


    for (const auto& type : allPieceTypes) {
        // Determine base value of the current piece type
        int baseValue = 0;

        switch (type.internal()) {
            case PieceType::PAWN: baseValue = PAWN_VALUE; break;
            case PieceType::KNIGHT: baseValue = KNIGHT_VALUE; break;
            case PieceType::BISHOP: baseValue = BISHOP_VALUE; break;
            case PieceType::ROOK: baseValue = ROOK_VALUE; break;
            case PieceType::QUEEN: baseValue = QUEEN_VALUE; break;
            case PieceType::KING: baseValue = KING_VALUE; break;
            default: break;
        }

        if (type == PieceType::KNIGHT) {
            whiteScore += knightValue(board, baseValue, Color::WHITE, info);
            blackScore += knightValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "K issue" << std::endl;
            }
        } else if (type == PieceType::BISHOP) {
            whiteScore += bishopValue(board, baseValue, Color::WHITE, info);
            blackScore += bishopValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "B issue" << std::endl;
            }
        } else if (type == PieceType::KING) {
            whiteScore += kingValue(board, baseValue, Color::WHITE, info);
            blackScore += kingValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "K issue" << std::endl;
            }
        }  else if (type == PieceType::PAWN) {
            whiteScore += pawnValue(board, baseValue, Color::WHITE, info);
            blackScore += pawnValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "P issue" << std::endl;
            }
        } else if (type == PieceType::ROOK) {
            whiteScore += rookValue(board, baseValue, Color::WHITE, info);
            blackScore += rookValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "R issue" << std::endl;
            }
        } else if (type == PieceType::QUEEN) {
            whiteScore += queenValue(board, baseValue, Color::WHITE, info);
            blackScore += queenValue(board, baseValue, Color::BLACK, info);
            if (whiteScore < 0) {
                std::cout << "Q issue" << std::endl; }
        } 
    }
    
    /*-------------------------------------------------------------------------
        Add a penalty for piece-material deficit. At the beginning, the deficit is 24 * 2 = 48.
        As the game progresses, the penalty decreases. 
        This is useful to avoid trading pieces for pawns early on.
    -------------------------------------------------------------------------*/
    const int knightValue = 3, bishopValue = 3, rookValue = 5, queenValue = 9, pawnValue = 1;

    int whitePieceValue = queenValue * whiteQueen.count() + rookValue * whiteRooks.count() + 
                        bishopValue * whiteBishops.count() + knightValue * whiteKnights.count();

    int blackPieceValue = queenValue * blackQueen.count() + rookValue * blackRooks.count() + 
                        bishopValue * blackBishops.count() + knightValue * blackKnights.count();
    
    int pieceDeficitPenalty = info.gamePhase * 5;
    
    if (whitePieceValue < blackPieceValue) {
        whiteScore -= pieceDeficitPenalty;
    } else if (blackPieceValue < whitePieceValue) {
        blackScore -= pieceDeficitPenalty;
    }

    /*--------------------------------------------------------------------------
        Add a penalty for material deficit to make sure the position advantage is real.
    --------------------------------------------------------------------------*/
    const int deficitPenalty = 50;
    int whiteMaterial = whitePieceValue + pawnValue * whitePawns.count();
    int blackMaterial = blackPieceValue + pawnValue * blackPawns.count();

    if (whiteMaterial < blackMaterial) {
        whiteScore -= deficitPenalty;
    } else if (blackMaterial < whiteMaterial) {
        blackScore -= deficitPenalty;
    }

    /*--------------------------------------------------------------------------
        Pattern detection
    --------------------------------------------------------------------------*/

    // Center control
    Bitboard center = e4 | d4 | e5 | d5;
    Bitboard extendedCenter =  c4 | c5 | f4 | f5;

    const int centerControlBonus = 15;
    const int extendedCenterControlBonus = 10;
    const int blockCentralPawnPenalty = 60;

    int whiteCenterControl = (board.us(Color::WHITE) & center).count() * centerControlBonus;
    int blackCenterControl = (board.us(Color::BLACK) & center).count() * centerControlBonus;

    whiteScore += whiteCenterControl;
    blackScore += blackCenterControl;

    int whiteExtendedCenterControl = (board.us(Color::WHITE) & extendedCenter).count() * extendedCenterControlBonus;
    int blackExtendedCenterControl = (board.us(Color::BLACK) & extendedCenter).count() * extendedCenterControlBonus;

    whiteScore += whiteExtendedCenterControl;
    blackScore += blackExtendedCenterControl;
    
    // Penalty  blocking central pawns
    if (board.occ() && d2) {
        Piece piece = board.at(Square(d2.lsb()));
        if (piece.type() == PieceType::PAWN && piece.color() == Color::WHITE) {
            whiteScore -= blockCentralPawnPenalty;
        }
    }
    if (board.occ() && e2) {
        Piece piece = board.at(Square(e2.lsb()));
        if (piece.type() == PieceType::PAWN && piece.color() == Color::WHITE) {
            whiteScore -= blockCentralPawnPenalty;
        }
    }
    if (board.occ() && d7) {
        Piece piece = board.at(Square(d7.lsb()));
        if (piece.type() == PieceType::PAWN && piece.color() == Color::BLACK) {
            blackScore -= blockCentralPawnPenalty;
        }
    }
    if (board.occ() && e7) {
        Piece piece = board.at(Square(d7.lsb()));
        if (piece.type() == PieceType::PAWN && piece.color() == Color::BLACK) {
            blackScore -= blockCentralPawnPenalty;
        }
    }

    bool whiteQueenDeveloped = (whiteQueen.count() > 0) && (whiteQueen.lsb() != d1.lsb());
    bool blackQueenDeveloped = (blackQueen.count() > 0) && (blackQueen.lsb() != d8.lsb());

    // Check if white knights and bishops haven't moved
    Bitboard whiteKnightNotMoved = board.pieces(PieceType::KNIGHT, Color::WHITE) & (b1 | g1);
    Bitboard whiteBishopNotMoved = board.pieces(PieceType::BISHOP, Color::WHITE) & (c1 | f1);

    // Check if black knights and bishops haven't moved
    Bitboard blackKnightNotMoved = board.pieces(PieceType::KNIGHT, Color::BLACK) & (b8 | g8);
    Bitboard blackBishopNotMoved = board.pieces(PieceType::BISHOP, Color::BLACK) & (c8 | f8);

    // Apply penalty if white queen moved early
    if (whiteQueenDeveloped) {
        whiteScore -= 7 * (whiteKnightNotMoved.count() + whiteBishopNotMoved.count());
    }

    if (blackQueenDeveloped) {
        blackScore -= 7 * (blackKnightNotMoved.count() + blackBishopNotMoved.count());
    }

    const int trappedBishopPenalty = 250;

    // Case 1: White bishop stuck on a7 or b8 (blocked by black pawns on b6 and c7)
    if ((whiteBishops & a7) || (whiteBishops & b8)) {
        if ((blackPawns & b6) && (blackPawns & c7)) {
            whiteScore -= trappedBishopPenalty; 
        }
    }

    // Case 2: White bishop stuck on h7 or g8 (blocked by black pawns on g6 and f7)
    if ((whiteBishops & h7) || (whiteBishops & g8)) {
        if ((blackPawns & g6) && (blackPawns & f7)) {
            whiteScore -= trappedBishopPenalty;
        }
    }

    // Case 1b: Black bishop stuck on a2 (blocked by white pawns on b3 and c2)
    if ((blackBishops & a2) || (blackBishops & b1)) {
        if ((whitePawns & b3) && (whitePawns & c2)) {
            blackScore -= trappedBishopPenalty;
        }
    }

    // Case 2b: Black bishop stuck on h2 (blocked by white pawns on g3 and f2)
    if ((blackBishops & h2) || (blackBishops & g1)) {
        if ((whitePawns & g3) && (whitePawns & f2)) {
            blackScore -= trappedBishopPenalty;
        }
    }

    const int blockedBishopPenalty = 20;

    // White bishop stuck on c1 or d2 (blocked by white pawn on e3)
    if ((whiteBishops & c1) || (whiteBishops & d2)) {
        if (whitePawns & e3) {
            whiteScore -= blockedBishopPenalty;

        }
    }

    // White bishop stuck on f1 or e2 (blocked by white pawn on d3)
    if ((whiteBishops & f1) || (whiteBishops & e2)) {
        if (whitePawns & d3) {
            whiteScore -= blockedBishopPenalty;
        }
    }

    // Black bishop stuck on c8 or d7 (blocked by black pawn on e6)
    if ((blackBishops & c8) || (blackBishops & d7)) {
        if (blackPawns & e6) {
            blackScore -= blockedBishopPenalty;
        }
    }

    // Black bishop stuck on f8 or e7 (blocked by black pawn on d6)
    if ((blackBishops & f8) || (blackBishops & e7)) {
        if (blackPawns & d6) {
            blackScore -= blockedBishopPenalty;
        }
    }

    // Blocked fianchettoed bishops
    const int blockedFianchettoPenalty = 30;

    if (((whiteBishops & b2) && (whitePawns & d4)) ||
        ((whiteBishops & g2) && (whitePawns & e4))) {
        whiteScore -= blockedFianchettoPenalty;
    }

    if (((blackBishops & b7) && (blackPawns & d5)) ||
        ((blackBishops & g7) && (blackPawns & e5))) {
        blackScore -= blockedFianchettoPenalty;
    }

    return whiteScore - blackScore;
}