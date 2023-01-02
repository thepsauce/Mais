// parses a type
// note: when this function is called, a token is already read, so no need to call an initial psreadtok

// examples of input:
// db
// [dd]
// dt Class
// dt Struct
// df(p1: db ?, p2: db ? ) dd
// dt Class<dd>
// [dt Class< dt Class<dd> >]

// this function is used by goto
// when we get a label like 'label:'
// we must check if it is a goto statement or a variable statement
// it is a variable statement if a type comes next which following function checks
bool psistype(struct parser *parser)
{
	// perfect minimal hash function: 
	// int hash = ((str[0] + str[1] - 198) % 10)
	static const char *typeIdentfs[] = {
		"db", "dw", "dd", "static", "df", "dq", "dr", "readonly", "dt"
	};
	if(parser->tok.type == TOKANY)
		return parser->tok.c == '[' || parser->tok.c == '<';
	if(parser->tok.type == TOKKEYWORD)
	{
		int hash;
		// 198 is the minimal sum that can be produced by the first two chars of a string inside typeIdentfs
		// % 10 just works here, it is a coincidence
		hash = (parser->tok.word[0] + parser->tok.word[1] - 198) % 10;
		return hash < ARRLEN(typeIdentfs) && !strcmp(typeIdentfs[hash], parser->tok.word);
	}
	return 0;
}

int parsetype(struct parser *parser, struct type *type)
{
	int err = 0;
	int depth = 0;
	// get modifiers of this type
	type->flags = 0;
	while(parser->tok.type == TOKKEYWORD)
	{
		if(!strcmp(parser->tok.word, "readonly"))
		{
			if(type->flags & TYPEREADONLY)
				psthrow(parser, "multiple 'readonly'");
			type->flags |= TYPEREADONLY;
		}
		else if(!strcmp(parser->tok.word, "static"))
		{
			if(type->flags & TYPESTATIC)
				psthrow(parser, "multiple 'static'");
			type->flags |= TYPESTATIC;
		}
		else
			break;
		psreadtok(parser);
	}
	// get depth of this type
	while(tokischar(&parser->tok, '['))
	{
		depth++;
		psreadtok(parser);
	}
	if(tokischar(&parser->tok, '<'))
	{
		// get generic name
		psreadtok(parser);
		if(parser->tok.type != TOKWORD)
		{
			err = psthrow(parser, "expected name of generic after '<'");
			type->genericName = NULL;
		}
		else
		{	
			type->flags |= TYPEGENERIC;
			type->genericName = strdup(parser->tok.word);
			psreadtok(parser);
		}
		if(!tokischar(&parser->tok, '>'))
			err = psthrow(parser, "expected '>' to close generic");
	}
	else
	{
		if(parser->tok.type != TOKKEYWORD || parser->tok.word[0] != 'd' || !parser->tok.word[1] || parser->tok.word[2])
		{
			if(depth)
				err = psthrow(parser, "expected type identifier after '['");
			else if(type->flags)
				err = psthrow(parser, "expected type identifier or '[' after type modifier");
			else
				return -1; // this is not a type
		}
		else switch(parser->tok.word[1])
		{
		case 'b': type->flags |= TYPEBYTE; break;
		case 'w': type->flags |= TYPEWORD; break;
		case 'd': type->flags |= TYPEDOUBLE; break;
		case 'q': type->flags |= TYPEQUAD; break;
		case 't': 
			type->flags |= TYPECLASS;
			psreadtok(parser);
			if(parser->tok.type != TOKWORD)
			{
				psthrow(parser, "expected class or struct name after 'dt'");
				break;
			}
			type->name = strdup(parser->tok.word);
			// check if there might be a generic behind the type name
			pspeektok(parser);
			if(!tokischar(&parser->tok, '<'))
				break;
			psskiptok(parser);
			// get generic type
			pspeektok(parser);
			if(parser->tok.type != TOKWORD)
			{
				if(tokischar(&parser->tok, '>'))
				{
					psskiptok(parser);
					err = psthrow(parser, "expected type within < >");
				}
			}
			else // we also allow the case: 'dt Gen<G>' where G is a generic type instead of: 'dt Gen< <G> >'
			{
				psskiptok(parser);
				type->generic = malloc(sizeof(*type->generic));
				type->generic->flags = TYPEGENERIC;
				type->generic->genericName = strdup(parser->tok.word);
				psreadtok(parser);
				if(!tokischar(&parser->tok, '>'))
					err = psthrow(parser, "expected '>' to close generic");
			}
			break;
		case 'r': type->flags |= TYPERESOURCE; break;
		case 'f': type->flags |= TYPEFUNC; break;
		}
	}
	type->depth = depth;
	while(depth)
	{
		depth--;
		pspeektok(parser);
		if(!tokischar(&parser->tok, ']'))
		{
			if(!err)
				err = psthrow(parser, "unclosed '['");
			break;
		}
		psskiptok(parser);
	}
	return err;
}