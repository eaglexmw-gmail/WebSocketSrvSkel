#ifndef _ZIP_FILE_SYSTEM_H_
#define _ZIP_FILE_SYSTEM_H_

#include "filesystem.h"

typedef void* ZIP_HANDLE;

// 这里只实现只读的Zip文件系统接口
class ZipFileSystem : public FileSystem
{
protected:
    ZIP_HANDLE zip_handle;

public:
    ZipFileSystem();

    bool init(const uint8_t* buf, uint32_t buf_len);

    virtual ~ZipFileSystem();

    // 返回File句柄
    virtual uint32_t open(const char* file_name, File** handle) override;
    // 返回Folder句柄
    virtual uint32_t opendir(const char* folder_name, Folder** handle) override;
    // 文件、目录是否存在
    virtual bool is_exist(const char* path) override;
    // 遍历文件夹
    virtual bool walk_dir(const char* folder, WalkDirCallback fn, void* This) override;
};

#endif
