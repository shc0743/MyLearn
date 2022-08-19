#pragma once
#ifndef __cplusplus
#error C++ required
#endif
// https://cnblogs.com/lrxing/p/5535601.html
#include <string>

/*base_64.h�ļ�*/
#ifndef BASE_64_H
#define BASE_64_H
/**
 * Base64 ����/����
 * @author liruixing
 */
class Base64 {
private:
    std::string _base64_table;
    static const char base64_pad = '=';
public:
    Base64() {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*����Base64����ʹ�õı�׼�ֵ�*/
    }
    /**
     * ���������unsigned���ͣ�����������ĵ�ʱ�����
     */
    std::string Encode(const unsigned char* str, size_t bytes);
    std::string Decode(const char* str, size_t bytes);
    //void Debug(bool open = true);
};
#endif

