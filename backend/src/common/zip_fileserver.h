#ifndef _ZIP_FILE_SERVER_H_
#define _ZIP_FILE_SERVER_H_

#include <string>
#include <vector>
#include <regex>

#include "zip_filesystem.h"

class ZipFileServer
{
protected:
    FileSystem* fs_handle;
    struct MountPointEntry {
        std::string mount_point;
        std::string base_dir;
    };
    std::vector<MountPointEntry> base_dirs_;

public:
    ZipFileServer(): fs_handle(nullptr) {}
    void set_fs_handle(FileSystem* fs) {
        fs_handle = fs;
    }
    bool set_fs_base_dir(const std::string &mount_point){
        return set_fs_mount_point(mount_point, "./");
    }
    bool set_fs_mount_point(const std::string &mount_point, const std::string &dir){
        std::string mnt = !mount_point.empty() ? mount_point : "/";
        if (!mnt.empty() && mnt[0] == '/') {
            base_dirs_.push_back({mnt, std::string("@@@@") + dir});
            return true;
        }
        return false;
    }
    bool read_file_content(const std::string& url, std::string& type_, std::string& body_);
};

#endif
