//
// Created by Administrator on 2019/3/13.
//

#ifndef ENGINE_TOOLS_PIPESERVER_H
#define ENGINE_TOOLS_PIPESERVER_H
#include <cstring>
#include <pipe/base/include/PipeBase.h>
#include <protocol/json/include/json_cmd.h>
class PipeServer;
/**
 * 客户端上下文
 */
struct pipe_client_ctx{
    int client_id;
    char* process_name;
    uv_pipe_t client;
    PipeServer* $this;
};


typedef void (*on_server_event_cb)(PipeServer* server, const work_req_baton* result);

class PipeServer : public PipeBase {
public:
    /**
    * 注册回调函数
    * @param event_name
    * @param event_read_cb
    */
    void on(const char* event_name,on_server_event_cb event_read_cb);
    int send(const char* json,uv_stream_t* write_to);
protected:
    void registor() override;
    /**
     * 连接监听
     * @return
     */
    int listen();

    void shutdown(uv_signal_t *handle, int signum) override;

    /**
     * 客户端连接回调，将调用onConnect方法
     * @param server
     * @param status
     */
    static void onConnectCb(uv_stream_t* pipe_stream,int status);
    /**
     * ,child to-be override
     * @param server
     * @param status
     */
    virtual void onConnect(uv_stream_t* server,int status);
    /**
     * 客户端连接数据释放
     * @param handle
     */
    static void afterClientCloseCb(uv_handle_t* handle);

    void onPipeRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
    /**
     * 数据读取回调，将调用onPipeRead
     * @param client
     * @param nread
     * @param buf
     */
    static void onPipeReadCb(uv_stream_t* client,ssize_t nread,const uv_buf_t* buf);

    /**
     * 管道写入回调,将调用onWrite
     * @param req
     * @param status
     */
    static void onWriteCb(uv_write_t *req, int status);

    void onWork(uv_work_t *req) override;

    void onWorkAfter(uv_work_t *req, int status) override;

private:
    map<int,pipe_client_ctx*> clients;
    /**
     * 定义一个Map用来存放函数名和函数地址
     */
    map<string,on_server_event_cb> event_map;

    uv_mutex_t write_lock;
};

#endif //ENGINE_TOOLS_PIPESERVER_H
