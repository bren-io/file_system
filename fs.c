#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//-----------
// Data
//-----------

#define INODES 100
#define BLOCKSIZE 512
#define NUMBLOCKS 1000
#define ROOT_INODE_ID 0
#define REG_FILE 1
#define DIR_FILE 2

unsigned char* fs;

struct superblock {
  uint32_t n_inodes;
  uint32_t n_blocks;
  size_t blocksize;
  size_t inode_offset;
  size_t fbl_offset;
};

struct inode {
  uint32_t inode_id;
  uint32_t status;
  size_t file_size;
  int file_type;
  char name[256];
  int data_blocks[NUMBLOCKS];
};

struct datablock {
  char data[BLOCKSIZE];
};

struct superblock * sb;
struct inode inodes[INODES];
uint32_t * fbl;

//------------
// END DATA
//------------

void mapfs(int fd){
  if ((fs = mmap(NULL, FSSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL){
      perror("mmap failed");
      exit(EXIT_FAILURE);
  }
}


void unmapfs(){
  munmap(fs, FSSIZE);
}


void formatfs(){
  // Calculate sizes of file system components
  size_t sb_size = sizeof(struct superblock);
  size_t inode_size = sizeof(struct inode) * INODES;
  int blocks = (FSSIZE - (sb_size + inode_size)) / BLOCKSIZE;
  blocks -= (blocks % 10);
  int e_fbl = (blocks / (sizeof(uint32_t) * 8)) + 1;

  // Init superblock
  sb = (struct superblock *)fs;
  sb->n_inodes = INODES;
  sb->n_blocks = blocks;
  sb->blocksize = BLOCKSIZE;
  sb->inode_offset = sb_size;

  // Init inodes
  for (int i = 0; i < sb->n_inodes; i++){
    inodes[i].inode_id = i;
    inodes[i].status = 0;
    inodes[i].file_size = 0;
    inodes[i].file_type = 0;

    for (int j = 0; j < NUMBLOCKS; j++){
      inodes[i].data_blocks[j] = -1;
    }

    if (i == ROOT_INODE_ID){
      inodes[i].status = 1;
      inodes[i].file_type = DIR_FILE;
    }
  }

  // Init fbl
  fbl = (uint32_t *)(fs + sb->fbl_offset);
  memset(fbl, 0xFF, e_fbl * sizeof(uint32_t));
}


void loadfs(){
  /*
  // Load superblock
  sb = (struct superblock *)fs;

  // Load inodes
  for (int i = 0; i < sb->n_inodes; i++){
    struct inode * e_inode = (struct inode *)(fs + sb->inode_offset + (i * sizeof(struct inode)));
    inodes[i] = *e_inode;
  }

  // Load fbl
  fbl = (uint32_t *)(fs + sb->fbl_offset);
  */
}


void lsfs(){
  
}


void addfilefs(char* fname){

  // Find free inode
  int free_inode = -1;
  for (int i = 0; i < sb->n_inodes; i++){
    if (inodes[i].status == 0){
      free_inode = i;
      break;
    }
  }
  if (free_inode == -1){
    perror("Error: No inode available");
    return;
  }

  // Init the inode
  struct inode * new_file = &inodes[free_inode];
  new_file->inode_id = free_inode;
  new_file->status = REG_FILE;
  new_file->file_size = 0;

  // Get the stats to see what type
  struct stat file_stat;
  if (stat(fname, &file_stat) == 0){
    if (S_ISDIR(file_stat.st_mode)){
      new_file->file_type = DIR_FILE;
    } else {
      new_file->file_type = REG_FILE;
    }
  }
  else{
    perror("Error: Failed to retrieve file stats");
    return;
  }

  strcpy(new_file->name, fname);
  
}


void removefilefs(char* fname){

}


void extractfilefs(char* fname){

}
