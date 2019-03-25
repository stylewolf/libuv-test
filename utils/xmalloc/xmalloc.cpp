#include <stddef.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <utils/xmalloc/include/xmalloc.h>
using namespace std;
static MEM_LEAK * ptr_start = NULL;  //用这两个指针将malloc和calloc申请的内存串起来，只能本文件中使用
static MEM_LEAK * ptr_next =  NULL;

// 向队列中添加申请的内存信息
void add(MEM_INFO alloc_info)
{

    MEM_LEAK * mem_leak_info = NULL;
    mem_leak_info = (MEM_LEAK *) malloc (sizeof(MEM_LEAK));
    mem_leak_info->mem_info.address = alloc_info.address;
    mem_leak_info->mem_info.size = alloc_info.size;
    strcpy(mem_leak_info->mem_info.file_name, alloc_info.file_name);
    mem_leak_info->mem_info.line = alloc_info.line;
    mem_leak_info->next = NULL;

    if (ptr_start == NULL)
    {
        ptr_start = mem_leak_info;
        ptr_next = ptr_start;
    }
    else {
        ptr_next->next = mem_leak_info;
        ptr_next = ptr_next->next;
    }

}

//从队列中移除内存信息
void erase(unsigned pos)
{

    unsigned index = 0;
    MEM_LEAK * alloc_info, * temp;

    if(pos == 0)
    {
        MEM_LEAK * temp = ptr_start;
        ptr_start = ptr_start->next;
        free(temp);
    }
    else
    {
        for(index = 0, alloc_info = ptr_start; index < pos;
            alloc_info = alloc_info->next, ++index)
        {
            if(pos == index + 1)
            {
                temp = alloc_info->next;
                alloc_info->next =  temp->next;
                free(temp);
                break;
            }
        }
    }
}

//从队列中移除所有的内存信息
void clear()
{
    MEM_LEAK * temp = ptr_start;
    MEM_LEAK * alloc_info = ptr_start;

    while(alloc_info != NULL)
    {
        alloc_info = alloc_info->next;
        free(temp);
        temp = alloc_info;
    }
}

//对malloc的包装
void * xmalloc (unsigned int size, const char * file, unsigned int line)
{
    void * ptr = malloc (size);
    if (ptr != NULL)
    {
        add_mem_info(ptr, size, file, line);
    }
    return ptr;
}

//对calloc的包装
void * xcalloc (unsigned int elements, unsigned int size, const char * file, unsigned int line)
{
    unsigned total_size;
    void * ptr = calloc(elements , size);
    if(ptr != NULL)
    {
        total_size = elements * size;
        add_mem_info (ptr, total_size, file, line);
    }
    return ptr;
}


//对free的包装
void xfree(void * mem_ref)
{
    remove_mem_info(mem_ref);
    free(mem_ref);
}

//获取申请分配的内存信息，并将它添加到队列中
void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line)
{
    MEM_INFO mem_alloc_info;

    memset( &mem_alloc_info, 0, sizeof ( mem_alloc_info ) );
    mem_alloc_info.address     = mem_ref;
    mem_alloc_info.size = size;
    strncpy(mem_alloc_info.file_name, file, FILE_NAME_LENGTH);
    mem_alloc_info.line = line;

    add(mem_alloc_info);
}

//从队列中移除已分配的内存信息
void remove_mem_info (void * mem_ref)
{
    unsigned short index;
    MEM_LEAK  * leak_info = ptr_start;

    for(index = 0; leak_info != NULL; ++index, leak_info = leak_info->next)
    {
        if ( leak_info->mem_info.address == mem_ref )
        {
            erase ( index );
            break;
        }
    }
}

//回调函数，向文件中写入内存泄露的情况
void report_mem_leak(void)
{
    unsigned short index;
    MEM_LEAK * leak_info;

    FILE * fp_write = fopen (OUTPUT_FILE, "wt");
    char info[1024];
    cout<<"手机错误"<<endl;
    if(fp_write != NULL)
    {
        sprintf(info, "%s\n", "Memory Leak Summary");
        fwrite(info, (strlen(info) + 1) , 1, fp_write);
        sprintf(info, "%s\n", "-----------------------------------");
        fwrite(info, (strlen(info) + 1) , 1, fp_write);

        for(leak_info = ptr_start; leak_info != NULL; leak_info = leak_info->next)
        {
            sprintf(info, "address : %d\n", leak_info->mem_info.address);
            fwrite(info, (strlen(info) + 1) , 1, fp_write);
            sprintf(info, "size    : %d bytes\n", leak_info->mem_info.size);
            fwrite(info, (strlen(info) + 1) , 1, fp_write);
            sprintf(info, "file    : %s\n", leak_info->mem_info.file_name);
            fwrite(info, (strlen(info) + 1) , 1, fp_write);
            sprintf(info, "line    : %d\n", leak_info->mem_info.line);
            fwrite(info, (strlen(info) + 1) , 1, fp_write);
            sprintf(info, "%s\n", "-----------------------------------");
            fwrite(info, (strlen(info) + 1) , 1, fp_write);
        }
    }
    clear();
}