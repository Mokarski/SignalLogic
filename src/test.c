#include <stdlib.h>
#include <stdio.h>

struct conf_limits {
  char *name;
  int low;
  int high;
};

struct conf_ctx {
  int limits_size;
  struct conf_limits *limits;
  struct hash_s *limits_hash;
};

int config_load_limits(struct conf_ctx *ctx, char *filename) {
  char buffer[512];
  char *name;
  char *low, *high;
  FILE *f = fopen(filename, "r");
  int i = 0;
  if(!f) {
    printf("Error: file %s not found\n", filename);
    return -1;
  }
  while(fgets(buffer, sizeof(buffer), f)) {
    ctx->limits_size ++;
  }
  ctx->limits = malloc(ctx->limits_size * sizeof(struct conf_limits));
  fseek(f, 0, SEEK_SET);
  while(fgets(buffer, sizeof(buffer), f)) {
    name = buffer;
    low = strchr(buffer, ':');
    if(!low) {
      printf("File format error: no ':' after name!\n");
      fclose(f);
      return -1;
    }
    *low = 0;
    low ++;
    high = strchr(low, ',');
    if(!high) {
      printf("File format error: no ',' after low limit!\n");
      fclose(f);
      return -1;
    }
    high ++;
    ctx->limits[i].name = strdup(name);
    ctx->limits[i].low = atoi(low);
    ctx->limits[i].high = atoi(high);
    i ++;
  }
  fclose(f);
  return 0;
}

int main() {
  struct conf_ctx *ctx = malloc(sizeof(struct conf_ctx));
  int res = config_load_limits(ctx, "limits.conf");
  printf("Loaded %d limits\n", ctx->limits_size);
}
