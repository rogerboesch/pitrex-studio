#ifndef bas_internal_h
#define bas_internal_h

#include "rb_bas_tokenizer.h"
#include "rb_bas_variables.h"

#include <stdint.h>

// -----------------------------------------------------------------------------
#pragma mark - Text

#define RETBAS_VERSION "V1.0"

static const unsigned char logo1[] = "__      ________ _____ \xfe______   __\xf1";
static const unsigned char logo2[] = "\\ \\    / /  ____/ ____\xfe|___ \\ \\ / /\xf1";
static const unsigned char logo3[] = " \\ \\  / /| |__ | |    \xfe  __) \\ V /\xf1";
static const unsigned char logo4[] = "  \\ \\/ / |  __|| |    \xfe |__ < > <\xf1";
static const unsigned char logo5[] = "   \\  /  | |___| |____ \xfe___) / . \\\xf1";
static const unsigned char logo6[] = "    \\/   |______\\_____\xfe|____/_/ \\_\\\xf1";

static const unsigned char startmsg1[] = "\1\1 VEC3x OS " RETBAS_VERSION " ";
static const unsigned char memorymsg[] = "BYTES FREE";

static const unsigned char readymsg[] = "READY";

static const unsigned char expectcomma_errmsg[] 		= "?COMMA EXPECTED";
static const unsigned char expectparclose_errmsg[] 		= "?CLOSING PARENTHIS EXPECTED";
static const unsigned char expectforstack_errmsg[] 		= "FOR STACK FRAME EXPECTED";
static const unsigned char expectgosubstack_errmsg[] 	= "?GOSUB STACK FRAME EXPECTED";
static const unsigned char gotolinenotfound_errmsg[] 	= "?GOTO LINE NOT FOUND";
static const unsigned char expectgotonumber_errmsg[] 	= "?GOTO NUMBER EXPECTED";
static const unsigned char ifnothen_errmsg[] 			= "?IF WITHOUT THEN";
static const unsigned char onoutofbounds_errmsg[] 		= "?ON OUT OF BOUNDS";
static const unsigned char onwithout_errmsg[] 			= "?ON WITHOUT GOTO OR GOSUB";
static const unsigned char linenotfound_errmsg[] 		= "!LINE NOT FOUND";
static const unsigned char linetoolong_errmsg[] 		= "!LINE TOO LONG";
static const unsigned char expectlitstring_errmsg[] 	= "?LITERAL STRING EXPECTED";
static const unsigned char maxarg_errmsg[] 				= "!MAX ARGUMENTS";
static const unsigned char maxdim_errmsg[] 				= "!MAX DIM";
static const unsigned char listmaxsize_errmsg[] 		= "!MAX LIST SIZE REACHED";
static const unsigned char expectnext_errmsg[] 			= "?NEXT WITH %s EXPECTED, GOT %s";
static const unsigned char expectnum_errmsg[] 			= "?NUMBER EXPECTED";
static const unsigned char expectnumfac_errmsg[] 		= "?NUMERIC FACTOR EXPECTED";
static const unsigned char expectnumrh_errmsg[] 		= "?NUMERIC RHT EXPECTED";
static const unsigned char expectnumvar_errmsg[] 		= "?NUMBER VAR EXPECTED";
static const unsigned char cantallocprogram_errmsg[] 	= "!PROGRAM SPACE CANT BE ALLOCATED";
static const unsigned char readnodata_errmsg[] 			= "?READ WITHOUT DATA";
static const unsigned char expectrelop_errmsg[] 		= "?RELOP EXPECTED";
static const unsigned char cantallocstack_errmsg[] 		= "!STACK SPAC CANT BE ALLOCATEDE";
static const unsigned char statement_errmsg[] 			= "?STATEMENT ERROR";
static const unsigned char expectstrrh_errmsg[] 		= "?STRING RHT EXPECTED";
static const unsigned char expectsterm_errmsg[] 		= "?STRING TERM EXPECTED";
static const unsigned char expectstr_errmsg[] 		    = "?STRING EXPECTED";
static const unsigned char expectstrvar_errmsg[] 		= "?STRING VAR EXPECTED";
static const unsigned char stackfull_errmsg[] 			= "!STACK FULL";
static const unsigned char expsyntax_errmsg[] 			= "?SYNTAX ERROR: EXPRESSSION";
static const unsigned char facsyntax_errmsg[] 			= "?SYNTAX ERROR: FACTOR";
static const unsigned char termsyntax_errmsg[] 			= "?SYNTAX ERROR: TERM";
static const unsigned char unexpectedsym_errmsg[] 		= "?UNEXPECTED SYMBOL";
static const unsigned char unexpectedtok_errmsg[] 		= "?UNEXPECTED TOKEN";
static const unsigned char unknownexp_errmsg[] 			= "?UNKNOWN EXPRESSSION";
static const unsigned char expectvar_errmsg[] 			= "?VAR EXPECTED";
static const unsigned char expectvread_errmsg[] 		= "?VREAD EXPECTED %d DATA VALUES";
static const unsigned char expectvvar_errmsg[] 			= "?VDRAW VARIABLE NOT AVAILABLE";

