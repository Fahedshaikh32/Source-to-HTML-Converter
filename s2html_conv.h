#ifndef S2HTML_CONV_H
#define S2HTML_CONV_H

#include <stdio.h>
#include "s2html_event.h"

/* HTML mode constants */
#define HTML_OPEN   1   // Used to indicate HTML start
#define HTML_CLOSE  0   // Used to indicate HTML end

/********** Function Prototypes **********/

// Function to write starting HTML structure

void html_begin(FILE* dest_fp, int type);

// Function to write ending HTML structure

void html_end(FILE* dest_fp, int type);

// Function to convert parser events into HTML formatted output

void source_to_html(FILE* fp, pevent_t *event);

#endif