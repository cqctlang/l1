
static void
writebarrier(void)
{
	/* force previous writes to be visible to other processors
	   before subsequent ones. */
	/* on x86 (through core 2 duo),
	   processor-ordering memory model precludes
	   need for explict barrier such as sfence.  if
	   the underlying memory were in WC mode (see
	   intel vol 3, chapter 10), things would be different. */
}

u64	rdtsc(void);
void*	read_and_clear(void *pp);
void	atomic_inc(int *p);
void	atomic_dec(int *p);
