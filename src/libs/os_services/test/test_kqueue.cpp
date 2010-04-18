#include <iostream>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>

#include <sys/event.h>
#include <sys/time.h> 


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

	//f = open("/home/fernando/temp1", O_RDONLY);
	f = open("/home/fernando/temp1", O_EVTONLY);
	
	if (f == -1)
	{
		perror("open");
	}

	EV_SET(&change, f, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB, 0, 0);

	for (;;) 
	{
		nev = kevent(kq, &change, 1, &event, 1, NULL);

		
		if (nev == -1)
		{
			perror("kevent");
		}
		else if (nev > 0) 
		{
			std::cout << "nev: " << nev << std::endl;

			std::cout << "event.ident: " << event.ident << std::endl;
			std::cout << "event.ident: " << event.ident << std::endl;
			std::cout << "event.filter: " << event.filter << std::endl;
			std::cout << "event.flags: " << event.flags << std::endl;
			std::cout << "event.fflags: " << event.fflags << std::endl;
			std::cout << "event.data: " << event.data << std::endl;
			std::cout << "event.udata: " << event.udata << std::endl;



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
			//if (event.fflags & NOTE_TRUNCATE)
			//{
			//	printf("NOTE_TRUNCATE -> PN_MODIFY\n");
			//}
			if (event.fflags & NOTE_EXTEND)
			{
				printf("NOTE_EXTEND -> PN_MODIFY\n");
			}
			if (event.fflags & NOTE_DELETE)
			{
				printf("NOTE_DELETE -> PN_DELETE\n");
				break;
			}
			if (event.fflags & NOTE_RENAME)
			{
				printf("NOTE_RENAME -> XXXXXXXXX\n");
			}
			if (event.fflags & NOTE_REVOKE)
			{
				printf("NOTE_REVOKE -> XXXXXXXXX\n");
			}
			if (event.fflags & NOTE_LINK)
			{
				printf("NOTE_LINK -> XXXXXXXXX\n");
			}


		}
	}

	close(kq);
	close(f);
	return EXIT_SUCCESS;
}
