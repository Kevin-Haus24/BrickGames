#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define NEXT_SIZE 4
#define TETROMINO_COUNT 7
#define HIGH_SCORE_FILE "high_score.txt"

/**
 * @brief Перечисление действий пользователя
 */
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

/**
 * @brief Структура с информацией о текущем состоянии игры
 */
typedef struct {
  int **field;     // Игровое поле (матрица)
  int **next;      // Следующая фигура (матрица)
  int score;       // Текущий счет
  int high_score;  // Лучший счет
  int level;       // Уровень игры
  int speed;       // Скорость игры
  int pause;       // Флаг паузы
} GameInfo_t;

/**
 * @brief Перечисление состояний игры
 */
typedef enum {
  GAME_START,
  GAME_SPAWN,
  GAME_MOVING,
  GAME_SHIFTING,
  GAME_PAUSE,
  GAME_OVER,
  GAME_EXIT
} GameState_t;

/**
 * @brief Структура тетромино
 */
typedef struct {
  int x, y;
  int type;
  int rotation;
} Tetromino_t;

/**
 * @brief Основная структура игры
 */
typedef struct {
  GameState_t state;
  GameInfo_t info;
  Tetromino_t current;
  Tetromino_t next;
  clock_t last_time;
  int lines_cleared;
} Game_t;

// Основные функции API
/**
 * @brief Обрабатывает пользовательский ввод
 * @param action Действие пользователя
 * @param hold Флаг удержания клавиши
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Обновляет текущее состояние игры
 * @return Структура с информацией о текущем состоянии игры
 */
GameInfo_t updateCurrentState();

// Вспомогательные функции
/**
 * @brief Инициализирует игру
 */
void initGame();

/**
 * @brief Освобождает память игры
 */
void freeGame();

/**
 * @brief Проверяет возможность движения тетромино
 * @param tetromino Тетромино для проверки
 * @param dx Смещение по оси X
 * @param dy Смещение по оси Y
 * @return true если движение возможно, false иначе
 */
bool canMove(Tetromino_t tetromino, int dx, int dy);

/**
 * @brief Проверяет возможность поворота тетромино
 * @param tetromino Тетромино для проверки
 * @return true если поворот возможен, false иначе
 */
bool canRotate(Tetromino_t tetromino);

/**
 * @brief Размещает тетромино на игровом поле
 * @param tetromino Тетромино для размещения
 */
void placeTetromino(Tetromino_t tetromino);

/**
 * @brief Очищает заполненные линии
 */
void clearLines();

/**
 * @brief Создает новое тетромино
 */
void spawnTetromino();

/**
 * @brief Поворачивает текущее тетромино
 */
void rotateTetromino();

/**
 * @brief Двигает тетромино
 * @param dx Смещение по оси X
 * @param dy Смещение по оси Y
 */
void moveTetromino(int dx, int dy);

/**
 * @brief Быстро опускает тетромино вниз
 */
void dropTetromino();

/**
 * @brief Обновляет счет игры
 * @param lines Количество очищенных линий
 */
void updateScore(int lines);

/**
 * @brief Сохраняет лучший результат
 */
void saveHighScore();

/**
 * @brief Загружает лучший результат
 */
void loadHighScore();

/**
 * @brief Получает блок тетромино
 * @param type Тип тетромино
 * @param rotation Поворот тетромино
 * @param x Координата X
 * @param y Координата Y
 * @return Значение блока (0 или 1)
 */
int getTetrominoBlock(int type, int rotation, int x, int y);

/**
 * @brief Глобальная переменная игры
 */
extern Game_t game;

#endif  // TETRIS_H