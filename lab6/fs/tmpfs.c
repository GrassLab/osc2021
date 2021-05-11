#include "tmpfs.h"
#include <vfs.h>
#include <varied.h>
#include <string.h>
#include <cpio.h>

void tmpfs_init() {
  struct filesystem tmp_fs;
  tmp_fs.name = "tmpfs";
  tmp_fs.setup_mount = setup_mount;

  tmpfs_fops.read = read;
  tmpfs_fops.write = write;
  tmpfs_vops.create = create;
  tmpfs_vops.lookup = lookup;

  register_filesystem(&tmp_fs);  
}

void* tmpfs_vnode_create(struct mount* _mount, enum tmpfs_type type) {
  struct vnode* v_node;
  struct tmpfs_inode* inode;
  
  v_node = (struct vnode* )varied_malloc(sizeof(struct vnode));

  if(v_node == null)
    return null;
  
  v_node->mount = _mount;
  v_node->f_ops = &tmpfs_fops;
  v_node->v_ops = &tmpfs_vops;

  inode = (struct tmpfs_inode*)varied_malloc(sizeof(struct tmpfs_inode));
  
  if(inode == null)
    return null;
  
  v_node->internal = inode;
  strncpy(inode->name, "/", 2);
  inode->type = type;
  inode->vnode = v_node;
  
  if(inode->type == file_t) {
    inode->block = (struct tmpfs_block*)varied_malloc(sizeof(struct tmpfs_block));

    if(inode->block == null) {  
      return null;
    }

    inode->block->size = 0;    
  }
  
  return v_node;
}

static int setup_mount(struct filesystem* fs, struct mount* _mount) {

  _mount->fs = fs;
  _mount->root = tmpfs_vnode_create(_mount, dir_t);
  if(_mount->root == null) {
    return null;
  }

  return 0;
}

static int write(struct file* file, const void* buf, size_t len) {
  struct tmpfs_inode *inode;
  struct tmpfs_block *block, *new_block;
  size_t pos, write_bytes, write_len;
  
  inode = file->vnode->internal;
  if(inode->type != file_t)
    return -1;
  
  block = inode->block;
  pos = file->f_pos;
  write_bytes = 0;
   
  //overwrite
  while(1) {
    //find position (?)
    //write bytes
    if(len < FS_BLOCK_SIZE) {
      
      memcpy(block->content + pos, (char *)buf + write_bytes, len);
      write_bytes += len;
      block->size = len;
      break;
    }
    else {
      
      write_len = FS_BLOCK_SIZE; 
      memcpy(block->content + pos, (char *)buf + write_bytes, write_len);
      
      len -= write_len;
      write_bytes += write_len; 

      block->size = FS_BLOCK_SIZE;
    }   
    
    if(block->next == null) {
      //allocate new data block
      new_block = (struct tmpfs_block*)varied_malloc(sizeof(struct tmpfs_block));

      if(new_block == null)
        return write_bytes;

      block->next = new_block;
    }

    block = block->next;
  }

  inode->size = write_bytes;
  return write_bytes;
}

static int read(struct file* file, void* buf, size_t len) {
  struct tmpfs_inode *inode, *trav_inode;
  struct tmpfs_block *block;
  size_t pos, read_bytes, read_len;
  
  inode = file->vnode->internal;
  read_bytes = 0;

  if(inode->type == dir_t) {
    //for ls
    //empty directory 
    if(inode->children == null)
      return read_bytes;

    //finish traverse
    if(file->f_pos == -1)
      return read_bytes;
    
    if(file->f_pos == 0) {
      file->f_pos = (size_t)inode->children;
    }
    
    trav_inode = (struct tmpfs_inode* )file->f_pos;

    strncpy(buf, trav_inode->name, strlen(trav_inode->name));
    read_bytes = strlen(trav_inode->name);

    file->f_pos = (size_t)trav_inode->sublings;
    
    if((void*)file->f_pos == null)
      file->f_pos = -1;
    
    return read_bytes;
  }
  //EOF
  if(file->f_pos >= inode->size)
    return 0;  
  
  block = inode->block;
  pos = file->f_pos;
  //read bytes
  while(block != null) {
   if(pos < block->size) {
      //in this block
      if(len <= block->size - pos) {
        memcpy((char *)buf + read_bytes, block->content + pos, len);
        read_bytes += len;
        file->f_pos += read_bytes;
        break; 
      }
      else {
        
        read_len = block->size - pos; 
        memcpy((char *)buf + read_bytes, block->content + pos, read_len);
        
        len -= read_len;
        read_bytes += read_len; 
      }
    } 
    else {
      //find next block
      pos -= block->size;
    }

    block = block->next;
  }
  
  return read_bytes;
} 

static int lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct tmpfs_inode* inode;
  int max_len;
  
  if(component_name == null)
    return 1;
  if(dir_node == null)
    return 1;

  inode = dir_node->internal;
  inode = inode->children;
  
  while(inode != null) {
    max_len = strlen(inode->name);
    if(max_len < strlen(component_name)) 
      max_len = strlen(component_name);
    if(strncmp(inode->name, component_name, max_len) == 0)  {
      *target = inode->vnode;
      //
      printf("find node: %s\n", component_name);
      return 0;  
    }
    inode = inode->sublings;
  }

  return 1;
}

