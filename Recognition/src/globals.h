/*
 * globals.h
 *
 *  Created on: Mar 20, 2013
 *      Author: slacker
 */

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

extern int STORE_VARIABLES; // set to yes when actually
                         // storing vars.
extern struct variables{
  char *varName;
  char *varValue;

  struct variables *next;
};


extern struct variables *var_Header;
extern struct variables *var_LL ;

extern int LINE_IN_DATABASE;

#endif /* GLOBALS_H_INCLUDED */
