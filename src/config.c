#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/hash.h"
#include "config.h"

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
      printf("File format error!\n");
      fclose(f);
      return -1;
    }
    *low = 0;
    low ++;
    high = strchr(low, ',');
    if(!high) {
      printf("File format error!\n");
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

int config_load(struct execution_context_s *ctx, char *filename) {
  char buffer[512];
  char *name;
  char *val;
  FILE *f = fopen(filename, "r");
  int i = 0;
  if(!f) {
    printf("Error: file %s not found\n", filename);
    return -1;
  }
  while(fgets(buffer, sizeof(buffer), f)) {
    name = buffer;
    val = strchr(buffer, ':');
    if(!val) {
      printf("File format error!\n");
      return -1;
    }
    *val = 0;
    val ++;
    signal_set(ctx, name, val);
    update_command(ctx, name, atoi(val));
  }
  fclose(f);
  return 0;
}

int config_check(struct conf_ctx *c, char *name, int value) {
  struct conf_limits *cl;
  int result = 0;
  
  cl = hash_find_first(c->limits_hash, name);

  if(!cl) return result;

  if(cl->low > value && cl->low != -1) result |= LOW_LIMIT_PASS;
  if(cl->high < value && cl->high != -1) result |= HIGH_LIMIT_PASS;

  return result;
}

int config_save(char *name, int value, char *filename) {
  char buffer[512];
  char *sname;
  char *val;
  FILE *f = fopen(filename, "r+");
  int i = 0, skipped = 0;
  off_t pos = 0, wpos = 0;

  if(!f) {
    printf("Error: file %s not found\n", filename);
    return SAVE_FAILED;
  }

  pos = ftell(f);
  wpos = ftell(f);
  while(fgets(buffer, sizeof(buffer), f)) {
    if(!skipped) {
      sname = buffer;
      val = strchr(buffer, ':');
      if(!val) {
        printf("File format error!\n");
        return SAVE_FAILED;
      }
      *val = 0;
      if(!strcmp(name, sname)) {
        skipped = 1;
        wpos = pos;
      }
      pos = ftell(f);
    }
    else
    {
      pos = ftell(f);
      fseek(f, wpos, SEEK_SET);
      fputs(buffer, f);
      wpos = ftell(f);
      fseek(f, pos, SEEK_SET);
    }
  }
  if(wpos != 0)
    fseek(f, wpos, SEEK_SET);

  i = sprintf(buffer, "%s:%d\n", name, value);
  wpos = ftell(f);
  fwrite(buffer, 1, i, f);
  fclose(f);
  truncate(filename, wpos + i);
  return 0;
}
