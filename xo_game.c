#include <ncurses.h>
#include <string.h>

#define _WIDTH		(9)

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void xo_wprintchseq(WINDOW *win, char *vect);
char check_win_cond(char win_cond[][3]);
void init_win_cond(char win_cond[][3]);

enum xo_boolean{
	xo_true = 1,
	xo_false = 0
};

int main()
{
	/*boxes*/
	WINDOW *b1;
	WINDOW *b2;

	/*gameplay stuff*/
	char key_binds[] = {'q', 'w', 'e',
						'a', 's', 'd',
						'z', 'x', 'c'
						, 0};

	enum xo_boolean perms[] = {	xo_true, xo_true, xo_true,
								xo_true, xo_true, xo_true,
								xo_true, xo_true, xo_true
							};
	char win_cond[3][3];
	int we_have_a_winner = 0;
	char player = 'X';
	char *test = 0;

	/*positioning stuff*/
	int startx = 7;
	int starty = 7;
	int dist = 20;
	int wdth = 7;
	int hght = 7;
	const char *arrow = "<=====";

	/*the pressed key*/
	int ch;

	initscr();
	/*cbreak(); /*safe*/
	raw();
	noecho();
	refresh(); /*the misterious refresh*/

	/*init the board*/
	int i;
start_game:
	init_win_cond(win_cond);
	for(i = 0; i < 9; i++)
		perms[i] = xo_true;
	clear();
	refresh();
	b1 = create_newwin(wdth, hght, starty, startx);
	b2 = create_newwin(wdth, hght, starty, startx+dist);
	move(starty+hght/2, startx+dist/2/*-strlen(arrow)/2*/);
	printw("%s", arrow);
	refresh();
	xo_wprintchseq(b2, key_binds);

	/*gameplay*/
	we_have_a_winner = 0;
	player = 'X';
	test = 0;
	do{
get_user_info:
		do{
			ch = getch();
		} while ((test = strchr(key_binds, ch)) == NULL);

		int index = (int)test - (int)key_binds;

		if(perms[index] == xo_false){
			move(LINES-1, 1);
			printw("That spot is busy!");
			refresh();
			goto get_user_info;
		}

		move(LINES - 1, 1);
		printw("                   ");
		refresh();
		int x = index % 3;
		int y = index / 3;
		wmove(b1, 2*y+1, 2*x+1);
		wprintw(b1, "%c", player);
		wrefresh(b1);
		win_cond[x][y] = player;
		player = (player == 'O') ? 'X' : 'O';
		perms[index] = xo_false;
		char win = check_win_cond(win_cond);
		if(win){
			destroy_win(b1);
			destroy_win(b2);
			clear();
			refresh();
			char win_msg[15];
			win_msg[14] = 0;
			win_msg[13] = 0;
			if(win == 1)
				strcpy(win_msg, "No player wins");
			else if(win == 'X')
				strcpy(win_msg, "PlayerX wins!");
			else
				strcpy(win_msg, "PlayerO wins!");
			move(LINES/2 - strlen(win_msg), COLS/2);
			int row, col;
			getyx(stdscr, row, col);
			printw("%s", win_msg);
			move(row+1, col);
			printw("%s", "Do you want to play again?(y/n)");
			refresh();
			do{
				ch = getchar();
			} while(ch != 'y' && ch != 'n');

			if(ch == 'y')
				goto start_game;
			else
				we_have_a_winner = 1;
		}

	} while(!we_have_a_winner);

//	getch(); //end "'Press any key' equivalent"

	delwin(b1);
	endwin();
	return 0;
}

void init_win_cond(char win_cond[][3])
{
	int i, j;
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			win_cond[i][j] = 0;
}

char check_win_cond(char win_cond[][3])
{
	int i;
	/*lines*/
	for(i = 0; i < 3; ++i)
		if(	win_cond[i][0] == win_cond[i][1] &&
			win_cond[i][0] == win_cond[i][2])
				return win_cond[i][0];
	/*collumns*/
	for(i = 0; i < 3; ++i)
		if(	win_cond[0][i] == win_cond[1][i] &&
			win_cond[0][i] == win_cond[2][i])
				return win_cond[0][i];
	/*diagonals*/
	if(	win_cond[0][0] == win_cond[1][1] &&
		win_cond[0][0] == win_cond[2][2])
			return win_cond[0][0];
	if(	win_cond[0][2] == win_cond[1][1] &&
		win_cond[0][2] == win_cond[2][0])
			return win_cond[0][2];

	enum xo_boolean full = true;
	for(i = 0; i < 3; ++i){
		int j;
		for(j = 0; j < 3; ++j){
			if(win_cond[i][j] == 0){
				full = false;
				break;
			}
		}
		if(full == false) break;
	}
	return full;
}

void xo_wprintchseq(WINDOW *win, char *vect)
{
	int i, j;
	for(i = 0; i < 3; ++i){
		for(j = 0; j < 3; ++j){
			wmove(win, 2*i + 1, 2*j + 1);
			wprintw(win, "%c", vect[i*3 + j]);	
		}
	}
	wrefresh(win);
}
 
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);

	int i;
	for(i = 1; i < 6; i++){
		wmove(local_win, i, 2);
		wprintw(local_win, "|");
		wmove(local_win, i, 4);
		wprintw(local_win, "|");
	}
	for(i = 1; i < 3; ++i){
		wmove(local_win, i*2, 1);
		wprintw(local_win, "_");
		wmove(local_win, i*2, 3);
		wprintw(local_win, "_");
		wmove(local_win, i*2, 5);
		wprintw(local_win, "_");
	}

	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}