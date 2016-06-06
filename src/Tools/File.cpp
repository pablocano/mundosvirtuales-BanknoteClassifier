/**
* @file Tools/File.cpp
* Declaration of class File for Windows and Linux.
*/

#ifdef WINDOWS
#include <windows.h>
//#include <direct.h>
#else
#include <cstdlib> // for getenv
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "File.h"


#ifdef WINDOWS
const char* File::getGTDir()
{
  static char dir[MAX_PATH] = {0};
  if(!dir[0])
  {
    // determine module file from command line
    const char* commandLine = GetCommandLine();
    size_t len;
    if(*commandLine == '"')
    {
      commandLine++;
      const char* end = strchr(commandLine, '"');
      if(end)
        len = end - commandLine;
      else
        len = strlen(commandLine);
    }
    else
      len = strlen(commandLine);
    if(len >= sizeof(dir) - 8)
      len = sizeof(dir) - 8;
    memcpy(dir, commandLine, len);
    dir[len] = '\0';

    // if there is no given directory, use the current working dir
    if(!strchr(dir, '\\'))
    {
      len = int(GetCurrentDirectory(sizeof(dir) - 9, dir));
      if(len && dir[len - 1] != '\\')
      {
        dir[len++] = '\\';
        dir[len] = '\0';
      }
    }

    //drive letter in lower case:
    if(len && dir[1] == ':')
      *dir |= tolower(*dir);

    // try to find the config directory
    char* end = dir + len - 1;
    for(;;)
    {
      if(*end == '/' || *end == '\\' || *end == ':' || end == dir - 1)
      {
        if(*end == ':')
          *(end++) = '\\';
        strcpy(end + 1, "Config");
        DWORD attr = GetFileAttributes(dir);
        if(attr != INVALID_FILE_ATTRIBUTES && attr & FILE_ATTRIBUTE_DIRECTORY)
        {
          end[end > dir ? 0 : 1] = '\0';
          for(; end >= dir; end--)
            if(*end == '\\')
              *end = '/';
          return dir;
        }
      }
      if(end < dir)
        break;
      end--;
    }
    ASSERT(false);
    strcpy(dir, ".");
  }
  return dir;
}
#endif

#if defined(LINUX) || defined(OSX)
const char* File::getGTDir()
{
  static char dir[FILENAME_MAX] = {0};
  if(!dir[0])
  {
    getcwd(dir, sizeof(dir) - 7);
    char* end = dir + strlen(dir) - 1;
    struct stat buff;;
    for(;;)
    {
      if(*end == '/' || end == dir - 1)
      {
        strcpy(end + 1, "Config");
        if(stat(dir, &buff) == 0)
          if(S_ISDIR(buff.st_mode))
          {
            end[end > dir ? 0 : 1] = '\0';
            return dir;
          }
      }
      if(end < dir)
        break;
      end--;
    }
    strcpy(dir, ".");
  }
  return dir;
}
#endif
