#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <stdio.h>

using namespace std;

/*
 * aim: prepare for using curses.h, to prevent text echoing to screen.
 * */
int tty_break()
{
				initscr();
				cbreak();
				noecho();
				start_color();
				return 0;
}

/*
 * aim: read char from buffer 
 * */
int tty_getchar()
{
				return getch();
}


/*
 * aim: to exit program 
 * */
int tty_fix()
{
				endwin();
				return 0;
}

/*
 * aim: to print a text in the centre of the screen
 * purpose: For title of program
 * */
int print_in_centre(char* text)
{
				int h=0 , w=0, space;
				getmaxyx(stdscr,h,w);
				space = (w/2) - ( strlen (text) / 2);
				for(int i = 0; i < space; i++)
								waddch(stdscr,' '); 
				printw("%s",text);
				}

 /* 
  * aim: to print performance statistics 
  * purpose: To be displayed after the program has exit.  
  * */
void print_state2 (int lt_count, int wcount, int time, int typos [96])
{
				double rate = 0.0;
				char x = 'a';
				cout << "Letters typed: " << lt_count << endl;
				cout << "Words typed: " << wcount << endl;
				cout << "Time Taken: " << time << " seconds" << endl;
				
				rate = (double) lt_count * 60 / time;
				cout << "Letters per minute: " << rate << endl;
				
				rate = (double) wcount * 60 / time;
				cout << "Words per minute: " << rate << endl;
				
				printw("\nErrors while typing\n");
				for(int i =0; i < 96; i++)
				{
								if(typos[i] > 0) {
												x = (char) i + 30;
												cout << x << ": " << typos[i] << endl;
								}
				}    

}

 /* 
  * aim: to print out performance statistics 
 * purpose:  Correct output when curses lib init 
 * */
void print_state1 (int lt_count, int wcount, int time, int typos [96])
{
				double rate = 0.0;
				char x = 'a';
				printw("Your Results\n\nLetters typed: %d\n",lt_count);
				printw("Words typed:%d\n",wcount);
				printw("Time taken: %d seconds\n", time);
				
				rate = (double) lt_count * 60 / time;
				printw("Letters per minute: %f\n", rate);
				
				rate = (double) wcount * 60 / time;
				printw("Words per minute: %f\n", rate);
				
				printw("\nErrors while typing\n");
				for(int i =0; i < 96; i++)
				{
								if(typos[i] > 0) {
												x = (char) i + 30;
												printw("%c: %d \n", x,typos[i]);
								}
				}
				wrefresh(stdscr);   
}

int main(int argc, char** argv)
{
				ifstream smpFile(argv[1]);
				int wcount = 0;
				unsigned lt_count = 0;
				string line2 = ""; 
				time_t start, finish;
				char input;
				int typos [100]; 
				
				if(argc < 2) 
				{
								printf("A text file argument is required\n");
								return 1;
				}
				
				tty_break();
				
				for(int i = 0; i < 100;i++)
								typos[i] =0;
				
				init_pair(1,COLOR_BLUE,COLOR_RED);
				init_pair(2,COLOR_WHITE,COLOR_BLACK);
				
				time(&start);
				
				int readcount = 0;
				while (smpFile.good())
				{
								getline(smpFile,line2);
								wcount++;
								print_in_centre("CTTT - Console Touch Typing Tutor");
								wmove(stdscr,2,0);
								wattron(stdscr,COLOR_PAIR(1));
								printw("%s\n",line2.c_str());   
								wrefresh(stdscr);
								wattron(stdscr,COLOR_PAIR(2));
								printw("word count %u\n",(unsigned) line2.size());
								lt_count = (unsigned) line2.size() + lt_count; 
				for(unsigned i = 0; i < line2.size() ;++i)
				{
								wmove(stdscr,2,0 + i);
								input = tty_getchar();
								if (line2[i] != input)
								{
								while(line2[i] != input) 
								{	  
												typos[(int) line2[i] - 30]++;
												wmove(stdscr,2,0 + i);
												input = tty_getchar();
								}
				} 
				//Correct Letter inputted - Replace character using COLOR_PAIR(2) 
				waddch(stdscr,input);
						
				//To count the number of words, 
				if(line2[i] == ' ') wcount++; 
				}
								werase(stdscr); 
				}
				
				time(&finish);
				smpFile.close();
				werase(stdscr);
				print_state1(lt_count,wcount,finish - start,typos);
				printw("Press any key to exit\n");
				wrefresh(stdscr);
				cin >> input;   
				tty_fix();
				print_state2(lt_count,wcount,finish - start,typos);   
				return 0;
}

