#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"

/* Helper function definitions */
static char get_board_at(game_state_t* state, int x, int y);
static void set_board_at(game_state_t* state, int x, int y, char ch);
static bool is_tail(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static int incr_x(char c);
static int incr_y(char c);
static void find_head(game_state_t* state, int snum);
static char next_square(game_state_t* state, int snum);
static void update_tail(game_state_t* state, int snum);
static void update_head(game_state_t* state, int snum);

/* Helper function to get a character from the board (already implemented for you). */
static char get_board_at(game_state_t* state, int x, int y) {
  return state->board[y][x];
}

/* Helper function to set a character on the board (already implemented for you). */
static void set_board_at(game_state_t* state, int x, int y, char ch) {
  state->board[y][x] = ch;
}

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t* game = (game_state_t*)malloc(sizeof(game_state_t));
  if (!game) {
    printf("Error, cannot create game\n");
  }
  game -> x_size = 14;
  game -> y_size = 10;
  unsigned int width = game->x_size;
  unsigned int height = game->y_size;
  game -> board = (char **)malloc(sizeof(char*) * height);
  if (!game->board) {
    printf("Error, cannot create game.board\n");
  }
  for (int i = 0; i < height; ++i) {
    game -> board[i] = (char*)malloc(sizeof(char) * width);
    for (int j = 0; j <width; ++j){
      if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
        game->board[i][j] = '#';
      } else {
        game->board[i][j] = ' ';
      }
    }
  }
  game -> num_snakes = 1;
  set_board_at(game, 9, 2, '*');
  set_board_at(game, 4, 4, 'd');
  set_board_at(game, 5, 4, '>');
  game -> snakes = (snake_t *) malloc (sizeof(snake_t) * game -> num_snakes);
  game -> snakes -> head_x = 5;
  game -> snakes -> head_y = 4;
  game -> snakes -> tail_x = 4;
  game -> snakes -> tail_y = 4;
  game -> snakes -> live = true;
  return game;
}

/* Task 2 */
void free_state(game_state_t* state) {
  free(state -> snakes);
  for (int i = 0; i < state -> y_size; ++i) {
    free(state->board[i]);
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int j = 0; j < state->y_size; ++j) {
    for (int i = 0; i < state->x_size; ++i) {
      fprintf(fp, "%c", state->board[j][i]);
    }
    fprintf(fp, "\n");
  }
  return;
}

/* Saves the current state into filename (already implemented for you). */
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */
static bool is_tail(char c) {
  char *str = "wasd";
  while (*str != '\0') {
    if (c == *str) {
      return true;
    }
    str++;
  }
  return false;
}

static bool is_snake(char c) {
  char *str = "wasd^<>vx";
  while (*str != '\0') {
    if (c == *str) {
      return true;
    }
    str++;
  }
  return false;
}

static char body_to_tail(char c) {
  char convert = ' ';
  switch (c)
  {
  case '^':
    convert = 'w';
    break;
  case '<':
    convert = 'a';
    break; 
  case '>':
    convert = 'd';
    break;
  case 'v':
    convert = 's';
    break;
  }
  return convert;
}

static int incr_x(char c) {
  if (c == '>' || c == 'd') {
    return 1;
  } 
  else if (c == '<' || c == 'a'){
    return -1;
  }
  return 0;
}

static int incr_y(char c) {
  if (c == 'v' || c == 's') {
    return 1;
  } 
  else if (c == '^' || c == 'w'){
    return -1;
  }
  return 0;
}

/* Task 4.2 */
static char next_square(game_state_t* state, int snum) {
  unsigned head_x = state->snakes[snum].head_x;
  unsigned head_y = state->snakes[snum].head_y;
  char snake_head = state->board[head_y][head_x];
  int dx = incr_x(snake_head);
  int dy = incr_y(snake_head);
  return state->board[head_y + dy][head_x + dx];
}

/* Task 4.3 */
static void update_head(game_state_t* state, int snum) {
  unsigned head_x = state->snakes[snum].head_x;
  unsigned head_y = state->snakes[snum].head_y;
  char snake_head = state->board[head_y][head_x];
  int dx = incr_x(snake_head);
  int dy = incr_y(snake_head);
  state->board[head_y + dy][head_x + dx] = snake_head;
  state->snakes[snum].head_x = head_x + dx;
  state->snakes[snum].head_y = head_y + dy;
  return;
}

/* Task 4.4 */
static void update_tail(game_state_t* state, int snum) {
  unsigned tail_x = state->snakes[snum].tail_x;
  unsigned tail_y = state->snakes[snum].tail_y;
  char snake_tail = state->board[tail_y][tail_x];
  state->board[tail_y][tail_x] = ' ';
  int dx = incr_x(snake_tail);
  int dy = incr_y(snake_tail);
  int new_tail_x = tail_x + dx;
  int new_tail_y = tail_y + dy;
  state->board[new_tail_y][new_tail_x] = body_to_tail(state->board[new_tail_y][new_tail_x]);
  state->snakes[snum].tail_x = new_tail_x;
  state->snakes[snum].tail_y = new_tail_y;
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  int snum = state->num_snakes;
  for (int i = 0; i < snum; ++i) {
    char next_step = next_square(state, i);
    if (is_snake(next_step)) {
      state->board[state->snakes[i].head_y][state->snakes[i].head_x] = 'x';
      state->snakes[i].live = false;
    }
    switch (next_step)
    {
    case ' ':
      update_head(state, i);
      update_tail(state, i);
      break;
    case '#':
      state->board[state->snakes[i].head_y][state->snakes[i].head_x] = 'x';
      state->snakes[i].live = false;
      break;
    case '*':
    update_head(state, i);
    add_food(state);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  game_state_t *game = (game_state_t*) malloc (sizeof(game_state_t));
  game->board = (char **) malloc (sizeof(char *) * 100);
  char buffer[100];
  game->snakes = (snake_t *) malloc (sizeof(snake_t) * 100);
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "\nCannot open the file\n");
    exit(1);
  }
  int length = 0;
  int i;
  for (i = 0; !feof(fp); ++i) {
    fgets(buffer, 100, fp);
    length = strlen(buffer);
    game->board[i] = (char *) malloc (sizeof(char) * length);
    for (int j = 0; j < length; ++j) {
      game->board[i][j] = buffer[j];
    }
  }
  game->x_size = length - 1;
  game->y_size = i - 1;
  fclose(fp);
  return game;
}

/* Task 6.1 */
static void find_head(game_state_t* state, int snum) {
  unsigned tail_x = state -> snakes[snum].tail_x;
  unsigned tail_y = state -> snakes[snum].tail_y;
  char snake_tail = state -> board[tail_y][tail_x];
  int dx = 0;
  int dy = 0;
  char next_ch = snake_tail;
  while (is_snake(next_ch)) {
    dx = incr_x(next_ch);
    dy = incr_y(next_ch);
    tail_x += dx;
    tail_y += dy;
    next_ch = state -> board[tail_y][tail_x];
  }
  state -> snakes[snum].head_x = tail_x - dx;
  state -> snakes[snum].head_y = tail_y - dy;
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  unsigned width = state->x_size;
  unsigned height = state->y_size;
  int snum = 0;
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      if (is_tail(state->board[j][i])) {
        state->snakes[snum].tail_y = j;
        state->snakes[snum].tail_x = i;
        find_head(state, snum);
        ++snum;
      }
    }
  }
  state->num_snakes = snum;
  return state;
}
