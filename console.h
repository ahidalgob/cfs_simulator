#ifndef CONSOLE_H
#define CONSOLE_H

/* some constants */
#include <curses.h>

int  console_init(void);  /* initialise LCD return sucess/fail */

/* LCD api */
void lcd_set_pos(int row, int column);

void lcd_set_colour(int foreground, int background);
void lcd_set_attr(int attributes);
void lcd_unset_attr(int attributes);
int lcd_write(const char *fmt,...);
int lcd_write_at(int row, int col, const char *fmt,...);
void print_running(int cpu, int id, int nice, long long v_runtime);
void print_ready(int cpu, int id, long long v_runtime);
void print_ready2(int cpu, int id, long long v_runtime);
void print_ready3(int cpu, int id, long long v_runtime, int size);
void print_queues(int queue, int id, int size, int last);

#endif