static int create(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct vnode *v_node;
  struct tmpfs_inode* inode, *par_inode;
  int max_len;
  
  if(component_name == null)
    return 1;
  if(dir_node == null)
    return 1;
  
  v_node = tmpfs_vnode_create(dir_node->mount, file_t);

  if(v_node == null)
    return 1;
  
  inode = v_node->internal;
  
  max_len = strlen(component_name);
  
  if(max_len >= FILE_NAME_LEN) 
    max_len = FILE_NAME_LEN - 1;

  strncpy(inode->name, component_name, max_len);
  
  //update parent children
  par_inode = ((struct tmpfs_inode* )dir_node->internal)->children;

  if(par_inode == null) {
    ((struct tmpfs_inode* )dir_node->internal)->children = inode;
  }
  else {
    while(par_inode->sublings != null) {
      par_inode = par_inode->sublings;
    }
    par_inode->sublings = inode;
  }
  
  *target = v_node;

  return 0;
}


void traversal(struct tmpfs_inode* inode) { 
  struct tmpfs_inode* ptr;
  struct tmpfs_block* block;
  char buff[33];
  int block_size;
  if(inode == null)
    return;
  
  printf("name: %s, type: %d\n", inode->name, inode->type);
  if(inode->type == file_t) {
   
    if(inode->block->size > 0) {
      if(inode->block->size < 32) {
        strncpy(buff, inode->block->content, inode->block->size);
        buff[inode->block->size] = '\0';
      }
      else {
        strncpy(buff, inode->block->content, 32);
        buff[33] = '\0';
      }
      block = inode->block;
      
      block_size = 0;
      while(block != null) {
        block_size += block->size;
        block = block->next;
      }

      printf("block size: %d size: %d\n", block_size, inode->size);
      printf("%s\n", buff);
    }
  }
  //else
  if(inode->type == dir_t) {   
    
    ptr = inode->children;
    while(ptr != null) {
      traversal(ptr);
      ptr = ptr->sublings;
    }
  }
  
  
}

int tmpfs_load_initramfs(struct mount* _mount) {
  struct tmpfs_inode *inode, *new_inode, *ptr;
  struct vnode *v_node;
  char *buff;
  char name[FILE_NAME_LEN];
  int name_len;
  int err;
  for(int i = 0; i < cpio_file_list_size; i++) {
    
    inode = _mount->root->internal;
    buff = (char *)cpio_file_list[i].name_address;
    name_len = 0;
    
    for(int j = 0; j < cpio_file_list[i].name_size; j++) {
      //check file name, if contains '/', create as directory
      if(buff[j] == '/') {
        //check directory exist
        name[name_len] = '\0';
        v_node = null;
        //lookup
        lookup(inode->vnode, &v_node, name);

        if(v_node == null) {
          //create directory vnode
          v_node = tmpfs_vnode_create(_mount, dir_t);
        
          if(v_node == null)
            return -1;
        
          new_inode = v_node->internal;
          strncpy(new_inode->name, name, name_len); 
          
          //update parent children
          ptr = inode->children;
          
          if(ptr == null) {
            inode->children = v_node->internal;
          }
          else {
            while(ptr->sublings != null) {
              ptr = ptr->sublings;
            }
            ptr->sublings = v_node->internal;
          }
        }
        
        inode = v_node->internal;
        name[0] = '\0';
        name_len = 0;
        
      }
      else {
        name[name_len++] = buff[j];  
      }
    }

    name[name_len] = '\0';
    //file
    //create vnode
    v_node = tmpfs_vnode_create(_mount, file_t);

    if(v_node == null)
    return -1;

    new_inode = v_node->internal;
    //set inode 
    strncpy(new_inode->name, name, name_len);  
    //copy file content
    err = tmpfs_copy_content_from_cpio(new_inode, cpio_file_list[i].file_address, cpio_file_list[i].file_size);
    
    if(err == -1) 
      return -1;
    
    //set parent child list
    ptr = inode->children;

    if(ptr == null) {
      inode->children = new_inode;
    }
    else {
      while(ptr->sublings != null) {
        ptr = ptr->sublings;
      }
      ptr->sublings = new_inode;
    }
    
  }

  traversal(_mount->root->internal);
  
  return 0; 
}

int tmpfs_copy_content_from_cpio(struct tmpfs_inode* inode, void* addr, size_t size) {
  struct tmpfs_block* block, *new_block;
  
  if(inode == null)
    return -1;
  if(inode->block == null)
    return -1;
  
  block = inode->block;
  
  while(block->next != null) {
    block = block->next;
  }
  
  while(1) {

    if(block->size < FS_BLOCK_SIZE) {
      //now block has space
      if(size > (FS_BLOCK_SIZE - block->size)) {
        //not enough need to allocate more space 
        memcpy(block->content, (char *)addr, FS_BLOCK_SIZE - block->size);
      
        addr += FS_BLOCK_SIZE - block->size;
        size -= FS_BLOCK_SIZE - block->size;

        inode->size += FS_BLOCK_SIZE - block->size;
        block->size = FS_BLOCK_SIZE;

      }
      else {
        //enough
        memcpy(block->content, (char *)addr, size);

        block->size += size;
        inode->size += size;
        break;
      }
    }
    
    //allocate new data block
    new_block = (struct tmpfs_block* )varied_malloc(sizeof(struct tmpfs_block)); 
    
    if(new_block == null)
      return -1;
    
    block->next = new_block;
    block = new_block;

  }

  return 0;
}