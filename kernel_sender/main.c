#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void send_img_through_uart(char *device, char *kfile);

int main() {
    char device[50] = { '\x00' };
    char kfile[50] = { '\x00' };

    printf("Please choose a device for kernel transmitting (ex: /dev/ttyUSB0): ");
    scanf("%s", device);
    device[49] = '\x00';

    printf("Please choose a kernel image for transmitting (ex: kernel8.img): ");
    scanf("%s", kfile);
    kfile[49] = '\x00';

    send_img_through_uart(device, kfile);

    return 0;
}

void send_img_through_uart(char *device, char *kfile) {
	int serial_fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(serial_fd == -1) {
		printf("\r *** Fail to open %s ... *** \n", device);
		return ;
	}

	printf("\r *** Listening on %s ... *** \n", device);

	int running = 1;
	int counter = 0;

	while(running) {
		char c;
		ssize_t len = read(serial_fd, &c, 1);

		if(len == 0) {
			counter = 0;
			continue;
		}

		if(c == '\x03')
			counter++;

		if(counter == 3) {
			counter = 0;

			/* send kernel's size */
			int file_fd = open(kfile, O_RDONLY);

			ssize_t size = lseek(file_fd, 0L, SEEK_END);

			if (size > 0x200000) {
				printf("\r *** Kernel too big %s ... *** \n", kfile);
				return ;
			}

			printf("\r *** Sending kernel %s [%zd byte] *** \n", kfile, size);

			/* sending kernel's size to Rpi3 */
			for (int i = 0; running & (i < 4); ++i) {
				char c = (size >> 8 * i) & 0xFF;
				write(serial_fd, &c, 1);
			}

			/* wait for OK */
			char buf[2] = {0};

			for (int i = 0; running && (i < 2); ++i) {
				read(serial_fd, &buf[i], 1);
				if (buf[i] == 0) 
					i--; 
			}

			if (buf[0] != 'O' || buf[1] != 'K') {
				printf("\r *** Error after sending size, got '%c%c' *** \n", buf[0], buf[1]);
				return;
			}

			while(running && (size > 0)) {
				char buf[65536];
				ssize_t pos = 0;
				ssize_t len = read(file_fd, buf, 65536);
				size -= len;
				while(running && (len > 0)) {
					ssize_t len2 = write(serial_fd, &buf[pos], len);
					len -= len2;
					pos += len2;
				}
			}
		
            printf("\r *** Finished Sending *** \n");

		    if (running) {
			    ssize_t len = write(STDOUT_FILENO, &c, 1);
			    if (len == 0) {
				    running = 0;
				    break;
			    }
		    }
	    }
    }
    close(serial_fd);
}