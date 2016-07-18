# streaming-worker and streaming-worker-sdk
`streaming-worker-sdk` is a C++/JS SDK for building streaming addons with streaming/event-emitter interfaces.  This is a C++ header file gives you a pattern for creating addons by inheriting from the `StreamingWorker` abstract class.  Your addon, at a minimum, needs to implement a few virtual functions (most importantly, `Execute`), and can utilize standard methods to read and write `Message` objects to and from JavaScript.  `Message` objects are name/value (string) pairs.

Once your C++ addon is built, you can interface with it from JavaScript using the [`streaming-worker`](https://www.npmjs.com/package/streaming-worker) package.  `streaming-worker` creates a bi-directional event-emitter and stream interface around addons created by `streaming-worker-sdk`.  

## Usage
To start creating your streaming addon, first add a dependency to `streaming-worker-sdk`.

```
$ npm install --save streaming-worker-sdk
```

Next, make sure you've added the include directives to your `binding.gyp` file for both `streaming-worker-sdk` and `nan`.

```js
"include_dirs" : [
    "<!(node -e \"require('nan')\")", 
    "<!(node -e \"require('streaming-worker-sdk')\")"
]
```

Note, to use `nan` and `streaming-worker-sdk`, you need to enable C++11 and use a fairly modern C++ compiler.

To build an addon you must:

1. `#include "streaming-worker.h"`
2. Create a class that inherits `StreamingWorker`
3. Implement a constructor, the `Execute` member function (which will be the heart of your addon), and a `create_worker` factory method (see below)
4. Register the your module with `StreamWorkerWrapper::Init` using `NODE_MODULE`.

## Example
Here's an example for setting up an addon that just streams successive integers to JavaScript.  

### Building the Addon
Start out with a directory `/simple/addon` and add a `binding.gyp` file:

```js
{
  "targets": [
    {
      "target_name": "simple_stream",
      "sources": [ "simple-stream.cpp" ], 
      "cflags": ["-Wall", "-std=c++11"],
      "cflags!": [ '-fno-exceptions' ],
      "cflags_cc!": [ '-fno-exceptions' ],
      "include_dirs" : ["<!(node -e \"require('nan')\")", "<!(node -e \"require('streaming-worker-sdk')\")"]
    }
  ]
}
```

... and a package.json

```js
{
  "name": "simple-stream",
  "version": "0.0.1",
  "gypfile": true,
  "dependencies": {
    "nan": "*",
    "streaming-worker-sdk": "^0.9.0"
  }
}
```

Next create the `simple-stream.cpp` source code file.  You will need to include the sdk and then create a class that extends `StreamingWorker`.  The class needs to have a constructor and an `Execute` function, which will contain your addon code that is generating/accepting data to/from JavaScript.

```cpp
#include "streaming-worker.h"

class Simple : public StreamingWorker {
   public:
   Simple(Callback *data, Callback *complete, Callback *error_callback, v8::Local<v8::Object> & options) 
          : StreamingWorker(data, complete, error_callback){

        // nothing needs to be here - just make sure you call base constructor
        // The options parameter is for your JavaScript code to pass in 
        // an options object.  You can use this for whatever you want.
    }
     
    // You must match the call signature here, `Execute` is a virtual function
    // defined on Streaing Worker.
    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {

      // Just send 100 integers and stop
      for (int i = 0; i < 100; i++ ) {
        Message tosend("integer", std::to_string(i));
        writeToNode(progress, tosend);
      }
    }
};
```

The last step is supplying a factory method to create instances of your addon class.  This function will be called by `StreamingWorker` automatically for you - you just need to define it (and match the name/call signature).

```cpp
StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, 
    v8::Local<v8::Object> & options) {
 
 return new Simple(data, complete, error_callback, options);
}

NODE_MODULE(simple_streample, StreamWorkerWrapper::Init)
```

To build, do `npm install` (since the package.json file had a `"gypfile": true` entry.  This will build the addon, which can now be used from Node.js using the `streaming-worker` package.

### Using the addon
The SDK module is only for creating the C++ addon.  To interface with the addon, you need to pull in the [`streaming-worker'](https://www.npmjs.com/package/streaming-worker-sdk) module for JavaScript.

Create a new directory (`/simple/js`) and do an `npm install streaming-worker`.  Now you can instantiate a `worker` by specifying the location of the addon executable.   

```js
"use strict"; 

const path = require('path');\
const worker = require("streaming-worker");
const addon_path = path.join(__dirname, "../addon/build/Release/simple_stream");
const simple_stream = worker(addon_path);
```

`streaming-worker` objects have a `from` event emitter, which will report all data the addon sends to node.

```
simple_stream.from.on('integer', function(value){
    console.log(value);
});
```

## More info
You can get the full distribution [here](https://github.com/freezer333/streaming-worker), which includes both the `streaming-worker` and `streaming-worker-sdk` library, along with four more detailed examples.  This library is based on a chapter in [Node C++ Integration](https://scottfrees.com/ebooks/nodecpp/) - which covers it's implementation in a lot more detail.  
