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
}

//32bit形式および24bit形式のビットマップ画像のみ対応。
int LoadImageBmp(IMAGEDATA* pData, int page, int scale)
{
    if (gLoadBuffer == NULL) {
        LOGE("LoadImageBmp : [error] gLoadBuffer is null\n");
        return -1;
    }
    if (gLoadBuffer[0] != 'B' || gLoadBuffer[1] != 'M') {
        LOGE("LoadImageBmp : [error] Bmp header is unknown\n");
        return -1;
    }

    char* ddata;
    ddata = &gLoadBuffer[0];
    int headerSize = (unsigned char)ddata[10] + (unsigned char)(ddata[11] << 8)
            + (unsigned char)(ddata[12] << 16) + (unsigned char)(ddata[13] << 24);
    //LOGI("LoadImageBmp : Bmp headerSize is %d\n", headerSize);

    int width = (unsigned char)ddata[18] + (unsigned char)(ddata[19] << 8)
            + (unsigned char)(ddata[20] << 16) + (unsigned char)(ddata[21] << 24);
    //LOGI("LoadImageBmp : Bmp width is %d\n", width);

    int height = (unsigned char)ddata[22] + (unsigned char)(ddata[23] << 8)
            + (unsigned char)(ddata[24] << 16) + (unsigned char)(ddata[25] << 24);
    //LOGI("LoadImageBmp : Bmp height is %d\n", height);

    int bits = (unsigned char)ddata[28] + (unsigned char)(ddata[29] << 8);
    //LOGI("LoadImageBmp : Bmp bits is %d\n", bits);

    int ch;
    int offsetCount = 0;
    if(bits == 24){
        ch = 3;
        //ビットマップは画像行データは4byte単位でなければならず、行データ最後尾の余分byteで調整されている
        //余分byte解析
        //参考：http://www.umekkii.jp/data/computer/file_format/bitmap.cgi
        while (true){
            if((width * ch + offsetCount) % 4 == 0) break;
            LOGI("LoadImageBmp : offsetCount++\n", bits);
            offsetCount++;
        }
    }
    else if(bits == 32){
        //alphaを含む
        ch = 4;
    }
    else{
        LOGE("LoadImageBmp : [error] Bmp bits is not 24bits or 32bits\n");
        return -1;
    }

    int buffindex = -1;
    int buffpos = 0;
    int linesize = (width + HOKAN_DOTS);
    int ret = 0;
    WORD* buffptr = NULL;


    //LOGI("LoadImageBmp : Height -> %d", height);
    for (int yy = 0; yy < height; yy++)
    {
        if (gCancel) {
            LOGD("LoadImageBmp : cancel.");
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
        // LOGD("DEBUG2:yy=%d, idx=%d, pos=%d", yy, buffindex, buffpos);

        // データセット
        int yd3 = gDitherY_3bit[yy & 0x07];
        int yd2 = gDitherY_2bit[yy & 0x03];
        int rr, gg, bb;
        LOGI("LoadImageBmp : decode start\n", bits);
        int buffsize = (width * ch + offsetCount) * (height - yy - 1);
        ddata = &gLoadBuffer[buffsize -1 + headerSize];

        for (int xx = 0; xx < width; xx++) {
            //alpha はスキップ
            if(ch == 4) ddata++;
            rr = (unsigned char)*ddata; ddata++;
            bb = (unsigned char)*ddata; ddata++;
            gg = (unsigned char)*ddata; ddata++;

            //LOGI("%d", yy);

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
            //LOGI("RGB : rr=%d, gg=%d, bb=%d \n", rr, gg, bb);
        }
        //for(int i = 0; i < offsetCount; i++, ddata++);

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

    return ret;
}