# HBase Thrift For C++ Client
This is a common C++ client that accesses HBase cluster through HBase ThriftServer. 
It based on HBase thrift, but not the new thrift interface in HBase 0.94+ version.

## Prerequisites
* Download and install [thrift-0.8.0](https://dist.apache.org/repos/dist/release/thrift/0.8.0/thrift-0.8.0.tar.gz).
* Download [hbase-0.92.1](http://www.fayea.com/apache-mirror/hbase/hbase-0.92.1/hbase-0.92.1.tar.gz) or previous versions.

## Instructions
* Run Thrift to generate the cpp module HBase:
     `thrift --gen cpp <hbase-root>/src/main/resources/org/apache/hadoop/hbase/thrift/Hbase.thrift`
* Make the project to generate binary program:
      `make`
* Execute the binary program:
     `./demo <host> <port>`

## Contributors
* Yuan Panfeng ([@ypf412](https://github.com/ypf412))
