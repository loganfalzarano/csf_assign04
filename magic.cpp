#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <cstdint>
#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <iostream> //<- I added this

#include "elf_names.h"

using namespace std;

int handle_input_file(const char * filename, void * &data) { //use a reference so we get the pointer value back
  //returns a file descriptor for the opened file, or will return a negative value if the file can’t be opened.
  int fd = open(filename, O_RDONLY); 

  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0 || fd < 0) { //error has occurred, print to std:err and return non-zero exit code
    return 1;
  } else {
    size_t file_size = statbuf.st_size;
    data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0); //so we can access data in memory
    if (data == ((void *)-1)) {
      return 1; //error has occurred
    } else {
      return 0; //successful read the file
    }
  }
}

int main(int argc, char **argv) {
  // TODO: implement
  (void) argc;
  void *data = NULL;
  if(handle_input_file(argv[1], data) != 0) {
    std::cerr << "An error has occurred" << std::endl;
    return 1;
  }

  Elf64_Ehdr *elf_header = (Elf64_Ehdr *) data;
  //cout << ".shstrtab section index is %u\n"

  printf(".shstrtab section index is %u\n", elf_header->e_shstrndx);
  return 0;

}
