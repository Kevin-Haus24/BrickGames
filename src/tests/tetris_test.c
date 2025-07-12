#include "tetris.h"

#include <check.h>

START_TEST(test_init_game) {
  initGame();
  ck_assert_ptr_nonnull(game.info.field);
  ck_assert_ptr_nonnull(game.info.next);
  ck_assert_int_eq(game.info.score, 0);
  ck_assert_int_eq(game.info.level, 1);
  ck_assert_int_eq(game.info.speed, 150);
  ck_assert_int_eq(game.info.pause, 0);
  freeGame();
}

START_TEST(test_user_input_start) {
  initGame();
  game.state = GAME_START;
  userInput(Start, false);
  ck_assert_int_eq(game.state, GAME_MOVING);
  freeGame();
}
END_TEST

START_TEST(test_user_input_pause) {
  initGame();
  game.state = GAME_MOVING;
  userInput(Pause, false);
  ck_assert_int_eq(game.state, GAME_PAUSE);
  ck_assert_int_eq(game.info.pause, 1);

  userInput(Pause, false);
  ck_assert_int_eq(game.state, GAME_MOVING);
  ck_assert_int_eq(game.info.pause, 0);
  freeGame();
}
END_TEST

START_TEST(test_user_input_terminate) {
  initGame();
  game.state = GAME_MOVING;
  userInput(Terminate, false);
  ck_assert_int_eq(game.state, GAME_EXIT);
  freeGame();
}
END_TEST

START_TEST(test_tetromino_spawn) {
  initGame();

  // Фиксируем seed для предсказуемости
  srand(12345);

  spawnTetromino();
  ck_assert_int_ge(game.current.type, 0);
  ck_assert_int_lt(game.current.type, TETROMINO_COUNT);
  ck_assert_int_eq(game.current.x, FIELD_WIDTH / 2 - 2);
  ck_assert_int_eq(game.current.y, 0);
  ck_assert_int_eq(game.current.rotation, 0);
  freeGame();
}
END_TEST

START_TEST(test_tetromino_movement) {
  initGame();

  srand(12345);

  spawnTetromino();
  game.state = GAME_MOVING;

  int initial_x = game.current.x;
  userInput(Left, false);
  ck_assert_int_eq(game.current.x, initial_x - 1);

  userInput(Right, false);
  ck_assert_int_eq(game.current.x, initial_x);

  freeGame();
}
END_TEST

START_TEST(test_tetromino_rotation) {
  initGame();

  srand(12345);

  spawnTetromino();
  game.state = GAME_MOVING;

  int initial_rotation = game.current.rotation;
  userInput(Action, false);
  ck_assert_int_eq(game.current.rotation, (initial_rotation + 1) % 4);

  freeGame();
}
END_TEST

START_TEST(test_can_move_boundaries) {
  initGame();
  spawnTetromino();

  game.current.type = 1;  // O-piece
  game.current.rotation = 0;

  // Тест движения за левую границу
  // O-piece имеет блоки в позициях (1,1), (1,2), (2,1), (2,2)
  // Поэтому при x = -1 блоки будут в позициях 0, 1 - это валидно
  // При x = -2 блоки будут в позициях -1, 0 - левый блок за границей
  game.current.x = -1;
  ck_assert_int_eq(canMove(game.current, -1, 0), 0);

  // Тест движения за правую границу
  // При x = FIELD_WIDTH - 2 блоки будут в позициях FIELD_WIDTH-1, FIELD_WIDTH
  // (за границей)
  game.current.x = FIELD_WIDTH - 2;
  ck_assert_int_eq(canMove(game.current, 1, 0), 0);

  // Тест движения за нижнюю границу
  // При y = FIELD_HEIGHT - 2 блоки будут в позициях FIELD_HEIGHT-1,
  // FIELD_HEIGHT (за границей)
  game.current.y = FIELD_HEIGHT - 2;
  ck_assert_int_eq(canMove(game.current, 0, 1), 0);

  freeGame();
}
END_TEST

START_TEST(test_score_calculation) {
  initGame();

  updateScore(1);
  ck_assert_int_eq(game.info.score, 100);

  game.info.score = 0;
  updateScore(2);
  ck_assert_int_eq(game.info.score, 300);

  game.info.score = 0;
  updateScore(3);
  ck_assert_int_eq(game.info.score, 700);

  game.info.score = 0;
  updateScore(4);
  ck_assert_int_eq(game.info.score, 1500);

  freeGame();
}
END_TEST

