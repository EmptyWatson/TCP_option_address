#ifndef __KALLSYMS_PARSE_H__
#define __KALLSYMS_PARSE_H__
#include <linux/types.h>

/**
 * @brief Get the kallsyms path object
 *
 * @param file_path [out] 文件路径
 * @param size file_path缓存的长度
 * @return int 0-成功，其他值-失败
 */
int get_kallsyms_path(char *file_path, int size);

/**
 * @brief 从文件/boot/System.map-xxxx中查找符号地址
 *
 * @param symbol_name 符号名字
 * @param file_path 文件路径
 * @return uint64_t ：找到的地址，0-未找到
 */
uint64_t kallsyms_lookup_name_from_file(const char *symbol_name, const char* file_path);

#endif //__KALLSYMS_PARSE_H__