#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    int fd;
    ssize_t nr, slen;
     
    openlog("Assignment1", LOG_NDELAY, LOG_USER);

    if (argc < 3) {
     	syslog(LOG_ERR, "[Usage] writer write_file write_str");
		exit(1);
    }

    fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    if (fd == -1) {
        syslog(LOG_ERR, "Failed to open file to write: %s", argv[1]);
		exit(1);
    } else {
        slen = strlen(argv[2]);

        nr = write(fd, argv[2], slen);

		if (nr == -1) {
	    	perror("Writer");
	    	syslog(LOG_ERR, "Failed to write: %s", argv[2]);
		} 
		else if (nr != slen) {
            syslog(LOG_ERR, "Partially written: %d of %d", (int) nr, (int) slen);
		}
		else {
	    	syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
		}

		close(fd);
    }

    exit(0);
}
