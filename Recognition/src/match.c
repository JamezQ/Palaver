/*
 * match.c
 *
 *  Created on: Mar 20, 2013
 *      Author: James Mcclain, Antonis Kalou
 */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "match.h"

// PROTOTYPES *******************

int check_equality(char *speech,char *buffer);
int any_match(char **buffer,char **speech,char start,char end);
char *line_match(char *buf,char **tmpSpeech);

//********************************


// square bracket match
int sb_match(char **buffer,char **speech) {
  return any_match(buffer,speech,'[',']');
}

// less than match
int lt_match(char **buffer,char **speech) {
  return any_match(buffer,speech,'<','>');
}

// Open paren (variable) match.
int op_match(char **buffer,char **speech) {

  char *buf = *buffer;
  char *tmpSpeech = *speech;

  if(*buf != '(') {
    printf("Incorrect used of op_match, **buffer != '('\n");
    exit(1);
  }

  ++buf;
  char *startVarName = buf;
  char *lineSpeech = NULL;
  while(*buf != ' ' &&	*buf != '\0' && *buf != '\n' &&
  	*buf != '\r' &&	*buf != '<') {

    ++buf;
  }


  if(*buf == '\0' || *buf == '\n' || *buf == '\r') {
    printf("Incorrect syntax in op_match!\n");
    exit(1);
  }
  char saveChar = *buf;
  *buf = '\0';


  if(strcmp(startVarName,"WORD") == 0) {
    *buf = saveChar;
    if(*buf == '<') {
      lt_match(&buf,&tmpSpeech);
    } else {
      while(*tmpSpeech != ' ' && *tmpSpeech != '\n' &&
	     *tmpSpeech != '\r' && *tmpSpeech != '\0') {
	       ++tmpSpeech;
      }
    }
  } else if(strcmp(startVarName,"LINE") == 0) {
    // solution, make Line in this format
    // (Line varname Match Until this)
    // With Match Until this being everything until the
    // ), with ( not allowed.
    // nothing there or just spaces simply means to match
    // until the end of the line.
    *buf = saveChar;
    if(*buf == '<') {
      lt_match(&buf,&tmpSpeech);
    } else {
      // buf won't change, but tmpSpeech must.
      lineSpeech = line_match(buf,&tmpSpeech);
    }
  }
  else {
    printf("%s is not a variable name or is not "
	   "implemented.\n",startVarName);
    exit(1);
  }


  if( !STORE_VARIABLES || !(tmpSpeech > *speech)) {
    while(*buf != ')' && *buf != '\n' && *buf != '\0' && *buf != '\r') {
      ++buf;
    }
    if(*buf != ')') {
      printf("Error in syntax of op_match()!\n");
      exit(1);
    }
    ++buf;
    *buffer = buf;
  } else {
    //get the variable name.
    //reusing startVarName

    if(var_LL->next != NULL) {
      printf("var_LL->next is not NULL!\n");
      exit(1);
    }

    var_LL->next = malloc(1*sizeof(struct variables));
    if(var_LL->next == NULL) {
      printf("Memory Error in op_match!\n");
      exit(1);
    }
    var_LL = var_LL->next;

   while(*buf == ' ') {
    	++buf;
    }
    if(*buf == ')') {
      printf("Incorrect syntax in op_match()!\n");
      exit(1);
    }
    startVarName = buf;
    while(*buf != ' ' && *buf != ')') {
      if(*buf == '$') {
      	printf("Error, can't have $ in a varname!\n");
      	exit(1);
      }
      ++buf;
    }
    saveChar = *buf;
    *buf = '\0';
    var_LL->varName = malloc(sizeof(char)*strlen(startVarName)+1);

    if(var_LL->varName == NULL) {
      printf("Memory error in op_match!\n");
      exit(1);
    }
    strcpy(var_LL->varName,startVarName);
    *buf = saveChar;
    while(*buf != ')' && *buf != '\n' && *buf != '\0' && *buf != '\r') {
      ++buf;
    }
    if(*buf != ')') {
      printf("Error in syntax of op_match()!\n");
      exit(1);
    }
    ++buf;
    *buffer = buf;
    var_LL->next = NULL;

  } if(tmpSpeech > *speech) { // A match!
    if(STORE_VARIABLES) {
      saveChar = *tmpSpeech;
      *tmpSpeech = '\0';
      var_LL->varValue = malloc(sizeof(char)*strlen(*speech)+1);

      if(var_LL->varValue == NULL) {
      	printf("MEMORY error in op_match!\n");
      	exit(1);
      }
      strcpy(var_LL->varValue,*speech);
      *tmpSpeech = saveChar;
    }
    if(!lineSpeech) {
      *speech = tmpSpeech;
    }
    else {
      *speech = lineSpeech;
    }
    return 1;
  }
  return 0;
}

