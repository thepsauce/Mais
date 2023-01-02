// parse a typedef

// examples:
// typedef A
// typedef [B]
// typdef C<M>
// typedef D : A
// typedef E : C<dd>
// typedef F<N> : C< <N> >
int parsetypedef(struct parser *parser)
{
	struct label label;
	int err = 0;
	memset(&label, 0, sizeof(label));
	label.pos = parser->tok.pos;
	// get name (which may be surrounded by '[]')
	psreadtok(parser);
	if(tokischar(&parser->tok, '['))
	{
		label.flags = LABELCLASS;
		// get actual name now
		psreadtok(parser);
		if(parser->tok.type != TOKWORD)
			err = psthrow(parser, "expected name of struct or class after typedef");
		else
		{
			// get closing ']'
			label.name = strdup(parser->tok.word);
			psreadtok(parser);
			if(!tokischar(&parser->tok, ']'))
				err = psthrow(parser, "expected ']' to finish class declaration");
		}
	}
	else
	{
		label.flags = LABELSTRUCT;
		if(parser->tok.type != TOKWORD)
			err = psthrow(parser, "expected name of struct or class after typedef");
		else
			label.name = strdup(parser->tok.word);
	}
	// get optional generic
	psreadtok(parser);
	if(tokischar(&parser->tok, '<'))
	{	
		// get name of generic
		psreadtok(parser);
		if(parser->tok.type != TOKWORD)
			err = psthrow(parser, "expected name of generic after '<'");
		else
		{
			// get closing '>'
			label.generic = strdup(parser->tok.word);
			psreadtok(parser);
			if(!tokischar(&parser->tok, '>'))
				err = psthrow(parser, "expected '>' to finish generic");
			psreadtok(parser); // goto next token for the next step
		}
	}
	// get optional extending class/struct
	if(tokischar(&parser->tok, ':'))
	{
		// get name of class/struct
		psreadtok(parser);
		if(parser->tok.type != TOKWORD)
			err = psthrow(parser, "expected name of class or struct after ':'");
		else
		{
			label.extends = strdup(parser->tok.word);
			// get generic that might be attached to the class
			psreadtok(parser);
			if(tokischar(&parser->tok, '<'))
			{	
				// get name of generic
				psreadtok(parser);
				if(parser->tok.type != TOKWORD)
					err = psthrow(parser, "expected name of generic after '<'");
				else
				{
					// get closing '>'
					label.extendGeneric = strdup(parser->tok.word);
					psreadtok(parser);
					if(!tokischar(&parser->tok, '>'))
						err = psthrow(parser, "expected '>' to finish generic");
					psreadtok(parser); // goto next token for the next step
				}
			}
		}
	}
	// get '{'
	if(!tokischar(&parser->tok, '{'))
	{
		psthrow(parser, "expected '{' after '%s'", label.name ? label.name : "typedef");
		if(err)
			free(label.name);
		return -1;
	}
	psaddlabel(parser, &label, 1);
	return err;
}