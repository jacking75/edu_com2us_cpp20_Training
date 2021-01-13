#ifndef _BOARD_H_
#define _BOARD_H_

extern object *board_obj[PMAX];
extern int board_handle[PMAX];
extern int board_handle2[PMAX];
extern long board_pos[PMAX];
extern char board_title[PMAX][44];

int look_board();
void list_board();
int  writeboard();
void write_board();
int read_board();
int del_board();

#endif
