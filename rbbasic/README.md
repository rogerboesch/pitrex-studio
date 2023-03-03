
# VEC X3 Basic

## List of keywords

* ABS
* ASC
* AND
* ATN
* CHR$
* CLEAR (NEW)
* CLS
* COS
* DATA
* DIR (FILES)
* DELETE
* DIM variable "(" expression ")"
* DUMP
* END
* EXP
* FILES (DIR)
* LET variable = expression
* LEN
* FOR numeric\_variable '=' numeric\_expression TO numeric_expression [ STEP number ] 
* GET
* GOSUB expression
* GOTO expression
* GRAPHICS mode (does nothing yet)
* IF relation-expression THEN statement
* INPUT variable-list
* INT
* LEN
* LEFT$
* LET
* LIST
* LOAD
* LOG
* MEM
* MID$
* NEXT
* NEW (CLEAR)
* NOT
* ON
* OR
* POKE  adr,value (Does nothing yet)
* POSITION x,y
* POW
* PRINT expression-list [ : ]
* READ variable "(" expression ")"
* RESTORE: Restore data pointer to zero
* RETURN
* REM
* RIGHT$
* RND
* RUN
* SAVE
* SETCOLOR  mode,color,brightness
* SGN
* SIN
* SLEEP
* SOUND voice,pitch,distortion,volume,[duration in s] (voice 0-3, distortion is applied to wave curve 1-5)
* SPC
* STOP
* STR$
* SQR
* TAB
* TAN
* VAL
* VBEGIN
* VDOT
* VDRAW variable
* VDDRAW variable
* VGRAPHICS use-pixel, use-wireframe
* VLINE
* VLIST
* VMOVE
* VMOVETO
* VPRINAT x,y,string
* VREAD variable "(" number ")"
* VROTATE variable, angle
* VSYNC
* VSETDOTMODE mode
* ? (PRINT)


# Syntax

  line = [number] statement [ : statement ] CR

  statement =
    PRINT expression-list [ ; ]
    | IF relation-expression THEN statement
    | GOTO expression
    | INPUT variable-list
    | LET variable = expression
    | GOSUB expression
    | RETURN
    | FOR numeric_variable '=' numeric_expression TO numeric_expression [ STEP number ] 
    | CLEAR | NEW
    | LIST
    | RUN
    | END
    | DIM variable "(" expression ")"
    | SAVE literal_string
    | LOAD literal_string
    | DELETE literal_string
    | DIR
  
  expression-list = ( string | expression ) [, expression-list]

  variable-list = variable [, variable-list]

  expression = string_expression | numeric_expression
  
  numeric_expression = ["+"|"-"] term {("+"|"-"|"OR") term} .

  term = factor {( "*" | "/" | "AND" ) factor} .

  factor = 
    func "(" expression ")" 
    | number
    | "(" expression ")"
    | variable
    | relation-expression

  relation-expression =
    expression relation-operator expression

  func =
    ABS
    | AND
    | ATN
    | COS
    | EXP
    | INT
    | LOG
    | NOT
    | MEM
    | OR
    | RND
    | SGN
    | SIN
    | SQR
    | TAN

  string = literal_string | string_func "(" string_expression ")"

  literal_string = '"' ... '"'
  
  string_func =
    CHR$
    ...

  string_expression = literal_string | string_variable

  variable = ( numeric_variable | string_variable | indexed_variable )

  numeric_variable = A | B | C ... | X | Y | Z

  string_variable = A$ | B$ | C$ ... | X$ | Y$ | Z$

  indexed_variable = ( numeric_variable | string_variable ) "(" expression ")"

  relation-operator = ( "<" | "<=" | "=" | ">=" | ">" )


# Extend

It's easy to register a new BASIC function, as shown here with a `sleep` function:

```C
register_function_1(basic_function_type_keyword, "SLEEP", do_sleep, kind_numeric);
...
int do_sleep(basic_type* delay, basic_type* rv)
{
  delay_ms(delay->value.number);
  
  rv->kind = kind_numeric;
  rv->value.number = 0;

  return 0;
}
```

Use the new keyword:

```REALbasic
10 INPUT "YOUR NAME?", NAME$
20 CLS
30 FOR I=1 TO LEN(NAME$)
40 PRINT MID$(NAME$,I,1); 
50 SLEEP(500)
60 NEXT
70 PRINT
```

Embed the interpreter into application:

```C
  basic_init(2048, 512); // memory size, stack size
  basic_eval("10 PRINT \"HELLO\"");
  basic_eval("RUN"); 
  basic_destroy();  
```
