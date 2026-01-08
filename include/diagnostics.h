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
    ERROR_TYPE_FATAL,
} ErrorType;

typedef enum {
    BODY_TYPE_UNDERCURL,
} BodyType;

typedef enum {
    FOOTER_TYPE_HELP, 
} FooterType; 


// global diagnostics 
extern Diagnostics diag;

    
Header* masm_diagnostics_init_header(int lineNum, int cursorNum, char* source, bool internal, ErrorType type, const char* text, ...);

void masm_diagnostics_free_header(Header* header);

Body* masm_diagnostics_init_body(char* line, char* suggestion, char* token, BodyType formatType);

void masm_diagnostics_free_body(Body* body);

Footer* masm_diagnostics_init_footer(char* suggestion, FooterType type);

void masm_diagnostics_free_footer(Footer* footer);

Message* masm_diagnostics_init_message(Header* header, Body* body, Footer* footer);

void masm_diagnostics_free_diagnostic(void);

void masm_diagnostics_append_message(Message* message);

void masm_diagnostics_render_header(Header* header);

void masm_diagnostics_render_body(Message* message);

void masm_diagnostics_render_footer(Footer* footer);

void masm_diagnostics_render_message(Message* message);

void masm_diagnostics_print_messages(void);

bool masm_diagnostics_panic();

bool masm_diagnostics_can_emit();
#endif
