/*****
 * Název projektu:
 *	Implementace interpretu imperativního jazyka IFJ16
 *
 * Zadání:
 * 	https://wis.fit.vutbr.cz/FIT/st/course-files-st.php/course/IFJ-IT/projects/ifj2016.pdf
 *
 * Tým:
 * 	022, varianta a/3/II
 *
 * Členové:
 *	Vít Ambrož		(xambro15)
 *	Tomáš Aubrecht	(xaubre02)
 *	David Bednařík	(xbedna62)
 *	Andrej Čulen		(xculen02)
 *	
 * Odkaz na repozitář:
 * 	https://github.com/xbedna62/IFJ16
 *
 * Modul:
 * 	Lexikalni analyzátor (Scanner)
 *
 * Autor: 
 * 	Tomáš Aubrecht (xaubre02)
 *
 *****/


#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

FILE* source = NULL;   //extern proměnné
int line = 1;

//################################################################

// funkce pro otevření souboru
void openFile(const char *filename)
{
	source = fopen(filename, "r");
    if(source == NULL)
    {
        Err = ERR_OTHER;
    }
}
// funkce pro uzavření souboru
void closeFile(void)
{
	if(fclose(source) == EOF)
    {
		Err = ERR_OTHER;
	}
}
// funkce pro prevod oktaloveho cisla na decimalni
int convert(int octalNumber)
{
    int decimal = 0, i = 0;

    while(octalNumber != 0)
    {
        decimal += (octalNumber % 10) * pow(8, i++);
        octalNumber /= 10;
    }

    return decimal;
}
// funkce pro inicializaci nového tokenu
void initToken(Token *token)
{
	token->type = tt_empty;
	token->keyword = key_none;
	token->line = 1;
	token->lenght = 0;
	token->cap = 0;
	token->string = NULL;
}
// funkce pro vyčištění tokenu
void clearToken(Token *token)
{
	if(token->string != NULL)
	{
		free(token->string);
	}
	token->type = tt_empty;
	token->keyword = key_none;
	token->lenght = 0;
	token->cap = 0;
	token->string = NULL;
}

// pomocná funkce na zkopírování obsahu tokenu do druhého tokenu
void copyToken(Token *dst, Token *src)
{
	dst->type = src->type;
	dst->keyword = src->keyword;
	dst->line = src->line;
	dst->lenght = src->lenght;
	dst->cap = src->cap;
	if(src->string != NULL)
	{
		if( (dst->string = (char *) malloc((strlen(src->string) + 1) * sizeof(char)) ) == NULL )
		{
			Err = ERR_OTHER;
			return;
		}
		strcpy(dst->string, src->string);
	}
}

