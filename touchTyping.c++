#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <vector>
#include <sstream>

#define TYPOSIZ 144 // 144 is from 33 (!) to 176 (~)

using namespace std;

ifstream smpFile;
int wcount = 0;
unsigned lt_count = 0;
time_t start, finish;
char input;
int typos[TYPOSIZ] = {};
short text_background_color = COLOR_BLACK;
short text_foreground_color = COLOR_BLUE;
short words_per_line = 5;
bool isRandom = false;
vector<string> wordlib;
const int minLetterType = 10;

void usage()
{
	cout << "CTTT - Console Touch Typing" << endl;
	cout << "THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY!" <<endl;
	cout << endl;
	cout << "usage: CTTT TEXTFILE OPTIONS" << endl;
	cout << "OPTIONS:" << endl;
	cout << "	--text-background-color=" << endl;
	cout << "	--text-foreground-color=" << endl;
	cout << "	--words-per-line=" << endl;
	cout << "	--randomize" << endl;
	cout << endl;
	cout << "Colors Include: red, green, yellow, blue, magenta, cyan, white" << endl;
	cout << endl;
	exit(0);
}

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
 * aim: to print text in the centre of the screen
 * purpose: For title of program
 * */
int print_in_centre(char* text)
{
	int h=0 , w=0, space;
	getmaxyx(stdscr,h,w);
	space = (w/2) - ( strlen (text) / 2);
	for(int i = 0; i < space; i++)
	{
		waddch(stdscr,' ');
	}
	printw("%s",text);
}

// generates stats report
string generate_report(int lt_count, int wcount, int time, int typos[TYPOSIZ]) {
	stringstream report;

	double rate = 0.0;
	bool anyMistakes = false;
	int total_typos = 0;
	
	report << "Letters typed: " << lt_count << endl;
	report << "Words typed: " << wcount << endl;
	report << "Time taken: " << time << " seconds" << endl;
	
	rate = (double)lt_count * 60 / time;
	report << "Letters per minute: " << rate << endl;

	rate /= 5.0;
	report << "Words per minute: " << rate << endl;

	//check for any errors
	for (int i = 0; i < TYPOSIZ && !anyMistakes; ++i) {
		if (typos[i] > 0) {
			anyMistakes = true;
		}
	}
	
	
	if (anyMistakes) {
		report << endl << "Errors while typing" << endl;
		for (int i = 0; i < TYPOSIZ; ++i) {
			if (typos[i] > 0) {
				char x = (char)i + 33;
				report << x << ": " << typos[i] << endl;
			}
			total_typos += typos[i];
		}
		
		report << "Total typos: " << total_typos << endl;
	}else if(lt_count > minLetterType){
		report << "No Mistakes: Great Work!\n";
	}

	return report.str();
}

 /* 
  * aim: to print performance statistics 
  * purpose: To be displayed after the program has exit.  
  * */
void print_state2 (int lt_count, int wcount, int time, int typos [TYPOSIZ])
{
	cout << generate_report(lt_count, wcount, time, typos);
}

 /* 
  * aim: to print out performance statistics 
 * purpose:  Correct output when curses lib init 
 * */
void print_state1 (int lt_count, int wcount, int time, int typos [TYPOSIZ])
{
	printw("%s", generate_report(lt_count, wcount, time, typos).c_str());
	wrefresh(stdscr);   
}

/*
 * aim: do all stuff required to exit the program
 * purpose: to be called whenever program program exits
 */
void end_program() 
{
	time(&finish);
	smpFile.close();
	werase(stdscr);
	print_state1(lt_count,wcount,finish - start,typos);
	printw("Press any key to exit\n");
	wrefresh(stdscr);
	cin >> input;   
	tty_fix();
	print_state2(lt_count,wcount,finish - start,typos);   

	exit(0);
}

/*
 * aim: handle sigint signal for CTRL-C terminating
 */
void signal_handler(int signum)
{
	end_program();
}

/*
 * aim: fetch command line option colors
 */
