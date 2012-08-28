# Author:
#   jiuling.ypf<jiuling.ypf@taobao.com>
#
# Date:
#   2012-08-23
#
# Instruction:
#   1. Run Thrift to generate the cpp module HBase:
#      thrift --gen cpp [hbase-root]/src/main/resources/org/apache/hadoop/hbase/thrift/Hbase.thrift
#   2. Make the project to generate binary program:
#      make demo
#      make test
#   3. Execute the binary program:
#      ./demo <host> <port>
#      ./testput <host> <port> <key_len> <val_len> <list_num>
#      ./testget <host> <port> <key_len> <val_len> <list_num> <block_cache_flag>

THRIFT_DIR = /usr/local/include/thrift

LIB_DIR = /usr/local/lib

GEN_SRC = ./gen-cpp/Hbase.cpp \
	  ./gen-cpp/Hbase_types.cpp \
	  ./gen-cpp/Hbase_constants.cpp

default: demo

demo: demo.cpp
	g++ -o demo -I${THRIFT_DIR} -I./gen-cpp -I./ -L${LIB_DIR} -lthrift demo.cpp hbcli.cpp ${GEN_SRC} -DHAVE_CONFIG_H

test: testput.cpp testget.cpp
	g++ -o testput -I${THRIFT_DIR} -I./gen-cpp -I./ -L${LIB_DIR} -lthrift testput.cpp hbcli.cpp ${GEN_SRC} -DHAVE_CONFIG_H
	g++ -o testget -I${THRIFT_DIR} -I./gen-cpp -I./ -L${LIB_DIR} -lthrift testget.cpp hbcli.cpp ${GEN_SRC} -DHAVE_CONFIG_H

clean:
	rm -rf demo
	rm -rf testput
	rm -rf testget
