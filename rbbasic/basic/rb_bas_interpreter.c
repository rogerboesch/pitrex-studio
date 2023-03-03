
#define BAS_INTERPRETTER

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "rb_bas_arch.h"
#include "rb_bas_error.h"
#include "rb_bas_lines.h"
#include "rb_bas_array.h"
#include "rb_bas_kbhit.h"
#include "rb_bas_io.h"
#include "rb_bas_internal.h"
#include "rb_bas_interpreter.h"

static array* basic_tokens = NULL;
static array* basic_functions = NULL;

static token t_keyword_print = -1;
static token t_keyword_print_short = -1;
static token t_keyword_spc = -1;
static token t_keyword_tab = -1;
static token t_keyword_goto = -1;
static token t_keyword_on = -1;
static token t_keyword_if = -1;
static token t_keyword_then = -1;
static token t_keyword_gosub = -1;
static token t_keyword_return = -1;
static token t_keyword_list = -1;
static token t_keyword_clear = -1;
static token t_keyword_new = -1;
static token t_keyword_run = -1;
static token t_keyword_end = -1;
static token t_keyword_stop = -1;
static token t_keyword_for = -1;
static token t_keyword_to = -1;
static token t_keyword_step = -1;
static token t_keyword_next = -1;
static token t_keyword_rem = -1;
static token t_keyword_dim = -1;
static token t_keyword_data = -1;
static token t_keyword_read = -1;
static token t_keyword_restore = -1;
static token t_keyword_load = -1;
static token t_keyword_save = -1;
static token t_keyword_delete = -1;
static token t_keyword_dir = -1;
static token t_keyword_test = -1;
static token t_keyword_sleep = -1;
static token t_keyword_poke = -1;

static token t_op_or = -1;
static token t_op_and = -1;

static char* __cursor = NULL;
static char* __memory = NULL;
static char* __stack = NULL;
static size_t __memory_size = -1;
static size_t __stack_size = -1;
static size_t __program_size = -1;
static size_t __stack_p = -1;

static data_pointer __data;
static token __sym = 0;

static bool __RUNNING = false;
static bool __PAUSED = false;
static bool __EXECUTE_ONE_LINE = false;
static bool __REPL = true;
static bool __FIRST = true;

bool __AUTORENDER = true;
uint16_t __line = -1;

static int basic_dispatch_function(basic_function* function, basic_type* rv);
static basic_function* find_basic_function_by_type(token sym, basic_function_type type);

static void move_to_next_statement(void);
static bool statement(void);
static void parse_line(void);

static void expression_print(expression_result* expr);

static void set_line( uint16_t line_number);

static char* string_expression(void);
static float numeric_expression(void);
static char* string_expression(void);
static bool string_condition(char *left, char *right, relop op);
static bool numeric_condition(float left, float right, relop op);
static relop get_relop(void);

int dump(basic_type* rv);
static char* string_term(void);

int str_len(basic_type* str, basic_type* rv);
int str_asc(basic_type* str, basic_type* rv);
int str_val(basic_type* str, basic_type* rv);
int str_str(basic_type* number, basic_type* rv);

// External token inits
void basic_vgraphic_init(void);
void basic_math_init(void);
void basic_sound_init(void);

// -----------------------------------------------------------------------------
#pragma mark - Logical helper

static float _or(float a, float b) {
    return (float) ( (int) a | (int) b );
}

static float _and(float a, float b) {
    return (float) ( (int) a & (int) b );
}

// -----------------------------------------------------------------------------
#pragma mark - String functions

static int is_comment(const char *s) {
	return s[0] == '#';
}

static int is_empty(const char *s) {
	while (*s != '\0') {
		if (!isspace(*s))
			return 0;
		s++;
	}
	
	return 1;
}

static void _trim(char* s) {
	char* p = s + strlen(s) - 1; // without the '\0'
	while (p >= s && isspace(*p)) {
		--p;
	}
	
	*(p+1) = '\0';
}

static int str_chr(basic_type* n, basic_type* rv) {
    rv->kind = kind_string;
    char *chr;
    asprintf(&chr, "%c", (int) n->value.number);
    rv->value.string = chr;
    rv->mallocd = true;
    
    return 0;
}

static int str_mid(basic_type* str, basic_type* start, basic_type* length, basic_type* rv) {
    rv->kind = kind_string;
    char* source = str->value.string;
    int _start = (int) start->value.number - 1;
    
    if (_start>strlen(source))
        _start = (int)strlen(source);
    
    int _length;
    
    if (length->empty) {
        _length = (int)strlen(source) - _start;
    }
    else {
        _length = (int) length->value.number;
    
        if (_length+_start > (int)strlen(source))
            _length = (int)strlen(source) - _start;
    }
    
    char* string = strdup(&source[_start]);
    string[_length] = '\0';
    rv->value.string = string;
    rv->mallocd = true;

    return 0;
}

static int str_right(basic_type* str, basic_type* length, basic_type* rv) {
    rv->kind = kind_string;
    char* source = str->value.string;
    rv->value.string = strdup(&source[(int)strlen(source) - (int) length->value.number]);
    rv->mallocd = true;
    
    return 0;
}

static int str_left(basic_type* str, basic_type* length, basic_type* rv) {
    rv->kind = kind_string;
    rv->value.string = strdup(str->value.string);
    rv->value.string[(int) length->value.number] = '\0';
    rv->mallocd = true;
    
    return 0;
}

int str_len(basic_type* str, basic_type* rv) {
	rv->kind = kind_numeric;
	rv->value.number = (int) strlen(str->value.string);
	
	return 0;
}

int str_asc(basic_type* str, basic_type* rv) {
	rv->kind = kind_numeric;
	rv->value.number = (int) *(str->value.string);
	
	return 0;
}

int str_val(basic_type* str, basic_type* rv) {
	rv->kind = kind_numeric;
	rv->value.number = atof(str->value.string);
	
	return 0;
}

