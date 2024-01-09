#include "kallsyms_parse.h"
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/fs_struct.h>
#include <linux/path.h>

#define PARSE_BUFFER_SIZE 1024

int get_kallsyms_path(char *file_path, int size)
{
    struct new_utsname *buf;
    int ret = 0;
    buf = utsname();
    //ret = snprintf(file_path, size, "/boot/System.map-%s.test", buf->release);
    ret = snprintf(file_path, size, "/boot/System.map-%s", buf->release);
    if (ret < 0)
    {
        return -1;
    }
    else if (ret == size)
    {
        return -1;
    }
    return 0;
}

unsigned long parse_one_line(char *line, int line_size, const char *symbol_name)
{
    char line_buf[PARSE_BUFFER_SIZE] = {0};
    unsigned long addr = 0;

    if (line_size > (PARSE_BUFFER_SIZE - 1))
    {
        pr_info("Line: %.*s out of bufer size %d", (int)strnlen(line_buf, line_size), line_buf, PARSE_BUFFER_SIZE - 1);
        return 0;
    }
    memcpy(line_buf, line, line_size);
    line_buf[line_size] = '\0';
    //pr_info("Line: %.*s", (int)strnlen(line_buf, line_size), line_buf);
    if (NULL == strstr(line_buf, symbol_name))
    {
        return 0;
    }
    sscanf(line, "%lx", &addr);
    return addr;
}

static int read_file(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    mm_segment_t oldfs;
    int bytes_read = 0;

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    bytes_read = vfs_read(filp, buf, count, f_pos);

    set_fs(oldfs);

    return bytes_read;
}

/**
 * @brief 从缓存中解析多行数据
 *
 * @param buffer 数据缓存
 * @param length [in/out] 返回还剩余的长度
 * @param symbol_name
 * @return unsigned long
 */
static unsigned long process_lines(char *buffer, int *length, const char *symbol_name)
{
    unsigned long address = 0;
    char *line = buffer;
    while (*length > 0)
    {
        line = strchr(line, '\n');
        if (line == NULL)
        {
            break;
        }
        // 解析
        address = parse_one_line(buffer, line - buffer, symbol_name);
        if (address)
        {
            return address;
        }

        // 跳过换行
        line++;
        *length -= (line - buffer);
        buffer = line;
    }

    return 0;
}

unsigned long kallsyms_lookup_name_from_file(const char *symbol_name, const char* file_path)
{
    unsigned long address = 0;
    struct file *file = NULL;
    char buffer[PARSE_BUFFER_SIZE];
    int bytes_read = 0;
    int used = 0;
    int remaining = 0;
    int valid_len = 0;

    file = filp_open(file_path, O_RDONLY, 0);
    if (IS_ERR(file))
    {
        pr_info("Failed to open file %s\n", file_path);
        // return PTR_ERR(file);
        return 0;
    }

    // Read and process the file line by line
    do
    {
        bytes_read = read_file(file, buffer + valid_len, PARSE_BUFFER_SIZE - 1 - valid_len, &file->f_pos);
        if (bytes_read > 0)
        {
            valid_len += bytes_read;
            buffer[valid_len] = '\0';
            remaining = valid_len;
            address = process_lines(buffer, &remaining, symbol_name);
            if (address)
            {
                goto out_label;
            }
            if (valid_len == remaining)
            {
                // 说明缓存小了，单行都超过buffer了
                pr_err("line out of buffer size %d\n", PARSE_BUFFER_SIZE - 1);
                goto out_label;
            }
            used = (valid_len - remaining);
            memmove(buffer, buffer + used, remaining);
            valid_len = remaining;
        }
        else
        {
            pr_err("read file %s failed  %d\n", file_path, bytes_read);
            goto out_label;
        }
    } while (bytes_read > 0);

out_label:
    filp_close(file, NULL);
    return address;
}