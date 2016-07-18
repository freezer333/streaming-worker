#include "streaming-worker.h"

class Simple : public StreamingWorker {
    public:
    Simple(Callback *data, Callback *complete, Callback *error_callback, v8::Local<v8::Object> & options) 
          : StreamingWorker(data, complete, error_callback){

        // nothing needs to be here - just make sure you call base constructor
        // The options parameter is for your JavaScript code to pass in 
        // an options object.  You can use this for whatever you want.
    }
     
    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
      for (int i = 0; i < 100; i++ ) {
        Message tosend("integer", std::to_string(i));
        writeToNode(progress, tosend);
      }
    }
};

StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, v8::Local<v8::Object> & options) {
 
 return new Simple(data, complete, error_callback, options);
}

NODE_MODULE(simple_streample, StreamWorkerWrapper::Init)
