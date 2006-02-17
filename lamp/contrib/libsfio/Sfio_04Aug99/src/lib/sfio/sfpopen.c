#include	"sfhdr.h"

#if _PACKAGE_ast
#include	<proc.h>
#else

#include	<signal.h>
typedef void(*Handler_t)_ARG_((int));
#define EXIT_NOTFOUND	127

#define READ		0
#define WRITE		1

#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif
static char	Meta[1<<CHAR_BIT], **Path;

/* execute command directly if possible; else use the shell */
#if __STD_C
static void execute(const char* argcmd)
#else
static void execute(argcmd)
char*	argcmd;
#endif
{
	reg char	*s, *cmd, **argv, **p, *interp;
	reg int		n;

	/* define interpreter */
	if((interp = getenv("SHELL")) &&
	   strcmp(interp,"/bin/sh") != 0 && strcmp(interp,"/bin/ksh") != 0 &&
	   access(interp,X_OK) == 0)
		goto do_interp;
	if(strcmp(interp,"/bin/sh") != 0 && strcmp(interp,"/bin/ksh") != 0)
		interp = "/bin/sh";

	/* if there is a meta character, let the shell do it */
	for(s = (char*)argcmd; *s; ++s)
		if(Meta[(uchar)s[0]])
			goto do_interp;

	/* try to construct argv */
	if(!(cmd = (char*)malloc(strlen(argcmd)+1)) )
		goto do_interp;
	strcpy(cmd,argcmd);
	if(!(argv = (char**)malloc(16*sizeof(char*))) )
		goto do_interp;
	for(n = 0, s = cmd;; )
	{	while(isspace(s[0]))
			++s;
		if(!s[0])
			break;
		argv[n] = s;
		while(s[0] && !isspace(s[0]))
			++s;
		if(!s[0])
			break;
		s += 1;
		n += 1;
		if((n%16) == 0 && !(argv = (char**)realloc(argv,(n+16)*sizeof(char*))) )
			goto do_interp;
	}
	if(n == 0)
		goto do_interp;
	else	argv[n] = NIL(char*);

	/* get the command name */
	cmd = argv[0];
	for(s = cmd+strlen(cmd)-1; s >= cmd; --s)
		if(*s == '/')
			break;
	argv[0] = s+1;

	/* could be a non-standard pathname like 3dfs, let the shell worry about it */
	for(s = cmd; *s; ++s)
		if(s[0] == '.' && s[1] == '.' && s[2] == '.')
			goto do_interp;

	if(cmd[0] == '/' ||
	   (cmd[0] == '.' && cmd[1] == '/') ||
	   (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/') )
	{	if(access(cmd,X_OK) != 0)
			goto do_interp;
		else	execv(cmd,argv);
	}
	else for(p = Path; *p; ++p)
	{	s = sfprints("%s/%s",p,cmd);
		if(access(s,X_OK) == 0)
			execv(s,argv);
	}

do_interp: 
	for(s = interp+strlen(interp)-1; s >= interp; --s)
		if(*s == '/')
			break;
	execl(interp, s, "-c", argcmd, NIL(char*));
	_exit(EXIT_NOTFOUND);
}

#endif /*_PACKAGE_ast*/

#ifndef macintosh
#if __STD_C
Sfio_t*	sfpopen(Sfio_t* f, const char* command, const char* mode)
#else
Sfio_t*	sfpopen(f,command,mode)
Sfio_t*	f;
char*	command;	/* command to execute */
char*	mode;		/* mode of the stream */
#endif
{
#if _PACKAGE_ast
	reg Proc_t*	proc;
	reg int		sflags;
	reg long	flags = PROC_IGNORE;
	char*		av[4];

	if (!command || !command[0] || !(sflags = _sftype(mode, NiL)))
		return 0;
	if (sflags & SF_READ)
		flags |= PROC_READ;
	if (sflags & SF_WRITE)
		flags |= PROC_WRITE;
	av[0] = "sh";
	av[1] = "-c";
	av[2] = (char*)command;
	av[3] = 0;
	if (!(proc = procopen(0, av, 0, 0, flags)))
		return 0;
	if (!(f = sfnew(f, NIL(Void_t*), (size_t)SF_UNBOUND,
	       		(sflags&SF_READ) ? proc->rfd : proc->wfd, sflags)) ||
	    _sfpopen(f, (sflags&SF_READ) ? proc->wfd : -1, proc->pid) < 0)
	{
		if (f) sfclose(f);
		procclose(proc);
		return 0;
	}
	procfree(proc);
	return f;
#else
	reg int		pid, pkeep, ckeep, sflags;
	int		parent[2], child[2];
	Sfio_t		sf;
	Handler_t	sig;

	/* set shell meta characters */
	if(Meta[0] == 0)
	{	reg char*	s;
		Meta[0] = 1;
		for(s = "!@#$%&*(){}[]:;<>~`'|\"\\"; *s; ++s)
			Meta[(uchar)s[0]] = 1;
	}
	if(!Path)
		Path = _sfgetpath("PATH");

	/* sanity check */
	if(!command || !command[0] || !(sflags = _sftype(mode,NIL(int*))))
		return NIL(Sfio_t*);

	/* make pipes */
	parent[0] = parent[1] = child[0] = child[1] = -1;
	if(pipe(parent) < 0)
		goto error;
	if((sflags&SF_RDWR) == SF_RDWR && pipe(child) < 0)
		goto error;

	switch((pid = fork()) )
	{
	default :	/* in parent process */
		if(sflags&SF_READ)
			{ pkeep = READ; ckeep = WRITE; }
		else	{ pkeep = WRITE; ckeep = READ; }

		/* make the streams */
		if(!(f = sfnew(f,NIL(Void_t*),(size_t)SF_UNBOUND,parent[pkeep],sflags)))
			goto error;
		CLOSE(parent[!pkeep]);

		if((sflags&SF_RDWR) == SF_RDWR)
			CLOSE(child[!ckeep]);

		/* save process info */
		if(_sfpopen(f,(sflags&SF_RDWR) == SF_RDWR ? child[ckeep] : -1,pid) < 0)
		{	(void)sfclose(f);
			goto error;
		}

#ifdef SIGPIPE	/* protect from broken pipe signal */
		if((sflags&SF_WRITE) &&
		   (sig = signal(SIGPIPE,SIG_IGN)) != SIG_DFL && sig != SIG_IGN)
			signal(SIGPIPE,sig);
#endif

		return f;

	case 0 :	/* in child process */
		(void)_sfpclose(NIL(Sfio_t*));

		/* determine what to keep */
		if(sflags&SF_READ)
			{ pkeep = WRITE; ckeep = READ; }
		else	{ pkeep = READ; ckeep = WRITE; }

		/* zap fd that we don't need */
		CLOSE(parent[!pkeep]);
		if((sflags&SF_RDWR) == SF_RDWR)
			CLOSE(child[!ckeep]);

		/* use sfsetfd to make these descriptors the std-ones */
		SFCLEAR(&sf);

		/* must be careful so not to close something useful */
		if((sflags&SF_RDWR) == SF_RDWR && pkeep == child[ckeep])
			if((child[ckeep] = dup(pkeep)) < 0)
				_exit(EXIT_NOTFOUND);

		if(parent[pkeep] != pkeep)
		{	sf.file = parent[pkeep];
			CLOSE(pkeep);
			if(sfsetfd(&sf,pkeep) != pkeep)
				_exit(EXIT_NOTFOUND);
		}

		if((sflags&SF_RDWR) == SF_RDWR && child[ckeep] != ckeep)
		{	sf.file = child[ckeep];
			CLOSE(ckeep);
			if(sfsetfd(&sf,ckeep) != ckeep)
				_exit(EXIT_NOTFOUND);
		}

		execute(command);
		return NIL(Sfio_t*);

	case -1 :	/* error */
	error:
		if(parent[0] >= 0)
			{ CLOSE(parent[0]); CLOSE(parent[1]); }
		if(child[0] >= 0)
			{ CLOSE(child[0]); CLOSE(child[1]); }
		return NIL(Sfio_t*);
	}
#endif /*_PACKAGE_ast*/
}
#endif
