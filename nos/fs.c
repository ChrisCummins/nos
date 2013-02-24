#include <nos/fs.h>

/* Filesystem root. */
struct fs_node_s *fs_root = 0;

void fs_open (struct fs_node_s *node, uint8_t read, uint8_t write)
{
  if(node->open) {
    node->open(node);
  }
}

void fs_close(struct fs_node_s *node)
{
  if(node->close) {
    node->close(node);
  }
}

uint32_t fs_read(struct fs_node_s *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
  if (node->read) {
    return node->read(node, offset, size, buffer);
  } else {
    return 0;
  }
}

uint32_t fd_write(struct fs_node_s *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
  if (node->write) {
    return node->write(node, offset, size, buffer);
  } else {
    return 0;
  }
}

struct dirent_s *fs_readdir(struct fs_node_s *node, uint32_t index)
{
  if (node->readdir && is_dir(node)) {
    return node->readdir(node, index);
  } else {
    return 0;
  }
}

struct fs_node_s *fs_finddir(struct fs_node_s *node, char *name)
{
  if (node->finddir && is_dir(node)) {
    return node->finddir(node, name);
  } else {
    return 0;
  }
}
