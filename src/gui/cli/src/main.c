#include "cli.h"

int main() {
  initInterface();
  initGame();
  gameLoop();
  cleanupInterface();
  freeGame();
  return 0;
}