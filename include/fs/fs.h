#ifndef _FS_H
#define _FS_H

#include <kernel/types.h>

/* fs_nodes->flags values. */
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

#define is_dir(node) ((node->flags & 0x7) == FS_DIRECTORY)

/* Internal prototypes. */
struct fs_node_s;
struct dirent_s;

typedef uint32_t inode_t;

/* These functions are used in struct fs_node_s as driver-dependent callbacks
 * for controlling access. */
typedef uint32_t (*read_func_t)(struct fs_node_s*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_func_t)(struct fs_node_s*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_func_t)(struct fs_node_s*);
typedef void (*close_func_t)(struct fs_node_s*);
typedef struct dirent_s  *(*readdir_func_t)(struct fs_node_s*,uint32_t);
typedef struct fs_node_s *(*finddir_func_t)(struct fs_node_s*,char *name);

struct fs_node_s {
	char     name[255];        /* Filename.                               */
	uint32_t permissions;      /* Permissions mask.                       */
	uint32_t uid;              /* Owning user ID.                         */
	uint32_t gid;              /* Owning group ID.                        */
	uint32_t flags;            /* Includes the node type.                 */
	inode_t  inode;            /* Device specific.                        */
	uint32_t size;             /* Size of the file, in bytes.             */
	uint32_t implementation;   /* Implementation-defined number.          */

	open_func_t open;          /* Open as file descriptor function.       */
	close_func_t close;        /* Close file descriptor function.         */
	read_func_t read;          /* Read function.                          */
	write_func_t write;        /* Write function.                         */
	readdir_func_t readdir;    /* Returns the nth child of a directory.   */
	finddir_func_t finddir;    /* Find a child in a directory by name.    */
	struct fs_node_s *pointer; /* Used by mountpoints and symlinks.       */
};

/* One of these is returned by the readdir call, according to POSIX. */
struct dirent_s {
	char name[255];            /* Filename.                               */
	inode_t inode;             /* Device specific.                        */
};

/* Filesystem root. */
extern struct fs_node_s *fs_root;

/* Here is our filesystem functions. */
void fs_open(struct fs_node_s *node, uint8_t read, uint8_t write);

void fs_close(struct fs_node_s *node);

uint32_t fs_read(struct fs_node_s *node,
		 uint32_t offset, uint32_t size,
		 uint8_t *buffer);

uint32_t fd_write(struct fs_node_s *node,
		  uint32_t offset, uint32_t size,
		  uint8_t *buffer);

struct dirent_s *fs_readdir(struct fs_node_s *node, uint32_t index);

struct fs_node_s *fs_finddir(struct fs_node_s *node, char *name);

#endif /* _FS_H */
