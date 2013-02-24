#include <stdio.h>
#include <string.h>
#include <malloc.h>

/* Kernel source prototypes. */
#define HEADER_ID    0xCC
#define HEADER_COUNT 64
#define FILENAME_LEN 128

struct initrd_file_header_s {
  char          name[FILENAME_LEN];
  unsigned char id;
  unsigned int  offset;
  unsigned int  size;
};

static void usage(FILE *stream)
{
  fprintf(stream, "Usage: intrd-gen: <image> [<path name> ...]\n");
  fprintf(stream, "\n");
  fprintf(stream, "Generate an init ramdisk <image> containing a list of files from arguments\n");
  fprintf(stream, "passed as as <path name> couplets, where <path> is the local file path, and\n");
  fprintf(stream, "<name> specifies the name of the file in the initrd. For example:\n");
  fprintf(stream, "\n");
  fprintf(stream, "  $ initrd-gen initrd.img ~/foo foo ~/bar bar\n");
  fprintf(stream, "\n");
  fprintf(stream, "This generates a file initrd.img which is a ramdisk containing the files\n");
  fprintf(stream, "~/foo and ~/bar, with the respective filenames 'foo' and 'bar'.\n");
  fprintf(stream, "\n");
  fprintf(stream, "It is possible to generate an initrd with no input files.\n");
}

int main(int argc, char **argv)
{
  struct initrd_file_header_s headers[HEADER_COUNT];
  int headers_count;
  char *output_file;
  unsigned int offset;
  FILE *write_stream;
  unsigned char *data;
  int i;

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--help")) {
      usage(stdout);
      return 1;
    }
  }

  if (argc < 2) {
    usage(stderr);
  } else if ((argc % 2) || (argc > (2 * HEADER_COUNT) + 2)) {
    /* Arg count doesn't match <path name> couplet, or is a greater number than
     * the number of file headers in ramdisk. */
    usage(stderr);
  }

  output_file = argv[1];
  headers_count = (argc - 2) / 2;

  printf("INITRD-GEN  %s\n", output_file);
  offset = sizeof(struct initrd_file_header_s) * HEADER_COUNT + sizeof(int);

  for(i = 0; i < headers_count; i++) {
    char *path = argv[(2 * i) + 2];
    char *name = argv[(2 * i) + 3];
    struct initrd_file_header_s *header = &headers[i];
    FILE *stream;

    if (strlen(path) > FILENAME_LEN) {
      fprintf(stderr, "initrd-gen: file name exceeds initrd max file length, %d.\n", FILENAME_LEN);
      return 1;
    }

    strcpy(header->name, name);
    header->offset = offset;

    stream = fopen(path, "r");
    if(!stream) {
      fprintf(stderr, "initrd-gen: file not found: '%s'\n", path);
      return 1;
    }

    fseek(stream, 0, SEEK_END);
    header->size = ftell(stream);

    offset += header->size;
    fclose(stream);
    header->id = HEADER_ID;

    printf("\t0x%x\t%s\t%d\t%s\n", offset, name, header->size, path);
  }

  write_stream = fopen(output_file, "w");
  if (!write_stream) {
    fprintf(stderr, "initrd-gen: unable to write to file: '%s'.\n", output_file);
    return 1;
  }

  data = (unsigned char *)malloc(offset);
  fwrite(&headers_count, sizeof(int), 1, write_stream);
  fwrite(headers, sizeof(struct initrd_file_header_s), HEADER_COUNT, write_stream);

  for(i = 0; i < headers_count; i++) {
    char *path = argv[(2 * i) + 2];
    unsigned char *buffer = (unsigned char *)malloc(headers[i].size);
    FILE *stream = fopen(path, "r");

    if(!stream) {
      fprintf(stderr, "initrd-gen: unable to read file: '%s'.\n", path);
      return 1;
    }

    fread(buffer, 1, headers[i].size, stream);
    fwrite(buffer, 1, headers[i].size, write_stream);
    fclose(stream);
    free(buffer);
  }

  fclose(write_stream);
  free(data);

  printf("\n\t/ ");
  for (i = 0; i < headers_count; i++) {
    printf("/%s ", argv[(2 * i) + 3]);
  }
  printf("\n");

  return 0;
}
