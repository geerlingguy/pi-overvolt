#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <vc_mem.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (128*1024)

int main(int argc, char * argv[])
{
    int fd;
    char *p;
    volatile unsigned char *vc;
    unsigned long address, size, my_size, base;
    int i;

    fd = open("/dev/vc-mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        printf("unable to open /dev/vc-mem ...\n");
        return 1;
    }
    ioctl(fd, VC_MEM_IOC_MEM_PHYS_ADDR, &address);
    printf("VC_MEM_IOC_MEM_PHYS_ADDR = %p\n", (char *)address);

    ioctl(fd, VC_MEM_IOC_MEM_SIZE, &size);
    printf("VC_MEM_IOC_MEM_SIZE = %d\n", size);

    my_size = (1024 * 512);  // 0.5MB

    ioctl(fd, VC_MEM_IOC_MEM_BASE, &base);
    printf("VC_MEM_IOC_MEM_BASE = %p\n", (char *)base);

    // import mmap
    // print mmap.PAGESIZE
    // 16384

    vc = (unsigned char *)mmap(
        0,  // address
        my_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd,
        // size - my_size);  // offset
        1069547520  // offset needs to be a multiple of page size
    );
    if (vc == (unsigned char *)-1)
    {
        printf("mmap failed %s\n", strerror(errno));
        return -1;
    }

    printf("vc = %p\n", vc);

    for (i = 0; i < my_size; i++) {
        if(vc[i] == 0xA6 && vc[i+1] == 0x4a && vc[i+2] == 0x07 && vc[i+3] == 0xc0 && vc[i+4] == 0x06 && vc[i+5] == 0x31 && vc[i+6] == 0x07)
        {
            printf("found A6 4a... replacing\n");
            vc[i+8] = 0x86;  // was 8a
            vc[i+9] = 0x01;  // was 51
        }

        // 3fc4c5c8 a6 4a           cmp        r6,r10
        // 3fc4c5ca 07 c0 06 31     mov.cs     r7,r6
        // 3fc4c5ce 07 c0 8a 51     mov.cc     r7,r10
        // 3fc4c5d2 7d 40           mov        r13,r7

        // patch:
        // 3fc4c5ce 07 c0 86 01     mov.cc     r7,r6

    }

    close(fd);
}
