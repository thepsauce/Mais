void printtok(const struct token *tok);
void printlabel(struct label *label, int leadCnt);
void printtype(struct type *type);
void printdata(struct datapoint *data);
void printgroup(struct group *gr, int leadCnt);
void printlabel(struct label *label, int leadCnt);

void printtok(const struct token *tok)
{
	assert(tok != NULL && "printtok > error: token is null\n");
    printf("%ld: ", tok->pos);
    switch(tok->type)
    {
    case TOKEOF:
        printf("EOF");
        break;
    case TOKNUMBER:
        printf("NUMBER: %I64d", tok->l);
        break;
    case TOKSTRING:
        printf("STRING: \"%.*s\"", tok->strLen, tok->str);
        break;
    case TOKWORD:
        printf("WORD: %s", tok->word);
        break;
    case TOKANY:
		switch(tok->c)
		{
		case 0x100:
			printf("CHAR: '<<'");
			break;
		case 0x101:
			printf("CHAR: '>>'");
			break;
		case 0x102:
			printf("CHAR: '!!'");
			break;
		default:
			printf("CHAR: '%c'", tok->c);
		}
        break;
    case TOKKEYWORD:
        printf("KEYWORD: %s", tok->word);
        break;
    default:
        printf("CORRUPT TOKEN");
        break;
    }
}

void printtype(struct type *type)
{
    for(int d = 0; d < type->depth; d++)
		printf("[");
	if(type->flags & TYPEREADONLY)
		printf("readonly ");
    switch(type->flags & 0xFF)
    {
    case TYPEBYTE: printf("db"); break;
    case TYPEWORD: printf("dw");  break;
    case TYPEDOUBLE: printf("dd"); break;
    case TYPEQUAD: printf("dq"); break;
    case TYPEGENERIC: printf("<%s>", type->genericName); break;
    case TYPESTRUCT:
    case TYPECLASS:
        printf("dt %s", type->name);
        if(type->generic)
        {
            printf("<");
            printtype(type->generic);
            printf(">");
        }
        break;
    case TYPERESOURCE:
        printf("dr '...'");//, type->resourceLen, type->resource);
        break;
    case TYPEFUNC:
        printf("df(");
        for(int p = 0, pc = arrcount(type->params); p < pc; p++)
        {
            if(p)
                printf(", ");
            printlabel(type->params + p, 0);
        }
        printf(")");
        if(type->ret)
        {
            printf(" ");
            printtype(type->ret);
        }
        break;
    }
    for(int d = 0; d < type->depth; d++)
        printf("]");
}

void printdata(struct datapoint *data)
{
    next:
    switch(data->flags)
    {
    case DATATIMES:
        printf("%u times ", data->times);
    case DATAARRAY:
		printf("[");
        printdata(data->point);
		printf("]");
        break;
    case DATANOC:
        printf("?");
        break;
    case DATANUMBER:
        printf("0x%I64x", data->number);
        break;
    case DATASTRING:
        printf("\"%.*s\"", data->strLen, data->str);
        break;
    }
    if(data->next)
    {
        data = data->next;
        printf(", ");
        goto next;
    }
}

void printgroup(struct group *gr, int leadCnt)
{
    while(leadCnt)
    {
        printf("  ");
        leadCnt--;
    }
    switch(GRTYPE(gr))
    {
    case 0:
        printf("(null group)");
        break;
    case GRNUMBER:
        printf("%I64x", gr->num);
        break;
    case GRSTRING:
        printf("\"%.*s\"", gr->strLen, gr->str);
        break;
    case GRLABEL:
        printf("%s", gr->label);
        break;
    case GRCALL:
        printf("call %s(", gr->calling);
        for(int i = 0, cnt = arrcount(gr->args); i < cnt; i++)
        {
            if(i)
				printf(", ");
            printgroup(gr->args + i, 0);
        }
        printf(")");
        if(gr->into)
        {
            printf(" into ");
            printgroup(gr->into, 0);
        }
        break;
    case GRLOPR:
        switch(gr->opr)
        {
        case OPRNOT: printf("!!"); break;
        default:
            printf("%lc", gr->opr);
        }
        printgroup((struct group*) gr->lopr, 0);
        break;
    case GRMOPR:
        printf("(");
        printgroup(gr->lopr, 0);
        switch(gr->opr)
        {
        case OPRLSH: printf("<<"); break;
        case OPRRSH: printf(">>"); break;
        default:
            printf("%lc", gr->opr);
        }
        printgroup(gr->ropr, 0);
        printf(")");
        break;
    case GRNOC:
        printf("?");
        break;
    case GRACCESS:
        printf("[");
        printgroup(gr->accessing, 0);
        printf("]");
        break;
    case GRACCESS2:
        printgroup(gr->accessing, 0);
        printf("[");
        printgroup(gr->offset, 0);
        printf("]");
        break;
    case GRSIZEOFEXPR:
        printf("sizeof(");
        printgroup(gr->arg, 0);
        printf(")");
        break;
    case GRSIZEOFTYPE:
        printf("sizeof(");
        printtype(&gr->type);
        printf(")");
        break;
	case GRINC:
		printf("inc ");
		printgroup(gr->arg, 0);
		break;
	case GRDEC:
		printf("dec ");
		printgroup(gr->arg, 0);
		break;
	case GRMUL:
		printf("mul ");
		printgroup(gr->args, 0);
		printf(", ");
		printgroup(gr->args + 1, 0);
		break;
    }
}

void printlabel(struct label *label, int leadCnt)
{
	bool cl = 0;
    for(int lc = leadCnt; lc; lc--)
		printf("  ");

    switch(LABELTYPE(label))
    {
    case LABELGOTO:
        printf("%s: %d", label->name, label->jump);
        break;
    case LABELVAR:
		printf("%s: ", label->name);
        printtype(&label->type);
        printf(" ");
        printdata(&label->data);
        break;
    case LABELFUNC:
        printf("%s(", label->name);
        for(int p = 0, pc = arrcount(label->params); p < pc; p++)
        {
            if(p)
                printf(", ");
            printlabel(label->params + p, 0);
        }
        printf(") ");
        if(label->ret)
            printtype(label->ret);
		else
			printf("?");
		printf("{");
		cl = 1;
		break;
	case LABELSTRUCT:
		printf("struct %s {", label->name);
		cl = 1;
		break;
	case LABELCLASS:
		printf("class %s", label->name);
		if(label->generic)
			printf("<%s>", label->generic);
		if(label->extends)
			printf(" : %s", label->extends);
		if(label->extendGeneric)
			printf("<%s>", label->extendGeneric);
		printf(" {");
		cl = 1;
		break;
    }
	for(int l = 0, lc = arrcount(label->children); l < lc; l++)
	{
		printf("\n");
		printlabel(label->children + l, leadCnt + 1);
	}
	for(int g = 0, gc = arrcount(label->groups); g < gc; g++)
	{
		printf("\n");
		printgroup(label->groups + g, leadCnt + 1);
	}
	if(cl)
	{
		printf("\n");
		for(int lc = leadCnt; lc; lc--)
			printf("  ");
		printf("}");
	}
}