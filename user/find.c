#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


void
_find(const char *path,const char* target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("find: usage find dir fn\n");
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){

        if(de.inum == 0||strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("ls: cannot stat %s\n", buf);
            continue;
        }
        switch (st.type){
            case T_DEVICE:
            case T_FILE:
                if(strcmp(de.name,target)==0){
                    printf("%s\n",buf);
                }
                break;
            case T_DIR:
                _find(buf,target);
                break;
        }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf("find: usage find dir fn\n");
    exit(1);
  }
  else{
    char const *path = argv[1];
	char const *target = argv[2];
	_find(path, target);
	exit(0);
  }
}