short fetch_color(char *col_str) {
	if (strcasecmp(col_str, "black") == 0) {
		return COLOR_BLACK;
	} else if (strcasecmp(col_str, "red") == 0) {
		return COLOR_RED;
	} else if (strcasecmp(col_str, "green") == 0) {
		return COLOR_GREEN;
	} else if (strcasecmp(col_str, "yellow") == 0) {
		return COLOR_YELLOW;
	} else if (strcasecmp(col_str, "blue") == 0) {
		return COLOR_BLUE;
	} else if (strcasecmp(col_str, "magenta") == 0) {
		return COLOR_MAGENTA;
	} else if (strcasecmp(col_str, "cyan") == 0) {
		return COLOR_CYAN;
	} else if (strcasecmp(col_str, "white") == 0) {
		return COLOR_WHITE;
	}
}

int main(int argc, char** argv)
{
	char name[] = {"CTTT - Console Touch Typing Tutor"}; 

	
	/* command line parsing */
	if(argc < 2) 
	{
		usage();
	}

	/* options */
	for (unsigned i = 1; i < argc; ++i) {

		const char tcb[] = "--text-background-color=";
		const char tcf[] = "--text-foreground-color=";
		const char wpl[] = "--words-per-line=";
		const char rnd[] = "--randomize";

		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			usage();
		} else if (strncmp(argv[i], tcb, strlen(tcb)) == 0) {
			text_background_color = fetch_color(argv[i] + strlen(tcb));			
		} else if (strncmp(argv[i], tcf, strlen(tcf)) == 0) {
			text_foreground_color = fetch_color(argv[i] + strlen(tcf));
		} else if (strncmp(argv[i], wpl, strlen(wpl)) == 0) {
			words_per_line = static_cast<short>(atoi(argv[i] + strlen(wpl)));
		} else if (strncmp(argv[i], rnd, strlen(rnd)) == 0) {
			isRandom = true;
			srand(time(0));
		}
	}
	
	smpFile.open(argv[1]);
	if (!smpFile.is_open())
	{
		cout << endl;
		cout << "Error while reading file" << endl;
		cout << endl;
		exit(1);
	}
	tty_break();
	
	for(int i = 0; i < 100;i++)
	{
		typos[i] = 0;
	}
	
	init_pair(1, text_foreground_color, text_background_color);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	
	time(&start);

	signal(SIGINT, signal_handler);

	// get words for random
	if (isRandom) {
		while (smpFile) {
			string buffer;
			getline(smpFile, buffer);
			wordlib.push_back(buffer);
		}
	}
	
	int readcount = 0;
	while (smpFile.good() || isRandom)
	{		
		string wordline;
		string word;

		// get new words
		for (int i = 0; i < words_per_line; ++i) {

			if (!smpFile.good() && !isRandom) {
				words_per_line = i;
				break;
			}
			
			if (!isRandom) {
				getline(smpFile, word);	
			} else {
				word = wordlib[rand() % wordlib.size()];
			}

			wordline += word + string(" ");	
		}

		print_in_centre(name);
		wmove(stdscr,2,0);
		wattron(stdscr,COLOR_PAIR(1));
		printw("%s\n", wordline.c_str());   
		wrefresh(stdscr);
		wattron(stdscr,COLOR_PAIR(2));
		printw("character count %u\n",(unsigned) wordline.size());
		for(unsigned i = 0; i < wordline.size(); ++i)
		{
			wmove(stdscr,2,0 + i);
			input = tty_getchar();
			while(wordline[i] != input) 
			{
				typos[(int)wordline[i] - 33]++;
				wmove(stdscr,2,0 + i);
				input = tty_getchar();
			}
			//Correct Letter inputted - Replace character using COLOR_PAIR(2) 
			waddch(stdscr,input);

			++lt_count;
			
			//To count the number of words, 
			if(wordline[i] == ' ')
			{
				wcount++;
			}
		}
		werase(stdscr); 
	}		
	end_program();
	return 0;
}

