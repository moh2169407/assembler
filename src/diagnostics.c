#include "diagnostics.h"

#include <assert.h>
#include <complex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "svec.h"

const char* RESET = "\033[0m";
const char* RED  = "\033[31m";
const char* GREEN  = "\033[32m";
const char* GOLDEN = "\033[38;5;214m";


const char* ITALIC = "\033[3m"; 
const char* BOLD = "\033[1m";
const char* UNDERLINE = "\033[4:3m";


struct header {
    int lineNum;
    int cursorNum;
    char* source;
    char* text;
    bool internal;
    ErrorType type;
};

struct body {
    char* line;
    char* suggestion;
    char* token;
    FormatType formatType;
};


// TODO
struct footer {

};

struct message {
    Header* header; 
    Body* body;
    Footer* footer;
    Message* next;
};

struct diagnostics{
    Message* next;
    Message* tail;
    bool panic;  
    bool canEmit;
};


// global diagnostics 
Diagnostics diag = {
    .next = NULL,
    .tail = NULL,
    .panic = false,
    .canEmit = true,
};


static char* _get_error_type(ErrorType type) {
    switch (type) {
    case ERROR_TYPE_WARNING:
            return "WARNING";
        break;
    case ERROR_TYPE_ERROR:
            return "ERROR";
        break;
    case ERRORTYPE_FATAL:
            return "FATAL ERROR";
      break;
    default:
        return "ERROR";
    }
}

static const char* _get_error_color(ErrorType type) {
    switch (type) {
    case ERROR_TYPE_WARNING:
            return GREEN;
        break;
    case ERROR_TYPE_ERROR:
            return RED;
        break;
    case ERRORTYPE_FATAL:
            return RED;
      break;
    default:
        return RED;
    }
}

Header* masm_diagnostics_init_header(int lineNum, int cursorNum, char* source, char* text, bool internal, ErrorType type) {
    assert(source != NULL && text != NULL);

    Header* header = xmalloc(sizeof(*header));
    header->lineNum = lineNum;
    header->cursorNum = cursorNum;
    header->source = strdup(source);
    header->text = strdup(text);
    header->internal = internal;
    header->type = type;

    return header;
}

void masm_diagnostics_free_header(Header* header) {
    assert(header != NULL);

    free(header->source);
    free(header->text);
    free(header);
}


Body* masm_diagnostics_init_body(char* line, char* suggestion, char* token, FormatType formatType) {
    Body* body = xmalloc(sizeof(*body));
    body->line = strdup(line);
    body->suggestion = suggestion ? strdup(suggestion) : NULL;
    body->token = strdup(token);
    body->formatType = formatType;

    return body;
}

void masm_diagnostics_free_body(Body* body) {
    if (body != NULL) {
        free(body->line);
        free(body->suggestion);
        free(body->token);
        free(body);
    }
}


Message* masm_diagnostics_init_message(Header* header, Body* body, Footer* footer) {
    Message* message = xmalloc(sizeof(*message));
    message->header = header;
    message->body = body;
    message->footer = footer;
    message->next = NULL;

    return message;
}

void masm_diagnostics_free_diagnostic(void) {
    Message* current = diag.next;
    Message* previous = NULL;

    while (current != NULL) {
        previous = current;
        current = current->next;

        masm_diagnostics_free_header(previous->header);
        masm_diagnostics_free_body(previous->body);

        free(previous);
    }
}

void masm_diagnostics_append_message(Message* message) {
    ErrorType type = message->header->type;
    if (diag.next == NULL) {
        diag.next = message;
        diag.tail = message;
    }
    else  {
        diag.tail->next = message;
        diag.tail = message;
    }

    if (type == ERRORTYPE_FATAL) {
        diag.panic = true;
        diag.canEmit = false;
    }

    if (type == ERROR_TYPE_ERROR) {
        diag.canEmit = false;
    }
}

void masm_diagnostics_render_header(Header* header) {
    assert(header);

    ErrorType type = header->type;

    if (header->internal) {
        fprintf(stdout, "%s%s:%s ", BOLD, header->source, RESET);
    }
    else {
        fprintf(stdout, "%s%s%s[%s%d%s:%s%d%s]: ", BOLD, header->source, RESET, GOLDEN, header->lineNum, RESET, GOLDEN, header->cursorNum, RESET);
    }

    fprintf(stdout, "%s%s%s:%s ", BOLD, _get_error_color(type), _get_error_type(type), RESET);

    fprintf(stdout, "%s\n", header->text);
}

void masm_diagnostics_render_body(Message* message) {
    Body* body = message->body;
    Header* header = message->header;

    if (!body) {return;}

    char* p = body->line;
    char* ret = strstr(body->line, body->token);
    int position = ret - p;

    fprintf(stdout, "\t|\n\t\b\b%s%d%s | ", GOLDEN,  message->header->lineNum, RESET);

    switch (body->formatType) {
        case FORMAT_TYPE_UNDERCURL: {
            while (*p && ret) {
                if (p < ret || p > ret) {
                    fprintf(stdout, "%c", *p);
                    p++;
                }
                else if (p == ret) {
                    fprintf(stdout, "%s", UNDERLINE);
                    for (int i = 0; i < strlen(body->token); i++) {
                        fprintf(stdout, "%c", *p);
                        p++;
                    }
                    fprintf(stdout, "%s", RESET);
                }
            }
            putc('\n', stdout);
        }
            break;
    }

    if (body->suggestion) {
        fprintf(stdout, "\t| ");
        for (int i = 0; i < position; i++) {
            putc(' ', stdout);
        }
        fprintf(stdout, "%s^-----%s%s\n", GREEN, body->suggestion, RESET);
    }
}

void masm_diagnostics_render_message(Message* message) {
    masm_diagnostics_render_header(message->header);
    masm_diagnostics_render_body(message);
}

void masm_diagnostics_print_messages(void) {
    Message* head = diag.next;   
    while (head != NULL) {
        masm_diagnostics_render_message(head);
        head = head->next;
    }
}

bool masm_diagnostics_panic() {
    return diag.panic;
}

bool masm_diagnostics_can_emit() {
    return diag.canEmit;
}
