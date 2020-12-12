#include "fs-creator.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

static const int32_t kTmpLen = 1024;

static char _tmpb[kTmpLen];


std::string str_toupper(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c)
  {
    return std::toupper(c);
  });
  return s;
}


std::string str_tolower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c)
  {
    return std::tolower(c);
  });
  return s;
}

FsCreator::FsCreator()
{
}

bool FsCreator::PrepareDirectory(std::string drvname, std::string basepath, bool rw)
{
  bool ret = false;

  // find free directory
  struct stat info;

  std::string work_dir_path;

  for (int32_t dirnum = 0; dirnum < 1000; dirnum++)
  {
    snprintf(_tmpb, kTmpLen, "%03d", dirnum);
    work_dir_path = basepath + "/" + _tmpb;

    if (stat(work_dir_path.c_str(), &info) != 0)
    {
      if (std::filesystem::create_directory(work_dir_path))
      {
        ret = true;
        break;
      }
    }
    else if (info.st_mode & S_IFDIR)
    {
      // directory exists, try next num
      continue;
    }
    else
    {
      if (std::filesystem::create_directory(work_dir_path) != 0)
      {
        ret = false;
        break;
      }
    }
  }

  if (ret)
  {
    // directory valid and exists, set all the values
    FS.drvname = drvname;
    FS.DRVNAME = str_toupper(drvname);

    std::string ldrvname = str_tolower(drvname);

    FS.core_h.dir = work_dir_path;
    FS.core_h.fname = ldrvname + ".h";
    FS.core_h.fpath = work_dir_path + "/" + FS.core_h.fname;

    FS.core_c.dir = work_dir_path;
    FS.core_c.fname = ldrvname + ".c";
    FS.core_c.fpath = work_dir_path + "/" + FS.core_c.fname;

    FS.util_h.dir = work_dir_path;
    FS.util_h.fname = ldrvname + "_binutil" + ".h";
    FS.util_h.fpath = work_dir_path + "/" + FS.util_h.fname;

    FS.util_c.dir = work_dir_path;
    FS.util_c.fname = ldrvname + "_binutil" + ".c";
    FS.util_c.fpath = work_dir_path + "/" + FS.util_c.fname;

    snprintf(_tmpb, kTmpLen, "%s_USE_BITS_SIGNAL", FS.DRVNAME.c_str());
    FS.usebits_def = _tmpb;

    snprintf(_tmpb, kTmpLen, "%s_USE_DIAG_MONITORS", FS.DRVNAME.c_str());
    FS.usemon_def = _tmpb;

    snprintf(_tmpb, kTmpLen, "%s_USE_CANSTRUCT", FS.DRVNAME.c_str());
    FS.usesruct_def = _tmpb;
  }

  return ret;
}
