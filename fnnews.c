#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/include/lib9.h"

static void
l1_news(VM *vm, Imm argc, Val *argv, Val *rv)
{
	static char newspath[] = "../doc/news";
	static char newstime[] = ".l1news";
	static char *month[] = { "jan", "feb", "mar", "apr", "may",
				 "jun", "jul", "aug", "sep", "oct",
				 "nov", "dec" };
	char *news, *enews, **lp, *full, *p;
	unsigned len;
	Cval *cv;
	Imm days;
	int i, fd;
	char ntpath[128];
	struct stat st;
	struct tm tm;
	time_t lim, t;

	if(argc != 0 && argc != 1)
		vmerr(vm, "wrong number of arguments to news");
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		days = cvalu(cv);
		lim = time(0)-(time_t)days*24*60*60;
	}else{
		snprint(ntpath, sizeof(ntpath), "%s/%s",
			getenv("HOME"), newstime);
		if(0 > stat(ntpath, &st))
			lim = 0;
		else
			lim = st.st_mtime;
		fd = open(ntpath, O_TRUNC|O_CREAT, 0644);
		if(fd >= 0)
			close(fd);
	}

	/* search vicinity of loadpath for news */
	lp = cqctloadpath;
	news = 0;
	while(*lp){
		len = strlen(*lp)+1+strlen(newspath)+1;
		full = emalloc(len);
		snprint(full, len, "%s/%s", *lp, newspath);
		news = readfile(full);
		efree(full);
		if(news)
			break;
		lp++;
	}
	if(news == 0)
		vmerr(vm, "cannot find the news");

	enews = news+strlen(news);
	p = news;

	/* find first news item to print */
	while(p && p < enews){
		if(!isspace(*p)){
			/* must be a date, the start of a new item */
			if(enews-p < 3)
				goto badnews;
			for(i = 0; i < 3; i++)
				p[i] = tolower(p[i]);
			for(i = 0; i < 12; i++)
				if(!strncmp(p, month[i], 3))
					break;
			if(i >= 12)
				goto badnews;
			memset(&tm, 0, sizeof(tm));
			tm.tm_sec = 59;
			tm.tm_min = 59;
			tm.tm_hour = 23;
			tm.tm_mon = i;
			if(2 != sscanf(p+3, "%d %d", &tm.tm_mday, &tm.tm_year))
				goto badnews;
			tm.tm_year -= 1900;
			t = mktime(&tm);
			if(t >= lim)
				break;
		}
		p = strchr(p, '\n');
		p++;
	}

	/* print news */
	if(p)
		cprintf(&l1stdout, "%s", p);

	efree(news);
	return;
badnews:
	xprintf("bad news\n");
	efree(news);
}

void
fnnews(Env env)
{
	FN(news);
}
