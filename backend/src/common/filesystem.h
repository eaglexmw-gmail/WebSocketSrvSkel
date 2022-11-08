#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <stdint.h>

#ifndef FILE_SIZE_TYPE
#define FILE_SIZE_TYPE uint32_t
#endif

#ifndef FILE_TIME_TYPE
#define FILE_TIME_TYPE uint32_t
#endif

#ifndef FILE_MODE_TYPE
#define FILE_MODE_TYPE uint32_t
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode) & 0170000) == (0040000))
#endif

// 目录信息
typedef struct DirInfo
{
    // 文件名
    const char*     name;
    // 文件模式：权限、是否目录等
    FILE_MODE_TYPE  mode;
    // 文件访问时间、修改时间、属性修改时间
    FILE_TIME_TYPE  atime, ctime, mtime;
}DirInfo;

// 文件信息
typedef struct FileInfo : public DirInfo
{
    // 文件大小
    FILE_SIZE_TYPE  file_size;
}FileInfo;

typedef bool (*WalkDirCallback)(const char* folder, const char* file, void* This);

class File
{
public:
    virtual ~File() = default;
    // 返回FileInfo
    virtual uint32_t stat(FileInfo* fi) = 0;
    // 读取多少个字节
    virtual uint32_t read(uint8_t* buf, FILE_SIZE_TYPE buf_len) = 0;
    // 关闭文件句柄
    virtual uint32_t close() = 0;
};

class FileWritable : public File
{
public:
    virtual ~FileWritable() = default;
    // 写入多少内容
    virtual uint32_t write(const uint8_t* buf, FILE_SIZE_TYPE buf_len) = 0;
};

class Folder
{
public:
    virtual ~Folder() = default;
    // 读取一个文件项（也可能是目录）
    virtual uint32_t read(FileInfo* info) = 0;
    // 关闭文件句柄
    virtual uint32_t close() = 0;
};

class FolderWritable : public Folder
{
public:
    virtual ~FolderWritable() = default;
    // 创建文件（文件0字节）、目录
    virtual uint32_t create(const char* path, FILE_MODE_TYPE mode) = 0;
    // 删除文件/目录
    virtual uint32_t remove(const char* path) = 0;
};

class FileSystem
{
public:
    virtual ~FileSystem() = default;
    // 返回File句柄
    virtual uint32_t open(const char* file_name, File** handle) = 0;
    // 返回Folder句柄
    virtual uint32_t opendir(const char* folder_name, Folder** handle) = 0;
    // 文件、目录是否存在
    virtual bool is_exist(const char* path) = 0;
    // 遍历文件夹
    virtual bool walk_dir(const char* folder, WalkDirCallback fn, void* This) = 0;
};

class FileSystemWritable : public FileSystem
{
public:
    virtual ~FileSystemWritable() = default;
    // 返回File句柄
    virtual uint32_t open2(const char* file_name, FileWritable** handle) = 0;
    // 返回Folder句柄
    virtual uint32_t opendir2(const char* folder_name, FolderWritable** handle) = 0;
};

#endif
