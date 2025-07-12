#include "tetris.h"

Game_t game = {0};

// Тетромино данные (7 типов × 4 поворота × 4×4 матрица)
static const int tetromino_shapes[7][4][4][4] = {
    // I-piece
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}},

    // O-piece
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}},

    // T-piece
    {{{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}}},

    // S-piece
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}},
     {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}}},

    // Z-piece
    {{{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}}},

    // J-piece
    {{{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}}},

    // L-piece
    {{{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}}};

void initGame() {
  // Инициализация игрового поля
  game.info.field = malloc(FIELD_HEIGHT * sizeof(int *));
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    game.info.field[i] = calloc(FIELD_WIDTH, sizeof(int));
  }

  // Инициализация поля для следующей фигуры
  game.info.next = malloc(NEXT_SIZE * sizeof(int *));
  for (int i = 0; i < NEXT_SIZE; i++) {
    game.info.next[i] = calloc(NEXT_SIZE, sizeof(int));
  }

  game.state = GAME_START;
  game.info.score = 500;
  game.info.level = 1;
  game.info.speed = 150;
  game.info.pause = 0;
  game.lines_cleared = 0;

  loadHighScore();
  srand(time(NULL));

  game.next.type = rand() % TETROMINO_COUNT;
  game.next.rotation = 0;

  // Обновляем матрицу next для отображения
  for (int y = 0; y < NEXT_SIZE; y++) {
    for (int x = 0; x < NEXT_SIZE; x++) {
      game.info.next[y][x] =
          getTetrominoBlock(game.next.type, game.next.rotation, x, y);
    }
  }
}

void freeGame() {
  if (game.info.field) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
      free(game.info.field[i]);
    }
    free(game.info.field);
  }

  if (game.info.next) {
    for (int i = 0; i < NEXT_SIZE; i++) {
      free(game.info.next[i]);
    }
    free(game.info.next);
  }
}

int getTetrominoBlock(int type, int rotation, int x, int y) {
  if (type < 0 || type >= TETROMINO_COUNT || rotation < 0 || rotation >= 4 ||
      x < 0 || x >= 4 || y < 0 || y >= 4) {
    return 0;
  }
  return tetromino_shapes[type][rotation][y][x];
}

bool canMove(Tetromino_t tetromino, int dx, int dy) {
  // Проверяем каждую ячейку в 4x4 матрице тетромино
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (getTetrominoBlock(tetromino.type, tetromino.rotation, x, y)) {
        // Вычисляем новые координаты блока после смещения
        int newX = tetromino.x + x + dx;
        int newY = tetromino.y + y + dy;
        // Проверяем, выходит ли блок за границы поля или пересекает другие
        // блоки
        if (newX < 0 || newX >= FIELD_WIDTH || newY >= FIELD_HEIGHT ||
            (newY >= 0 && game.info.field[newY][newX])) {
          return false;
        }
      }
    }
  }
  return true;
}

bool canRotate(Tetromino_t tetromino) {
  int newRotation = (tetromino.rotation + 1) % 4;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (getTetrominoBlock(tetromino.type, newRotation, x, y)) {
        int newX = tetromino.x + x;
        int newY = tetromino.y + y;

        if (newX < 0 || newX >= FIELD_WIDTH || newY >= FIELD_HEIGHT ||
            (newY >= 0 && game.info.field[newY][newX])) {
          return false;
        }
      }
    }
  }
  return true;
}

void placeTetromino(Tetromino_t tetromino) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (getTetrominoBlock(tetromino.type, tetromino.rotation, x, y)) {
        int fieldX = tetromino.x + x;
        int fieldY = tetromino.y + y;
        if (fieldY >= 0) {
          game.info.field[fieldY][fieldX] = 1;
        }
      }
    }
  }
}

void clearLines() {
  int linesCleared = 0;

  for (int y = FIELD_HEIGHT - 1; y >= 0; y--) {
    bool fullLine = true;
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (!game.info.field[y][x]) {
        fullLine = false;
        break;
      }
    }

    if (fullLine) {
      // Сдвигаем все линии вниз
      for (int moveY = y; moveY > 0; moveY--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          game.info.field[moveY][x] = game.info.field[moveY - 1][x];
        }
      }
      // Очищаем верхнюю линию
      for (int x = 0; x < FIELD_WIDTH; x++) {
        game.info.field[0][x] = 0;
      }
      linesCleared++;
      y++;  // Проверяем эту же линию снова
    }
  }

  if (linesCleared > 0) {
    updateScore(linesCleared);
    game.lines_cleared += linesCleared;
  }
}

