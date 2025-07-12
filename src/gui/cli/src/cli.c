#include "cli.h"

#include <unistd.h>

static WINDOW *game_win;
static WINDOW *info_win;

void initInterface() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_RED);
  }

  // Создаем окна
  game_win = newwin(GAME_WINDOW_HEIGHT, GAME_WINDOW_WIDTH, 1, 1);
  info_win =
      newwin(INFO_WINDOW_HEIGHT, INFO_WINDOW_WIDTH, 1, GAME_WINDOW_WIDTH + 2);

  box(game_win, 0, 0);
  box(info_win, 0, 0);

  mvwprintw(game_win, 0, 1, " TETRIS ");
  mvwprintw(info_win, 0, 1, " INFO ");

  wrefresh(game_win);
  wrefresh(info_win);
  refresh();
}

void cleanupInterface() {
  delwin(game_win);
  delwin(info_win);
  endwin();
}

void drawField(WINDOW *win, int **field) {
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      mvwaddch(win, y + 1, x * 2 + 1, field[y][x] ? '[' : ' ');
      mvwaddch(win, y + 1, x * 2 + 2, field[y][x] ? ']' : ' ');
    }
  }

  // Отрисовка текущей фигуры
  extern Game_t game;
  if (game.state == GAME_MOVING) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (getTetrominoBlock(game.current.type, game.current.rotation, x, y)) {
          int fieldX = game.current.x + x;
          int fieldY = game.current.y + y;
          if (fieldX >= 0 && fieldX < FIELD_WIDTH && fieldY >= 0 &&
              fieldY < FIELD_HEIGHT) {
            mvwaddch(win, fieldY + 1, fieldX * 2 + 1, '{');
            mvwaddch(win, fieldY + 1, fieldX * 2 + 2, '}');
          }
        }
      }
    }
  }
}

void drawNext(WINDOW *win, int **next) {
  mvwprintw(win, 2, 2, "NEXT:");
  for (int y = 0; y < NEXT_SIZE; y++) {
    for (int x = 0; x < NEXT_SIZE; x++) {
      mvwaddch(win, y + 4, x * 2 + 2, next[y][x] ? '{' : ' ');
      mvwaddch(win, y + 4, x * 2 + 3, next[y][x] ? '}' : ' ');
    }
  }
}

void drawInfo(WINDOW *win, GameInfo_t info) {
  mvwprintw(win, 10, 2, "SCORE: %d", info.score);
  mvwprintw(win, 11, 2, "HIGH: %d", info.high_score);
  mvwprintw(win, 12, 2, "LEVEL: %d", info.level);
  mvwprintw(win, 13, 2, "SPEED: %d", info.speed);

  if (info.pause) {
    wattron(win, COLOR_PAIR(3));
    mvwprintw(win, 15, 2, "PAUSED");
    wattroff(win, COLOR_PAIR(3));
  } else {
    mvwprintw(win, 15, 2, "      ");
  }

  mvwprintw(win, 17, 2, "Controls:");
  mvwprintw(win, 18, 2, "S - Start");
  mvwprintw(win, 19, 2, "P - Pause");
  mvwprintw(win, 20, 2, "Q - Quit");
}

void drawGame(GameInfo_t info) {
  extern Game_t game;

  // Очищаем окна
  werase(game_win);
  werase(info_win);

  box(game_win, 0, 0);
  box(info_win, 0, 0);

  mvwprintw(game_win, 0, 1, " TETRIS ");
  mvwprintw(info_win, 0, 1, " INFO ");

  drawField(game_win, info.field);
  drawNext(info_win, info.next);
  drawInfo(info_win, info);

  if (game.state == GAME_START) {
    wattron(game_win, COLOR_PAIR(2));
    mvwprintw(game_win, FIELD_HEIGHT / 2 - 1, (GAME_WINDOW_WIDTH - 10) / 2,
              "WELCOME TO");
    mvwprintw(game_win, FIELD_HEIGHT / 2, (GAME_WINDOW_WIDTH - 6) / 2,
              "TETRIS");
    mvwprintw(game_win, FIELD_HEIGHT / 2 + 2, (GAME_WINDOW_WIDTH - 10) / 2,
              "Press S to");
    mvwprintw(game_win, FIELD_HEIGHT / 2 + 3, (GAME_WINDOW_WIDTH - 5) / 2,
              "START");
    wattroff(game_win, COLOR_PAIR(2));
  } else if (game.state == GAME_OVER) {
    wattron(game_win, COLOR_PAIR(3));
    mvwprintw(game_win, FIELD_HEIGHT / 2, (GAME_WINDOW_WIDTH - 9) / 2,
              "GAME OVER");
    mvwprintw(game_win, FIELD_HEIGHT / 2 + 1, (GAME_WINDOW_WIDTH - 7) / 2,
              "Press S");
    wattroff(game_win, COLOR_PAIR(3));
  }

  wrefresh(game_win);
  wrefresh(info_win);
  refresh();
}

int getInput(UserAction_t *action) {
  int ch = getch();
  switch (ch) {
    case 's':
    case 'S':
      *action = Start;
      return 1;
    case 'p':
    case 'P':
      *action = Pause;
      return 1;
    case 'q':
    case 'Q':
      *action = Terminate;
      return 1;
    case KEY_LEFT:
      *action = Left;
      return 1;
    case KEY_RIGHT:
      *action = Right;
      return 1;
    case KEY_DOWN:
      *action = Down;
      return 1;
    case KEY_UP:
      *action = Up;
      return 1;
    case ' ':
      *action = Action;
      return 1;
    default:
      return 0;  // Нет действия
  }
}

void gameLoop() {
  extern Game_t game;

  while (game.state != GAME_EXIT) {
    UserAction_t action;
    if (getInput(&action)) {
      userInput(action, false);
    }

    GameInfo_t info = updateCurrentState();
    drawGame(info);

    napms(1);
  }
}