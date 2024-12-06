#define BUFFER_SIZE 16

typedef struct {
  int length;
  char *message;
} item_t;

typedef struct {
  item_t buffer[BUFFER_SIZE];
  int in;
  int out;
} shared_t;