void updateScore(int lines) {
  static const int scores[] = {0, 100, 300, 700, 1500};
  if (lines > 0 && lines <= 4) {
    game.info.score += scores[lines];
    if (game.info.score > game.info.high_score) {
      game.info.high_score = game.info.score;
      saveHighScore();
    }

    // Увеличение уровня каждые 600 очков
    int newLevel = game.info.score / 600 + 1;
    if (newLevel > game.info.level && newLevel <= 10) {
      game.info.level = newLevel;
      // Скорость уменьшается на 20мс каждый уровень (150, 130, 110, 90, 70,
      // 50, 30, 30, 30)
      game.info.speed = 150 - (game.info.level - 1) * 20;
      if (game.info.speed < 30)
        game.info.speed = 30;  // Минимальная скорость 30мс
    }
  }
}

void spawnTetromino() {
  game.current = game.next;
  game.current.x = FIELD_WIDTH / 2 - 2;
  game.current.y = 0;

  // Генерируем следующую фигуру
  game.next.type = rand() % TETROMINO_COUNT;
  game.next.rotation = 0;

  // Обновляем матрицу next
  for (int y = 0; y < NEXT_SIZE; y++) {
    for (int x = 0; x < NEXT_SIZE; x++) {
      game.info.next[y][x] =
          getTetrominoBlock(game.next.type, game.next.rotation, x, y);
    }
  }

  // Проверяем окончена ли игра
  if (!canMove(game.current, 0, 0)) {
    game.state = GAME_OVER;
  } else {
    game.state = GAME_MOVING;
  }
}

void rotateTetromino() {
  if (canRotate(game.current)) {
    game.current.rotation = (game.current.rotation + 1) % 4;
  }
}

void moveTetromino(int dx, int dy) {
  if (canMove(game.current, dx, dy)) {
    game.current.x += dx;
    game.current.y += dy;
  }
}

void dropTetromino() {
  while (canMove(game.current, 0, 1)) {
    game.current.y++;
  }
  placeTetromino(game.current);
  clearLines();
  game.state = GAME_SPAWN;
}

void saveHighScore() {
  FILE *file = fopen(HIGH_SCORE_FILE, "w");
  if (file) {
    fprintf(file, "%d", game.info.high_score);
    fclose(file);
  }
}

void loadHighScore() {
  FILE *file = fopen(HIGH_SCORE_FILE, "r");
  if (file) {
    fscanf(file, "%d", &game.info.high_score);
    fclose(file);
  } else {
    game.info.high_score = 0;
  }
}

void userInput(UserAction_t action, bool hold) {
  (void)hold;  // Пока не используем

  if (game.state == GAME_SHIFTING && action != Pause && action != Terminate) {
    return;
  }

  switch (action) {
    case Start:
      if (game.state == GAME_START) {
        spawnTetromino();
        game.state = GAME_MOVING;
        game.last_time = clock();
      } else if (game.state == GAME_OVER) {
        initGame();
      } else if (game.state == GAME_PAUSE) {
        game.state = GAME_MOVING;
        game.info.pause = 0;
      }
      break;
    case Pause:
      if (game.state == GAME_MOVING) {
        game.state = GAME_PAUSE;
        game.info.pause = 1;
      } else if (game.state == GAME_PAUSE) {
        game.state = GAME_MOVING;
        game.info.pause = 0;
      }
      break;
    case Terminate:
      game.state = GAME_EXIT;
      break;
    case Left:
      if (game.state == GAME_MOVING) {
        moveTetromino(-1, 0);
      }
      break;
    case Right:
      if (game.state == GAME_MOVING) {
        moveTetromino(1, 0);
      }
      break;
    case Down:
      if (game.state == GAME_MOVING) {
        dropTetromino();
      }
      break;
    case Action:
      if (game.state == GAME_MOVING) {
        rotateTetromino();
      }
      break;
    case Up:
      // Не используется
      break;
  }
}

GameInfo_t updateCurrentState() {
  clock_t current_time = clock();

  // Переход из GAME_MOVING в GAME_SHIFTING по таймеру
  if (game.state == GAME_MOVING &&
      (current_time - game.last_time) >
          (game.info.speed * CLOCKS_PER_SEC / 1000)) {
    game.state = GAME_SHIFTING;
  }

  if (game.state == GAME_SHIFTING) {
    if (canMove(game.current, 0, 1)) {
      // Фигура может двигаться вниз - перемещаем её
      game.current.y++;
      game.state = GAME_MOVING;  // Возвращаемся в состояние ожидания ввода
      game.last_time = current_time;  // Обновляем время только после сдвига
    } else {
      placeTetromino(game.current);
      clearLines();
      game.state = GAME_SPAWN;
    }
  }

  if (game.state == GAME_SPAWN) {
    spawnTetromino();
  }

  return game.info;
}