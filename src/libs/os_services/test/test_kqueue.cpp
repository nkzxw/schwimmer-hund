#include <sys/event.h>
#include <sys/time.h> 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 

int main(void)
{
	int f, kq, nev;
	struct kevent change;
	struct kevent event;

	kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue");
	}

	f = open("~/temp1", O_RDONLY);
	if (f == -1)
	{
		perror("open");
	}

	EV_SET(&change, f, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB, 0, 0);

	for (;;) 
	{
		nev = kevent(kq, &change, 1, &amp;event, 1, NULL);
		if (nev == -1)
		{
			perror("kevent");
		}
		else if (nev > 0) 
		{
			if (event.fflags & NOTE_EXTEND || event.fflags & NOTE_WRITE)
			{
				printf("NOTE_ATTRIB -> PN_ATTRIB\n");
			}

			if (event.fflags & NOTE_ATTRIB)
			{
				printf("NOTE_ATTRIB -> PN_ATTRIB\n");
			}
			if (event.fflags & NOTE_WRITE)
			{
				printf("NOTE_WRITE -> PN_MODIFY\n");
			}
			if (event.fflags & NOTE_TRUNCATE)
			{
				printf("NOTE_TRUNCATE -> PN_MODIFY\n");
			}
			if (event.fflags & NOTE_EXTEND)
			{
				printf("NOTE_EXTEND -> PN_MODIFY\n");
			}
			if (kev.fflags & NOTE_DELETE)
			{
				printf("NOTE_DELETE -> PN_DELETE\n");
				break;
			}
			if (kev.fflags & NOTE_RENAME)
			{
				//printf("NOTE_RENAME -> XXXXXXXXX\n");
			}
			if (kev.fflags & NOTE_REVOKE)
			{
				printf("NOTE_REVOKE -> XXXXXXXXX\n");
			}
			if (kev.fflags & NOTE_LINK)
			{
				printf("NOTE_LINK -> XXXXXXXXX\n");
			}


		}
	}

	close(kq);
	close(f);
	return EXIT_SUCCESS;
}