START_TEST(test_level_progression) {
  initGame();

  game.info.score = 500;
  updateScore(1);  // +100 очков
  ck_assert_int_eq(game.info.level, 2);
  ck_assert_int_eq(game.info.speed, 130);

  game.info.score = 1100;
  updateScore(1);  // +100 очков
  ck_assert_int_eq(game.info.level, 3);
  ck_assert_int_eq(game.info.speed, 110);

  game.info.score = 5300;
  updateScore(1);  // +100 очков
  ck_assert_int_eq(game.info.level, 10);
  ck_assert_int_eq(game.info.speed, 30);

  freeGame();
}
END_TEST

START_TEST(test_high_score) {
  initGame();

  int initial_high_score = game.info.high_score;
  game.info.score = initial_high_score + 100;
  updateScore(1);  // Должно обновить рекорд
  ck_assert_int_ge(game.info.high_score, initial_high_score + 100);

  freeGame();
}
END_TEST

START_TEST(test_field_initialization) {
  initGame();

  // Проверяем, что поле пустое
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      ck_assert_int_eq(game.info.field[y][x], 0);
    }
  }

  freeGame();
}
END_TEST

START_TEST(test_tetromino_shapes) {
  initGame();

  // Проверяем, что все типы тетромино валидны
  for (int type = 0; type < TETROMINO_COUNT; type++) {
    for (int rotation = 0; rotation < 4; rotation++) {
      int has_blocks = 0;
      for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
          if (getTetrominoBlock(type, rotation, x, y)) {
            has_blocks = 1;
          }
        }
      }
      ck_assert_int_eq(has_blocks, 1);  // Каждая фигура должна иметь блоки
    }
  }

  freeGame();
}
END_TEST

START_TEST(test_game_over_condition) {
  initGame();

  // Заполняем верхнюю часть поля
  for (int x = 0; x < FIELD_WIDTH; x++) {
    game.info.field[0][x] = 1;
    game.info.field[1][x] = 1;
  }

  // Пытаемся создать новую фигуру
  spawnTetromino();
  ck_assert_int_eq(game.state, GAME_OVER);

  freeGame();
}
END_TEST

START_TEST(test_update_current_state) {
  initGame();
  game.state = GAME_MOVING;

  GameInfo_t info = updateCurrentState();
  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
  ck_assert_int_ge(info.score, 0);
  ck_assert_int_ge(info.level, 1);
  ck_assert_int_ge(info.speed, 100);

  freeGame();
}
END_TEST

START_TEST(test_place_tetromino) {
  initGame();

  Tetromino_t tetromino;
  tetromino.type = 1;  // O-piece (квадрат)
  tetromino.rotation = 0;
  tetromino.x = 4;
  tetromino.y = 17;

  placeTetromino(tetromino);

  // Проверяем, что блоки фигуры появились на поле
  int blocks_placed = 0;
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (game.info.field[y][x]) {
        blocks_placed++;
      }
    }
  }
  ck_assert_int_eq(blocks_placed, 4);  // O-piece состоит из 4 блоков

  freeGame();
}
END_TEST

START_TEST(test_clear_lines) {
  initGame();

  // Заполняем нижнюю линию полностью
  for (int x = 0; x < FIELD_WIDTH; x++) {
    game.info.field[FIELD_HEIGHT - 1][x] = 1;
  }

  // Заполняем предпоследнюю линию частично
  for (int x = 0; x < FIELD_WIDTH - 1; x++) {
    game.info.field[FIELD_HEIGHT - 2][x] = 1;
  }

  int initial_score = game.info.score;
  clearLines();

  // Проверяем, что очки начислены
  ck_assert_int_gt(game.info.score, initial_score);

  // Проверяем, что нижняя линия теперь пустая
  int bottom_line_blocks = 0;
  for (int x = 0; x < FIELD_WIDTH; x++) {
    if (game.info.field[FIELD_HEIGHT - 1][x]) {
      bottom_line_blocks++;
    }
  }
  ck_assert_int_eq(bottom_line_blocks,
                   FIELD_WIDTH - 1);  // Сдвинулась частично заполненная линия

  freeGame();
}
END_TEST

START_TEST(test_clear_multiple_lines) {
  initGame();

  // Заполняем 3 нижние линии полностью
  for (int y = FIELD_HEIGHT - 3; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      game.info.field[y][x] = 1;
    }
  }

  int initial_score = game.info.score;
  clearLines();

  // Проверяем, что очки начислены за 3 линии (700 очков)
  ck_assert_int_eq(game.info.score, initial_score + 700);

  freeGame();
}
END_TEST

