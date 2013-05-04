/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// Find errors or debug code by searching DEBUG

// The code is a little messy, a dictionary 2.0 would
// be separated into files instead of one big one.
// But it could be worse.

// (c) James McClain <jamezmcclain@gmail.com>



// The first line returned is the command to execute.
// The 2nd line to return is the line number the match was at
// if requested. This is so in a dictionary with mutiple matchs
// for one sentence, they can all be found by running the program again
// with that line+1.
//
// If you don't want to skip any terms but want a number back, just tell
// it to start at 0.
//
// If there is no match, the program will return "No Command recognized.
// *and* exit with the exit code 2. 
// All other errors have the exit code one.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "match.h"
#include "commands.h"


int STORE_VARIABLES = 0; // set to yes when actually
                         // storing vars.

struct variables *var_LL = NULL;
struct variables *var_Header = NULL;

int LINE_IN_DATABASE = 0;

/***********************************************************************
  PROTOTYPES
************************************************************************/

static int parse_args(int argc,char *argv[],char **speech,char **database);
static int is_there_enough_args(int argc);

// Print with everything after the first space in single quotes
// to stop the shell from getting to it.

// ie. testing 1 2 3
// becomes
// testing '1 2 3'
static void print_arg_quoted(char *string);

int main(int argc, char *argv[]) {
  
  char *speech = NULL; // What the user said.
  char *database = NULL; // File to check what command to run
			 // based on what was said.

  char *command = NULL; // The command to run.
  
  if(parse_args(argc,argv,&speech,&database) != 0) {
    exit(EXIT_FAILURE);
  }
  command = get_command(database,speech);

  free(speech); // No oppression allowed in this program.
  free(database);
  if(command) {
    printf("%s\n",command);
    //print_arg_quoted(command);
    free(command);
    while(var_Header) {
      free(var_Header->varName);
      free(var_Header->varValue);
      var_LL = var_Header->next;
      free(var_Header);
      var_Header = var_LL;
    }
    if(argc == 4) { // if we ask for a line, we get one back.
      printf("%d\n",LINE_IN_DATABASE-1);
    }
    
  } else {
    fprintf(stderr,"No Command recognized.\n");
    exit(2);
  }

  return 0;
}
static void print_arg_quoted(char *string) {
  while(*string != ' ' && *string != '\0') {
    printf("%c",*string);
    ++string;
  }
  if(*string != '\0') {
    printf("%c",*string);
  } else {
    printf("\n");
    return;
  }
  ++string;
  printf("'");
  while(*string != '\0') {
    if(*string == '\'') {
      printf("'\"'\"'");
    }
    else {
      printf("%c",*string);
    }
    ++string;
  }
  printf("'");
  printf("\n");
  
  return;
}

static int parse_args(int argc,char *argv[],char **speech,char **database) {

  // Could reduce amount of code here. Also, maybe logic can be fixed
  if(! is_there_enough_args(argc)) {
    printf("Incorrect number of arguments: \n"
	   "USAGE %s <speech> <database> [starting-line]\n",argv[0]);
    return 1;
  }
  if(*speech != NULL || *database != NULL) {
    fprintf(stderr,"ERROR, speech and database pointers must be NULL\n"
	    "before calling parse_args().");
    return 1;
  }
  *speech = malloc(sizeof(char)*strlen(argv[1])+1);
  *database = malloc(sizeof(char)*strlen(argv[2])+1);

  if(*speech == NULL || *database == NULL) {
   perror("malloc:");
   exit(EXIT_FAILURE);
  }
  strcpy(*speech,argv[1]);
  strcpy(*database,argv[2]);

  if(argc == 4) {
    int i = -1;
    
    sscanf(argv[3],"%d",&i);
    if(i <= 0) {
      fprintf(stderr,"'%s' is not a valid line to start on.\n",argv[3]);
      exit(EXIT_FAILURE);
    }
    LINE_IN_DATABASE = i-1;

  }
  return 0;
}

static inline int is_there_enough_args(int argc) {
  return ((argc == 3) || (argc == 4));
}
