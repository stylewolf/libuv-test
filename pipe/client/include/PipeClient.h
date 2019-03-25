//
// Created by Administrator on 2019/3/25.
//

#ifndef ENGINE_TOOLS_PIPECLIENT_H
#define ENGINE_TOOLS_PIPECLIENT_H


#include <pipe/base/include/PipeBase.h>
#include <protocol/json/include/json_cmd.h>
#include <cstring>
#include <unistd.h>
class PipeClient;
typedef void (*on_client_event_cb)(PipeClient* client, const work_req_baton* result);
class PipeClient: public PipeBase {
public:
    /**
    * 注册回调函数
    * @param event_name
    * @param event_read_cb
    */
    void on(const char* event_name,on_client_event_cb event_read_cb);

    void waitClientReady();

    int send(const char* json);
protected:
    void onPipeRead(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);
    /**
     * 数据读取回调，将调用onPipeRead
     * @param client
     * @param nread
     * @param buf
     */
    static void onPipeReadCb(uv_stream_t* server,ssize_t nread,const uv_buf_t* buf);

    /**
     * 管道写入回调,将调用onWrite
     * @param req
     * @param status
     */
    static void onWriteCb(uv_write_t *req, int status);

    void onWork(uv_work_t *req) override;

    void onWorkAfter(uv_work_t *req, int status) override;

    void registor() override;

    /**
     * 连接回调，将调用onConnect方法
     * @param server
     * @param status
     */
    static void onConnectCb(uv_connect_t* req, int status);
    /**
     * ,child to-be override
     * @param server
     * @param status
     */
    virtual void onConnect(uv_stream_t* server,int status);


protected:
    uv_connect_t pipe_connect;
    bool ready;
    /**
     * 定义一个Map用来存放函数名和函数地址
     */
    map<string,on_client_event_cb> event_map;
};


#endif //ENGINE_TOOLS_PIPECLIENT_H
