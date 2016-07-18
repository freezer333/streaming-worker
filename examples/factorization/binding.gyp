{
  "targets": [
    {
      "target_name": "factorization",
      "sources": [ "factorization.cpp" ], 
      "cflags": ["-Wall", "-std=c++11"],
      "include_dirs" : ["<!(node -e \"require('nan')\")", "<!(node -e \"require('streaming-worker-sdk')\")"]
    }
  ]
}