#include "zip_fileserver.h"

inline std::string file_extension(const std::string &path)
{
  std::smatch m;
  static auto re = std::regex("\\.([a-zA-Z0-9]+)$");
  if (std::regex_search(path, m, re)) { return m[1].str(); }
  return std::string();
}

inline constexpr unsigned int str2tag_core(const char *s, size_t l,
                                           unsigned int h) {
  return (l == 0) ? h
                  : str2tag_core(s + 1, l - 1,
                                 (h * 33) ^ static_cast<unsigned char>(*s));
}

inline unsigned int str2tag(const std::string &s) {
  return str2tag_core(s.data(), s.size(), 0);
}

namespace udl {

inline constexpr unsigned int operator"" _t(const char *s, size_t l) {
  return str2tag_core(s, l, 0);
}

} // namespace udl

static  const char * find_content_type(const std::string &path) {
    auto ext = file_extension(path);

    using udl::operator""_t;

    switch (str2tag(ext)) {
        default: return nullptr;
        case "css"_t: return "text/css";
        case "csv"_t: return "text/csv";
        case "htm"_t:
        case "html"_t: return "text/html";
        case "js"_t:
        case "mjs"_t: return "text/javascript";
        case "txt"_t: return "text/plain";
        case "vtt"_t: return "text/vtt";

        case "apng"_t: return "image/apng";
        case "avif"_t: return "image/avif";
        case "bmp"_t: return "image/bmp";
        case "gif"_t: return "image/gif";
        case "png"_t: return "image/png";
        case "svg"_t: return "image/svg+xml";
        case "webp"_t: return "image/webp";
        case "ico"_t: return "image/x-icon";
        case "tif"_t: return "image/tiff";
        case "tiff"_t: return "image/tiff";
        case "jpg"_t:
        case "jpeg"_t: return "image/jpeg";

        case "mp4"_t: return "video/mp4";
        case "mpeg"_t: return "video/mpeg";
        case "webm"_t: return "video/webm";

        case "mp3"_t: return "audio/mp3";
        case "mpga"_t: return "audio/mpeg";
        case "weba"_t: return "audio/webm";
        case "wav"_t: return "audio/wave";

        case "otf"_t: return "font/otf";
        case "ttf"_t: return "font/ttf";
        case "woff"_t: return "font/woff";
        case "woff2"_t: return "font/woff2";

        case "7z"_t: return "application/x-7z-compressed";
        case "atom"_t: return "application/atom+xml";
        case "pdf"_t: return "application/pdf";
        case "json"_t: return "application/json";
        case "rss"_t: return "application/rss+xml";
        case "tar"_t: return "application/x-tar";
        case "xht"_t:
        case "xhtml"_t: return "application/xhtml+xml";
        case "xslt"_t: return "application/xslt+xml";
        case "xml"_t: return "application/xml";
        case "gz"_t: return "application/gzip";
        case "zip"_t: return "application/zip";
        case "wasm"_t: return "application/wasm";
    }
}

bool ZipFileServer::read_file_content(const std::string& strUri, std::string& type_, std::string& rspBody)
{
    if (nullptr == fs_handle) {
      return false;
    }
    for (const auto &entry : base_dirs_) {
        // Prefix match, 
        if (strUri.compare(0, entry.mount_point.size(), entry.mount_point)) {
            continue;
        }
        std::string sub_path = strUri.substr(entry.mount_point.size());
        // in zip file system.
        auto path = entry.base_dir.substr(6) + sub_path;
        // need drop first / tag
        if (path[0] == '/') { path = path.substr(1);}
        // 
        if (path.back() == '/') { path += "index.html"; }
        if (path.length() == 0) { path = "index.html"; }
        // req的相应文件路径，在mount_point中，是否能够访问到
        if (!fs_handle->is_exist(path.c_str())) {
            continue;
        }
        File* zip_file = nullptr;
        // 打开文件
        if (0 != fs_handle->open(path.c_str(), &zip_file)) {
            continue;
        }
        FileInfo fi;
        memset(&fi, 0, sizeof(fi));
        // 获取解压后的大小
        if ((0 != zip_file->stat(&fi)) || (fi.file_size <= 0)) {
            // 释放文件句柄。
            zip_file->close();
            continue;
        }
        // 使用resize，这样同时会修改std::string的size
        rspBody.resize(static_cast<size_t>(fi.file_size));
        // 读取文件
        zip_file->read((uint8_t*)&rspBody[0], static_cast<std::streamsize>(fi.file_size));
        // 并解析ContentType
        auto type = find_content_type(path);
        if (type) { type_ = type; }
        // 释放文件句柄。
        zip_file->close();
        return true;
    }
    return false;
}
