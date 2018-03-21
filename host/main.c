#include <stdio.h>
#include <stdlib.h>
#include "rs232.h"
#include "spi.h"
#include "flash25.h"

int main(int argc, const char** argv) {
	if (argc < 2) {
		printf("RS232 port is not specified, on Linux, normally it should be /dev/ttyACM0, on Windows it should be something like COM8. \n");
		return EXIT_FAILURE;
	}

	const char* portName = argv[1];
	HCOM hCom = RS232_Open(portName);
	//Set the buffer size
	if (RS232_IS_INVALID(hCom)) {
		printf("Unable to open port %s \n", portName);
		return EXIT_FAILURE;
	}

	printf("Chip ID = %x\n", SPI_Flash25_ReadID(hCom));

	unsigned char* src = (unsigned char*)malloc(0x200000);
	FILE *file = fopen("D:\\Administrator\\Desktop\\rikka.png", "rb");
	fseek(file, 0L, SEEK_END);
	long src_len = ftell(file);
	rewind(file);
	fread(src, src_len, 1, file);
	fclose(file);

	printf("File size = %ld\n", src_len);
	SPI_Flash25_BulkErase(hCom);
	printf("Chip Erased\n");
	SPI_Flash25_BufferWrite(hCom, src, 0, 256, src_len);
	printf("Writing done!\n");

	unsigned char* dst = (unsigned char*)malloc(0x200000);
	SPI_Flash25_BufferRead(hCom, dst, 0, src_len);
	printf("Reading done!\n");

	for (int i = 0; i < src_len; i++) {
		if (src[i] != dst[i]) {
			printf("diff %d\n", i);
		}
	}

	RS232_Close(hCom);

	system("pause");
	return 0;
}