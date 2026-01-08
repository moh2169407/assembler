#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stdbool.h>

// Forwards Declares Types
typedef struct header Header;
typedef struct body Body;
typedef struct footer Footer;
typedef struct message Message;
typedef struct diagnostics Diagnostics;

typedef enum {
    ERROR_TYPE_WARNING = 0,
    ERROR_TYPE_ERROR,
    ERRORTYPE_FATAL,
} ErrorType;

typedef enum {
    FORMAT_TYPE_UNDERCURL,
} FormatType;


// global diagnostics 
extern Diagnostics diag;

    
Header* masm_diagnostics_init_header(int lineNum, int cursorNum, char* source, char* text, bool internal, ErrorType type);

void masm_diagnostics_free_header(Header* header);

Body* masm_diagnostics_init_body(char* line, char* suggestion, char* token, FormatType formatType);

void masm_diagnostics_free_body(Body* body);

Message* masm_diagnostics_init_message(Header* header, Body* body, Footer* footer);

void masm_diagnostics_free_diagnostic(void);

void masm_diagnostics_append_message(Message* message);

void masm_diagnostics_render_header(Header* header);

void masm_diagnostics_render_body(Message* message);

void masm_diagnostics_render_message(Message* message);

void masm_diagnostics_print_messages(void);

bool masm_diagnostics_panic();

bool masm_diagnostics_can_emit();
#endif
