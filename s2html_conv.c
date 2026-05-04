#include <stdio.h>
#include "s2html_event.h"
#include "s2html_conv.h"

/* Add starting HTML structure */
void html_begin(FILE *dest_fp, int type)
{
    fprintf(dest_fp, "<!DOCTYPE html>\n");
    fprintf(dest_fp, "<html lang=\"en-US\">\n");
    fprintf(dest_fp, "<head>\n");
    fprintf(dest_fp, "<title>sode2html</title>\n");
    fprintf(dest_fp, "<meta charset=\"UTF-8\">\n");
    fprintf(dest_fp, "<link rel=\"stylesheet\" href=\"styles.css\">\n");
    fprintf(dest_fp, "</head>\n");
    fprintf(dest_fp, "<body style=\"background-color:lightgrey;\">\n");
    fprintf(dest_fp, "<pre>\n");
}

/* Add ending HTML structure */
void html_end(FILE *dest_fp, int type)
{
    fprintf(dest_fp, "</pre>\n");
    fprintf(dest_fp, "</body>\n");
    fprintf(dest_fp, "</html>\n");
}

/* Helper: write string escaping HTML special characters */
static void write_escaped(FILE *fp, const char *str, int len)
{
    for (int i = 0; i < len; i++)
    {
        if      (str[i] == '<') fprintf(fp, "&lt;");
        else if (str[i] == '>') fprintf(fp, "&gt;");
        else if (str[i] == '&') fprintf(fp, "&amp;");
        else                    fputc(str[i], fp);
    }
}

/* Convert parsed event into HTML formatted output */
void source_to_html(FILE *fp, pevent_t *event)
{
    if (event == NULL)
        return;

#ifdef DEBUG
    printf("%s", event->data);
#endif

    switch (event->type)
    {
    case PEVENT_MULTI_LINE_COMMENT:
    case PEVENT_SINGLE_LINE_COMMENT:
        fprintf(fp, "<span class=\"comment\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_STRING:
        fprintf(fp, "<span class=\"string\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_NUMERIC_CONSTANT:
        fprintf(fp, "<span class=\"numeric_constant\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_RESERVE_KEYWORD:
        fprintf(fp, "<span class=\"reserved_key1\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_ASCII_CHAR:
        fprintf(fp, "<span class=\"ascii_char\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_PREPROCESSOR_DIRECTIVE:
        fprintf(fp, "<span class=\"preprocess_dir\">");
        write_escaped(fp, event->data, event->length);
        fprintf(fp, "</span>");
        break;

    case PEVENT_HEADER_FILE:
    {
        char first = event->data[0];

        if (first == '<')
        {
           
            fprintf(fp, "<span class=\"header_file\">&lt;%.*s&gt;</span>",
                    event->length - 2,
                    event->data + 1);
        }
        else if (first == '"')
        {
            
            fprintf(fp, "<span class=\"header_file\">&quot;%.*s&quot;</span>",
                    event->length - 2,
                    event->data + 1);
        }
        else
        {
            /* Fallback */
            fprintf(fp, "<span class=\"header_file\">");
            write_escaped(fp, event->data, event->length);
            fprintf(fp, "</span>");
        }
        break;
    }

    case PEVENT_REGULAR_EXP:
    case PEVENT_EOF:
        write_escaped(fp, event->data, event->length);
        break;

    default:
        write_escaped(fp, event->data, event->length);
        break;
    }
}