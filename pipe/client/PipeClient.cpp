//
// Created by Administrator on 2019/3/25.
//

#include <pipe/client/include/PipeClient.h>

void PipeClient::waitClientReady() {
    while(!ready){
        usleep(100*1000);
    }
}

int PipeClient::send(const char *json) {
    string json_string = string(json);
    json_string.push_back('\0');

    write_baton* baton = (write_baton*)malloc(sizeof(write_baton));
    baton->buf = uv_buf_init(const_cast<char*>(json_string.data()), json_string.length());
    baton->write_req.data = baton;
    cout<<"baton->buf.len:"<<baton->buf.len<< "uv_is_writable(pipe_connect.handle):"<<uv_is_active((uv_handle_t*)pipe_connect.handle)<<endl;

    int ret =  uv_write(&baton->write_req, pipe_connect.handle, &baton->buf, 1, onWriteCb);

    return ret;
}

void PipeClient::on(const char *event_name, on_client_event_cb event_read_cb) {
    event_map[event_name] = event_read_cb;
}

void PipeClient::registor() {
    int ret = uv_pipe_init(&root_loop, &uv_pipe, 1);
    uv_pipe.data = this;
    if (ret != 0) {
        uv_out("初始化管道失败", ret);
    }
    pipe_connect.data = this;
    uv_pipe_connect(&pipe_connect,&uv_pipe,pipe_name,onConnectCb);
    on(cmd_ready,[](PipeClient* client,const work_req_baton* result){
        client->ready = true;
    });
}

void PipeClient::onConnectCb(uv_connect_t *req, int status) {
    PipeClient* client = ( PipeClient*)req->data;
    client->onConnect(req->handle,status);
}

void PipeClient::onConnect(uv_stream_t *server, int status) {
    server->data = this;
    int ret = uv_read_start(server,allocPipeBufferCb,onPipeReadCb);
    if (ret != 0) {
        uv_out("客户端监听开启失败", ret);
    }
    struct json_object* connect_json = json_object_new_object();
    json_object_object_add(connect_json, json_name_process, json_object_new_string(process_name));
    json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_connect));
    json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
    const char* json = json_object_to_json_string(connect_json);
    send(json);
    json_object_put(connect_json);
}

void PipeClient::onPipeRead(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf) {
    cout<<"onPipeRead:"<<nread<<":"<< strlen(buf->base)<<"--"<<buf->base<<endl;
    if (nread < 0) {
        /* Error or EOF */
        if (nread == UV_EOF) {
            cout << "disconnect!" << endl;
        } else if (nread == UV_ECONNRESET) {
            cout << "connection reset!" << endl;
        } else {
            cout << "read error!" << endl;
        }
        //uv_close((uv_handle_t *)pipe_connect afterClientCloseCb);
    } else if (0 == nread) {
        /* Everything OK, but nothing read. */
//        free(buf->base);
    } else {
        int pos = 0;
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
                wrb->write_to = server;
                //remember free temp_json
                wrb->json = (char*)malloc(strlen(temp) + 1);

                memset(wrb->json,'\0',strlen(temp) + 1);

                memcpy(wrb->json, temp, strlen(temp));
//                cout<<strcmp(wrb->json,temp)<<":"<<wrb->json<<endl;
                wrb->work_req.data = wrb;
                uv_queue_work(&root_loop, &wrb->work_req, onWorkCb, onWorkAfterCb);
            }
        }
        free(buf->base);
    }
}

void PipeClient::onPipeReadCb(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf) {
    PipeClient* client = (PipeClient*) server->data;
    client->onPipeRead(server,nread,buf);
}

void PipeClient::onWriteCb(uv_write_t *req, int status) {
    write_baton *wb = (write_baton *)req->data;
    //[free write_baton]
    free(wb);
}

void PipeClient::onWork(uv_work_t *req) {
    work_req_baton* wrb = (work_req_baton*)req->data;
    struct json_object *json = json_tokener_parse(wrb->json);

    json_object *tmpjson = NULL;
    json_object_object_get_ex(json, json_name_cmd, &tmpjson);
    if (tmpjson != nullptr) {
        const char *cmd_name = json_object_get_string(tmpjson);
        if(event_map.count(cmd_name)>0){
            event_map[cmd_name](this,wrb);
        }
    }
    json_object_put(json);
}

void PipeClient::onWorkAfter(uv_work_t *req, int status) {
    work_req_baton* wrb = (work_req_baton*)req->data;
//   [free temp_json]
    free(wrb->json);
//    [free wrb]
    free(wrb);
}
