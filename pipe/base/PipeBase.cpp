//
// Created by Administrator on 2019/3/13.
//

#include <pipe/base/include/PipeBase.h>
//#public
int PipeBase::start(const char *pipe_name, const char *process_name) {
    return attachStart(pipe_name,process_name,root_loop);
}

int PipeBase::start() {
    return uv_run(&root_loop,UV_RUN_DEFAULT);
}

int PipeBase::attach(const char *pipe_name, const char *process_name, uv_loop_t attach_uv_loop) {
    root_loop = attach_uv_loop;
    //获取当前pid
    pid = uv_os_getpid();
    running_time = time(0);
    int ret = uv_loop_init(&root_loop);
    if(ret!=0){
        cout<<(uv_err_name(ret))<<endl;
        uv_loop_close(&root_loop);
        return 1;
    }
    this->pipe_name = const_cast<char*>(pipe_name);
    this->process_name = const_cast<char*>(process_name);
    //注册自定义句柄
    registor();
    return 0;
}

int PipeBase::attachStart(const char *pipe_name, const char *process_name, uv_loop_t attach_uv_loop) {
    attach(pipe_name,process_name,attach_uv_loop);
    return uv_run(&root_loop,UV_RUN_DEFAULT);
}

char *PipeBase::getProcessName() {
    return process_name;
}

//#protected
void PipeBase::registor() {
    registorShutdownSignal();
}
//private
int PipeBase::registorShutdownSignal() {
    shutdown_signal.data = this;
    int ret = uv_signal_init(&root_loop, &shutdown_signal);
    //程序终止时删除sock文件PIPENAME
    ret = uv_signal_start(&shutdown_signal, shutdownSignalCb, SIGINT);
    uv_out("注册进程关闭处理",ret);
    return ret;
}

void PipeBase::shutdownSignalCb(uv_signal_t *handle, int signum) {
    PipeBase* base = (PipeBase*)handle->data;
    base->shutdown(handle,signum);
}

void PipeBase::shutdown(uv_signal_t *handle, int signum) {
    //child to-be override
}

void PipeBase::uv_out(const char * msg, int ret) {
    if(show_uv_out){
        if(ret){
            cout<<msg<<":"<<uv_err_name(ret)<<"["<<ret<<"]"<<endl;
        }
        else{
            cout<<msg<<":"<<"["<<ret<<"]"<<endl;
        }
    }
}

void PipeBase::allocPipeBufferCb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;

}

int PipeBase::getPid() const {
    return pid;
}

void PipeBase::onWorkCb(uv_work_t *req) {
    work_req_baton* wrb = (work_req_baton*) req->data;
    wrb->$this->onWork(req);
}

void PipeBase::onWork(uv_work_t *req) {
    uv_out("子类复写工作队列处理",0);
}

void PipeBase::onWorkAfterCb(uv_work_t *req, int status) {
    work_req_baton* wrb = (work_req_baton*) req->data;
    wrb->$this->onWorkAfter(req,status);
}

void PipeBase::onWorkAfter(uv_work_t *req, int status) {
    uv_out("子类复写工作队列资源释放处理",0);
}
