/*
 * match.h
 *
 *  Created on: Mar 20, 2013
 *      Author: James Mcclain, Antonis Kalou
 */

#ifndef MATCH_H_INCLUDED
#define MATCH_H_INCLUDED

#include "globals.h"

// < match
int lt_match(char **buffer,char **speech);
// [ match
int sb_match(char **buffer,char **speech);
// ( match
int op_match(char **buffer,char **speech);

int is_match(char *speech,char *buf);

#endif /* MATCH_H_INCLUDED */
