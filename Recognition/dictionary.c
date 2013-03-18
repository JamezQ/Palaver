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

int STORE_VARIABLES = 0; // set to yes when actually
                         // storing vars.
struct variables {
  char *varName;
  char *varValue;
  
  struct variables *next;
};
struct variables *var_LL = NULL;
struct variables *var_Header = NULL;

int LINE_IN_DATABASE = 0;

/***********************************************************************
  PROTOTYPES
************************************************************************/

// < match 
int lt_match(char **buffer,char **speech);
// [ match
int sb_match(char **buffer,char **speech);
// ( match
int op_match(char **buffer,char **speech);

// Print with everything after the first space in single quotes
// to stop the shell from getting to it.

// ie. testing 1 2 3
// becomes
// testing '1 2 3'
void print_arg_quoted(char *string);

int main(int argc, char *argv[]) {
  int parse_args(int argc,char *argv[],char **speech,char **database);
  
  char *get_command(char *database,char *speech);
  
  char *speech = NULL; // What the user said.
  char *database = NULL; // File to check what command to run
			 // based on what was said.

  char *command = NULL; // The command to run.
  
  if(parse_args(argc,argv,&speech,&database) != 0) {
    exit(1);
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
    printf("No Command recognized.\n");
    exit(2);
  }

  return 0;
}
void print_arg_quoted(char *string) {
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

char *get_command(char *database,char *speech) {
  void store_special_variables(char *speech,char *buf);
  char *create_command(char *buf);
  int is_match(char *speech,char *buf);

  int file_exists(char *database);
  FILE *file;
  char buf[1024];
  char *ret = NULL; // The command to return.
  
  if(! file_exists(database)) {
    printf("The database \"%s\" doesn't exist!\n",database);
    exit(2);
  }

  file = fopen(database,"r");

  int i;
  if(LINE_IN_DATABASE != 0) {
    for(i = 0;i < LINE_IN_DATABASE;++i) {
      if(!(fgets(buf,1024,file))) {
      	return ret;
      }
    }
  }

  while( fgets(buf,1024,file)) {
    ++LINE_IN_DATABASE;
    if(is_match(speech,buf)) {
      // Yes the speech matches, now to get variables in it.
      STORE_VARIABLES = 1;
      store_special_variables(speech,buf);
      is_match(speech,buf); // Will now store variables in in a LL

      fgets(buf,1024,file);++LINE_IN_DATABASE;
      ret = create_command(buf);
      break;
    }
  }
  fclose(file);
  return ret;
}
void store_special_variables(char *speech,char *buf) {
  if(var_LL == NULL) {
    // first time add special var $SPEECH$
    var_LL = malloc(1*sizeof(struct variables));
    if(var_LL == NULL) {
      printf("Memory Error in op_match!\n");
      exit(1);
    }
    // Set the var_Header to access the head later.
    var_Header = var_LL;
    var_LL->next = NULL;
    // The stupid error before "var_LL->next == NULL;"
    var_LL->varName = malloc(strlen("SPEECH")+1);
    strcpy(var_LL->varName,"SPEECH");
    var_LL->varValue = malloc(strlen(speech)+1);
    strcpy(var_LL->varValue,speech);
  } else {
    printf("var_LL is not null!!\n");
    exit(1);
  }

}

char *create_command(char *buf) {
  
  /* while(var_Header != NULL) { */
  /*   printf("%s == %s\n",var_Header->varName,var_Header->varValue); */
  /*   var_Header = var_Header->next; */

  /* } */
  /* printf("%s\n",buf); */
  char *ret = malloc(sizeof(char)*1024); // get a kilo at a time
  if(ret == NULL) {
    printf("Memory problems in create command!\n");
    exit(1);
  }
  int i = 0;
  char saveChar;
  char *startVarName;
  char *tmpCharPtr;
  struct variables *tmpHeader;

  // First skip leading spaces
  if(*buf != ' ') {
    printf("This line does not start with a space!\n");
    exit(1);
  }
  while(*buf == ' ') {
    ++buf;
  }
  while(*buf != '\0' && *buf != '\n' && *buf != '\r' && *buf != '#') {
    
    if( i % 1023) {
      ret = realloc(ret,sizeof(char)*1024*((i/1023)+1));
      if(ret == NULL) {
      	printf("Memory problems in create command!\n");
      	exit(1);
      }
    }
    if(*buf == '\\') {
      buf++;
      if(*buf == '\n' || *buf == '\0' || *buf == '\r') {
      	printf("Error, '\\' char at the end\n");
      	exit(1);
      }
      ret[i] = *buf;
    }
    else if(*buf != '$') {
      ret[i] = *buf;
    } else {
      ++buf;
      if(*buf == '$' || *buf == '\n' || *buf == '\0' || *buf == '\r') {
      	printf("Bad syntax in create command!\n");
      	exit(1);
      }
      startVarName = buf;
      while(*buf != '$' && *buf != '\n' && *buf != '\0' && *buf != '\r') {
      	++buf;
      }

      if(*buf != '$') {
      	printf("Bad syntax in create command!\n");
      	exit(1);
      }

      saveChar = *buf;
      *buf = '\0';
      tmpHeader = var_Header;

      while(tmpHeader != NULL) {
      	if(strcmp(startVarName,tmpHeader->varName) == 0) {

      	  tmpCharPtr = tmpHeader->varValue;

      	  while(*tmpCharPtr != '\0') {
	    
      	    ret[i] = *tmpCharPtr;
      	    ++i;
      	    ++tmpCharPtr;

      	    // It technically could happen.
      	    if( i % 1023) {
      	      ret = realloc(ret,sizeof(char)*1024*((i/1023)+1));
      	      if(ret == NULL) {
            		printf("Memory problems in create command!\n");
            		exit(1);
      	      }
      	    }

      	  }
          --i;
          *buf = saveChar;
      	}
      	// We found a match so leave this loop.
        if(*buf != '\0') {
          break;
      	}
      	tmpHeader = tmpHeader->next;
      }
      if(tmpHeader == NULL) {
      	--i;
      	*buf = saveChar;
      }

    }
     ++i;
    ++buf;
  }
  ret[i] = '\0';

  return ret;
}

int is_match(char *speech,char *buf) {
  int check_equality(char *speech,char *buffer);
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

// Open paren (variable) match.
int op_match(char **buffer,char **speech) {
  char *line_match(char *buf,char **tmpSpeech);

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

  }

  if(tmpSpeech > *speech) { // A match!
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
// square bracket match
int sb_match(char **buffer,char **speech) {
  int any_match(char **buffer,char **speech,char start,char end);

  return any_match(buffer,speech,'[',']');
}
// less than match
int lt_match(char **buffer,char **speech) {
  int any_match(char **buffer,char **speech,char start,char end);

  return any_match(buffer,speech,'<','>');
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

int file_exists(char *database) {
  FILE *file;
  if((file = fopen(database,"r"))) {
    fclose(file);
    return 1;
  }
  return 0;
}

int parse_args(int argc,char *argv[],char **speech,char **database) {
  int is_there_enough_args(int argc);

  // Could reduce amount of code here. Also, maybe logic can be fixed
  if(! is_there_enough_args(argc)) {
    printf("Incorrect number of arguments: \n"
	   "USAGE %s <speech> <database> [starting-line]\n",argv[0]);
    return 1;
  }
  if(*speech != NULL || *database != NULL) {
    printf("ERROR, speech and database pointers must be NULL\n"
	   "before calling parse_args().");
    return 1;
  }
  *speech = malloc(sizeof(char)*strlen(argv[1])+1);
  *database = malloc(sizeof(char)*strlen(argv[2])+1);

  if(*speech == NULL || *database == NULL) {
    printf("Memory error in allocating strings to hold speech \n"
	   "and database.");
  }
  strcpy(*speech,argv[1]);
  strcpy(*database,argv[2]);

  if(argc == 4) {
    int i = -1;
    
    sscanf(argv[3],"%d",&i);
    if(i <= 0) {
      printf("'%s' is not a valid line to start on.\n",argv[3]);
      exit(1);
    }
    LINE_IN_DATABASE = i-1;

  }
  return 0;
}

int is_there_enough_args(int argc) {
  return ((argc == 3) || (argc == 4));
}
