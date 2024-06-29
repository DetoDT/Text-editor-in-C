/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s){
  perror(s);
  exit(1);
}

void disableRawMode() {
 if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

// Turn off ECHO feature which causes each key typed
// to be printed in terminal (like when typing passwd)
// and Canonical mode and CTRLC CTRLZ CTRLS CTRLQ CTRLV
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON | ICRNL);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);
  // ~ is the bitwise not operator to get the opposite
  // of ECHO from 00..001000 to 11..110111
  // &= AND and ASSIGN to force fourth bit to be a 0
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
    die ("tcsetattr");
}

/*** init ***/

int main(){
  enableRawMode();

  while (1){
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c)){ // check if it's a control character
      printf("%d\r\n", c);
    }else{
      printf("%d ('%c')\r\n", c, c);
    }
    
    if (c=='q') break;
  }
  return 0;
}
