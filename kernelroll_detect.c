/* fiction was here */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static char* getname(char *link) 
{
   struct stat sb;
   static char *linkname;
   ssize_t r;

   if (lstat(link, &sb) == -1) 
   {
       perror("lstat");
       return NULL;
   }

   linkname = malloc(sb.st_size + 1);
   if (linkname == NULL) 
   {
       fprintf(stderr, "Insufficient memory\n");
       return NULL; 
   }

   r = readlink(link, linkname, sb.st_size + 1);

   if (r < 0) 
   {
       perror("lstat");
       return NULL; 
   }

   if (r > sb.st_size) 
   {
       fprintf(stderr, "Symlink increased in size "
		       "between lstat() and readlink()\n");
       return NULL;
   }

   linkname[sb.st_size] = '\0';

   return linkname;
}

int detect(char *self)
{
   char name[64], nname[64];
   char *tmp, *a, *b;
   int ret;

   if ((tmp = tempnam(".", "rrtst")) == NULL) 
   {
       perror("tempnam");
       return -1;
   }

   sprintf(name, "%s.mp3", tmp);
   free(tmp);

   if (link(self, name)) 
   {
       perror("link");
       return -1;
   }

   int fd = open(name, O_RDONLY);
   if (fd < 0)
   {
       perror("open");
       return -1;
   }

   sprintf(nname, "/proc/self/fd/%d", fd);
   tmp = getname(nname);
   if (tmp == NULL)
   {
       return -1;
   } 

   a = rindex(name, '/');
   b = rindex(tmp, '/');

   if (a > 0 && b > 0 && strcmp(a, b) == 0)
   {
       ret = 0;
   }
   else
   {
       fprintf(stderr, "Rick rolling with: %s\n", tmp);
       ret = strlen(tmp);
   }

   free(tmp); 
   unlink(name);

   return ret;
}

int main(int argc, char **argv)
{
  int result = detect(argv[0]);
  if (result < 0)
  {
     printf("[-] Detection failed\n");
     return 1;
  }
  else if (result == 0) 
  {
     printf("[-] Rick rolling kernel not found\n");
     return 2;
  }
  else
  {
     printf("[+] Rick rolling kernel found (path length = %d)\n", result);
  }

  return 0;
}
