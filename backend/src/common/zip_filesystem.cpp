#include "zip_filesystem.h"

//#include "vector_buffer.h"

#include <stdio.h>
#include <string.h>
#include <zip-unzip/Zip/unzip.h>

class ZipFile : public File
{
protected:
    int file_idx_;
    ZIP_HANDLE zip_handle_;
    ZIPENTRY zip_entry_;

public:
    explicit ZipFile(int file_idx, const ZIP_HANDLE zip_handle, ZIPENTRY& ze)
    : file_idx_(file_idx), zip_handle_(zip_handle), zip_entry_(ze)
    {
    }

    virtual ~ZipFile() {
    };
    // 返回FileInfo
    virtual uint32_t stat(FileInfo* fi) override {
        if (nullptr != fi) {
            fi->name = zip_entry_.name;
            fi->file_size = zip_entry_.unc_size;
        }
        return 0;
    }
    // 读取多少个字节
    virtual uint32_t read(uint8_t* buf, FILE_SIZE_TYPE buf_len) override {
        if (nullptr != buf){
            if (buf_len <= zip_entry_.unc_size) {
                return (uint32_t)UnzipItem((HZIP)zip_handle_, file_idx_, buf, buf_len);
            }
        }
        return 0;
    }
    // 关闭文件句柄
    virtual uint32_t close() override {
        delete this;
        return 0;
    }

    friend class ZipFileSystem;
};

class ZipFolder : public Folder
{
private:
    // vectorbuf<char> folder_name_;
    ZIP_HANDLE      zip_handle_;
    ZIPENTRY        zip_entry_;

public:
    explicit ZipFolder(const ZIP_HANDLE zip_handle, const char* folder_)
    : zip_handle_(zip_handle)
    {
#if 0
        unsigned int len_ = strlen(folder_);
        folder_name_.reserve(len_ + 1);
        strcpy(&(folder_name_[0]), folder_);
        folder_name_[len_] = '\0';
#endif
    }
    virtual ~ZipFolder() {

    };
    // 读取一个文件项（也可能是目录）
    virtual uint32_t read(FileInfo* info) override{
        return 0;
    }
    // 关闭文件句柄
    virtual uint32_t close() override {
        return 0;
    }

    friend class ZipFileSystem;
};

ZipFileSystem::ZipFileSystem()
  : zip_handle(nullptr)
{
}

ZipFileSystem::~ZipFileSystem()
{
    if (nullptr != zip_handle) {
        CloseZip((HZIP)zip_handle);
    }
}

bool ZipFileSystem::init(const uint8_t* buf, uint32_t buf_len)
{
    zip_handle = OpenZip((uint8_t*)buf, buf_len, 0);
    if (nullptr == zip_handle) {
        return false;
    }
    return true;
}

// 返回File句柄
uint32_t ZipFileSystem::open(const char* file_name, File** handle)
{
    // 从句柄中查找文件
    int file_idx = 0;
    ZIPENTRY ze;
    uint32_t ret = FindZipItem((HZIP)zip_handle, file_name, false, &file_idx, &ze);
    if (ZR_OK == ret) {
        if (nullptr != handle) {
            *handle = new ZipFile(file_idx, zip_handle, ze);
        }
    }

    return ret;
}

// 返回Folder句柄
uint32_t ZipFileSystem::opendir(const char* folder_name, Folder** handle)
{
    // 从句柄中查找文件夹
    int file_idx = 0;
    ZIPENTRY ze;
    uint32_t ret = FindZipItem((HZIP)zip_handle, folder_name, false, &file_idx, &ze);
    if (ZR_OK == ret) {
        if (nullptr != handle) {
            *handle = new ZipFolder(zip_handle, folder_name);
        }
    }
    return 0;
}

// 文件、目录是否存在
bool ZipFileSystem::is_exist(const char* path)
{
    if (nullptr == path) {
        return false;
    }
    
    // 过滤掉空目录
    if (path[0] == '\0') {
        return true;
    }
    // 过滤掉根目录
    if (path[1] == '\0') {
        if ( (path[0] == '.')||(path[0] == '/')||(path[0] == '\\')){
            return true;
        }
    }
    if (0 == FindZipItem((HZIP)zip_handle, path, false, nullptr, nullptr) ) {
        return true;
    }
    return false;
}

// 遍历文件夹
bool ZipFileSystem::walk_dir(const char* folder, WalkDirCallback fn, void* This)
{
    return false;
}

