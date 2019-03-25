#include <iostream>
#include <uv.h>
#include <pipe/server/include/PipeServer.h>
#include <unistd.h>

using namespace std;
const char* pipe_name = "../pip_sock";
const char* process_name = "test-pipe-server";
int n = 0;
int main() {

    uv_loop_t root_loop;
    PipeServer pipeServer;

    cout<<"prepare test pipe server pid:"<<pipeServer.getPid()<<endl;
    pipeServer.on(cmd_db_read,[](PipeServer* server,const work_req_baton* result){
        cout<<"\n"<<endl;
        cout<<"------------------------------------"<<endl;
        cout<<"接收："<<result->json<<endl;
        struct json_object* connect_json = json_object_new_object();
        json_object_object_add(connect_json, json_name_process, json_object_new_string(server->getProcessName()));
        json_object_object_add(connect_json, json_name_cmd, json_object_new_string(cmd_db_read));
        json_object_object_add(connect_json, json_name_client_id, json_object_new_int(uv_os_getpid()));
        json_object_object_add(connect_json, json_name_cmd_key, json_object_new_int(n));

        struct json_object* connect_json1 = json_tokener_parse(result->json);
        json_object *tmpjson = NULL;
        json_object_object_get_ex(connect_json1, json_name_cmd_key, &tmpjson);
        int client_key = json_object_get_int(tmpjson);
        json_object_object_add(connect_json, json_name_fire, json_object_new_int(client_key));



        const char* json = json_object_to_json_string(connect_json);
        cout<<"发送："<<json<<endl;
        server->send(json,result->write_to);
        json_object_put(connect_json);
        json_object_put(connect_json1);
        n++;
        cout<<"------------------------------------"<<endl;
        cout<<"\n"<<endl;
    });
    pipeServer.attachStart(pipe_name,process_name,root_loop);
}