START_TEST(test_can_rotate_blocked) {
  initGame();

  game.current.type = 2;  // T-piece
  game.current.rotation = 0;
  game.current.x = 4;
  game.current.y = 1;

  // Создаем препятствие для поворота
  game.info.field[1][6] = 1;  // Блокируем справа
  game.info.field[2][5] = 1;  // Блокируем снизу-справа

  // Пытаемся повернуть - должно быть заблокировано
  bool can_rotate = canRotate(game.current);
  ck_assert_int_eq(can_rotate, 0);

  freeGame();
}
END_TEST

START_TEST(test_get_tetromino_block_invalid) {
  initGame();

  // Тестируем с невалидными параметрами
  ck_assert_int_eq(getTetrominoBlock(-1, 0, 0, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(TETROMINO_COUNT, 0, 0, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(0, -1, 0, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(0, 4, 0, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(0, 0, -1, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(0, 0, 4, 0), 0);
  ck_assert_int_eq(getTetrominoBlock(0, 0, 0, -1), 0);
  ck_assert_int_eq(getTetrominoBlock(0, 0, 0, 4), 0);

  freeGame();
}
END_TEST

START_TEST(test_drop_tetromino) {
  initGame();

  srand(12345);

  spawnTetromino();

  // Используем userInput для вызова dropTetromino
  game.state = GAME_MOVING;
  userInput(Down, false);

  // Обновляем состояние для обработки GAME_SPAWN
  updateCurrentState();

  // После drop должна появиться новая фигура
  // (текущая должна была упасть и разместиться)
  ck_assert_int_eq(game.current.y, 0);  // Новая фигура появляется сверху

  freeGame();
}
END_TEST

START_TEST(test_save_load_high_score) {
  initGame();

  // Устанавливаем рекорд
  game.info.high_score = 12345;
  saveHighScore();

  // Сбрасываем рекорд
  game.info.high_score = 0;

  // Загружаем рекорд
  loadHighScore();

  // Проверяем, что рекорд загрузился
  ck_assert_int_eq(game.info.high_score, 12345);

  freeGame();
}
END_TEST

START_TEST(test_pause_toggle) {
  initGame();
  game.state = GAME_MOVING;
  ck_assert_int_eq(game.info.pause, 0);

  userInput(Pause, false);
  ck_assert_int_eq(game.state, GAME_PAUSE);
  ck_assert_int_eq(game.info.pause, 1);

  userInput(Pause, false);
  ck_assert_int_eq(game.state, GAME_MOVING);
  ck_assert_int_eq(game.info.pause, 0);

  userInput(Pause, false);
  userInput(Start, false);
  ck_assert_int_eq(game.state, GAME_MOVING);
  ck_assert_int_eq(game.info.pause, 0);

  freeGame();
}
END_TEST

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core, *tc_movement, *tc_scoring, *tc_gameplay;

  s = suite_create("Tetris");

  // Основные тесты
  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_init_game);
  tcase_add_test(tc_core, test_field_initialization);
  tcase_add_test(tc_core, test_tetromino_shapes);
  tcase_add_test(tc_core, test_update_current_state);
  suite_add_tcase(s, tc_core);

  // Тесты движения и управления
  tc_movement = tcase_create("Movement");
  tcase_add_test(tc_movement, test_user_input_start);
  tcase_add_test(tc_movement, test_user_input_pause);
  tcase_add_test(tc_movement, test_user_input_terminate);
  tcase_add_test(tc_movement, test_tetromino_spawn);
  tcase_add_test(tc_movement, test_tetromino_movement);
  tcase_add_test(tc_movement, test_tetromino_rotation);
  tcase_add_test(tc_movement, test_can_move_boundaries);
  tcase_add_test(tc_movement, test_pause_toggle);
  suite_add_tcase(s, tc_movement);

  // Тесты подсчета очков
  tc_scoring = tcase_create("Scoring");
  tcase_add_test(tc_scoring, test_score_calculation);
  tcase_add_test(tc_scoring, test_level_progression);
  tcase_add_test(tc_scoring, test_high_score);
  suite_add_tcase(s, tc_scoring);

  // Тесты игрового процесса
  tc_gameplay = tcase_create("Gameplay");
  tcase_add_test(tc_gameplay, test_game_over_condition);
  tcase_add_test(tc_gameplay, test_place_tetromino);
  tcase_add_test(tc_gameplay, test_clear_lines);
  tcase_add_test(tc_gameplay, test_clear_multiple_lines);
  tcase_add_test(tc_gameplay, test_can_rotate_blocked);
  tcase_add_test(tc_gameplay, test_get_tetromino_block_invalid);
  tcase_add_test(tc_gameplay, test_drop_tetromino);
  tcase_add_test(tc_gameplay, test_save_load_high_score);
  suite_add_tcase(s, tc_gameplay);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}