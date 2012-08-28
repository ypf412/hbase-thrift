# HBase Thrift For C++ Client
This is a common C++ client that accesses HBase cluster through HBase ThriftServer. 
It based on HBase thrift, but not the new thrift interface in HBase 0.94+ version.

## Prerequisites
* Download, decompress and install [thrift-0.8.0](https://dist.apache.org/repos/dist/release/thrift/0.8.0/thrift-0.8.0.tar.gz).
<pre>
wget https://dist.apache.org/repos/dist/release/thrift/0.8.0/thrift-0.8.0.tar.gz
tar zxvf thrift-0.8.0.tar.gz
sudo yum install automake libtool flex bison pkgconfig gcc-c++ boost-devel libevent-devel zlib-devel python-devel ruby-devel
cd thrift-0.8.0
./configure
make
sudo make install
</pre>
* Download and decompress [hbase-0.92.1](http://www.fayea.com/apache-mirror/hbase/hbase-0.92.1/hbase-0.92.1.tar.gz) or previous versions.
<pre>
wget http://www.fayea.com/apache-mirror/hbase/hbase-0.92.1/hbase-0.92.1.tar.gz
tar zxvf hbase-0.92.1.tar.gz
</pre>

## Instructions
* Run Thrift to generate the cpp module HBase:
<pre>
thrift --gen cpp [hbase-root]/src/main/resources/org/apache/hadoop/hbase/thrift/Hbase.thrift
</pre>
* Make the project to generate binary program:
<pre>
make demo
make perf
</pre>
* Execute the binary program:
<pre>
./demo <host> <port>
./testput <host> <port> <key_len> <val_len> <list_num>
./testget <host> <port> <key_len> <val_len> <list_num> <block_cache_flag>
</pre>

## Contributors
* Yuan Panfeng ([@ypf412](https://github.com/ypf412))
