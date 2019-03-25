#include <iostream>
#include <third-party/libuv/include/arm/uv.h>
#include <third-party/jsonc/include/arm/json_object.h>
#include <protocol/json/include/json_cmd.h>
#include <pipe/base/include/PipeBase.h>
#include <cstring>
#include <unistd.h>
#include <pipe/client/include/PipeClient.h>

using namespace std;

static void allocPipeBufferCb(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf){
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

static void onPipeReadCb(uv_stream_t* server,ssize_t nread,const uv_buf_t* buf){
    cout<<"onPipeRead"<<buf->base<<" nread:"<<nread<<endl;
    if (nread < 0) {/* Error or EOF */
        if (nread == UV_EOF) {
            cout<<"server:disconnect!"<<endl;
        } else if (nread == UV_ECONNRESET) {
            cout<<"server:connection reset!"<<endl;
        } else {
            cout<<"server:read error!"<<endl;
        }
        //uv_close((uv_handle_t*)pipe_connect,afterClientCloseCb);
        return;
    } else if (0 == nread)  {/* Everything OK, but nothing read. */

    } else {
        free(buf->base);
    }
}

void echo_write(uv_write_t *req, int status) {
    if (status < 0) {
        fprintf(stderr, "Write error %s\n", uv_err_name(status));
    }
    write_baton* baton = (write_baton*) req->data;
    free(baton);
}

void threadCb(void* arg){
    uv_connect_t *req = (uv_connect_t *)arg;
    while(1){
        struct json_object* connect_json = json_object_new_object();
        json_object_object_add(connect_json, json_name_process, json_object_new_string("1111"));
        json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_connect));
        json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
        const char* json = json_object_to_json_string(connect_json);


        string send = string(json);
        send.push_back('\0');
        cout<<"-------threadCb-------1"<<endl;
        write_baton* baton = (write_baton*)malloc(sizeof(write_baton));
        baton->buf = uv_buf_init(const_cast<char*>(send.data()), send.length());
        cout<<"-------threadCb-------2"<<endl;
        baton->write_req.data = baton;
        cout<<"-------threadCb-------3"<<endl;
        uv_write(&baton->write_req, req->handle, &baton->buf, 1, echo_write);
        cout<<"-------threadCb-------4"<<endl;
        json_object_put(connect_json);
        usleep(50*1000);

    }
}
void onConnectCb(uv_connect_t *req, int status){
    int ret = uv_read_start(req->handle,allocPipeBufferCb,onPipeReadCb);
    if(ret){
        cout<<"监听客户端失败"<<uv_err_name(status)<<endl;
        //uv_close((uv_handle_t*)req,NULL);
    }
    cout<<"监听客户端成功"<<endl;

//    uv_thread_create(&
//    while(1){
//        struct json_object* connect_json = json_object_new_object();
//        json_object_object_add(connect_json, json_name_process, json_object_new_string("1111"));
//        json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_connect));
//        json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
//        const char* json = json_object_to_json_string(connect_json);
//        json_object_put(connect_json);
//
//        string send = string(json);
//        cout<<"--------------1"<<strlen(send.data())<<endl;
//        send.push_back('\0');
//        cout<<"--------------2"<<strlen(send.data())<<endl;
//        write_baton* baton = (write_baton*)malloc(sizeof(write_baton));
//        baton->buf = uv_buf_init(const_cast<char*>(send.data()), send.length());
//        baton->write_req.data = baton;
//        uv_write(&baton->write_req, req->handle, &baton->buf, 1, echo_write);
//        usleep(500);
//    }
}
void threadClientCb(void* arg){
    PipeClient* client = (PipeClient*)arg ;
    const char* pipe_name = "../pip_sock";
    const char* process_name = "test-pipe-client";
    client->start(pipe_name,process_name);
    cout<<"--------------"<<endl;
}
int i = 0;
int main() {
    cout<<"test pipe client"<<endl;

    PipeClient pipeClient;

    pipeClient.on(cmd_db_read,[](PipeClient* client ,const work_req_baton* result){
        cout<<"\n"<<endl;
        cout<<"------------------------------------"<<endl;
        cout<<"接收："<<result->json<<endl;
        struct json_object* connect_json = json_object_new_object();
        json_object_object_add(connect_json, json_name_process, json_object_new_string(client->getProcessName()));
        json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_db_read));
        json_object_object_add(connect_json, json_name_cmd_key, json_object_new_int(i));
        json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
        const char* json = json_object_to_json_string(connect_json);
        client->send(json);
        cout<<"发送："<<json<<endl;
        cout<<"------------------------------------"<<endl;
        cout<<"\n"<<endl;
        json_object_put(connect_json);
        i++;

    });
    uv_thread_t thread;
    uv_thread_create(&thread,threadClientCb,&pipeClient);

    pipeClient.waitClientReady();


    struct json_object* connect_json = json_object_new_object();
    json_object_object_add(connect_json, json_name_process, json_object_new_string(pipeClient.getProcessName()));
    json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_db_read));
    json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
    json_object_object_add(connect_json, json_name_cmd_key, json_object_new_int(i));
    const char* json = json_object_to_json_string(connect_json);

    cout<<"客户端就绪："<<endl;
    pipeClient.send(json);
    json_object_put(connect_json);

    uv_loop_t root_loop;
    uv_loop_init(&root_loop);
    uv_timer_t main_timer;
    uv_timer_init(&root_loop,&main_timer);
    uv_timer_start(&main_timer,[](uv_timer_t* handle){

        },0,100);
    uv_run(&root_loop,UV_RUN_DEFAULT);
}