int str_str(basic_type* number, basic_type* rv) {
	rv->kind = kind_string;
	asprintf(&(rv->value.string), "%f", number->value.number);
	rv->mallocd = true;
	
	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Misc.

void ready(void) {
	if (__REPL) {
		textbuffer_print_line((char *)readymsg);
	}
}

static int do_rem(basic_type* rv) {
	  accept_token(t_keyword_rem);
	  set_line(lines_next(__line));
	  get_sym();
	
	  return 0;
}

static int do_run(basic_type* rv) {
	__line = lines_first();
	__cursor = lines_get_contents(__line);
	tokenizer_init( __cursor );
	__RUNNING = true;
	__PAUSED = false;
	__EXECUTE_ONE_LINE = false;
	__AUTORENDER = true;
	
	while (__cursor && __RUNNING && last_error == NULL) {
		get_sym();
		
		if (__sym == T_EOF) {
			__line = lines_next(__line);
			__cursor = lines_get_contents(__line);
			
			if (__cursor == NULL) {
				__RUNNING = false;
				break;
			}
			
			tokenizer_init( __cursor );
		}
		
		parse_line();
		
		if (__AUTORENDER) {
			//platform_render_frame();
		}
		
		// Check if we are in PAUSE mode or step mode
		while (__PAUSED && !__EXECUTE_ONE_LINE) {
            platform_sleep(100);
		}
		
		__EXECUTE_ONE_LINE = false;
	}

	ready();

	__RUNNING = false;

	return 0;
}

static int do_let(basic_type* rv) {
	bool is_array = false;
	size_t vector[5];

	if (__sym != T_VARIABLE_NUMBER && __sym != T_VARIABLE_STRING) {
		error((char *)expectvar_errmsg);
		return 0;
	}

	char name[tokenizer_variable_length];
	tokenizer_get_variable_name(name);
	token var_type = __sym;
	get_sym();
	
	if (__sym == T_LEFT_BANANA) {
		is_array = true;
		size_t l = strlen(name);
		name[l] = '(';
		name[l+1] = '\0';
		
		accept_token(T_LEFT_BANANA);
		get_vector(vector, 5);
		expect(T_RIGHT_BANANA);
	}

	expect(T_EQUALS);

	if (var_type == T_VARIABLE_NUMBER) {
		float value = numeric_expression();
		
		if (is_array) {
			variable_array_set_numeric(name, value, vector);
		}
		else {
			variable_set_numeric(name, value);
		}
	}

	if (var_type == T_VARIABLE_STRING) {
		char *value = string_expression();
		
		if (is_array) {
			variable_array_set_string(name, value, vector);
		}
		else {
			variable_set_string(name, value);
		}
		
		free(value);
	}

	return 0;
}

int do_sleep(basic_type* rv) {
    accept_token(t_keyword_sleep);
    
    float seconds = 0;

    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        seconds = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

    platform_sleep(seconds*1000.0);
	
	return 0;
}

static int f_mem(basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = __memory_size - lines_memory_used();
    
    return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Listing

static void list_out(uint16_t number, char* contents) {
	char buffer[tokenizer_string_length];
	snprintf(buffer, sizeof(buffer), "%d %s\n", number, contents);
	
	basic_io_print(buffer);
}

static int do_list(basic_type* rv) {
	uint16_t start = 0;
	uint16_t end = 0;

	accept_token(t_keyword_list);

	if (__sym == T_NUMBER) {
		start = (uint16_t) tokenizer_get_number();
		accept_token(T_NUMBER);
	
		if (__sym==T_MINUS) {
			accept_token(T_MINUS);
			
			if (__sym==T_NUMBER) {
				end = (uint16_t) tokenizer_get_number();
				accept_token(T_NUMBER);
			}
		}
	}

	lines_list(start, end, list_out);
	ready();

	return 0;
}

static int do_clear(basic_type* rv) {
	accept_token(t_keyword_clear);
	lines_clear();
	ready();
	
	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Print

static char* string_term(void) {
	char *string = NULL;
	char var_name[tokenizer_variable_length];

	switch (__sym) {
		case T_STRING:
			string = strdup(tokenizer_get_string());
			accept_token(T_STRING);
			break;
		case T_VARIABLE_STRING:
			tokenizer_get_variable_name(var_name);
			get_sym();
	
			if (__sym == T_LEFT_BANANA) {
				size_t l = strlen(var_name);
				var_name[l] = '(';
				var_name[l+1] = '\0';
				accept_token(T_LEFT_BANANA);
				size_t vector[5];
				get_vector(vector,5);
                //do_dump(0L);
				string = strdup(variable_array_get_string(var_name, vector));
				expect(T_RIGHT_BANANA);
			}
			else {
				string = strdup(variable_get_string(var_name));
				accept_token(T_VARIABLE_STRING);
			}
			break;
		default: {
			basic_function* bf = find_basic_function_by_type(__sym, basic_function_type_string);
	
			if ( bf != NULL ) {
				basic_type rv;
				basic_dispatch_function( bf, &rv);

				if (rv.kind != kind_string) {
					error((char *)expectsterm_errmsg);
				}
				
				string = strdup(rv.value.string);
				if(rv.mallocd==true)
					free(rv.value.string);
			}
		}
		break;
	}

	return string;
}

static char* string_expression(void) {
	char* s1 = string_term();

	while (__sym == T_PLUS) {
		accept_token(T_PLUS);
		char *s2 = string_term();
		size_t len = strlen(s1) + strlen(s2) + 1;
		s1 = realloc(s1, len);
		s1 = strcat(s1, s2);
		free(s2);
	}

	return s1;
}

static int do_print(basic_type* rv) {
	accept_token(t_keyword_print);
	accept_token(t_keyword_print_short);

	if (__sym == T_EOF || __sym == T_COLON) {
		textbuffer_print_line("");
	}
	else {
		while (__sym != T_EOF && __sym != T_COLON) {
			basic_function* bf = find_basic_function_by_type(__sym, basic_function_type_print);

			if (bf != NULL) {
				basic_type rv;
				basic_dispatch_function( bf, &rv);
			}
			else {
				expression_result expr;
				expression(&expr);
				expression_print(&expr);
				
				if (expr.type == expression_type_string) {
				   free(expr.value.string);
				}
			}

			if (__sym == T_SEMICOLON) {
				accept_token(T_SEMICOLON);
			}
			else if (__sym == T_COMMA) {
				accept_token(T_COMMA);
				textbuffer_print_char('\t');
			}
			else {
                textbuffer_print_line("");
			}
		}
	}

	return 0;
}

static int do_spc(basic_type* n, basic_type* rv) {
	for (size_t i=0; i<n->value.number; i++) {
		textbuffer_print_char(' ');
	}
	
	rv->kind = kind_numeric;
	rv->value.number = 0;
	
	return 0;
}

static int do_tab(basic_type* n, basic_type* rv) {
	for (size_t i=0; i<n->value.number; i++) {
		textbuffer_print_char(' ');
	}
	
	rv->kind = kind_numeric;
	rv->value.number = 0;
	
	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Branching

static int do_goto(basic_type* rv) {
	accept_token(t_keyword_goto);

	if (__sym != T_NUMBER) {
		error((char *)expectgotonumber_errmsg);
		return 0;
	}

	int line_number = (int) tokenizer_get_number();
	accept_token(T_NUMBER);

	char* line = lines_get_contents(line_number);
	if (line == NULL) {
		error((char *)gotolinenotfound_errmsg);
		return 0;
	}

	set_line( line_number );

	return 0;
}

static size_t get_list(size_t* list, size_t max_size) {
	size_t size = 0;
	
	do {
		if (__sym == T_COMMA) {
		  accept_token(T_COMMA);
		}

		float n = numeric_expression();
		list[size] = n;
		size++;
		
		if (size>max_size) {
			error((char *)listmaxsize_errmsg);
			return size;
		}
	} while (__sym == T_COMMA);

	return size;
}

static int do_on_goto(basic_type* rv) {
	accept_token(t_keyword_on);

	int index = numeric_expression();

	token what = T_EOF;
	
	if (__sym == t_keyword_goto) {
		what = t_keyword_goto;
	}
	else if (__sym == t_keyword_gosub) {
		what = t_keyword_gosub;
	}
	else {
		error((char *)onwithout_errmsg);
		return 0;
	}
	
	accept_token(what);

	size_t list[10];
	size_t size = get_list(list, 10);

	if (index > size) {
		error((char *)onoutofbounds_errmsg);
		return 0;
	}

	size_t line_number = list[index-1];
	if (what == t_keyword_goto) {
		char* line = lines_get_contents(line_number);
		
		if (line == NULL) {
            error((char *)linenotfound_errmsg);
		}
		
		set_line( line_number );
	}
	else {
		stack_frame_gosub *g;
		
		if ( __stack_p < sizeof(stack_frame_gosub)) {
			error((char *)stackfull_errmsg);
			return 0;
		}

		__stack_p -= sizeof(stack_frame_gosub);
		g = (stack_frame_gosub*) &(__stack[__stack_p]);

		g->type = stack_frame_type_gosub;
		g->line = __line;
		g->cursor = tokenizer_char_pointer(NULL);
		
		set_line( line_number );
	}
	
	return 0;
}

static int do_gosub(basic_type* rv) {
	accept_token(t_keyword_gosub);

	if (__sym != T_NUMBER) {
		error((char *)expectnum_errmsg);
		return 0;
	}
	
	int line_number = (int) tokenizer_get_number();
	accept_token(T_NUMBER);

	stack_frame_gosub *g;
	if (__stack_p < sizeof(stack_frame_gosub)) {
		error((char *)stackfull_errmsg);
		return 0;
	}

	__stack_p -= sizeof(stack_frame_gosub);
	g = (stack_frame_gosub*) &(__stack[__stack_p]);

	g->type = stack_frame_type_gosub;
	g->line = __line;
	g->cursor = tokenizer_char_pointer(NULL);

	set_line( line_number );

	return 0;
}

static int do_return(basic_type* rv) {
	accept_token(t_keyword_return);

	stack_frame_gosub *g;
	g = (stack_frame_gosub*) &(__stack[__stack_p]);

	if (g->type != stack_frame_type_gosub) {
		error((char *)expectgosubstack_errmsg);
		return 0;
	}

	__line = g->line;
	tokenizer_char_pointer(g->cursor);

	__stack_p += sizeof(stack_frame_gosub);

	return 0;
}

static int do_for(basic_type* rv) {
	accept_token(t_keyword_for);

	if (__sym != T_VARIABLE_NUMBER) {
		error((char *)expectvar_errmsg);
		return 0;
	}

	char name[tokenizer_variable_length];
	tokenizer_get_variable_name(name);
	get_sym();
	expect(T_EQUALS);
	
	float value = numeric_expression();
	variable_set_numeric(name, value);

	expect(t_keyword_to);

	float end_value = numeric_expression();

	float step = 1.0;
	if (__sym != T_EOF && __sym != T_COLON) {
		expect(t_keyword_step);
		step = numeric_expression();
	}

	stack_frame_for *f;
	if (__stack_p <  sizeof(stack_frame_for)) {
		error((char *)stackfull_errmsg);
		return 0;
	}

	__stack_p -= sizeof(stack_frame_for);
	f = (stack_frame_for*) &(__stack[__stack_p]);

	f->type = stack_frame_type_for;
	strncpy(f->variable_name, name, tokenizer_variable_length);
	f->end_value = end_value;
	f->step = step;
	f->line = __line;
	f->cursor = tokenizer_char_pointer(NULL);

	return 0;
}

static int do_next(basic_type* rv) {
	accept_token(t_keyword_next);

	stack_frame_for *f;
	f = (stack_frame_for*) &(__stack[__stack_p]);

	if (f->type != stack_frame_type_for) {
		error((char *)expectforstack_errmsg);
		return 0;
	}

	if (__sym == T_VARIABLE_NUMBER) {
		char var_name[tokenizer_variable_length];
		tokenizer_get_variable_name(var_name);
		accept_token(T_VARIABLE_NUMBER);
		
		if (strcmp(var_name, f->variable_name) != 0) {
			char _error[32];
			snprintf(_error, sizeof(_error), (char *)expectnext_errmsg, var_name, f->variable_name);
			error(_error);
			
			return 0;
		}
	}

	// check end condition
	float value = variable_get_numeric(f->variable_name) + f->step;
	if ((f->step > 0 && value > f->end_value) || (f->step < 0 && value < f->end_value)) {
	  __stack_p += sizeof(stack_frame_for);
	  return 0;
	}

	variable_set_numeric(f->variable_name, value);

	__line = f->line;
	tokenizer_char_pointer( f->cursor );

	return 0;
}

static int do_end(basic_type* rv) {
    __RUNNING = false;
    return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Data

size_t get_vector(size_t* vector, size_t size) {
	for (size_t i=0; i<size; i++) {
		vector[i] = 0;
	}

	size_t dimensions = 0;
	while (__sym != T_RIGHT_BANANA) {
		float n = numeric_expression();
		vector[dimensions] = n;
		dimensions++;
		
		if (dimensions>size) {
			error((char *)maxdim_errmsg);
			return dimensions;
		}
		
		// accept(T_NUMBER);
		if (__sym == T_COMMA) {
			accept_token(T_COMMA);
		}
	}
	
	return dimensions;
}

static int do_dim(basic_type* rv) {
	accept_token(t_keyword_dim);

	while (__sym != T_EOF && __sym != T_COLON) {
		if (__sym == T_VARIABLE_NUMBER || __sym == T_VARIABLE_STRING ) {
			variable_type type = (__sym == T_VARIABLE_STRING) ? variable_type_string : variable_type_numeric ;
			size_t vector[5];
			char name[tokenizer_variable_length];
			tokenizer_get_variable_name(name);

			size_t l = strlen(name);
			name[l] = '(';
			name[l+1] = '\0';
			accept_token(__sym);
			expect(T_LEFT_BANANA);
			size_t dimensions = get_vector(vector, 5);
			expect(T_RIGHT_BANANA);

			variable_array_init(name, type, dimensions, vector);
		}

		if (__sym == T_COMMA) {
		  accept_token(T_COMMA);
		}
	}

	return 0;
}

static int do_data(basic_type* rv) {
	accept_token(t_keyword_data);
	move_to_next_statement();
	
	return 0;
}

static bool _data_find(variable_type type, value* value) {
	tokenizer_init(__data.cursor);
	tokenizer_char_pointer(__data.char_pointer);
	
	while (__data.cursor) {
		get_sym();
		
		if (__sym == T_ERROR) {
			error((char *)unexpectedtok_errmsg);
			return false;
		}
		
		while (__sym != T_EOF) {
			if (__sym == T_ERROR) {
				error((char *)unexpectedtok_errmsg);
				return false;
			}

			if (__sym == t_keyword_data) {
				accept_token(t_keyword_data);
			
				if (type == variable_type_string) {
					value->string = tokenizer_get_string();
				}
				else {
					value->number = tokenizer_get_number();
				}
				
				__data.state = data_state_read;
				__data.char_pointer = tokenizer_char_pointer(NULL);
	
				return true;
			}
			else if (__sym == t_keyword_rem) {
				// Parsing REM Further results in unwanted side effects
				__sym = T_EOF;
			}
			else {
				get_sym();
			}
		}
		
		__data.line = lines_next(__data.line);
		__data.cursor = lines_get_contents(__data.line);
		tokenizer_init(__data.cursor);
	}
	
	return false;
}  

static bool _data_read(variable_type type, value* value) {
	bool rv = false;

	tokenizer_init( __data.cursor );
	tokenizer_char_pointer( __data.char_pointer );
	get_sym();
	
	if (__sym != T_EOF) {
		accept_token(T_COMMA); // seperated by comma's
		
		int factor = 1;
		if (__sym == T_MINUS) {
			factor *= -1;
			accept_token(T_MINUS);
		}
		
		if (type == variable_type_string) {
			value->string = tokenizer_get_string();
		}
		else {
			float f = tokenizer_get_number();
			f *= factor;
			
			value->number = f;
		}
		
		__data.char_pointer = tokenizer_char_pointer(NULL);
		rv = true;
	}
	else {
		__data.cursor = lines_get_contents(__data.line);
	}
	
	return rv;
}  

bool _do_data_read(variable_type type, value* value) {
	char* save_pointer = tokenizer_char_pointer(NULL);
	bool rv = false;

	switch (__data.state) {
		case data_state_init:
			__data.line = lines_first();
			__data.cursor = lines_get_contents(__data.line);
			__data.char_pointer = tokenizer_char_pointer(NULL);
			__data.state = data_state_find;
			rv = _data_find(type, value);
			break;

		case data_state_find:
			rv = _data_find(type, value);
			break;

		case data_state_read: {
			bool data_found = _data_read(type, value);
			if (data_found) {
				rv = true;
			}
			else {
				rv = _data_find(type, value);
			}
		}
	}

	tokenizer_init( __cursor );
	tokenizer_char_pointer(save_pointer);

	return rv;
}

static int do_read(basic_type* rv) {
	bool is_array =  false;
	size_t vector[5];

	accept_token(t_keyword_read);

	// if not initialized data_pointer, find first data statement
	// while not end of variable list read data, put in variable
	// proceed to next data statement
	while (__sym != T_EOF && __sym != T_COLON) {
		if (__sym == T_VARIABLE_NUMBER || __sym == T_VARIABLE_STRING) {
			variable_type type = (__sym == T_VARIABLE_STRING) ? variable_type_string : variable_type_numeric ;
			char name[tokenizer_variable_length];
			tokenizer_get_variable_name(name);
			accept_token(__sym);
			
			if (__sym == T_LEFT_BANANA) {
				is_array = true;
				size_t l = strlen(name);
				name[l] = '(';
				name[l+1] = '\0';
				accept_token(T_LEFT_BANANA);
				get_vector(vector,5);

				if (!expect(T_RIGHT_BANANA)) {
					error((char *)expectparclose_errmsg);
					return 0;
				}
			}

			value v;
			bool read_ok = _do_data_read(type, &v);
			
			if (!read_ok) {
				error((char *)readnodata_errmsg);
				return 0;
			}
			
			if (type == variable_type_string) {
				if (is_array) {
					variable_array_set_string(name, v.string, vector);
				}
				else {
					variable_set_string(name, v.string);
				}
			}
			else {
				if (is_array) {
					variable_array_set_numeric(name, v.number, vector);
				}
				else {
					variable_set_numeric(name, v.number);
				}
			}
		}
		
		get_sym();
		accept_token(T_COMMA);
	}

	return 0;
}

static int do_restore(basic_type* rv) {
	accept_token(t_keyword_restore);
	__data.line = 0;
	__data.cursor = 0;
	__data.state = data_state_init;

	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - File handling

static void _store(char* line) {
	int number;
	sscanf(line, "%d", &number);

	char* p = line;
	while (isdigit(*p)) {
		p++;
	}
	
	while (isspace(*p)) {
		p++;
	}
	
	_trim(p);
	lines_store((uint16_t)number, p);
}  

static void _load_cb(char* line, void* context) {
	if (!(is_empty(line) || is_comment(line))) {
		_store(line);
	}
}  

static int do_load(basic_type* rv) {
	accept_token(t_keyword_load);
	
	if (__sym != T_STRING) {
		error((char *)expectlitstring_errmsg);
		return 0;
	}
	
    // TODO: Get project path
    char path[256];
	char *filename = tokenizer_get_string();
	accept_token(T_STRING);
	lines_clear();
	arch_load(filename, path, _load_cb, NULL);
	ready();

	return 0;
}

typedef struct {
    uint16_t number;
} _save_cb_ctx;

static uint16_t _save_cb(char** line, void* context) {
	_save_cb_ctx* ctx = (_save_cb_ctx*) context;
	uint16_t number = ctx->number;
	ctx->number = lines_next(number);

	*line = lines_get_contents(number);

	return number;
}  

static int do_save(basic_type* rv) {
	accept_token(t_keyword_save);
	
	if (__sym != T_STRING) {
		error((char *)expectlitstring_errmsg);
		return 0;
	}
	
	char *filename = tokenizer_get_string();
	accept_token(T_STRING);
	_save_cb_ctx ctx;
	ctx.number = lines_first();
	arch_save(filename, _save_cb, &ctx);
	ready();

	return 0;
}

static int do_delete(basic_type* rv) {
	accept_token(t_keyword_delete);
	
	if (__sym != T_STRING) {
		error((char *)expectlitstring_errmsg);
		return 0;
	}
	
	char *filename = tokenizer_get_string();
	accept_token(T_STRING);

	arch_delete(filename);
	ready();

	return 0;
}

static void _dir_cb(char* name, char* ext, size_t size, bool label, void* context) {
    char temp[255];
    sprintf(temp, ">%s%s\n", name, ext);
    textbuffer_print(temp);
}

static int do_dir(basic_type* rv) {
	accept_token(t_keyword_dir);
	
	arch_dir(".BAS", _dir_cb, NULL);
	ready();

	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Condition

static relop get_relop(void) {
	if (__sym == T_LESS) {
		accept_token(T_LESS);
		
		if (__sym == T_EQUALS) {
		  accept_token(T_EQUALS);
		  return OP_LE;
		}
		else if (__sym == T_GREATER) {
			accept_token(T_GREATER);
			return OP_NE;
		}
		
		return OP_LT;
	}

	if (__sym == T_EQUALS) {
		accept_token(T_EQUALS);
		return OP_EQ;
	}

	if (__sym == T_GREATER) {
		accept_token(T_GREATER);
		
		if (__sym == T_EQUALS) {
			accept_token(T_EQUALS);
			return OP_GE;
		}
		
		return OP_GT;
	}

	return OP_NOP;
}

static bool numeric_condition(float left, float right, relop op) {
	switch(op) {
		case OP_NOP:
			error((char *)expectrelop_errmsg);
			break;
		case OP_LT:
			return left < right;
		case OP_LE:
			return left <= right;
		case OP_EQ:
			return left == right;
		case OP_GE:
			return left >= right;
		case OP_GT:
			return left > right;
		case OP_NE:
			return left != right;
	}

	return false;
}

static bool string_condition(char *left, char *right, relop op) {
	int comparison = strcmp(left, right);

	switch(op) {
		case OP_NOP:
			error((char *)expectrelop_errmsg);
			break;
		case OP_LT:
			return comparison < 0;
		case OP_LE:
			return comparison <= 0;
		case OP_EQ:
			return comparison == 0;
		case OP_NE:
			return comparison != 0;
		case OP_GE:
			return comparison >= 0;
		case OP_GT:
			return comparison > 0;
	}

	return false;
}

static bool condition(expression_result *left_er, expression_result *right_er, relop op) {
	if (left_er->type == expression_type_numeric) {
		if (right_er->type != expression_type_numeric) {
			error((char *)expectnumrh_errmsg);
		}
		
		return numeric_condition(left_er->value.numeric, right_er->value.numeric, op);
	}
	else {
		if (right_er->type != expression_type_string) {
			error((char *)expectstrrh_errmsg);
		}
		
		return string_condition(left_er->value.string, right_er->value.string, op);
	}
}

static void move_to_next_statement(void) {
	while (__sym != T_EOF && __sym != T_COLON) {
		get_sym();
	}
}

static void move_to_next_line(void) {
	set_line(lines_next(__line));
	get_sym();
}

static int do_if(basic_type* rv) {
	expression_result left_side, right_side;
	bool result;

	expression(&left_side);

	if (left_side.type == expression_type_string) {
		relop op = get_relop();
		expression(&right_side);
		result = condition(&left_side, &right_side, op);
	}
	else {
		result = left_side.value.numeric == 1.0;
	}

	if (__sym != t_keyword_then) {
		error((char *)ifnothen_errmsg);
		return 0;
	}

	if (result) {
		get_sym();

		if (__sym == T_NUMBER) {
			float line_number = tokenizer_get_number();
			accept_token(T_NUMBER);
			set_line(line_number);
		}
		else {
			parse_line();
		}
	}
	else {
		move_to_next_line();
	}

	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Input

static int do_input(basic_type* rv) {
	bool prompt = false;
	expression_result expr;

	if (__sym != T_VARIABLE_NUMBER && __sym != T_VARIABLE_STRING) {
		expression(&expr);
		
		if (__sym == T_COMMA || __sym == T_SEMICOLON) {
			get_sym();
		}
		else {
			error((char *)unexpectedtok_errmsg);
		}
		
		prompt = true;
	}

	if (__sym != T_VARIABLE_NUMBER && __sym != T_VARIABLE_STRING) {
		error((char *)expectvar_errmsg);
		return 0;
	}

	char name[tokenizer_variable_length];
	token type = __sym;
	
	if (type == T_VARIABLE_NUMBER) {
		tokenizer_get_variable_name(name);
		accept_token(T_VARIABLE_NUMBER);
	}

	if (type == T_VARIABLE_STRING) {
		tokenizer_get_variable_name(name);
		accept_token(T_VARIABLE_STRING);
	}

	if (prompt) {
		expression_print(&expr);
		
		if (expr.type == expression_type_string) {
			free(expr.value.string);
		}
	}

	char line[MAX_LINE];
	basic_io_readline( (prompt ? " " : "? "), line, sizeof(line), true);

	if (type == T_VARIABLE_NUMBER) {
		char* t;
		float value = strtof(line, &t);
		
		variable_set_numeric(name, value);
	}

	if (type == T_VARIABLE_STRING) {
		variable_set_string(name, line);
	}

	return 0;
}

static int do_get(basic_type* rv) {
	if (__sym != T_VARIABLE_STRING) {
		error((char *)expectstrvar_errmsg);
		return 0;
	}

	char name[tokenizer_variable_length];
	tokenizer_get_variable_name(name);

	accept_token(T_VARIABLE_STRING);

	char c[4] = "";
	if (kbhit()) {
		int ch = getchar();
		
		if ( ch == 10 ) {
		  ch = 13;
		}
		
		snprintf(c, sizeof(c), "%c", ch);
	}
	
	variable_set_string(name, c);

	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Dump

void dump_var(variable* var, void* context) {
    variable_dump(var);
}

int do_dump(basic_type* rv) {
    variables_each(dump_var, NULL);
    return 0;
}

int do_test(basic_type* rv) {
    accept_token(t_keyword_test);
	ready();
	
	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Peek & Poke

int do_poke(basic_type* rv) {
    accept_token(t_keyword_poke);

    int adr = 0;
    int value = 0;
	
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        adr = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }
	
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);
    }
    else {
        error((char *)expectcomma_errmsg);
        return 0;
    }

    expression(&expr);

    if (expr.type == expression_type_numeric) {
        value = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

	char temp[255];
	sprintf(temp, "POKE %d, %d will not be executed", adr, value);
    platform_print_console(temp);
	
	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Parser

static void parse_line(void) {
	while (__sym != T_EOF && __sym != T_COLON && __sym != T_THE_END) {
		bool ok = statement();
		
		if (! ok) {
			break;
		}
	}
}

static bool statement(void) {
	switch(__sym) {
		case T_ERROR:
			error((char *)statement_errmsg);
			break;
		default: {
			basic_function* bf = find_basic_function_by_type(__sym, basic_function_type_keyword);
			
			if (bf != NULL) {
				basic_type rv;
				basic_dispatch_function( bf, &rv);
			}
			else {
				do_let(NULL);
			}
		}
		break;
	}
	
	return last_error == NULL;
}

float evaluate(char *expression_string) {
	last_error = NULL;
	tokenizer_init(expression_string);
	get_sym();
	
	float result = numeric_expression();
	expect(T_EOF);
	
	return result;
}

void evaluate_print(char *line) {
	float result = evaluate(line);
    
    char temp[256];
	sprintf(temp, "%s = %f\n", line, result);
    platform_print_console(temp);
}

const char* evaluate_last_error(void) {
	return last_error;
}

void clear_last_error(void) {
	last_error = NULL;
}

static basic_function* find_basic_function_by_type(token sym, basic_function_type type) {
	for (size_t i=0; i<array_size(basic_functions); i++) {
		basic_function* bf = (basic_function*) array_get(basic_functions, i);
		
		if (bf->type == type && bf->token == sym) {
			return bf;
		}
	}
	
	return NULL;
}

static void get_parameter(kind k, basic_type* v) {
	v->empty = false;
	v->mallocd = false;
	
	if (k == kind_string) {
		char *s = string_expression();
		
		v->kind = kind_string;
		v->value.string = s;
	}
	else {
		float n = numeric_expression();
		
		v->kind = kind_numeric;
		v->value.number = n;
	}
}

static kind function_kind_i(basic_function *function, int i) {
	switch(i){
		case 0:
			return function->kind_1;
		case 1:
			return function->kind_2;
		case 2:
			return function->kind_3;
		case 3:
			return function->kind_4;
		case 4:
			return function->kind_5;
		default:
			return kind_numeric;
	}
}

static int basic_dispatch_function(basic_function* function, basic_type* rv) {
	if (function->nr_arguments > 5) {
		error((char *)maxarg_errmsg);
		return -1;
	}

	accept_token(__sym);

	if  (function->nr_arguments==0 && function->type == basic_function_type_keyword) {
		function->function.function_0(rv);
		return 0;
	}

	basic_type v[5];
	expect(T_LEFT_BANANA);
	int i = 0;
	
	for (;i<function->nr_arguments; i++) {
		if (__sym!=T_RIGHT_BANANA) {
			get_parameter(function_kind_i(function, i), &(v[i]));
			
			if (i<function->nr_arguments-1) {
				if (__sym != T_COMMA) {
					// Probably the rest are default parameters...
					break; // break for loop
				}
				
				expect(T_COMMA);
			}
		}
	}

	// loop further, marking empty the variables
	for (; i<function->nr_arguments; i++) {
		v[i].empty = true;
		v[i].mallocd = false;
		v[i].kind = kind_numeric;
	}
	
	expect(T_RIGHT_BANANA);

	rv->mallocd = false;
	switch (function->nr_arguments) {
		case 0:
			function->function.function_0(rv);
			break;
		case 1:
			function->function.function_1(&(v[0]), rv);
			break;
		case 2:
			function->function.function_2(&(v[0]), &(v[1]), rv);
			break;
		case 3:
			function->function.function_3(&(v[0]), &(v[1]), &(v[2]), rv);
			break;
		case 4:
			function->function.function_4(&(v[0]), &(v[1]), &(v[2]), &(v[3]), rv);
			break;
		case 5:
			function->function.function_5(&(v[0]), &(v[1]), &(v[2]), &(v[3]), &(v[4]), rv);
			break;
		default:
			return -1;
	}

	for (int i=0; i<function->nr_arguments; i++) {
		if (v[i].kind == kind_string) {
			free(v[i].value.string);
		}
	}

	return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Token handling

bool accept_token(token t) {
    if (t == __sym) {
        get_sym();
        return true;
    }

    return false;
}

bool expect(token t) {
    if (accept_token(t)) {
        return true;
    }
    
    error((char *)unexpectedsym_errmsg);
    return false;
}

static float factor(void);

static float numeric_factor(void) {
    float number = 0;
    basic_function* bf;
    
    if ((bf = find_basic_function_by_type(__sym, basic_function_type_numeric)) != NULL ){
        basic_type rv;
        basic_dispatch_function(bf, &rv);
        
        if (rv.kind != kind_numeric) {
            error((char *)expectnumfac_errmsg);
        }
        
        number = rv.value.number;
    }
    else if (__sym == T_NUMBER) {
        number = tokenizer_get_number();
        accept_token(T_NUMBER);
    }
    else if (__sym == T_VARIABLE_NUMBER) {
        char var_name[tokenizer_variable_length];
        tokenizer_get_variable_name(var_name);
        get_sym();
        
        if (__sym == T_LEFT_BANANA) {
            size_t l = strlen(var_name);
            var_name[l] = '(';
            var_name[l+1] = '\0';

            accept_token(T_LEFT_BANANA);
            size_t vector[5];
            get_vector(vector,5);
            number = variable_array_get_numeric(var_name, vector);
            expect(T_RIGHT_BANANA);
        }
        else {
            number = variable_get_numeric(var_name);
            accept_token(T_VARIABLE_NUMBER);
        }
    }
    else if (accept_token(T_LEFT_BANANA)) {
        number = numeric_expression();
        expect(T_RIGHT_BANANA);
    }
    else {
        error((char *)facsyntax_errmsg);
        get_sym();
    }

    relop op = get_relop();

    if (op != OP_NOP) {
        float right_number = factor();
        number = numeric_condition(number, right_number, op);
    }

    return number;
}

static float factor(void) {
    if (__sym == T_STRING || __sym == T_VARIABLE_STRING) {
        char* s1 = string_term();
        relop op = get_relop();
    
        if (op == OP_NOP) {
            free(s1);
            error((char *)expectrelop_errmsg);
        
            return 0;
        }
        
        char* s2 = string_term();
        float r = string_condition(s1, s2, op);
        free(s2);
        free(s1);
        
        return r;
    }
    else {
        return numeric_factor();
    }
}

static float term(void) {
	float f1 = factor();
	
	while (__sym == T_MULTIPLY || __sym == T_DIVIDE || __sym == t_op_and ) {
		token operator = __sym;
		get_sym();
		
		float f2 = factor();
		switch (operator) {
			case T_MULTIPLY:
				f1 = f1 * f2;
				break;
			case T_DIVIDE:
				f1 = f1 / f2;
				break;
			default:
				if (operator == t_op_and) {
					f1 = _and( f1, f2 );
				}
				else {
					error((char *)termsyntax_errmsg);
				}
		}
	}
	
	return f1;
}

static float numeric_expression(void) {
	token operator = T_PLUS;
	
	if (__sym == T_PLUS || __sym == T_MINUS) {
		operator = __sym;
		get_sym();
	}
	
	float t1 = term();
	if (operator == T_MINUS) {
		t1 = -1 * t1;
	}
	
	while ( __sym == T_PLUS || __sym == T_MINUS || __sym == t_op_or ) {
		operator = __sym;
		get_sym();
		
		float t2 = term();
		switch(operator) {
			case T_PLUS:
				t1 = t1 + t2;
				break;
			case T_MINUS:
				t1 = t1 - t2;
				break;
			default:
				if ( operator == t_op_or ) {
				  t1 = _or( t1, t2 );
				}
				else {
					error((char *)expsyntax_errmsg);
				}
		}
	}

	return t1;
}


// -----------------------------------------------------------------------------
#pragma mark - Expression handling

void get_sym(void) {
    __sym = tokenizer_get_next_token();
}

token current_sym(void) {
    return __sym;
}

static void set_line( uint16_t line_number ) {
    __line = line_number;
    char *cursor = lines_get_contents( __line );
    tokenizer_char_pointer( cursor );
}

void expression(expression_result *result) {
    char *string = string_expression();
    
    if (NULL != string) {
        // Got string, check for relop and apply
        relop op = get_relop();
    
        if (op == OP_NOP) {
            result->type = expression_type_string;
            result->value.string = string;
        }
        else {
            char *string_right = string_expression();
            result->type = expression_type_numeric;
            result->value.numeric = string_condition(string, string_right, op);
            free(string_right);
            free(string);
        }
    }
    else {
        result->type = expression_type_numeric;
        result->value.numeric = numeric_expression();
    }
}

static void expression_print(expression_result* expr) {
    if (expr->type == expression_type_string) {
        basic_io_print(expr->value.string);
    }
    else if (expr->type == expression_type_numeric) {
        char buffer[16];
        float value = expr->value.numeric;
        long ivalue = (int) value;
        
        if (ivalue == value) {
            snprintf(buffer, sizeof(buffer), "%ld", ivalue);
            basic_io_print(buffer);
        }
        else {
            snprintf(buffer, sizeof(buffer), "%f", value);
            basic_io_print(buffer);
        }
    }
    else {
        error((char *)unknownexp_errmsg);
    }
}

int expression_int_get(bool *flag) {
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        *flag = true;
        return expr.value.numeric;
    }
    else {
        *flag = false;
        error((char *)expectnum_errmsg);
        
        return 0;
    }
}

float expression_float_get(bool *flag) {
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        *flag = true;
        return expr.value.numeric;
    }
    else {
        *flag = false;
        error((char *)expectnum_errmsg);
        
        return 0;
    }
}

boolean accept_comma() {
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);
        return true;
    }
    else {
        error((char *)expectcomma_errmsg);
        return false;
    }
}

// -----------------------------------------------------------------------------
#pragma mark - Register tokens

static size_t basic_token_id = TOKEN_TYPE_END + 1000;

token register_token(char* token_name) {
    token_entry token;

    token.token = (unsigned int)basic_token_id++;
    token.name = token_name;

    tokenizer_register_token(&token);

    return token.token;
}

token register_function_0(basic_function_type type, char* keyword, function_0 function) {
    token t = register_token(keyword);
    basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 0,
        .function.function_0 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

token register_function_1(basic_function_type type, char* keyword, function_1 function, kind v1) {
    token t = register_token(keyword);
    basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 1,
        .kind_1 = v1,
        .function.function_1 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

token register_function_2(basic_function_type type, char* keyword, function_2 function, kind v1, kind v2) {
    token t = register_token(keyword);
    basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 2,
        .kind_1 = v1,
        .kind_2 = v2,
        .function.function_2 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

token register_function_3(basic_function_type type, char* keyword, function_3 function, kind v1, kind v2, kind v3) {
    token t = register_token(keyword);
    basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 3,
        .kind_1 = v1,
        .kind_2 = v2,
        .kind_3 = v3,
        .function.function_3 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

token register_function_4(basic_function_type type, char* keyword, function_4 function, kind v1, kind v2, kind v3, kind v4) {
    token t = register_token(keyword);
        basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 4,
        .kind_1 = v1,
        .kind_2 = v2,
        .kind_3 = v3,
        .kind_4 = v4,
        .function.function_4 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

token register_function_5(basic_function_type type, char* keyword, function_5 function, kind v1, kind v2, kind v3, kind v4, kind v5) {
    token t = register_token(keyword);
    basic_function bf = {
        .token = t,
        .type = type,
        .nr_arguments = 5,
        .kind_1 = v1,
        .kind_2 = v2,
        .kind_3 = v3,
        .kind_4 = v4,
        .kind_5 = v5,
        .function.function_5 = function
    };

    array_push(basic_functions, &bf);

    return t;
}

// -----------------------------------------------------------------------------
#pragma mark - Basic integration

void basic_init(size_t memory_size, size_t stack_size) {
    __memory = malloc(memory_size);
    
    if (!__memory) {
        error((char *)cantallocprogram_errmsg);
        return;
    }
    
    __memory_size = memory_size;
    __program_size = __memory_size;

    __stack = malloc(stack_size);
    if (!__stack) {
        error((char *)cantallocstack_errmsg);
        return;
    }
    __stack_size = stack_size;
    __stack_p = __stack_size;

    __line = 0;
    __data.state = data_state_init;

    basic_tokens = array_new(sizeof(token_entry));
    basic_functions = array_new(sizeof(basic_function));

    tokenizer_setup();

	// Base
    t_keyword_list = register_function_0(basic_function_type_keyword, "LIST", do_list);
    t_keyword_clear = register_function_0(basic_function_type_keyword, "CLEAR", do_clear);
    t_keyword_new = register_function_0(basic_function_type_keyword, "NEW", do_clear);
    t_keyword_goto = register_function_0(basic_function_type_keyword, "GOTO", do_goto);
    t_keyword_on = register_function_0(basic_function_type_keyword, "ON", do_on_goto);
    t_keyword_gosub = register_function_0(basic_function_type_keyword, "GOSUB", do_gosub);
    t_keyword_return = register_function_0(basic_function_type_keyword, "RETURN", do_return);
    t_keyword_run = register_function_0(basic_function_type_keyword, "RUN", do_run);
    t_keyword_if = register_function_0(basic_function_type_keyword, "IF", do_if);
    t_keyword_then = register_token("THEN");
    t_keyword_for = register_function_0(basic_function_type_keyword, "FOR", do_for);
    t_keyword_to = register_token("TO");
    t_keyword_step = register_token("STEP");
    t_keyword_next = register_function_0(basic_function_type_keyword, "NEXT", do_next);
    t_keyword_end = register_function_0(basic_function_type_keyword, "END", do_end);
    t_keyword_stop = register_function_0(basic_function_type_keyword, "STOP", do_end);
    t_keyword_rem = register_function_0(basic_function_type_keyword, "REM", do_rem);
    t_keyword_dim = register_function_0(basic_function_type_keyword, "DIM", do_dim);
    t_keyword_poke = register_function_0(basic_function_type_keyword, "POKE", do_poke);

	register_function_0(basic_function_type_keyword, "LET", do_let);
    t_op_or = register_token("OR");
    t_op_and = register_token("AND");

	// String
    register_function_1(basic_function_type_numeric, "LEN", str_len, kind_string);
    register_function_1(basic_function_type_string, "CHR$", str_chr, kind_numeric);
    register_function_3(basic_function_type_string, "MID$", str_mid, kind_string, kind_numeric, kind_numeric);
    register_function_2(basic_function_type_string, "LEFT$", str_left, kind_string, kind_numeric);
    register_function_2(basic_function_type_string, "RIGHT$", str_right, kind_string, kind_numeric);
    register_function_1(basic_function_type_numeric, "ASC", str_asc, kind_string);
    register_function_1(basic_function_type_numeric, "VAL", str_val, kind_string);
    register_function_1(basic_function_type_string, "STR$", str_str, kind_numeric);

	// Data
    t_keyword_data = register_function_0(basic_function_type_keyword, "DATA", do_data);
    t_keyword_read = register_function_0(basic_function_type_keyword, "READ", do_read);
    t_keyword_restore = register_function_0(basic_function_type_keyword, "RESTORE", do_restore);

	// Print
    t_keyword_print = register_function_0(basic_function_type_keyword, "PRINT", do_print);
    t_keyword_print_short = register_function_0(basic_function_type_keyword, "?", do_print);
    t_keyword_spc = register_function_1(basic_function_type_print, "SPC", do_spc, kind_numeric);
    t_keyword_tab = register_function_1(basic_function_type_print, "TAB", do_tab, kind_numeric);

	// I/O
    register_function_0(basic_function_type_keyword, "INPUT", do_input);
    register_function_0(basic_function_type_keyword, "GET", do_get);
	t_keyword_load = register_function_0(basic_function_type_keyword, "LOAD", do_load);
    t_keyword_save = register_function_0(basic_function_type_keyword, "SAVE", do_save);
    t_keyword_delete = register_function_0(basic_function_type_keyword, "DELETE", do_delete);
    t_keyword_dir = register_function_0(basic_function_type_keyword, "DIR", do_dir);

	// Misc
    register_function_0(basic_function_type_numeric, "MEM", f_mem);
    register_function_0(basic_function_type_keyword, "SLEEP", do_sleep);
    register_function_0(basic_function_type_keyword, "DUMP", do_dump);
    register_function_0(basic_function_type_keyword, "TEST", do_test);

	// Modules
	basic_math_init();
    basic_vgraphic_init();
    basic_sound_init();

	lines_init(__memory, __program_size);
    variables_init();
    __data.line = 0;
    __data.cursor = 0;
    __data.state = data_state_init;

    arch_init();
    
    textbuffer_initialize();
}

void basic_eval(char *line) {
    if (strlen(line) > 0 && (strlen(line)-1)>tokenizer_string_length) {
        error((char *)linetoolong_errmsg);
        return;
    }

    char line_string[tokenizer_string_length];
    strncpy(line_string, line, sizeof(line_string));
    _trim(line_string);
    
    if (is_empty(line_string) || is_comment(line_string)){
        return;
    }

    last_error = NULL;
    tokenizer_init( line_string );
    get_sym();
    
    if (__sym == T_NUMBER ) {
        float line_number = tokenizer_get_number();
        char* line = tokenizer_char_pointer(NULL);
        
        get_sym();
        
        if (__sym == T_EOF) {
            lines_delete( line_number );
        }
        else {
            lines_store( line_number, line);
        }
    }
    else {
        parse_line();
    }
}

void basic_show_prompt(void) {
    textbuffer_print_line((char *)readymsg);
}

void basic_reset(void) {
   // __line = 0;
	__data.line = 0;
	__data.cursor = 0;
	__data.state = data_state_init;
	__data.char_pointer = NULL;
	__cursor = NULL;
	
    platform_clear_screen();
}

void basic_run(void) {
    __REPL = true;
	basic_reset();
    basic_eval("RUN");
    platform_render_frame();
}

void basic_load(char* filename, char* path) {
    lines_clear();
    arch_load(filename, path, _load_cb, NULL);

    platform_clear_screen();
}

void basic_pause(bool flag) {
	__PAUSED = flag;
}

void basic_stop() {
	__RUNNING = false;
}

void basic_next_line_only(void) {
	__PAUSED = true;
	__EXECUTE_ONE_LINE = true;
}

bool basic_program_is_running() {
	return __RUNNING;
}

void basic_restart(void) {
	basic_stop();
	basic_reset();
	lines_clear();

	variables_destroy();
	variables_init();
	
	__FIRST = true;

    platform_clear_screen();
	
    textbuffer_print_line("");
    textbuffer_print_line((char *)readymsg);
}

void basic_destroy(void) {
    variables_destroy();
    tokenizer_free_registered_tokens();
    array_destroy(basic_tokens);
    array_destroy(basic_functions);
    
    free(__stack);
    free(__memory);
}
