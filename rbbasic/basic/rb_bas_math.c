
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "rb_bas_internal.h"
#include "rb_bas_error.h"
#include "rb_bas_math.h"

// -----------------------------------------------------------------------------
#pragma mark - Math functions

static int f_abs(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = fabs(n->value.number);
    
    return 0;
}

static int f_rnd(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
   
    if (n->value.number > 0) {
        int random = rand();
        rv->value.number = (random * 1.0) / RAND_MAX;
        return 0;
    }

    if (n->value.number < 0) {
        srand(n->value.number);
        int random = rand();
        rv->value.number = (random * 1.0) / RAND_MAX;
        return 0;
    }

    time_t now;
    struct tm *tm;
    now = time(NULL);
    tm = localtime(&now);
    rv->value.number = (tm->tm_sec * 1.0) / 60;
    
    return 0;
}

static int f_int(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    int i = (int) n->value.number;
    rv->value.number = 1.0 * i;
    
    return 0;
}

static int f_sqr(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = (float) sqrt( (double) n->value.number );
   
    return 0;
}

static int f_sgn(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    
    if (n->value.number < 0) {
        rv->value.number = -1.0;
    }
    else if (n->value.number > 0) {
        rv->value.number = 1.0;
    }
    else {
        rv->value.number = 0.0;
    }
    
    return 0;
}

static int f_not(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = (float) ( ~ (int) n->value.number );
    
    return 0;
}

static int f_sin(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = sinf(n->value.number);
    
    return 0;
}

static int f_cos(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = cosf(n->value.number);
    
    return 0;
}

static int f_tan(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = tanf(n->value.number);
    
    return 0;
}

static int f_log(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = logf(n->value.number);
    
    return 0;
}

static int f_exp(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = expf(n->value.number);
    
    return 0;
}

static int f_pow(basic_type* x, basic_type* y, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = powf(x->value.number, y->value.number);
    
    return 0;
}

static int f_atn(basic_type* n, basic_type* rv) {
    rv->kind = kind_numeric;
    rv->value.number = atanf(n->value.number);
    
    return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Transform

float deg2rad(int degree) {
	float radian = degree * M_PI/180;
	return radian;
}

// -----------------------------------------------------------------------------
#pragma mark - Transform

void rotate(int* x, int* y, int angle) {
	float theta = deg2rad(angle);
	float cs = cos(theta);
	float sn = sin(theta);

	float px = *x * cs - *y * sn;
	float py = *x * sn + *y * cs;
	
	*x = (int)px;
	*y = (int)py;
}

// -----------------------------------------------------------------------------
#pragma mark - Initialize

void basic_math_init() {
	register_function_1(basic_function_type_numeric, "ABS", f_abs, kind_numeric);
	register_function_1(basic_function_type_numeric, "SIN", f_sin, kind_numeric);
	register_function_1(basic_function_type_numeric, "COS", f_cos, kind_numeric);
	register_function_1(basic_function_type_numeric, "RND", f_rnd, kind_numeric);
	register_function_1(basic_function_type_numeric, "INT", f_int, kind_numeric);
	register_function_1(basic_function_type_numeric, "TAN", f_tan, kind_numeric);
	register_function_1(basic_function_type_numeric, "SQR", f_sqr, kind_numeric);
	register_function_1(basic_function_type_numeric, "SGN", f_sgn, kind_numeric);
	register_function_1(basic_function_type_numeric, "LOG", f_log, kind_numeric);
	register_function_1(basic_function_type_numeric, "EXP", f_exp, kind_numeric);
	register_function_2(basic_function_type_numeric, "POW", f_pow, kind_numeric, kind_numeric);
	register_function_1(basic_function_type_numeric, "ATN", f_atn, kind_numeric);
	register_function_1(basic_function_type_numeric, "NOT", f_not, kind_numeric);
}

