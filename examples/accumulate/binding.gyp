{
  "targets": [
    {
      "target_name": "accumulate",
      "sources": [ "accumulate.cpp" ], 
      "cflags": ["-Wall", "-std=c++11"],
      "cflags!": [ '-fno-exceptions' ],
      "cflags_cc!": [ '-fno-exceptions' ],
      "include_dirs" : ["<!(node -e \"require('nan')\")", "<!(node -e \"require('streaming-worker-sdk')\")"]
    }
  ]
}