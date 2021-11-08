#include <time.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <android/log.h>

#include "Image.h"

extern char* gLoadBuffer;
extern long gLoadFileSize;
extern int gLoadError;

extern BUFFMNG* gBuffMng;
extern long gBuffNum;

extern int gCancel;

extern char gDitherX_3bit[8][8];
extern char gDitherX_2bit[4][4];
extern char gDitherY_3bit[8];
extern char gDitherY_2bit[4];

extern "C"
{
//#define XMD_H
#include "../libwebp121/src/webp/decode.h"
}

int LoadImageWebp(IMAGEDATA* pData, int page, int scale)
{
    LOGI("LoadImageWebp : Start Webp");
    VP8StatusCode ret_; /* webp関数の戻り値格納 */
    WebPBitstreamFeatures features; /* 入力webpファイルの情報 */
    uint8_t* decodedData; /* 入力webpデコードデータ */
    //int width; /* webpデコードデータの幅 */
    //int height; /* webpデコードデータの高さ */

    if (gLoadBuffer == NULL) {
        LOGE("LoadImageWebp : [error] gLoadBuffer is null\n");
        return -1;
    }

    //unsigned char* buffer[1];

    /* WebPデータの情報取得 */
    ret_ = WebPGetFeatures((uint8_t*)gLoadBuffer, gLoadFileSize, &features);
    if (ret_ != VP8_STATUS_OK) {
        LOGE("LoadImageWebp : [error] WebPGetFeatures error\n");
        return -1;
    }
    int width = features.width;
    int height = features.height;
    int ch = 3;
    if (features.has_alpha != 0) {
        LOGI("LoadImageWebp : Load Alpha Webp\n");
        ch = 4;
    }

    /* デコード */
    LOGI("LoadImageWebp : Decode phase\n");
    if (features.has_alpha == 0) {
        decodedData = WebPDecodeRGB((uint8_t*)gLoadBuffer, gLoadFileSize, &width, &height);
    }
    else {
        decodedData = WebPDecodeRGBA((uint8_t*)gLoadBuffer, gLoadFileSize, &width, &height);
    }
    LOGI("LoadImageWebp : Decode Finished\n");

    /* デコードデータ用メモリ確保 */
    //LOGI("LoadImageWebp : Get Memory phase\n");
    //int buffsize = width * height * ch;
    //unsigned char *pSample = (unsigned char*)malloc(buffsize * sizeof(unsigned char));
    //if (pSample == NULL) {
        //LOGE("LoadImageWebp : [error] data malloc error\n");
        //LOGD("LoadImageWebp : MAlloc Error. size=%d", sizeof(unsigned char) * width * height * ch);
        //WebPFree(decodedData);
        //return -7;
    //}

    //buffer[0] = decodedData;

    int buffindex = -1;
    int buffpos = 0;
    int linesize = (width + HOKAN_DOTS);
    int ret = 0;
    WORD* buffptr = NULL;

    LOGI("LoadImageWebp : Height -> %d", height);
    for (int yy = 0; yy < height; yy++)
    {
        if (gCancel) {
            LOGD("LoadImageWebp : cancel.");
            ReleaseBuff(page, -1, -1);
            ret = -9;
            break;
        }

        // ライン毎のバッファの位置を保存
        if (buffindex < 0 || BLOCKSIZE - buffpos < linesize) {
            for (buffindex++; buffindex < gBuffNum; buffindex++) {
                if (gBuffMng[buffindex].Page == -1) {
                    break;
                }
            }
            if (buffindex >= gBuffNum) {
                // 領域不足
                ret = -10;
                break;
            }
            buffpos = 0;
            gBuffMng[buffindex].Page = page;
            gBuffMng[buffindex].Type = 0;
            gBuffMng[buffindex].Half = 0;
            gBuffMng[buffindex].Size = 0;
            gBuffMng[buffindex].Index = 0;
        }

        buffptr = gBuffMng[buffindex].Buff + buffpos + HOKAN_DOTS / 2;
        //		LOGD("DEBUG2:yy=%d, idx=%d, pos=%d", yy, buffindex, buffpos);

        // データセット
        int yd3 = gDitherY_3bit[yy & 0x07];
        int yd2 = gDitherY_2bit[yy & 0x03];
        int rr, gg, bb;

        for (int xx = 0; xx < width; xx++) {
            //alpha はスキップ
            rr = decodedData[(yy * width * ch) + (ch * xx)];
            gg = decodedData[(yy * width * ch) + (ch * xx) + 1];
            bb = decodedData[(yy * width * ch) + (ch * xx) + 2];

            LOGI("%d", yy);
            LOGI("RGB : rr=%02x, gg=%02x, bb=%02x \n", (int)decodedData[(yy * width * ch) + (ch * xx)], decodedData[(yy * width * ch) + (ch * xx)], (int)decodedData[(yy * width * ch) + (ch * xx)]);

            // 切り捨ての値を分散
            if (rr < 0xF8) {
                rr = rr + gDitherX_3bit[rr & 0x07][(xx + yd3) & 0x07];
            }
            if (gg < 0xFC) {
                gg = gg + gDitherX_2bit[gg & 0x03][(xx + yd2) & 0x03];
            }
            if (bb < 0xF8) {
                bb = bb + gDitherX_3bit[bb & 0x07][(xx + yd3) & 0x07];
            }
            buffptr[xx] = MAKE565(rr, gg, bb);
        }

        // 補完用の余裕
        buffptr[-2] = buffptr[0];
        buffptr[-1] = buffptr[0];
        buffptr[width + 0] = buffptr[width - 1];
        buffptr[width + 1] = buffptr[width - 1];

        // go to next line
        buffpos += linesize;
        gBuffMng[buffindex].Size += linesize;
    }
    pData->UseFlag = 1;
    pData->OrgWidth = width;
    pData->OrgHeight = height;

    WebPFree(decodedData);
    return ret;
}