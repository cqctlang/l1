/*
this file is a modification of jsmn.
its copyright and MIT license follow.

Copyright (c) 2010 Serge A. Zaitsev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "sys.h"
#include "util.h"
#include "syscqct.h"


/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_PRIMITIVE = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3
} jsmntype_t;

typedef enum {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3,
	/* Everything was fine */
	JSMN_SUCCESS = 0
} jsmnerr_t;

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct {
	jsmntype_t type;
	int start;
	int end;
	int size;
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int pos; /* offset in the JSON string */
	int toknext; /* next token to allocate */
	int toksuper; /* suporior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
static void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
static jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, 
		jsmntok_t *tokens, unsigned int num_tokens);


/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, 
		jsmntok_t *tokens, size_t num_tokens) {
	unsigned int i;
	for (i = parser->toknext; i < num_tokens; i++) {
		if (tokens[i].start == -1 && tokens[i].end == -1) {
			parser->toknext = i + 1;
			return &tokens[i];
		}
	}
	return NULL;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type, 
                            int start, int end) {
	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
		jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;
	int start;

	start = parser->pos;

	for (; js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
#ifndef JSMN_STRICT
			/* In strict mode primitive must be followed by "," or "}" or "]" */
			case '\t' : case '\r' : case '\n' : case ' ' : case ':': 
#endif
			case ','  : case ']'  : case '}' :
				goto found;
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
#ifdef JSMN_STRICT
	/* In strict mode primitive must be followed by a comma/object/array */
	parser->pos = start;
	return JSMN_ERROR_PART;
#endif

found:
	token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == NULL) {
		parser->pos = start;
		return JSMN_ERROR_NOMEM;
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
	parser->pos--;
	return JSMN_SUCCESS;
}

/**
 * Filsl next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
		jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == NULL) {
				parser->pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos);
			return JSMN_SUCCESS;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\') {
			parser->pos++;
			switch (js[parser->pos]) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					/* TODO */
					break;
				/* Unexpected symbol */
				default:
					parser->pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
static jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, jsmntok_t *tokens, 
		unsigned int num_tokens) {
	int r;
	int i;
	jsmntok_t *token;

	/* initialize the rest of tokens (they could be reallocated) */
	for (i = parser->toknext; i < num_tokens; i++) {
		jsmn_fill_token(&tokens[i], JSMN_PRIMITIVE, -1, -1);
	}

	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->pos];
		switch (c) {
			case '{': case '[':
				token = jsmn_alloc_token(parser, tokens, num_tokens);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				if (parser->toksuper != -1)
					tokens[parser->toksuper].size++;
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;
				parser->toksuper = parser->toknext - 1;
				break;
			case '}': case ']':
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
				for (i = parser->toknext - 1; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						parser->toksuper = -1;
						token->end = parser->pos + 1;
						break;
					}
				}
				/* Error if unmatched closing bracket */
				if (i == -1) return JSMN_ERROR_INVAL;
				for (; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						parser->toksuper = i;
						break;
					}
				}
				break;
			case '\"':
				r = jsmn_parse_string(parser, js, tokens, num_tokens);
				if (r < 0) return r;
				if (parser->toksuper != -1)
					tokens[parser->toksuper].size++;
				break;
			case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
				break;
#ifdef JSMN_STRICT
			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
#else
			/* In non-strict mode every unquoted value is a primitive */
			default:
#endif
				r = jsmn_parse_primitive(parser, js, tokens, num_tokens);
				if (r < 0) return r;
				if (parser->toksuper != -1)
					tokens[parser->toksuper].size++;
				break;

#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
				return JSMN_ERROR_INVAL;
#endif

		}
	}

	for (i = parser->toknext - 1; i >= 0; i--) {
		/* Unmatched opened object or array */
		if (tokens[i].start != -1 && tokens[i].end == -1) {
			return JSMN_ERROR_PART;
		}
	}

	return JSMN_SUCCESS;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens 
 * available.
 */
static void jsmn_init(jsmn_parser *parser) {
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}

static Val
convert(VM *vm, char *s, jsmntok_t *t, u32 *ndx)
{
	Val rv, k, v;
	u32 ti, i, isneg;
	jsmntok_t *tp;
	Imm n;
	Tab *tab;
	List *l;

	rv = 0;
	ti = *ndx;
	tp = &t[ti++];
	switch(tp->type){
	case JSMN_OBJECT:
		tab = mktab();
		if(tp->size%2)
			bug();
		for(i = 0; i < tp->size/2; i++){
			k = convert(vm, s, t, &ti);
			v = convert(vm, s, t, &ti);
			tabput(tab, k, v);
		}
		rv = mkvaltab(tab);
		break;
	case JSMN_ARRAY:
		l = mklist();
		for(i = 0; i < tp->size; i++)
			_listappend(l, convert(vm, s, t, &ti));
		rv = mkvallist(l);
		break;
	case JSMN_PRIMITIVE:
		i = tp->start;
		isneg = 0;
		switch(s[i]){
		case 't':
			rv = mkvalcval2(cval1);
			break;
		case 'f':
			rv = mkvalcval2(cval0);
			break;
		case 'n':
			rv = Xnil;
			break;
		case '-':
			isneg = 1;
			i++;
			if(i >= tp->end)
				bug();
			/* fall through */
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			n = s[i++]-'0';
			while(i < tp->end){
				n *= 10;
				n += s[i++]-'0';
			}
			if(isneg)
				rv = mkvallitcval(Vvlong, -n);
			else
				rv = mkvallitcval(Vuvlong, n);
			break;
		default:
			vmerr(vm, "invalid json primitive: %.*s",
			      tp->end-tp->start, s+tp->start);
			return 0;
		}
		break;
	case JSMN_STRING:
		rv = mkvalstr(mkstr(s+tp->start, tp->end-tp->start));
		break;
	default:
		bug();
	}
	
	*ndx = ti;
	return rv;
}

static void
l1_json2val(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s;
	int r;
	u32 ndx, mtok;
	jsmn_parser jsmn;
	jsmntok_t *tok;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to json2val");
	checkarg(vm, argv, 0, Qstr);
	s = str2cstr(valstr(argv[0]));
	if(strlen(s) == 0){
		efree(s);
		vmerr(vm, "no json input"); 
	}
	mtok = 1024;

	jsmn_init(&jsmn);
retry:
	tok = emalloc(mtok*sizeof(jsmntok_t));
	r = jsmn_parse(&jsmn, s, tok, mtok);

	switch(r){
	case JSMN_SUCCESS:
		break;
	case JSMN_ERROR_NOMEM:
		tok = erealloc(tok, mtok*sizeof(jsmntok_t),
			       2*mtok*sizeof(jsmntok_t));
		mtok *= 2;
		goto retry;
	case JSMN_ERROR_INVAL:
		efree(tok);
		efree(s);
		vmerr(vm, "invalid json input");
	case JSMN_ERROR_PART:
		efree(tok);
		efree(s);
		vmerr(vm, "incomplete json input");
	default:
		efree(tok);
		efree(s);
		vmerr(vm, "json parser returned undefined status");
	}
	ndx = 0;

	*rv = convert(vm, s, tok, &ndx);
	efree(tok);
	efree(s);
}

void
fnjson(Env *env)
{
	FN(json2val);
}