int any_match(char **buffer,char **speech,char start,char end) {

  if(**buffer != start) {
    printf("ERROR, any_match() called, but **buffer != '%c'\n",
	   start);

    exit(1);
  }
  ++(*buffer);

  char *buf = *buffer;
  char *Gspeech = *speech;
  char *tmpSpeech = *speech;

  int stop = 1;

  while(stop != 0) { // While we are in a balanced expression.
    if(**buffer == '\n' || **buffer == '\0' || **buffer == '\r') {
      printf("ERROR, inconnect syntax of %c%c\n",start,end);
      exit(1);
    }
    if(**buffer == start) {
      ++stop;
    }
    if(**buffer == end) {
      --stop;
    }
    ++(*buffer);
    if(**buffer == '\\') {
      // Not sure about the flows of these ifs
      ++(*buffer);
      if(**buffer != '\0')
	     ++(*buffer);
    }
  }
  --(*buffer);
  // We are now at the matching > or ]

  while(buf != *buffer) {
    if(*buf == ',') {
      if(tmpSpeech > Gspeech) {
      	Gspeech = tmpSpeech;
      	tmpSpeech = *speech;
      }
      ++buf;
    }
    if(*buf == '<') {
      if(! lt_match(&buf,&tmpSpeech)) {
      	tmpSpeech = *speech;

      	while(!(buf == *buffer || *buf == ',')) {
      	  ++buf;
      	}
      }
      continue;
    }
    if(*buf == '(') {
      if(! op_match(&buf,&tmpSpeech)) {
      	tmpSpeech = *speech;

      	while(!(buf == *buffer || *buf == ',')) {
      	  ++buf;
      	}
      }
      continue;
    }

    if(*buf == '[') {
      sb_match(&buf,&tmpSpeech);
      continue;
    }

    if(*buf == '\\') { // We will check what is after it.
      ++buf;
    }

    if(*buf == *tmpSpeech) {
      ++buf;
      ++tmpSpeech;
    } else {
      tmpSpeech = *speech;

      while(!(buf == *buffer || *buf == ',')) {
      	++buf;
      }
    }
  }
  if(tmpSpeech > Gspeech) {
    Gspeech = tmpSpeech;
    tmpSpeech = *speech;
  }
  ++buf;
  *buffer = buf;

  if(Gspeech > *speech) {
    *speech = Gspeech;
    return 1;
  }

  return 0;
}

