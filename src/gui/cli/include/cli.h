#ifndef CLI_H
#define CLI_H

#include <ncurses.h>
#include <unistd.h>

#include "tetris.h"

#define GAME_WINDOW_WIDTH 22
#define GAME_WINDOW_HEIGHT 22
#define INFO_WINDOW_WIDTH 20
#define INFO_WINDOW_HEIGHT 22

/**
 * @brief Инициализирует интерфейс пользователя
 */
void initInterface();

/**
 * @brief Очищает интерфейс пользователя
 */
void cleanupInterface();

/**
 * @brief Отрисовывает игру
 * @param info Информация о состоянии игры
 */
void drawGame(GameInfo_t info);

/**
 * @brief Отрисовывает игровое поле
 * @param win Окно для отрисовки
 * @param field Двумерный массив игрового поля
 */
void drawField(WINDOW *win, int **field);

/**
 * @brief Отрисовывает следующее тетромино
 * @param win Окно для отрисовки
 * @param next Двумерный массив следующего тетромино
 */
void drawNext(WINDOW *win, int **next);

/**
 * @brief Отрисовывает информационную панель
 * @param win Окно для отрисовки
 * @param info Информация о состоянии игры
 */
void drawInfo(WINDOW *win, GameInfo_t info);

/**
 * @brief Получает пользовательский ввод
 * @param action Указатель на переменную для сохранения действия
 * @return Код клавиши или 0 если ввода нет
 */
int getInput(UserAction_t *action);

/**
 * @brief Основной игровой цикл
 */
void gameLoop();

#endif  // CLI_H