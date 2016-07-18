"use strict"; 
const path = require('path');
const worker = require("streaming-worker");
const addon_path = path.join(__dirname, "../addon/build/Release/simple_stream");
const simple_stream = worker(addon_path);

simple_stream.from.on('integer', function(value){
    console.log(value);
});

const through = require('through');
var output = simple_stream.from.stream();
output
   .pipe(
        through(function (data) {
            // the data coming in is an array, 
           	// Element 0 is the name of the event emitted by the addon ("integer")
           	// Element 1 is the data - which in this case is just the integer 
           	this.queue(data[1]);
        }))
   .pipe(process.stdout);
