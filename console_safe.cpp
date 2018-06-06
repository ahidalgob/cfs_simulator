#define  _XOPEN_SOURCE_EXTENDED 1

#include <unistd.h>
#include <curses.h>
#include <locale.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <assert.h>
#include <semaphore.h>

#include "console.h"

static sem_t sem;

static short setcolor(short fg, short bg)
{
    static short pairs = 4;
    short p;
    for( p=4 ; p<pairs ; p++){
        short f,b;
        pair_content(p, &f, &b);
        if( f==fg && b==bg ) return p;
    }
    init_pair(p, fg, bg);
    pairs++;
    return p;
}

/*
0x00-0x07:  standard colors (as in ESC [ 30–37 m)
0x08-0x0F:  high intensity colors (as in ESC [ 90–97 m)
0x10-0xE7:  6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)
0xE8-0xFF:  grayscale from black to white in 24 steps
*/
void lcdsetcolor(short fg, short bg)
{
    color_set( setcolor(fg,bg), NULL );
}


void lcdshutdown()
{
    endwin();
}

static WINDOW *lcd;
static WINDOW *screen;

int console_init()
{
    int rc;

    setlocale(LC_ALL, "");
    initscr();			    /* Start curses mode              */
    cbreak();			    /* Line buffering disabled        */
    noecho();			    /* Don't echo() while we do getch */
    start_color();
    curs_set(0);

    atexit(lcdshutdown);
    lcd = newwin(0, 0, 1,0);
    screen = derwin(lcd,LINES-3,COLS-2,1,1);
    nodelay(screen,TRUE);
    wcolor_set(lcd,setcolor(7,8), NULL);
    box(lcd,0,0);

    mvwaddstr(lcd,0,LINES / 4,"CPU 1");
    mvwaddstr(lcd,0,LINES,"CPU 2");
    mvwaddstr(lcd,0,2*LINES,"CPU 3");
    mvwaddstr(lcd,0,3*LINES,"CPU 4");
    wrefresh(lcd);
    init_pair(1, 9, 8);
    init_pair(2, 10, 8);
    init_pair(3, 12, 8);
    keypad(screen, TRUE);	/* We get F1, F2 etc..            */
    wrefresh(screen);
    rc = sem_init(&sem, 0, 1);
    assert(rc == 0);

    lcd_write_at(1, (LINES/4)-1, "TASKS");
    lcd_write_at(1, LINES-1, "TASKS");
    lcd_write_at(1, 2*LINES-1, "TASKS");
    lcd_write_at(1, 3*LINES-1, "TASKS");

    for (int i = 0; i < 4; ++i)
        print_running(i, 0, 0, 0);

    lcd_write_at(9, (LINES/4)-10, "Red Black Tree");
    lcd_write_at(10, (LINES/4)-10, "---------------------");

    for (int i = 0; i < 4; ++i)
        print_ready(i, 0, 0);

    for (int i = 0; i < 4; ++i)
        print_ready2(i, 0, 0);

    for (int i = 0; i < 4; ++i)
        print_ready3(i, 0, 0, 0);

    lcd_write_at(27, (LINES/4)-10, "Waitqueues");
    lcd_write_at(28, (LINES/4)-10, "---------------------");

    for (int i = 0; i < 4; ++i)
        print_queues(i, 0, 0, -1);

    return 1;
}

void lcd_set_pos(int row, int column)
{
    int rc;
    rc = sem_wait(&sem);
    assert(rc == 0);
    wmove(screen,row,column);
    rc = sem_post(&sem);
    assert(rc == 0);
}

void lcd_set_colour(int foreground, int background)
{
    int rc;
    rc = sem_wait(&sem);
    assert(rc == 0);
    wcolor_set(screen,setcolor(foreground,background),NULL);
    rc = sem_post(&sem);
    assert(rc == 0);
}

void lcd_set_attr(int attributes)
{
    int rc;
    rc = sem_wait(&sem);
    assert(rc == 0);
    wattron(screen, attributes);
    rc = sem_post(&sem);
    assert(rc == 0);
}

void lcd_unset_attr(int attributes)
{
    int rc;
    rc = sem_wait(&sem);
    assert(rc == 0);
    wattroff(screen, attributes);
    rc = sem_post(&sem);
    assert(rc == 0);
}

int  lcd_write(const char *fmt,...)
{
    int rc;
    int ret;
    va_list args;
    rc = sem_wait(&sem);
    assert(rc == 0);
    va_start(args, fmt);
    ret = vw_printw(screen, fmt, args);
    va_end(args);
    wrefresh(lcd);
    wrefresh(screen);
    rc = sem_post(&sem);
    assert(rc == 0);
    return ret;
}

int  lcd_write_at(int row, int col, const char *fmt,...)
{
    int rc;
    int ret;
    va_list args;
    rc = sem_wait(&sem);
    assert(rc == 0);
    wmove(screen,row,col);
    va_start(args, fmt);
    ret = vw_printw(screen, fmt, args);
    va_end(args);
    wrefresh(lcd);
    wrefresh(screen);
    rc = sem_post(&sem);
    assert(rc == 0);
    return ret;
}