// funkce pro přidávání znaků do řetězce
void addToToken(Token *token, int c)
{
	if(token->string == NULL) // zatím nealokovaná paměť
	{
		token->cap = 32; // nastavení si paměti na 32 bitů
		token->string = (char *) malloc(token->cap * sizeof(char));
		if(token->string == NULL)
		{
			Err = ERR_OTHER;
		}
		token->string[token->lenght++] = c; // připojení znaku do řetězce
	}
	else if(token->cap > token->lenght) // je zde místo pro další znak
	{
		token->string[token->lenght++] = c;
	}
	else // není místo pro další znak
	{	
		token->cap*=2; // zdvojnásobení kapacity
		token->string = realloc(token->string, (token->cap * sizeof(char)));
		token->string[token->lenght++] = c;
	}
}
// funkce na naplnění tokenu daty
void processToken(Token *token)
{
	int c; // aktuálně načtený znak
	int p = 0; // pozice znaku v řetězci
	char octal[]="000";
	int octalNumber;

	ScannerState state = ss_empty; // nastavení počátečního stavu automatu

	while(1)
	{
		c = fgetc(source);

		switch(state)
		{
			case ss_empty:
			{
				if( (c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					(c == '_') ||
					(c == '$'))
				{
					state = ss_identifier;
					token->line = line; 
					token->type = tt_identifier;
					addToToken(token, c);
					break;
				}
				else if((c >= '0') && (c <= '9'))
				{	
					token->line = line;
					token->type = tt_int;
					state = ss_number;
					addToToken(token, c);
					break;
				}

				else
				{
					switch(c)
					{
						case EOF:
						{	
							token->line = line;
							token->type = tt_EOF;
							return;
						}
						case ' ':
						{
							break;
						}
						case '\n':
						{
							line++;
							break;
						}
						case '\t':
						{
							break;
						}
						/*case '.':
						{
							token->line = line;
							token->type = tt_dot;
							return;
						}*/
						case ',':
						{
							token->line = line;
							token->type = tt_comma;
							return;
						}
						case ';':
						{
							token->line = line;
							token->type = tt_semicolon;
							return;
						}
						case '(':
						{
							token->line = line;
							token->type = tt_bracket_left;
							return;
						}
						case ')':
						{
							token->line = line;
							token->type = tt_bracket_right;
							return;
						}
						case '{':
						{
							token->line = line;
							token->type = tt_curly_left;
							return;
						}
						case '}':
						{
							token->line = line;
							token->type = tt_curly_right;
							return;
						}
						case '*':
						{
							token->line = line;
							token->type = tt_mul;
							return;
						}
						case '/':
						{
							state = ss_divide;
							break;
						}
						case '+':
						{
							token->line = line;
							token->type = tt_plus;
							return;
						}
						case '-':
						{
							token->line = line;
							token->type = tt_minus;
							return;
						}
						case '<':
						{
							token->line = line;
							state = ss_less;
							break;
						}
						case '>':
						{
							token->line = line;
							state = ss_greater;
							break;
						}
						case '=':
						{
							token->line = line;
							state = ss_equal;
							break;
						}
						case '!':
						{
							token->line = line;
							state = ss_not;
							break;
						}
						case '"':
						{
							token->line = line;
							token->type = tt_literal;
							state = ss_literal;
							break;
						}
						default:
						{
							token->line = line;
							token->type = tt_invalid;	// neplatný znak, není součástí jazyka.
							return;
						}
					}
					break;
				}
			}
			case ss_identifier:
			{	
				if( (c >= '0' && c <= '9') ||
					(c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					(c == '_') ||
					(c == '$'))
				{	
					addToToken(token, c);
					break;
				}
				else if(c == '.')
				{
					addToToken(token, '\0');
					checkString(token, p);
					if(token->type != tt_identifier)
					{
						token->type = tt_invalid;
						return;
					}
					else
					{
						state = ss_full_identifier;
						token->lenght--;
						addToToken(token, c);
						p = token->lenght;
						break;
					}
				}
				else if((c == ' ') ||
						(c == '\n')||
						(c == '\t')||
						(c == ',') ||
						(c == ';') ||
						(c == '(') ||
						(c == ')') ||
						(c == '*') ||
						(c == '/') ||
						(c == '+') ||
						(c == '-') ||
						(c == '>') ||
						(c == '<') ||
						(c == '=') ||
						(c == '!'))
				{
					addToToken(token, '\0');
					checkString(token, p);
					ungetc(c, source);
					return;
				}
				else
				{
				    token->type = tt_invalid;
				    return;
				}
			}
			case ss_full_identifier:
			{
				if( (c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					(c == '_') ||
					(c == '$'))
				{	
					state = ss_full_id_second;
					token->type = tt_full_id;
					addToToken(token, c);
					break;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_full_id_second:
			{
				if( (c >= '0' && c <= '9') ||
					(c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') || 
					(c == '_') ||
					(c == '$'))
				{	
					addToToken(token, c);
					break;
				}
				else if((c == ' ') ||
						(c == '\n')||
						(c == '\t')||
						(c == ',') ||
						(c == ';') ||
						(c == '(') ||
						(c == ')') ||
						(c == '*') ||
						(c == '/') ||
						(c == '+') ||
						(c == '-') ||
						(c == '>') ||
						(c == '<') ||
						(c == '=') ||
						(c == '!'))
				{
					addToToken(token, '\0');
					checkString(token, p);
					if(token->type != tt_full_id)
					{
						token->type = tt_invalid;
						return;
					}
					else
					{
						ungetc(c, source);
						return;
					}
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_number:
			{
				if((c >= '0') && (c <= '9'))
				{
					addToToken(token, c);
					break;
				}
				else if(c == '.')
				{
					
					state = ss_decimal;
					token->type = tt_double;
					addToToken(token, c);
					break;
				}
				else if((c == 'e') || (c == 'E'))
				{	
					state = ss_exponent;
					token->type = tt_double;
					addToToken(token, c);
					break;
				}
				else if((c == ' ') ||
						(c == '\n')||
						(c == '\t')||
						(c == ',') ||
						(c == ';') ||
						(c == ')') ||
						(c == '*') ||
						(c == '/') ||
						(c == '+') ||
						(c == '-') ||
						(c == '>') ||
						(c == '<') ||
						(c == '=') ||
						(c == '!'))
				{	
					addToToken(token, '\0');
					ungetc(c, source);
					return;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_decimal:
			{
				if((c >= '0') && (c <= '9'))
				{
					state = ss_decimal_part;
					addToToken(token, c);
					break;
				}
				else
				{	
					token->type = tt_invalid;
					return;
				}
			}
			case ss_decimal_part:
			{
				if((c >= '0') && (c <= '9'))
				{
					addToToken(token, c);
					break;
				}
				else if((c == 'e') || (c == 'E'))
				{	
					state = ss_exponent;
					addToToken(token, c);
					break;
				}
				else if((c == ' ') ||
						(c == '\n')||
						(c == '\t')||
						(c == ',') ||
						(c == ';') ||
						(c == ')') ||
						(c == '*') ||
						(c == '/') ||
						(c == '+') ||
						(c == '-') ||
						(c == '>') ||
						(c == '<') ||
						(c == '=') ||
						(c == '!'))
				{	
					addToToken(token, '\0');
					ungetc(c, source);
					return;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_exponent:
			{
				if((c >= '0') && (c <= '9'))
				{
					state = ss_exponent_part;
					addToToken(token, c);
					break;
				}
				else if((c == '+') || (c == '-'))
				{
					state = ss_exponent_sign;
					addToToken(token, c);
					break;
				}
				else
				{	
					token->type = tt_invalid;
					return;
				}
			}
			case ss_exponent_sign:
			{
				if((c >= '0') && (c <= '9'))
				{
					state = ss_exponent_part;
					addToToken(token, c);
					break;
				}
				else
				{	
					token->type = tt_invalid;
					return;
				}
			}
			case ss_exponent_part:
			{
				if((c >= '0') && (c <= '9'))
				{
					addToToken(token, c);
					break;
				}
				else if((c == ' ') ||
						(c == '\n')||
						(c == '\t')||
						(c == ',') ||
						(c == ';') ||
						(c == ')') ||
						(c == '*') ||
						(c == '/') ||
						(c == '+') ||
						(c == '-') ||
						(c == '>') ||
						(c == '<') ||
						(c == '=') ||
						(c == '!'))
				{	
					addToToken(token, '\0');
					ungetc(c, source);
					return;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_divide:
			{
				if(c == '/')
				{
					state = ss_line_comment;
					break;
				}
				else if(c == '*')
				{
					state = ss_block_comment;
					break;
				}
				else
				{
					ungetc(c, source);
					token->line = line;
					token->type = tt_div;
					return;
				}
			}
			case ss_line_comment:
			{
				if(c == '\n')
				{
					line++;
					state = ss_empty;
				}
				else if(c == EOF)
				{
					token->type = tt_EOF;
					return;
				}
				break;
			}
			case ss_block_comment:
			{
				if(c == '*')
				{
					state = ss_comment_end;
					break;
				}
				else if(c == '/')
				{
					state = ss_comment_error;
					break;
				}
				else if(c == '\n')
				{
					line++;
					break;
				}
				else if(c == EOF)
				{
					token->type = tt_invalid;
					return;
				}
				else
				{
					break;
				}
			}
			case ss_comment_end:
			{
				if(c == '/')
					{
						state = ss_empty;
						break;
					}
				else
				{
					state = ss_block_comment;
					ungetc(c, source);
					break;
				}
			}
			case ss_comment_error:
			{
				if(c == '*')
				{
					token->type = tt_invalid;
					return;
				}
				else
				{
					state = ss_block_comment;
					ungetc(c, source);
					break;
				}	
			}
			case ss_less:
			{
				if(c == '=')
				{
					token->type = tt_lequal;
					return;
				}
				else
				{
					token->type = tt_less;
					ungetc(c, source);
					return;
				}
			}
			case ss_greater:
			{
				if(c == '=')
				{
					token->type = tt_gequal;
					return;
				}
				else
				{
					token->type = tt_greater;
					ungetc(c, source);
					return;
				}
			}
			case ss_equal:
			{
				if(c == '=')
				{
					token->type = tt_same;
					return;
				}
				else
				{
					token->type = tt_equal;
					ungetc(c, source);
					return;
				}
			}
			case ss_not:
			{
				if(c == '=')
				{
					token->type = tt_not_equal;
					return;
				}
				else
				{
					token->type = tt_invalid;
					ungetc(c, source);
					return;
				}
			}
			case ss_literal:
			{
				if(c == '"') // konec řetězcového literálu
				{
					addToToken(token, '\0'); // přiřazení ukončovacího znaku na konec řetězce
					return;
				}
				else if(c == '\\')
				{
					state = ss_sequence;
					break;
				}
				else if(c == '\n')
				{
					token->type = tt_invalid;
					return;
				}
				else if(c == EOF)
				{
					token->type = tt_invalid;
					return;
				}
				
				else
				{
					addToToken(token, c);
					break;
				}
			}
			case ss_sequence:
			{
				if(c == '"')
				{
					addToToken(token, '"');
					state = ss_literal;
					break;
				}
				else if(c == '\\')
				{
					addToToken(token, '\\');
					state = ss_literal;
					break;
				}
				else if(c == 'n')
				{
					addToToken(token, '\n');
					state = ss_literal;
					break;
				}
				else if(c == 't')
				{
					addToToken(token, '\t');
					state = ss_literal;
					break;
				}
				else if((c >= '0') && (c <= '3'))
				{
					octal[0] = c;
					state = ss_octal;
					break;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_octal:
			{
				if((c >= '0') && (c <= '7'))
				{
					octal[1] = c;
					state = ss_octal_last;
					break;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
			case ss_octal_last:
			{
				if((c >= '0') && (c <= '7'))
				{
					octal[2] = c;
					octalNumber = atoi(octal);
					if((octalNumber > 377) || (octalNumber < 1))
					{
						token->type = tt_invalid;
						break;
					}
					addToToken(token, convert(octalNumber));
					state = ss_literal;
					break;
				}
				else
				{
					token->type = tt_invalid;
					return;
				}
			}
		}
	}
}
// funkce pro kontrolu stringu, zdali není klíčové slovo nebo datový typ
void checkString(Token *token, int p)
{
	switch(token->string[p])	//začáteční písmeno řetězce
	{
		case 'b':
		{
			if(!strcmp(&token->string[p], "boolean"))
			{	
				token->type = tt_keyword;
				token->keyword = key_boolean;
			}
			else if(!strcmp(&token->string[p], "break"))
			{
				token->type = tt_keyword;
				token->keyword = key_break;
			}
			break;
		}
		case 'c':
		{
			if(!strcmp(&token->string[p], "class"))
			{	
				token->type = tt_keyword;
				token->keyword = key_class;
			}
			else if(!strcmp(&token->string[p], "continue"))
			{
				token->type = tt_keyword;
				token->keyword = key_continue;
			}
			break;
		}
		case 'd':
		{
			if(!strcmp(&token->string[p], "do"))
			{	
				token->type = tt_keyword;
				token->keyword = key_do;
			}
			else if(!strcmp(&token->string[p], "double"))
			{
				token->type = tt_data_type;
				token->keyword = key_double;
			}
			break;
		}
		case 'e':
		{
			if(!strcmp(&token->string[p], "else"))
			{	
				token->type = tt_keyword;
				token->keyword = key_else;
			}
			break;
		}
		case 'f':
		{
			if(!strcmp(&token->string[p], "false"))
			{	
				token->type = tt_keyword;
				token->keyword = key_false;
			}
			else if(!strcmp(&token->string[p], "for"))
			{
				token->type = tt_keyword;
				token->keyword = key_for;
			}
			break;
		}
		case 'i':
		{
			if(!strcmp(&token->string[p], "if"))
			{	
				token->type = tt_keyword;
				token->keyword = key_if;
			}
			else if(!strcmp(&token->string[p], "int"))
			{
				token->type = tt_data_type;
				token->keyword = key_int;
			}
			break;
		}
		case 'r':
		{
			if(!strcmp(&token->string[p], "return"))
			{	
				token->type = tt_keyword;
				token->keyword = key_return;
			}
			break;
		}
		case 'S':
		{
			if(!strcmp(&token->string[p], "String"))
			{	
				token->type = tt_data_type;
				token->keyword = key_String;
			}
			break;
		}
		case 's':
		{
			if(!strcmp(&token->string[p], "static"))
			{	
				token->type = tt_keyword;
				token->keyword = key_static;
			}
			break;
		}
		case 't':
		{
			if(!strcmp(&token->string[p], "true"))
			{	
				token->type = tt_keyword;
				token->keyword = key_true;
			}
			break;
		}
		case 'v':
		{
			if(!strcmp(&token->string[p], "void"))
			{	
				token->type = tt_keyword;
				token->keyword = key_void;
			}
			break;
		}
		case 'w':
		{
			if(!strcmp(&token->string[p], "while"))
			{	
				token->type = tt_keyword;
				token->keyword = key_while;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}
