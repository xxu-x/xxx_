#include "../sylar/bytearray.h"
#include "../sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test(){
#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i){ \
        vec.push_back(rand()); \
    } \
    sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len)); \
    for (auto& i : vec){  \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i){ \
        type v = ba->read_fun(); \
        SYLAR_ASSERT(v == vec[i]);  \
    } \
    SYLAR_ASSERT(ba->getReadSize() == 0); \
    SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                            " (" #type " ) len=" << len \
                            << " base_len=" << base_len \
                            << " size=" << ba->getSize(); \
}
    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 4);
    XX(int16_t, 100, writeFint16, readFint16, 4);
    XX(uint16_t, 100, writeFuint16, readFuint16, 4);
    XX(int32_t, 100, writeFint32, readFint32, 4);
    XX(uint32_t, 100, writeFuint32, readFuint32, 4);
    XX(int64_t, 100, writeFint64, readFint64, 4);
    XX(uint64_t, 100, writeFuint64, readFuint64, 4);
#undef XX
}

int main(){

    test();

    return 0;
}