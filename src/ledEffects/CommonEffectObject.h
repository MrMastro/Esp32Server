#ifndef COMMON_EFFECT_OBJECT_H
#define COMMON_EFFECT_OBJECT_H

#include <vector>

constexpr int MATRIX_ROWS = 8;
constexpr int MATRIX_COLS = 8;

// Dichiarazione della matrice come un array bidimensionale costante
constexpr int LED_MATRIX_POSITIONS[MATRIX_ROWS][MATRIX_COLS] = {
    {56, 55, 40, 39, 24, 23, 8, 7}, 
    {57, 54, 41, 38, 25, 22, 9, 6}, 
    {58, 53, 42, 37, 26, 21, 10, 5}, 
    {59, 52, 43, 36, 27, 20, 11, 4}, 
    {60, 51, 44, 35, 28, 19, 12, 3}, 
    {61, 50, 45, 34, 29, 18, 13, 2}, 
    {62, 49, 46, 33, 30, 17, 14, 1}, 
    {63, 48, 47, 32, 31, 16, 15, 0}
};

#endif