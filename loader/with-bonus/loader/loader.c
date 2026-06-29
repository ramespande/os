#include "loader.h"

Elf32_Ehdr ehdr;
Elf32_Phdr phdr;
int fd,mem;
void* virtual_mem;

void loader_cleanup()  {
	if (virtual_mem != NULL) {
        	munmap(virtual_mem, mem);
        	virtual_mem = NULL;
	}
}

void load_and_run_elf(char** exe) {
	fd = open(exe[1], O_RDONLY);
	if (fd<0) {
    		printf("%s","error opening elf file");
    		return;
	}
  	if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        	printf("%s","ehdr is faulty");
        	return;
    	}
  	off_t e_phoff = ehdr.e_phoff;
	lseek(fd, e_phoff, SEEK_SET);
  	int prev,prevoffset;
  	for (int i = 0; i < ehdr.e_phnum; i++) {
      		Elf32_Phdr phdr;
      		if (read(fd, &phdr, sizeof(phdr)) != sizeof(phdr)) {
          		printf("%s","Error");
          		return;
      		}
      		if (phdr.p_type==1 && i!=0) {
  	  		if (ehdr.e_entry>=prev && ehdr.e_entry<=phdr.p_vaddr) {
	        		virtual_mem = mmap(NULL,mem,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_ANONYMOUS|MAP_PRIVATE,0,0);
				break;
            		}
        	}
		prevoffset=phdr.p_offset;
        	prev=phdr.p_vaddr;
		mem=phdr.p_memsz;
	}
   	lseek(fd,prevoffset,SEEK_SET);
   	if (read(fd, virtual_mem, mem)!=mem) {
		printf("%s","Error");
		return;
   	}
   	void* entry = virtual_mem + (ehdr.e_entry-prev);
	int (*start)() = (int(*)())entry;
	int result = start();
  	printf("User _start return value = %d\n",result);
	close(fd);
}
