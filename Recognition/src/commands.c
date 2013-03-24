/*
 * commands.c
 *
 *  Created on: Mar 20, 2013
 *      Author: James Mcclain, Antonis Kalou
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

// PROTOTYPES *****************

void store_special_variables(char *speech,char *buf);

//****************************

char *get_command(char *database,char *speech) {

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

      fgets(buf,1024,file);
      ++LINE_IN_DATABASE;
      ret = create_command(buf);
      break;
    }
  }
  fclose(file);
  return ret;
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
