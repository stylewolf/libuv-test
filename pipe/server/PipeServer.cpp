//
// Created by Administrator on 2019/3/13.
//

#include <pipe/server/include/PipeServer.h>
#include <unistd.h>
#include <assert.h>

void PipeServer::on(const char *event_name, on_server_event_cb event_read_cb) {
    event_map[event_name] = event_read_cb;
}

int PipeServer::send(const char *json,uv_stream_t* write_to) {
    int ret = 0;
    uv_mutex_lock(&write_lock);
    write_baton* baton = (write_baton*)malloc(sizeof(write_baton));
    if(write_to->type==UV_NAMED_PIPE){
        string json_string = string(json);
        json_string.push_back('\0');
        baton->buf = uv_buf_init(const_cast<char*>(json_string.data()), json_string.length());
        baton->write_req.data = baton;
        try{
            ret = uv_write(&baton->write_req, write_to, &baton->buf, 1, onWriteCb);
        }catch (exception& e){
            cerr<<e.what()<<endl;
        }
    }else{
        cout<<"write_to->type!=UV_NAMED_PIPE:"<<json<<endl;
    }
    uv_mutex_unlock(&write_lock);
    return ret;
}

//protected
void PipeServer::registor() {
    PipeBase::registor();
    uv_fs_t req;
    uv_fs_unlink(&root_loop, &req, pipe_name, NULL);
    listen();
    on(cmd_connect,[](PipeServer* server,const work_req_baton* result){
        //回复ready
        struct json_object *ready_json = json_object_new_object();
        json_object_object_add(ready_json, json_name_process, json_object_new_string(server->process_name));
        json_object_object_add(ready_json, json_name_cmd, json_object_new_string(cmd_ready));
        const char *ready_json_string = json_object_to_json_string(ready_json);
        server->send(ready_json_string,result->write_to);
        json_object_put(ready_json);
    });
    uv_mutex_init(&write_lock);
}

int PipeServer::listen() {
    int ret = uv_pipe_init(&root_loop, &uv_pipe, 1);
    if (ret != 0) {
        uv_out("初始化管道失败", ret);
        return ret;
    }
    uv_pipe.data = this;

    ret = uv_pipe_bind(&uv_pipe, pipe_name);
    if (ret != 0) {
        uv_out("创建管道绑定失败", ret);
        return ret;
    }
    //pipe监听
    ret = uv_listen((uv_stream_t *) &uv_pipe, 64, onConnectCb);
    if (ret != 0) {
        uv_out("服务端监听开启失败", ret);
        return ret;
    }
    uv_out("服务端监听开启", ret);
    return ret;
}

void PipeServer::shutdown(uv_signal_t *handle, int signum) {
    PipeBase::shutdown(handle, signum);
    uv_fs_t req;
    int ret = uv_fs_unlink(handle->loop, &req, pipe_name, NULL);
    uv_out("进程关闭:", ret);
    cout << "运行时间：" << time(0) - running_time << "秒" << endl;
    exit(0);
}

void PipeServer::onConnectCb(uv_stream_t *pipe_stream, int status) {
    PipeServer* server = ( PipeServer*)pipe_stream->data;
    server->onConnect(pipe_stream,status);
}

void PipeServer::onConnect(uv_stream_t *server, int status) {
    if (status) {
        uv_out("连接失败！", status);
        return;
    }
    //客户端上下文
    //remember [free ctx]
    struct pipe_client_ctx *ctx = (pipe_client_ctx *) malloc(sizeof(pipe_client_ctx));
    ctx->client.data = ctx;
    ctx->$this = this;
    int ret = uv_pipe_init(&root_loop, &ctx->client, 1);
    if (ret) {
        uv_out("初始化客户端连接失败", ret);
        return;
    }
    ret = uv_accept(server, (uv_stream_t *) &ctx->client);
    if (ret) {
        uv_out("客户端连接失败", ret);
        return;
    }
    ret = uv_read_start((uv_stream_t *) &ctx->client, allocPipeBufferCb, onPipeReadCb);
    if (ret) {
        uv_out("监听客户端状态失败", ret);
    }
}

void PipeServer::onPipeRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    cout<<"onPipeRead:"<<nread<<":"<< strlen(buf->base)<<"--"<<buf->base<<endl;

    pipe_client_ctx *ctx = (pipe_client_ctx *) client->data;
    if (ctx == nullptr) {
        return;
    }
    if (nread < 0) {
        /* Error or EOF */
        if (nread == UV_EOF) {
            cout << "client:" << ctx->client_id << "disconnect!" << endl;
        } else if (nread == UV_ECONNRESET) {
            cout << "client:" << ctx->client_id << "connection reset!" << endl;
        } else {
            cout << "client:" << ctx->client_id << "read error!" << endl;
        }
//        free(buf->base);
        uv_close((uv_handle_t *) &ctx->client, afterClientCloseCb);
    } else if (0 == nread) {
        /* Everything OK, but nothing read. */
//        free(buf->base);
    } else {
        int pos = 0;
        cout<<"buf->base:"<<buf->base<<endl;
        for (int i = 0; i < nread; ++i) {
            //处理多组数据
            if (buf->base[i] == '\0'){
                int size = i - pos + 1;
                char temp[size];
                memset(temp,'\0', size);
                strncpy(temp,buf->base + pos,size);

                pos = i + 1;
                //remember free wrb
                work_req_baton* wrb = (work_req_baton*)malloc(sizeof(work_req_baton));
                wrb->$this = this;
                wrb->write_to = client;
                //remember free temp_json
                wrb->json = (char*)malloc(strlen(temp) + 1);
                memset(wrb->json,'\0',strlen(temp) + 1);
                memcpy(wrb->json, temp, strlen(temp));
                wrb->work_req.data = wrb;
                uv_queue_work(&root_loop, &wrb->work_req, onWorkCb, onWorkAfterCb);
            }
        }
        free(buf->base);
    }
}

void PipeServer::onPipeReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    pipe_client_ctx *ctx = (pipe_client_ctx *) client->data;
    ctx->$this->onPipeRead(client, nread, buf);
}

void PipeServer::afterClientCloseCb(uv_handle_t *handle) {
    pipe_client_ctx *ctx = (pipe_client_ctx *) handle->data;
    try{
        //从客户端容器移除
        if (ctx->$this->clients.count(ctx->client_id) > 0) {
            ctx->$this->clients.erase(ctx->client_id);
        }

    }catch (exception& e){

    }
    //[free ctx]
    free(ctx);
}

void PipeServer::onWriteCb(uv_write_t *req, int status) {
    write_baton *wb = (write_baton *)req->data;
    //[free write_baton]
    free(wb);
}

void PipeServer::onWork(uv_work_t *req) {
    work_req_baton* wrb = (work_req_baton*)req->data;
    struct json_object *json = json_tokener_parse(wrb->json);
    try{
        json_object *tmpjson = NULL;
        json_object_object_get_ex(json, json_name_cmd, &tmpjson);
        if (tmpjson != nullptr) {
            const char *cmd_name = json_object_get_string(tmpjson);
            if(event_map.count(cmd_name)>0){
                event_map[cmd_name](this,wrb);
            }
        }
    }catch (exception& e){

    }
    json_object_put(json);
}

void PipeServer::onWorkAfter(uv_work_t *req, int status) {
    work_req_baton* wrb = (work_req_baton*)req->data;
//   [free temp_json]
    free(wrb->json);
//    [free wrb]
    free(wrb);
}