char *line_match(char *buf,char **tmpSpeech) {
  // buf should be right before the name of the
  // variable, on a space. What we need to do is skip the name
  // of the variable, and get the data we need so we can
  // stop we we hit a match. Then move tmpSpeech to to before that
  // however we also need to make sure *speech is moved after that
  // so we need an if statement for that.
  if(*buf != ' ') {
    printf("Incorrect syntax in line_match!\n");
    exit(1);
  }


  while(*buf == ' ') {
    ++buf;
  }
  if(*buf == ')' || *buf == '\n' ||
     *buf == '\r' || *buf == '\0') {
    printf("Incorrect syntax in line_match!\n");
    exit(1);
  }

  // We are at the varName, lets pass it.
  while(*buf != ' ' && *buf != '\n' && *buf != '\r' &&
    *buf != '\0' && *buf != ')') {
    ++buf;
  }
  if(*buf == ')') { // This means it matches the rest of
		                // the speech

    *tmpSpeech = *tmpSpeech + strlen(*tmpSpeech);

    return *tmpSpeech;
  }
  if(*buf != ' ') {
    printf("Incorrect syntax in line_match!\n");
    exit(1);
  }
  while(*buf == ' ') {
    ++buf;
  }
  if(*buf == '\n' || *buf == '\0' || *buf == '\r') {
    printf("Incorrect syntax in line_match!\n");
    exit(1);
  }
  if(*buf == ')') {
    *tmpSpeech = *tmpSpeech + strlen(*tmpSpeech);
    return *tmpSpeech;
  }

  char *matchExpression;
  // Okay we are finally at the start of the end-expression.
  matchExpression = buf;
  // okay lets send buf to ) and make sure everything checks out.

  while(*buf != ')' && *buf != '\0') {
    if(*buf == '(') {
      printf("Not allowed, no variables in an end-expression!\n");
      exit(1);
    }
    ++buf;
  }
  if(*buf != ')') {
    printf("Incorrect syntax in line_match!\n");
    exit(1);
  }
  // Finally we are here, we can now start building out Line.

  //reusing buf, we know we have a ) to stop at.
  buf = matchExpression;
  char *speechChecker = *tmpSpeech;
  char *speechWalker  = *tmpSpeech;

  // Now for each character until you either get a match
  // or hit '\0' or '\n', check if there is a match with
  // match expression. When you do, set tmpSpeech to *before*
  // it and return a pointer to *after* it.
  // if '\0' or '\n' is hit, set tmpSpeech to it's orginal postion
  // and return NULL. (alternativly don't change it.


  // Allows optional stop to work
  int only_sb = 1;
  while(*speechWalker != '\0' && *speechWalker != '\n' && *speechWalker != '\r') {

    while(*buf != ')') {
      if(*buf == '<') {
	only_sb = 0;
	if(! lt_match(&buf,&speechChecker)) {
	  buf = matchExpression;
	  speechChecker = speechWalker;
	  break;
	}
	continue;
      } else if(*buf == '[') {
	sb_match(&buf,&speechChecker);
	continue;
      }
      if(*buf != *speechChecker) {
	only_sb = 0;
	break;
      }
      ++buf;
      ++speechChecker;
    }
    if(*buf != ')') { // no dice
      buf = matchExpression;
      speechChecker = speechWalker;
    } else { // we have a match
      if(speechChecker > speechWalker) { // If there is nothing to put in
				      // the variable, it's not a match.
	*tmpSpeech = speechWalker;
	return speechChecker;
      } else {
	buf = matchExpression;
	speechChecker = speechWalker;
      }
    }
    ++speechWalker;
    ++speechChecker;
  }
  if (only_sb) {
    *tmpSpeech = *tmpSpeech + strlen(*tmpSpeech);
    return *tmpSpeech;
  }

  return NULL;
}

int is_match(char *speech,char *buf) {
  char *ptr;
  char *speechPtr;

  ptr = buf;

  // Check if the line is empty
  if(*ptr == '\n' || *ptr == '\0' || *ptr == '\r')
    return 0;

  // Check if line is a comment.
  if(*ptr == '#')
    return 0;

  // If we get here, check if they are equal.
  speechPtr = speech;

  return check_equality(speechPtr,ptr);
}

int check_equality(char *speech,char *buffer) {
  // if there is a < in buffer, don't move the speech pointer,
  // try to match from that point everything in <a,b,c>
  // and then move the speech pointer to the end of the biggest match
  // if nothing matches, return a 0.
  //
  // If there is a [ hit in the buffer, don't move the speech pointer,
  // act exactly like <>,matching the largest possible.
  // however, this time if nothing matches, simply do not move the
  // speech pointer and go the the next thing in the buffer.
  //
  // If you hit a (,first check if it is a type<li,st> and just treat
  // it as <> (but convert to number if it is type Number.
  //
  // Otherwise, just read as much as the type needs. But don't save the
  // data here.
  //
  // \< \( and \[ are treated as their literal values.

  //  printf("Testing if %s == %s\n",speech,buffer); // DEBUG



  while(*buffer != '\0') {
    if(*buffer == '\n' || *buffer == '#' || *buffer == '\r') {
      *buffer = '\0';
    }
    if (*buffer == '<') {
      if(! lt_match(&buffer,&speech)) {
      	return 0;
      }
      // If there is a match here, both speech and buffer
      // will be in the right place.
      continue;
    }
    else if (*buffer == '[') {
      sb_match(&buffer,&speech);
      // Same as lt_match, but not need to return no match
      // if nothing is found, that is okay.
      continue;
    }
    else if(*buffer == '(') {
      if( ! op_match(&buffer,&speech)) {
        return 0;
      }

      continue;
    }
    else if(*buffer == '\\') {
      ++buffer;
      // Duplicates ??
      if(*speech != *buffer)
        return 0;
      }
    else {
      if(*speech != *buffer) {
      	//	printf("%c != %c\n",*speech,*buffer); // DEBUG
      	return 0;
      }
    }
    if(*buffer != '\0')
      ++buffer;
    if(*speech != '\0')
      ++speech;
  }
  return 1;
}
