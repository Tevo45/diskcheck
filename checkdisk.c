#include <u.h>
#include <libc.h>

const uint PATTERNS[] = { 0xC0A11E5C, 0xDEADBEEF, 0xB16B00B5, 0x00000000 };

void
usage(void)
{
	fprint(2, "usage: %s [-s start] [-t threshold] disk\n", argv0);
	exits("usage");
}

void*
emalloc(ulong size)
{
	void *buf = malloc(size);
	if(buf == nil)
		sysfatal("malloc: %r");
	return buf;
}

void
repeat(void *dbuf, uint pat, ulong size)
{
	uchar *buf = (uchar*)dbuf;
	for(int c = 0; c < size; c++)
		buf[c] = (uchar)(pat >> c%sizeof(pat));
}

int
chkdsk(int fd, int bsize, vlong seek, int thr)
{
	void *buf = emalloc(bsize), 
		 *pat = emalloc(bsize);
	int pos = seek, read, bad = 0;
	for(int c = 0; c < sizeof(PATTERNS); c++)
	{
		repeat(pat, PATTERNS[c], bsize);
		while((read = pread(fd, buf, bsize, pos)) != 0)
		{
			pwrite(fd, pat, read, pos);
			pread(fd, buf, read, pos);
			if(memcmp(buf, pat, read) != 0)
			{
				print("%d\n", pos);
				bad++;
			}
			if(thr > 0 && bad >= thr)
			{
				goto cleanup; /* Need the goto to break out of the outer loop */
			}
			pos += read;
		}
	}
	cleanup:
	free(buf);
	free(pat);
	return bad;
}

void
main(int argc, char **argv)
{
	int thr = 0, bsize = 512, diskfd;
	vlong seek = 0;

	ARGBEGIN {
	case 's':
		seek = atoll(EARGF(usage()));
		break;
	case 't':
		thr = atoi(EARGF(usage()));
		if(thr < 0)
			usage();
		break;
	default:
		usage();
		break;
	} ARGEND;

	if(argc != 1)
		usage();

	if((diskfd = open(argv[0], ORDWR)) < 0)
		sysfatal("open: %r");

	chkdsk(diskfd, bsize, seek, thr);

	close(diskfd);

	exits(0);
}
