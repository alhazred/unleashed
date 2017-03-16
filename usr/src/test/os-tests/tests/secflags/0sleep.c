#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int fd;

	if (argc != 2)
		return 1;

	if ((fd = open("/dev/zero", O_RDWR)) < 0)
		return 2;
	(void) mmap(NULL, 1, PROT_READ, MAP_PRIVATE|MAP_FIXED, fd, 0);
	(void) close(fd);

	(void) sleep(atoi(argv[1]));

	return 0;
}
