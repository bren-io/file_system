#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//---------
// Data
//-----------

#define INODES 100
#define BLOCKSIZE 512
#define NUMBLOCKS 1000

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
  int data_blocks[NUMBLOCKS];
  int file_type;
};

struct datablock {
  char data[BLOCKSIZE];
};

struct direntry {
  unsigned int inode;
  char name[256];
  int file_type;
  uint32_t status;
};

struct superblock * sb;
struct inode inodes[INODES];
uint32_t * fbl;

// END DATA

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
  }

  // Init fbl
  fbl = (uint32_t *)(fs + sb->fbl_offset);
  memset(fbl, 0xFF, e_fbl * sizeof(uint32_t));
}


void loadfs(){
  
  sb = (struct superblock *)fs;

  // Load inodes
  for (int i = 0; i < sb->n_inodes, i++){
    size_t e_inode = sb->inode_offset + (i * sizeof(struct inode));
    memcpy(&inodes[i], fs + e_inode, sizeof(struct inode));
  }

  fbl = (uint32_t *)(fs + sb->fbl_offset);
  
}


void lsfs(){
  
}

void addfilefs(char* fname){
  
}


void removefilefs(char* fname){

}


void extractfilefs(char* fname){

}
