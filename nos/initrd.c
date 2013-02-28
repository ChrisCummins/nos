#include <initrd.h>

#include <assert.h>
#include <fs.h>
#include <kheap.h>
#include <kstream.h>
#include <string.h>
#include <types.h>

#define HEADER_ID  0xCC
#define is_header(h)  ((((struct initrd_file_header_s*)h)->id == HEADER_ID) \
                       ? 1 : 0)

static struct initrd_header_s *header;
static struct initrd_file_header_s *file_headers;
static struct fs_node_s *root;
static struct fs_node_s *dev;
static struct dirent_s dirent;
static struct fs_node_s *nodes;
static int nodes_count;

static uint32_t _initrd_read(struct fs_node_s *node, uint32_t offset,
                             uint32_t size, uint8_t *buffer)
{
  struct initrd_file_header_s header = file_headers[node->inode];
  uint32_t read_length = size;

  assert(is_header(&header));

  if (offset > header.size) {
    /* Invalid read (out of bounds) */
    k_warn("initrd: invalid offset of %h in node size %h\n", offset, size);
    return 0;
  }

  if (offset + size > header.size) {
    /* Reduce read size as necessary to prevent overshoot. */
    read_length = header.size - offset;
  }

  memcpy(buffer, (uint8_t *)(header.offset + offset), read_length);

  return read_length;
}

static struct dirent_s *_initrd_readdir(struct fs_node_s *node, uint32_t index)
{
  if (node == root && !index) {
    strcpy(dirent.name, "dev");
    dirent.name[3] = 0;
    dirent.inode = 0;

    return &dirent;
  }

  if ((int)(index - 1) >= nodes_count) {
    /* Return nothing on out of bounds request. */
    return 0;
  }

  strcpy(dirent.name, nodes[index - 1].name);
  dirent.name[strlen(nodes[index - 1].name)] = 0;
  dirent.inode = nodes[index - 1].inode;

  return &dirent;
}

static struct fs_node_s *_initrd_finddir(struct fs_node_s *node, char *name)
{
  int i;

  if (node == root && !strcmp(name, "dev")) {
    return dev;
  }

  for (i = 0; i < nodes_count; i++) {
    if (!strcmp(name, nodes[i].name)) {
      return &nodes[i];
    }
  }

  return 0;
}

struct fs_node_s *init_initrd(uint32_t location)
{
  int i;

  /* Initialise the main and file header pointers and populate the root
   * directory. */
  header = (struct initrd_header_s *)location;
  file_headers  = (struct initrd_file_header_s *)(location              \
                                                  + sizeof(struct initrd_header_s));

  /* Initialise the root (/) directory. */
  root = kcreate(struct fs_node_s, 1);
  strcpy(root->name, "initrd");
  root->permissions = 0;
  root->uid = 0;
  root->gid = 0;
  root->inode = 0;
  root->size = 0;
  root->flags = FS_DIRECTORY;
  root->read = 0;
  root->write = 0;
  root->open = 0;
  root->close = 0;
  root->readdir = &_initrd_readdir;
  root->finddir = &_initrd_finddir;
  root->pointer = 0;
  root->implementation = 0;

  /* Initialise the devices (/dev) directory. */
  dev = kcreate(struct fs_node_s, 1);
  strcpy(dev->name, "dev");
  dev->permissions = 0;
  dev->uid = 0;
  dev->gid = 0;
  dev->inode = 0;
  dev->size = 0;
  dev->flags = FS_DIRECTORY;
  dev->read = 0;
  dev->write = 0;
  dev->open = 0;
  dev->close = 0;
  dev->readdir = &_initrd_readdir;
  dev->finddir = &_initrd_finddir;
  dev->pointer = 0;
  dev->implementation = 0;

  nodes = kcreate(struct fs_node_s, header->file_count);
  nodes_count = header->file_count;

  /* Iterate over all files. */
  for (i = 0; i < nodes_count; i++) {
    struct fs_node_s *node = &nodes[i];
    struct initrd_file_header_s *header = &file_headers[i];

    /* We need to change the file's header so that it holds the file offset
     * relative to the start of memory, instead of the offset relative to the
     * start of the ramdisk. */
    header->offset += location;

    /* Create a file node. */
    strcpy(node->name, &header->name[0]);
    node->permissions = 0;
    node->uid = 0;
    node->gid = 0;
    node->size = header->size;
    node->inode = i;
    node->flags = FS_FILE;
    node->read = &_initrd_read;
    node->write = 0;
    node->readdir = 0;
    node->finddir = 0;
    node->open = 0;
    node->close = 0;
    node->implementation = 0;
  }

  return root;
}
