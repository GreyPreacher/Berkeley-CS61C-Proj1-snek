#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"
#include <regex.h>

#define BOARD_ROW 10
#define BOARD_COL 14

const char SNAKE_TAIL[4] = {'w', 'a', 's', 'd'};
const char SNAKE_BODY[5] = {'^', '<', 'v', '>', 'x'};

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
    game_state_t* state = (game_state_t*)malloc(sizeof(game_state_t));
    state->x_size = BOARD_COL;
    state->y_size = BOARD_ROW;
    state->board = (char**) malloc(sizeof(char*) * BOARD_ROW);
    for (int i = 0; i < BOARD_ROW; ++i) {
        state->board[i] = (char*)malloc(sizeof(char) * BOARD_COL);
        if (i == 0 || i == BOARD_ROW - 1) {
            memset(state->board[i], '#', sizeof(char) * BOARD_COL);
        } else {
            memset(state->board[i], ' ', sizeof(char) * BOARD_COL);
            memset(state->board[i], '#', sizeof(char));
            memset(state->board[i] + BOARD_COL - 1, '#', sizeof(char));
        }
    }
    set_board_at(state, 9, 2, '*');
    state->num_snakes = 1;
    state->snakes = (snake_t*)malloc(sizeof(snake_t));
    state->snakes[0].head_x = 5;
    state->snakes[0].head_y = 4;
    state->snakes[0].tail_x = 4;
    state->snakes[0].tail_y = 4;
    set_board_at(state, state->snakes[0].head_x, state->snakes[0].head_y, '>');
    set_board_at(state, state->snakes[0].tail_x, state->snakes[0].tail_y, 'd');
    state->snakes[0].live = true;
    return state;
  return NULL;
}

