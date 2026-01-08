#include  <stdio.h>
#include <string.h>

#include "diagnostics.h"

// Private Functions

// static void _parse_main_arguments(int args, char** argv) {
//
// }


int main (int args, char** argv) {
    Header* header = masm_diagnostics_init_header(0, 0, "masm", "unknown masm argument", true, ERROR_TYPE_ERROR);
    
    Message* message = masm_diagnostics_init_message(header, NULL, NULL);

    masm_diagnostics_append_message(message);


    Header* newHeader = masm_diagnostics_init_header(1, 5, "~/Downloads/Wallpapers/alena-aenami-stay-1k.jpeg", "Unknown token", false, ERROR_TYPE_ERROR);

    Body* body = masm_diagnostics_init_body("ADDI $R0, $R5, $R2", "Registers $R{0-31}", "$R5", FORMAT_TYPE_UNDERCURL);


    Message* newMessage = masm_diagnostics_init_message(newHeader, body, NULL);

    masm_diagnostics_append_message(newMessage);
    masm_diagnostics_print_messages();

    // masm_diagnostics_render_message(message);

    masm_diagnostics_free_diagnostic();
}
