//
// Created by Administrator on 2019/3/13.
//

#ifndef ENGINE_TOOLS_PIPEBASE_H
#define ENGINE_TOOLS_PIPEBASE_H
#include <iostream>
#include <uv.h>
#include <json.h>
#include <map>

using namespace std;


class PipeBase;

struct write_baton{
    uv_write_t write_req;
    uv_buf_t buf;
    PipeBase* $this;
};
struct work_req_baton{
    PipeBase *$this;
    uv_work_t work_req;
    uv_stream_t* write_to;
    char* json;
    char* cmd_name;
};
/**
 * 异步监听处理器
 */
typedef void (*on_event_cb)(const work_req_baton* result);

class PipeBase {
public:
    /**
     * 启动
     * @param pipe_name 管道名称
     * @param process_name 进程名称
     * @return
     */
    int start(const char* pipe_name,const char* process_name);
    /**
     * attach 后启动
     * @return
     */
    int start();
    /**
     * 将服务绑定至已有的loop
     * @param pipe_name  管道名称
     * @param process_name  进程名称
     * @param attach_uv_loop 外部循环
     * @return
     */
    int attach(const char* pipe_name,const char* process_name,uv_loop_t attach_uv_loop);
    /**
     * 将服务绑定至已有的loop
     * @param pipe_name  管道名称
     * @param process_name  进程名称
     * @param attach_uv_loop 外部循环
     * @return
     */
    int attachStart(const char* pipe_name,const char* process_name,uv_loop_t attach_uv_loop);


    /**
     * 获取进程pid
     * @return
     */
    int getPid() const;

    char* getProcessName();

protected:
    /**
     * 注册handle，无需显示调用,child to-be override
     * @return
     */
    virtual void registor();

     /**
     * 关闭时处理,child to-be override
     * @param handle
     * @param signum
     * @return
     */
     virtual void shutdown(uv_signal_t *handle, int signum);
    /**
     * 打印uv信息
     * @param ret
     */
    void uv_out(const char*,int ret);

    /**
     * 缓冲区分配
     * @param handle
     * @param suggested_size
     * @param buf
     */
    static void allocPipeBufferCb(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf);
    /**
     * 工作队列处理
     * @param req
     */
    static void onWorkCb(uv_work_t* req);
    virtual void onWork(uv_work_t* req);

    static void onWorkAfterCb(uv_work_t* req, int status);
    virtual void onWorkAfter(uv_work_t* req, int status);
protected:
    uv_loop_t root_loop;
    /**
     * 管道名称
     */
    char* pipe_name;

    /**
     * 进程名称
     */
    char* process_name;


    /**
     * 程序退出信号
     */
    uv_signal_t shutdown_signal;
    /**
     * 是否显示输出
     */
    int show_uv_out = 1;
    /**
     * 客户端或服务器管道
     */
    uv_pipe_t uv_pipe;
    /**
     * 运行时间
     */
    long long int running_time;
private:
    /**
     * 注册关闭时信号处理
     * @param flag 1执行
     * @return
     */
    int registorShutdownSignal();
    /**
     * 程序关闭回调
     * @param handle
     * @param signum
     */
    static void shutdownSignalCb(uv_signal_t *handle, int signum);

private:
    int pid;
};



#endif //ENGINE_TOOLS_PIPEBASE_H