/* Task 2 */
void free_state(game_state_t* state) {
    if (state) {
      if (state->snakes) {
          free(state->snakes);
      }
      for (int i = 0; i < state->y_size; ++i) {
          if (state->board[i]) {
              free(state->board[i]);
          }
      }
      free(state->board);
      free(state);
  }
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int i = 0; i < state->y_size; ++i) {
    for (int j = 0; j < state->x_size; ++j) {
      fprintf(fp, "%c", state->board[i][j]);
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
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}

static bool is_snake(char c) {
  return (c == 'v' || c == 'x' || c == '<' || c == '>' || c == '^' || is_tail(c));
}

static char body_to_tail(char c) {
  char temp;
  switch (c)
  {
    case '^': temp = 'w'; break;
    case '<': temp = 'a'; break;
    case 'v': temp = 's'; break;
    case '>': temp = 'd'; break;
  }
  return temp;
}

static int incr_x(char c) {
  if (c == 'd' || c == '>') {
    return 1;
  }
  if (c == 'a' || c == '<') {
    return -1;
  }
  return 0;
}

static int incr_y(char c) {
  if (c == 'w' || c == '^') {
    return -1;
  }
  if (c == 's' || c == 'v') {
    return 1;
  }
  return 0;
}

/* Task 4.2 */
static char next_square(game_state_t* state, int snum) {
  if (snum < 0 || snum >= state->num_snakes) {
    printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
    return '?';
  }
  if (!state->snakes[snum].live) {
    printf("snake is dead, called by %s at %d\n", __func__, __LINE__);
    return '?';
  }
  char snake_head = get_board_at(state, state->snakes[snum].head_x, state->snakes[snum].head_y);
  int x_ret = incr_x(snake_head) + state->snakes[snum].head_x;
  int y_ret = incr_y(snake_head) + state->snakes[snum].head_y;
  return get_board_at(state, x_ret, y_ret);
}

/* Task 4.3 */
static void update_head(game_state_t* state, int snum) {
  if (snum < 0 || snum >= state->num_snakes) {
    printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
    return;
  }
  if (!state->snakes[snum].live) {
    printf("snake is dead, called by %s at %d\n", __func__, __LINE__);
    return;
  }
  char snake_head = get_board_at(state, state->snakes[snum].head_x, state->snakes[snum].head_y);
  int x_ret = incr_x(snake_head) + state->snakes[snum].head_x;
  int y_ret = incr_y(snake_head) + state->snakes[snum].head_y;
  state->snakes[snum].head_x = x_ret;
  state->snakes[snum].head_y = y_ret;
  set_board_at(state, x_ret, y_ret, snake_head);
  return;
}

/* Task 4.4 */
static void update_tail(game_state_t* state, int snum) {
  if (snum < 0 || snum >= state->num_snakes) {
    printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
    return;
  }
  if (!state->snakes[snum].live) {
    printf("snake is dead, called by %s at %d\n", __func__, __LINE__);
    return;
  }

  char snake_tail = get_board_at(state, state->snakes[snum].tail_x, state->snakes[snum].tail_y);
  set_board_at(state, state->snakes[snum].tail_x, state->snakes[snum].tail_y, ' ');

  int x_ret = incr_x(snake_tail) + state->snakes[snum].tail_x;
  int y_ret = incr_y(snake_tail) + state->snakes[snum].tail_y;
  char next_tail = get_board_at(state, x_ret, y_ret);
  state->snakes[snum].tail_x = x_ret;
  state->snakes[snum].tail_y = y_ret;
  set_board_at(state, state->snakes[snum].tail_x, state->snakes[snum].tail_y, body_to_tail(next_tail));
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (int i = 0; i < state->num_snakes; ++i) {
    if (state->snakes[i].live){
      char next_step = next_square(state, i);
      if (next_step == '#' || is_snake(next_step)) {
        state->snakes[i].live = false;
        set_board_at(state, state->snakes[i].head_x, state->snakes[i].head_y, 'x');
      } else if (next_step == '*') {
        update_head(state, i);
        add_food(state);
      } else {
        update_head(state, i);
        update_tail(state, i);
      }
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  int row, col;
  char ch;
  game_state_t *state = (game_state_t*)malloc(sizeof(game_state_t));
  
  FILE *fp = fopen(filename, "r");
  for (row = 0;; row++) {
    if ((ch = fgetc(fp)) == EOF){
      break;
    }
    for (col = 0;; ch = fgetc(fp), col++) {
      if (ch != '\n') {
        continue;
      } else {
        break;
      }
    }
  }
  fclose(fp);
  fp = NULL;
  ch = '\0';

  //get board size
  state->y_size = row;
  state->x_size = col;

  //initialize board
  state->board = (char**)malloc(sizeof(char*) * row);
  for (int i = 0; i < row; i++) {
    state->board[i] = (char*)malloc(sizeof(char) * col);
  }
  FILE *f = fopen(filename, "r");
  for (row = 0;; row++) {
    if ((ch = fgetc(f)) == EOF){
      break;
    }
    for (col = 0;; ch = fgetc(f), col++) {
      if (ch != '\n') {
        state->board[row][col] = ch;
      } else {
        break;
      }
    }
  }
  fclose(f);

  return state;
}

/* Task 6.1 */
static void find_head(game_state_t* state, int snum) {
  if (snum < 0 || snum >= state->num_snakes) {
    printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
    return;
  }

  int x = state->snakes[snum].tail_x;
  int y = state->snakes[snum].tail_y;
  char ch = get_board_at(state, x, y), next_ch;
  while (1) {
    int next_x = incr_x(ch) + x;
    int next_y = incr_y(ch) + y;
    next_ch = get_board_at(state, next_x, next_y);
    if (is_snake(next_ch)) {
      x = next_x;
      y = next_y;
      ch = next_ch;
    } else {
      state->snakes[snum].head_x = x;
      state->snakes[snum].head_y = y;
      return;
    }
  }
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  state->num_snakes = 0;
  state->snakes = (snake_t*)malloc(sizeof(snake_t));

  for (int row = 0; row < state->y_size; ++row) {
    for (int col = 0; col < state->x_size; ++col) {
      char ch = get_board_at(state, col, row);
      if (is_tail(ch)) {
        state->num_snakes++;
        state->snakes = (snake_t*)realloc(state->snakes, sizeof(snake_t) * state->num_snakes);
        state->snakes[state->num_snakes - 1].tail_x = col;
        state->snakes[state->num_snakes - 1].tail_y = row;
        state->snakes[state->num_snakes - 1].live = true;
        find_head(state, state->num_snakes - 1);
      }
    }
  }
  return state;
}
