grammar VYPa19;

/**
 * Parser Rules
 */
program             : (definition)* EOF;
definition          : function_definition | class_definition;
function_definition : (VOID | data_type) ID '(' param_list? ')' codeblock;
data_type           : INT | STRING | ID;
param_list          : VOID | parameter (',' parameter)*;
parameter           : data_type ID;
codeblock           : '{' statement* '}';

class_definition    : CLASS ID ':' ID class_body;
class_body          : '{' (variable_definition | function_definition)* '}';
variable_definition : data_type ID (',' ID)* ';';

statement           : variable_definition | assignment | conditional | iteration | function_call | return_call;
assignment          : path '=' expr ';';
conditional         : IF '(' expr ')' codeblock ELSE codeblock;
iteration           : WHILE '(' expr ')' codeblock;
function_call       : ((path | expr) '.')? ID arguments ';';
arguments           : '(' (expr (',' expr)*)? ')';
return_call         : RETURN expr? ';';

expr                : value
                    | NEW ID
                    | '(' expr ')'
                    | '(' data_type ')' expr
                    | ID arguments | expr '.' ID arguments
                    | '!' expr
                    | expr ('*'|'/') expr
                    | expr ('+'|'-') expr
                    | expr ('<'|'<='|'>'|'>=') expr
                    | expr ('=='|'!=') expr 
                    | expr '&&' expr 
                    | expr '||' expr
                    ;

value               : INTEGER | STRING_LITERAL | path;
path                : atomic_path ('.' atomic_path)*;
atomic_path         : ID | SUPER | THIS;


/**
 * Lexer Rules
 */
fragment LETTER            : [a-zA-Z];
fragment DIGIT             : [0-9];
fragment HEX               : DIGIT | [a-fA-F];
fragment PRINTABLE_UTF8    : ~[\\\u0000-\u001F\u0022];  // ASCII value greater than 31 (apart from 34)
fragment ESCAPE_SEQUENCE   : '\\' ([nt\\"] | 'x' HEX HEX HEX HEX HEX HEX);

// Keywords
CLASS             : 'class';
ELSE              : 'else';
IF                : 'if';
INT               : 'int';
NEW               : 'new';
RETURN            : 'return';
STRING            : 'string';
SUPER             : 'super';
THIS              : 'this';
VOID              : 'void';
WHILE             : 'while'; 

ID                : (LETTER | '_') (LETTER | DIGIT | '_')*;
STRING_LITERAL    : '"' (PRINTABLE_UTF8 | ESCAPE_SEQUENCE)* '"';
INTEGER           : '0' | [1-9] DIGIT*;

BC_START          : '/*';  // block comment start
BC_END            : '*/';  // block comment end
WHITESPACE        : [ \t\r\n]+ -> skip;
LINE_COMMENT      : '//' ~[\r\n]* -> skip;
BLOCK_COMMENT     : BC_START .*? BC_END -> skip;