void print_running(int cpu, int id, int nice, long long v_runtime)
{
    if (cpu == 0)
    {
        if (id == 0)
            lcd_write_at(3, (LINES/4)-10, "       Idle");
               
        else
            lcd_write_at(3, (LINES/4)-10, "Runnig:      ");

        lcd_write_at(4, (LINES/4)-8, "Task ID: %03d   ", id);
        lcd_write_at(5, (LINES/4)-8, "Nice: %d    ", nice);
        lcd_write_at(6, (LINES/4)-8, "V_Runtime:           ");
        lcd_write_at(6, (LINES/4)-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 1 )
    {
        if (id == 0)
            lcd_write_at(3, LINES-10, "        Idle");
        
        else
            lcd_write_at(3, LINES-10, "Runnig:      ");
        
        lcd_write_at(4, LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(5, LINES-8, "Nice: %d    ", nice);
        lcd_write_at(6, LINES-8, "V_Runtime:           ");
        lcd_write_at(6, LINES-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 2)
    {
        if (id == 0)
            lcd_write_at(3, 2*LINES-10, "        Idle");
 
        else
            lcd_write_at(3, 2*LINES-10, "Runnig:      ");
       
        lcd_write_at(4, 2*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(5, 2*LINES-8, "Nice: %d    ", nice);
        lcd_write_at(6, 2*LINES-8, "V_Runtime:           ");
        lcd_write_at(6, 2*LINES-8, "V_Runtime: %lld     ", v_runtime);
    }
    
    if (cpu == 3)
    {
        if (id == 0)
            lcd_write_at(3, 3*LINES-10, "        Idle");    
        
        else
            lcd_write_at(3, 3*LINES-10, "Runnig:      ");
        
        lcd_write_at(4, 3*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(5, 3*LINES-8, "Nice: %d   ", nice);
        lcd_write_at(6, 3*LINES-8, "V_Runtime:           ");                    
        lcd_write_at(6, 3*LINES-8, "V_Runtime: %lld", v_runtime);    
    }
}

void print_ready(int cpu, int id, long long v_runtime)
{
    if (cpu == 0)
    {
        if (id == 0)
            lcd_write_at(12, (LINES/4)-10, "       Empty");
                
        else
            lcd_write_at(12, (LINES/4)-10, "Ready:       ");
        
        lcd_write_at(13, (LINES/4)-8, "Task ID: %03d  ", id);
        lcd_write_at(14, (LINES/4)-8, "V_Runtime:           ");
        lcd_write_at(14, (LINES/4)-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 1 )
    {
        if (id == 0)
            lcd_write_at(12, LINES-10, "        Empty");
        
        else
            lcd_write_at(12, LINES-10, "Ready:       ");

        lcd_write_at(13, LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(14, LINES-8, "V_Runtime:           ");
        lcd_write_at(14, LINES-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 2)
    {
        if (id == 0)
            lcd_write_at(12, 2*LINES-10, "        Empty");
                            
        else
            lcd_write_at(12, 2*LINES-10, "Ready:       ");
        
        lcd_write_at(13, 2*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(14, 2*LINES-8, "V_Runtime:           ");
        lcd_write_at(14, 2*LINES-8, "V_Runtime: %lld     ", v_runtime);
    }
    
    if (cpu == 3)
    {
        if (id == 0)
            lcd_write_at(12, 3*LINES-10, "        Empty");     
        
        else
            lcd_write_at(12, 3*LINES-10, "Ready:       ");
        
        lcd_write_at(13, 3*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(14, 3*LINES-8, "V_Runtime:           ");                    
        lcd_write_at(14, 3*LINES-8, "V_Runtime: %lld", v_runtime);
    }
}

void print_ready2(int cpu, int id, long long v_runtime)
{
    if (cpu == 0)
    {
        if (id == 0)
            lcd_write_at(16, (LINES/4)-10, "       Empty");
        
        else
            lcd_write_at(16, (LINES/4)-10, "Ready:       ");
        
        lcd_write_at(17, (LINES/4)-8, "Task ID: %03d  ", id);
        lcd_write_at(18, (LINES/4)-8, "V_Runtime:           ");
        lcd_write_at(18, (LINES/4)-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 1 )
    {
        if (id == 0)
            lcd_write_at(16, LINES-10, "        Empty");
        
        else
            lcd_write_at(16, LINES-10, "Ready:       ");        

        lcd_write_at(17, LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(18, LINES-8, "V_Runtime:           ");
        lcd_write_at(18, LINES-8, "V_Runtime: %lld", v_runtime);
    }

    if (cpu == 2)
    {
        if (id == 0)
            lcd_write_at(16, 2*LINES-10, "        Empty");
                    
        else
            lcd_write_at(16, 2*LINES-10, "Ready:       ");        

        lcd_write_at(17, 2*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(18, 2*LINES-8, "V_Runtime:           ");
        lcd_write_at(18, 2*LINES-8, "V_Runtime: %lld     ", v_runtime);

    }
    
    if (cpu == 3)
    {
        if (id == 0)
            lcd_write_at(16, 3*LINES-10, "        Empty");
       
        else
            lcd_write_at(16, 3*LINES-10, "Ready:       ");
        
        lcd_write_at(17, 3*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(18, 3*LINES-8, "V_Runtime:           ");                    
        lcd_write_at(18, 3*LINES-8, "V_Runtime: %lld", v_runtime);
    }
}

void print_ready3(int cpu, int id, long long v_runtime, int size)
{
    if (cpu == 0)
    {
        if (id == 0)
            lcd_write_at(20, (LINES/4)-10, "       Empty");
        
        else
            lcd_write_at(20, (LINES/4)-10, "Ready:       ");
        
        lcd_write_at(21, (LINES/4)-8, "Task ID: %03d  ", id);
        lcd_write_at(22, (LINES/4)-8, "V_Runtime:           ");
        lcd_write_at(22, (LINES/4)-8, "V_Runtime: %lld", v_runtime);
        lcd_write_at(24, (LINES/4)-8, "Total: %d   ", size);

    }

    if (cpu == 1 )
    {
        if (id == 0)
            lcd_write_at(20, LINES-10, "        Empty");
        else
            lcd_write_at(20, LINES-10, "Ready:       ");
        
        lcd_write_at(21, LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(22, LINES-8, "V_Runtime:           ");
        lcd_write_at(22, LINES-8, "V_Runtime: %lld", v_runtime);
        lcd_write_at(24, LINES-8, "Total: %d   ", size);
    }

    if (cpu == 2)
    {
        if (id == 0)
            lcd_write_at(20, 2*LINES-10, "        Empty");
                 
        else
        {
            lcd_write_at(20, 2*LINES-10, "Ready:       ");
        }

        lcd_write_at(21, 2*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(22, 2*LINES-8, "V_Runtime:           ");
        lcd_write_at(22, 2*LINES-8, "V_Runtime: %lld     ", v_runtime);
        lcd_write_at(24, 2*LINES-8, "Total: %d   ", size);
    }
    
    if (cpu == 3)
    {
        if (id == 0)
            lcd_write_at(20, 3*LINES-10, "        Empty");        
        
        else        
            lcd_write_at(20, 3*LINES-10, "Ready:       ");        
        
        lcd_write_at(21, 3*LINES-8, "Task ID: %03d   ", id);
        lcd_write_at(22, 3*LINES-8, "V_Runtime:           ");                    
        lcd_write_at(22, 3*LINES-8, "V_Runtime: %lld", v_runtime);
        lcd_write_at(24, 3*LINES-8, "Total: %d   ", size);
    }
}

void print_queues(int queue, int id, int size, int last)
{
    last++;
    if (queue == 0)
    {
        if (id == 0)
            lcd_write_at(30, (LINES/4)-10, "       Empty");
        
        else
            lcd_write_at(30, (LINES/4)-10, "Ready:       ");

        lcd_write_at(31, (LINES/4)-8, "Front ID: %03d  ", id);
        lcd_write_at(32, (LINES/4)-8, "Last CPU: %d   ", last);
        lcd_write_at(33, (LINES/4)-8, "Total: %d   ", size);

    }

    if (queue == 1 )
    {
        if (id == 0)
            lcd_write_at(30, LINES-10, "       Empty");
        
        else
            lcd_write_at(30, LINES-10, "Ready:       ");
        
        lcd_write_at(31, LINES-8, "Front ID: %03d   ", id);
        lcd_write_at(32, LINES-8, "Last CPU: %d   ", last);
        lcd_write_at(33, LINES-8, "Total: %d   ", size);
    }

    if (queue == 2)
    {
        if (id == 0)
            lcd_write_at(30, 2*LINES-10, "       Empty");        
                    
        else
            lcd_write_at(30, 2*LINES-10, "Ready:       ");

        lcd_write_at(31, 2*LINES-8, "Front ID: %03d   ", id);
        lcd_write_at(32, 2*LINES-8, "Last CPU: %d   ", last);
        lcd_write_at(33, 2*LINES-8, "Total: %d   ", size);

    }
    
    if (queue == 3)
    {
        if (id == 0)
            lcd_write_at(30, 3*LINES-10, "       Empty");
        
        else
            lcd_write_at(30, 3*LINES-10, "Ready:       ");
        
        lcd_write_at(31, 3*LINES-8, "Front ID: %03d   ", id);
        lcd_write_at(32, 3*LINES-8, "Last CPU: %d   ", last);
        lcd_write_at(33, 3*LINES-8, "Total: %d   ", size);
    }
}