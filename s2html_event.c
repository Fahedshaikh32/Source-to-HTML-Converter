#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "s2html_event.h"

typedef enum
{
    PSTATE_IDLE,
    PSTATE_NUMERIC_CONSTANT,
    PSTATE_STRING,
    PSTATE_SINGLE_LINE_COMMENT,
    PSTATE_MULTI_LINE_COMMENT,
    PSTATE_PREPROCESSOR,
    PSTATE_WORD,
    PSTATE_HEADER_FILE,
    PSTATE_USER_HEADER_FILE,  /* NEW: for #include "file.h" */
    PSTATE_CHAR_LITERAL       /* for 'a' '\n' '"' etc. */
} pstate_e;

/********** GLOBAL VARIABLES **********/

static pstate_e state = PSTATE_IDLE;
static pevent_t pevent_data;
static int event_data_idx = 0;

static char word[256];
static int word_idx = 0;

/********** HELPER **********/

static void set_parser_event(pstate_e s, pevent_e e)
{
    pevent_data.data[event_data_idx] = '\0';
    pevent_data.length = event_data_idx;
    event_data_idx = 0;
    state = s;
    pevent_data.type = e;
}

/********** KEYWORD CHECK **********/

int is_keyword(char *w)
{
    const char *keywords[] = {
        "int", "return", "if", "else", "for", "while",
        "char", "float", "double", "void",
        "short", "long", "unsigned", "signed",
        "static", "const", "volatile", "extern",
        "struct", "union", "enum", "typedef",
        "switch", "case", "break", "continue",
        "do", "goto", "default", "sizeof"
    };

    int count = (int)(sizeof(keywords) / sizeof(keywords[0]));
    for (int i = 0; i < count; i++)
    {
        if (strcmp(w, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

/********** CHAR LITERAL HANDLER **********/

pevent_t *pstate_char_literal_handler(FILE *fp, int ch)
{
    static int prev_was_backslash = 0;

    pevent_data.data[event_data_idx++] = ch;

    if (prev_was_backslash)
    {
        prev_was_backslash = 0;
        return NULL;
    }

    if (ch == '\\')
    {
        prev_was_backslash = 1;
        return NULL;
    }

    if (ch == '\'')
    {
        set_parser_event(PSTATE_IDLE, PEVENT_ASCII_CHAR);
        prev_was_backslash = 0;
        return &pevent_data;
    }

    return NULL;
}

/********** STRING HANDLER **********/

pevent_t *pstate_string_handler(FILE *fp, int ch)
{
    static int prev = 0;

    pevent_data.data[event_data_idx++] = ch;

    if (ch == '"' && prev != '\\')
    {
        set_parser_event(PSTATE_IDLE, PEVENT_STRING);
        prev = 0;
        return &pevent_data;
    }

    prev = ch;
    return NULL;
}

/********** NUMBER HANDLER **********/

pevent_t *pstate_numeric_constant_handler(FILE *fp, int ch)
{
    if (isdigit(ch))
    {
        pevent_data.data[event_data_idx++] = ch;
    }
    else
    {
        fseek(fp, -1, SEEK_CUR);
        set_parser_event(PSTATE_IDLE, PEVENT_NUMERIC_CONSTANT);
        return &pevent_data;
    }
    return NULL;
}

/********** WORD HANDLER **********/

pevent_t *pstate_word_handler(FILE *fp, int ch)
{
    if (isalnum(ch) || ch == '_')
    {
        word[word_idx++] = ch;
    }
    else
    {
        word[word_idx] = '\0';

        fseek(fp, -1, SEEK_CUR);

        strcpy(pevent_data.data, word);
        pevent_data.length = strlen(word);

        if (is_keyword(word))
            pevent_data.type = PEVENT_RESERVE_KEYWORD;
        else
            pevent_data.type = PEVENT_REGULAR_EXP;

        state = PSTATE_IDLE;
        word_idx = 0;

        return &pevent_data;
    }
    return NULL;
}

/********** SINGLE LINE COMMENT **********/

pevent_t *pstate_single_line_comment_handler(FILE *fp, int ch)
{
    if (ch == '\n')
    {
        set_parser_event(PSTATE_IDLE, PEVENT_SINGLE_LINE_COMMENT);
        return &pevent_data;
    }

    pevent_data.data[event_data_idx++] = ch;
    return NULL;
}

/********** MULTI LINE COMMENT **********/

pevent_t *pstate_multi_line_comment_handler(FILE *fp, int ch)
{
    pevent_data.data[event_data_idx++] = ch;

    if (ch == '*')
    {
        int next = fgetc(fp);

        if (next == '/')
        {
            pevent_data.data[event_data_idx++] = next;
            set_parser_event(PSTATE_IDLE, PEVENT_MULTI_LINE_COMMENT);
            return &pevent_data;
        }
        else
        {
            fseek(fp, -1, SEEK_CUR);
        }
    }

    return NULL;
}

/********** PREPROCESSOR **********/

pevent_t *pstate_preprocessor_handler(FILE *fp, int ch)
{
    /* Standard header: #include <stdio.h> */
    if (ch == '<')
    {
        fseek(fp, -1, SEEK_CUR);
        set_parser_event(PSTATE_IDLE, PEVENT_PREPROCESSOR_DIRECTIVE);
        return &pevent_data;
    }

    if (ch == '"')
    {
        
        fseek(fp, -1, SEEK_CUR);
        set_parser_event(PSTATE_USER_HEADER_FILE, PEVENT_PREPROCESSOR_DIRECTIVE);
        return &pevent_data;
    }

    pevent_data.data[event_data_idx++] = ch;

    if (ch == '\n')
    {
        set_parser_event(PSTATE_IDLE, PEVENT_PREPROCESSOR_DIRECTIVE);
        return &pevent_data;
    }

    return NULL;
}

/********** STANDARD HEADER FILE <stdio.h> **********/

pevent_t *pstate_header_file_handler(FILE *fp, int ch)
{
    pevent_data.data[event_data_idx++] = ch;

    if (ch == '>')
    {
        set_parser_event(PSTATE_IDLE, PEVENT_HEADER_FILE);
        return &pevent_data;
    }

    return NULL;
}

/********** USER HEADER FILE "file.h" — NEW **********/

pevent_t *pstate_user_header_file_handler(FILE *fp, int ch)
{
    pevent_data.data[event_data_idx++] = ch;

    /* closing quote — but not if it's the very first char (opening ") */
    if (ch == '"' && event_data_idx > 1)
    {
        set_parser_event(PSTATE_IDLE, PEVENT_HEADER_FILE);
        return &pevent_data;
    }

    return NULL;
}

/********** IDLE HANDLER **********/

pevent_t *pstate_idle_handler(FILE *fp, int ch)
{
    int next;

    switch (ch)
    {
    case '#':
        if (event_data_idx != 0)
        {
            fseek(fp, -1, SEEK_CUR);
            set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
            return &pevent_data;
        }
        state = PSTATE_PREPROCESSOR;
        pevent_data.data[event_data_idx++] = ch;
        break;

    case '<':
        if (state == PSTATE_PREPROCESSOR)
            state = PSTATE_HEADER_FILE;
        pevent_data.data[event_data_idx++] = ch;
        break;

    case '"':
        if (event_data_idx != 0)
        {
            fseek(fp, -1, SEEK_CUR);
            set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
            return &pevent_data;
        }
        state = PSTATE_STRING;
        pevent_data.data[event_data_idx++] = ch;
        break;

    case '\'':
        if (event_data_idx != 0)
        {
            fseek(fp, -1, SEEK_CUR);
            set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
            return &pevent_data;
        }
        state = PSTATE_CHAR_LITERAL;
        pevent_data.data[event_data_idx++] = ch;
        break;

    case '/':
        next = fgetc(fp);

        if (next == '*' || next == '/')
        {
            if (event_data_idx != 0)
            {
                fseek(fp, -2, SEEK_CUR);
                set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
                return &pevent_data;
            }

            state = (next == '*') ? PSTATE_MULTI_LINE_COMMENT
                                  : PSTATE_SINGLE_LINE_COMMENT;
            pevent_data.data[event_data_idx++] = ch;
            pevent_data.data[event_data_idx++] = next;
        }
        else
        {
            pevent_data.data[event_data_idx++] = ch;
            fseek(fp, -1, SEEK_CUR);
        }
        break;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        if (event_data_idx != 0)
        {
            fseek(fp, -1, SEEK_CUR);
            set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
            return &pevent_data;
        }
        state = PSTATE_NUMERIC_CONSTANT;
        pevent_data.data[event_data_idx++] = ch;
        break;

    default:
        if (isalpha(ch) || ch == '_')
        {
            if (event_data_idx != 0)
            {
                fseek(fp, -1, SEEK_CUR);
                set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
                return &pevent_data;
            }

            state = PSTATE_WORD;
            word_idx = 0;
            word[word_idx++] = ch;
        }
        else
        {
            pevent_data.data[event_data_idx++] = ch;
        }
        break;
    }

    return NULL;
}

/********** MAIN PARSER **********/

pevent_t *get_parser_event(FILE *fp)
{
    int ch;
    pevent_t *evptr = NULL;

    while ((ch = fgetc(fp)) != EOF)
    {
        switch (state)
        {
        case PSTATE_IDLE:
            if ((evptr = pstate_idle_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_STRING:
            if ((evptr = pstate_string_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_NUMERIC_CONSTANT:
            if ((evptr = pstate_numeric_constant_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_WORD:
            if ((evptr = pstate_word_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_SINGLE_LINE_COMMENT:
            if ((evptr = pstate_single_line_comment_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_MULTI_LINE_COMMENT:
            if ((evptr = pstate_multi_line_comment_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_PREPROCESSOR:
            if ((evptr = pstate_preprocessor_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_HEADER_FILE:
            if ((evptr = pstate_header_file_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_USER_HEADER_FILE:
            if ((evptr = pstate_user_header_file_handler(fp, ch)) != NULL)
                return evptr;
            break;

        case PSTATE_CHAR_LITERAL:
            if ((evptr = pstate_char_literal_handler(fp, ch)) != NULL)
                return evptr;
            break;

        default:
            state = PSTATE_IDLE;
            break;
        }
    }

    if (event_data_idx != 0)
    {
        set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
        return &pevent_data;
    }

    set_parser_event(PSTATE_IDLE, PEVENT_EOF);
    return &pevent_data;
}