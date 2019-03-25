#define  FILE_NAME_LENGTH            256
//#define DEBUG 1
//#ifdef DEBUG
#define  OUTPUT_FILE                 "/media/mmcblk0p1/uc/leak_info.txt"      //存放内存泄露的信息
#define  malloc(size)                xmalloc (size, __FILE__, __LINE__)   //重新实现malloc、calloc和free
#define  calloc(elements, size)      xcalloc (elements, size, __FILE__, __LINE__)
#define  free(mem_ref)               xfree(mem_ref)
//#endif
struct _MEM_INFO   //一次分配的内存信息
{
    void            *address;                       //分配的地址
    unsigned int    size;                           //地址大小
    char            file_name[FILE_NAME_LENGTH];    //在哪个文件申请
    unsigned int    line;                           //行数
};
typedef struct _MEM_INFO MEM_INFO;

struct _MEM_LEAK {                  //将所有malloc和calloc申请的内存串起来
    MEM_INFO mem_info;
    struct _MEM_LEAK * next;
};
typedef struct _MEM_LEAK MEM_LEAK;

void add(MEM_INFO alloc_info);
void erase(unsigned pos);
void clear(void);

void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
void xfree(void * mem_ref);

void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line);
void remove_mem_info (void * mem_ref);
void report_mem_leak(void);             //在atexit注册的终止函数