#define FILENAME_AUTOLOAD "~AUTOLOAD"
#define FILENAME_LISTING "~LISTING"

// -----------------------------------------------------------------------------
#pragma mark - Tokenizer

typedef float (*function)(float number);

typedef struct {
    token _token;
    function _function;
} token_to_function;

typedef enum {
    basic_function_type_keyword,
    basic_function_type_op,
    basic_function_type_numeric,
    basic_function_type_string,
    basic_function_type_print
} basic_function_type;

typedef enum {
    kind_numeric,
    kind_string
} kind;

typedef union {
    float number;
    char* string;
} value;

typedef struct {
    kind kind;
    bool empty;
    bool mallocd;
    value value;
} basic_type;

typedef int (*function_0)(basic_type* rv);
typedef int (*function_1)(basic_type* v1, basic_type* rv);
typedef int (*function_2)(basic_type* v1, basic_type* v2, basic_type* rv);
typedef int (*function_3)(basic_type* v1, basic_type* v2, basic_type* v3, basic_type* rv);
typedef int (*function_4)(basic_type* v1, basic_type* v2, basic_type* v3, basic_type* v4, basic_type* rv);
typedef int (*function_5)(basic_type* v1, basic_type* v2, basic_type* v3, basic_type* v4, basic_type* v5, basic_type* rv);

token register_token(char* token_name);
token register_function_0(basic_function_type type, char* keyword, function_0 function);
token register_function_1(basic_function_type type, char* keyword, function_1 function, kind v1);
token register_function_2(basic_function_type type, char* keyword, function_2 function, kind v1, kind v2);
token register_function_3(basic_function_type type, char* keyword, function_3 function, kind v1, kind v2, kind v3);
token register_function_4(basic_function_type type, char* keyword, function_4 function, kind v1, kind v2, kind v3, kind v4);
token register_function_5(basic_function_type type, char* keyword, function_5 function, kind v1, kind v2, kind v3, kind v4, kind v5);

#define MAX_LINE tokenizer_string_length

typedef union {
    function_0 function_0;
    function_1 function_1;
    function_2 function_2;
    function_3 function_3;
    function_4 function_4;
    function_5 function_5;
} basic_function_union;

typedef struct {
    token token;
    basic_function_type type : 3;
    size_t nr_arguments : 3;
    kind kind_1 : 1;
    kind kind_2 : 1;
    kind kind_3 : 1;
    kind kind_4 : 1;
    kind kind_5 : 1;
    basic_function_union function;
} basic_function;

// -----------------------------------------------------------------------------
#pragma mark - Runtime

typedef enum {
    data_state_init,
    data_state_find,
    data_state_read
} data_state;

typedef struct {
    uint16_t line;
    char* cursor;
    char* char_pointer;
    data_state state : 2;
} data_pointer;


typedef union {
    float numeric;
    char *string;
} expression_value;

typedef enum {
    expression_type_numeric,
    expression_type_string
} expression_type;

typedef struct {
    expression_type type;
    expression_value value;
} expression_result;

typedef enum {
    stack_frame_type_for,
    stack_frame_type_gosub
} stack_frame_type;

typedef struct {
    stack_frame_type type;
    char variable_name[tokenizer_variable_length];
    float end_value;
    float step;
    size_t line;
    char* cursor;
} stack_frame_for;

typedef struct {
    stack_frame_type type;
    size_t line;
    char* cursor;
} stack_frame_gosub;

typedef enum {
    OP_NOP,
    OP_LT,
    OP_LE,
    OP_EQ,
    OP_GE,
    OP_GT,
    OP_NE
} relop;

// -----------------------------------------------------------------------------
#pragma mark - Internal functions

#ifdef BAS_INTERPRETTER

void expression(expression_result *result);
bool accept_token(token t);
size_t get_vector(size_t* vector, size_t size);
bool expect(token t);
bool _do_data_read(variable_type type, value* value);

void get_sym(void);
token current_sym(void);

void ready(void);

int expression_int_get(bool *flag);
float expression_float_get(bool *flag);
boolean accept_comma(void);

#else

extern void expression(expression_result *result);
extern bool accept_token(token t);
extern size_t get_vector(size_t* vector, size_t size);
extern bool expect(token t);
extern bool _do_data_read(variable_type type, value* value);

extern void get_sym(void);
extern token current_sym(void);

extern int expression_int_get(bool *flag);
extern float expression_float_get(bool *flag);
extern bool accept_comma(void);

extern void ready(void);

#endif

#endif
