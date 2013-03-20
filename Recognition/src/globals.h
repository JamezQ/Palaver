/*
 * globals.h
 *
 *  Created on: Mar 20, 2013
 *      Author: James Mcclain, Antonis Kalou
 */

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

extern int STORE_VARIABLES; // set to yes when actually
                         // storing vars.
extern struct variables{
  char *varName;
  char *varValue;

  struct variables *next;
} *var_Header, *var_LL;


extern int LINE_IN_DATABASE;

#endif /* GLOBALS_H_INCLUDED */
