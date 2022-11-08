#ifndef _WEB_ASSETS_H_
#define _WEB_ASSETS_H_

#ifdef __cplusplus
extern "C" {
#endif

static const unsigned char web_assets_zip[] = {
    0x00,
};

void get_web_assets(const unsigned char** buf, unsigned int* buf_len)
{
    if (0 != buf)       *buf = web_assets_zip;
    if (0 != buf_len)   *buf_len = sizeof(web_assets_zip);
}

#ifdef __cplusplus
};
#endif

#endif
