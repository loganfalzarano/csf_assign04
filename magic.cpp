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

  const char * objtype =  get_type_name(elf_header->e_type);
  const char * machtype = get_machine_name(elf_header->e_machine);
  const char * endianness;
  if (elf_header->e_ident[EI_DATA] == 0x01) {
    endianness = "Little endian\0";
  } else if (elf_header->e_ident[EI_DATA] == 0x02) {
    endianness = "Big endian\0";
  }
  printf("Object file type: %s\nInstruction set: %s\nEndianness: %s\n", objtype, machtype, endianness);

  //gets the index of the headers that contains string table
  uint32_t string_table_section_index = elf_header->e_shstrndx;

  //gets the total number of section headers
  uint32_t num_section_headers = elf_header->e_shnum;

  //gets pointer to the sections headers
  unsigned char *section_headers_pointer = (unsigned char *)(elf_header) + elf_header->e_shoff;

  //casts pointer to Elf64_Shdr so we can access the fields
  Elf64_Shdr *section_headers = (Elf64_Shdr *) section_headers_pointer;

  //accesses section with string table
  Elf64_Shdr string_table_section = section_headers[string_table_section_index];

  //gets a pointer to the beginning of the string table data
  const char *shstrtab_data = ((char *) data) + string_table_section.sh_offset;

  //gets pointer to first section header
  Elf64_Shdr * sec_header = &(section_headers[0]);

  //gets the name of the first section header
  //const char *section_name = shstrtab_data + sec_header->sh_name;

  uint32_t symtab_section_index = 0;
  uint32_t strtab_section_index = 0;
  for (uint32_t i=0; i<num_section_headers; i++) {
    Elf64_Shdr curr = section_headers[i];
    const char *section_name = shstrtab_data + curr.sh_name;
    if (curr.sh_type == SHT_SYMTAB && (strcmp(section_name, ".symtab")==0)) { //do we need the first if condition?
      symtab_section_index = i;
    }
    if (curr.sh_type == SHT_STRTAB && (strcmp(section_name, ".strtab")==0)) { //do we need the first if condition?
      strtab_section_index = i;
    }
    printf("Section header %d: name=%s, type=%lx, offset=%lx, size=%lx\n", i, section_name, curr.sh_type, curr.sh_offset, curr.sh_size);
  }

  //get symbol section
  Elf64_Shdr symbol_table_section = section_headers[symtab_section_index];
  //gets a pointer to the beginning of the symbol table data
  const char *symtab_data_pointer = ((char *) data) + symbol_table_section.sh_offset;
  //cast symtab_data_pointer to a (Elf64_Sym *) pointer
  Elf64_Sym * symbol_table_data = (Elf64_Sym *) symtab_data_pointer;
  //gets number of symbols
  uint32_t num_symbols = symbol_table_section.sh_size;

  //get symbol section
  Elf64_Shdr strtab_section = section_headers[strtab_section_index];
  //gets a pointer to the beginning of the strtab data
  const char *strtab_data_pointer = ((char *) data) + strtab_section.sh_offset;
  //cast symtab_data_pointer to a (Elf64_Sym *) pointer
  //Elf64_Sym * strtab_data = (Elf64_Sym *) symtab_data_pointer;
  //gets number of symbols
  //uint32_t num_symbols = symbol_table_section.sh_size;


  //iterate through all the symbols
  for (uint32_t i=0; i<num_symbols; i++) {
    Elf64_Sym curr = symbol_table_data[i];
    const char * symbol_name = strtab_data_pointer + curr.st_name;
    printf("Symbol %ld: name=%s, size=%lx, info=%lx, other=%lx\n", i, symbol_name, curr.st_size, curr.st_info, curr.st_other);
  }

  

  
 // cout << symbol_table_section.sh_name << endl;
 // cout << symbol_table_section.sh_type << endl;
 // cout << symbol_table_section.sh_flags << endl;
 // cout << symbol_table_section.sh_addr << endl;
//
 // cout << symbol_table_section.sh_offset << endl;
 // cout << symbol_table_section.sh_size << endl;
//
 // cout << symbol_table_section.sh_link << endl;
 // cout << symbol_table_section.sh_info << endl;
 // cout << symbol_table_section.sh_addralign << endl;
 // cout << symbol_table_section.sh_entsize << endl;

}
