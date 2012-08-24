/**
 *
 * This is a common class that accesses HBase cluster through thrift protocol.
 * It based on HBase thrift, but not the new thrift interface in HBase 0.94+ version.
 *
 * Author:
 *   jiuling.ypf<jiuling.ypf@taobao.com>
 *
 * Date:
 *   2012-08-22
 *
 */

#ifndef HBCLI_H_
#define HBCLI_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "Hbase.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace apache::hadoop::hbase::thrift;

typedef std::vector<std::string> StrVec;
typedef std::map<std::string, std::string> StrMap;
typedef std::vector<ColumnDescriptor> ColVec;
typedef std::map<std::string, ColumnDescriptor> ColMap;
typedef std::map<std::string, TCell> CellMap;
typedef std::vector<TRowResult> ResVec;
typedef std::map<std::string, std::map<std::string, std::string> > RowMap;

class HbCli {
  public:
    // Constructor and Destructor
    HbCli(const char *server, const char *port);
    ~HbCli();

    // Util Functions
    bool connect();
    bool disconnect();
    bool reconnect();
    inline bool isconnect();
   
    // HBase DDL Functions 
    bool createTable(const std::string table, const ColVec &columns);
    bool deleteTable(const std::string table);
    bool tableExists(const std::string table);
    
    // HBase DML Functions 
    bool putRow(const std::string table, const std::string row, const std::string column, const std::string value);
    bool putRowWithColumns(const std::string table, const std::string row, const StrMap columns);
    bool putRows(const std::string table, const RowMap rows);
    bool getRow(const std::string table, const std::string row, ResVec &rowResult);
    bool getRowWithColumns(const std::string table, const std::string row, const StrVec columns, ResVec &rowResult);
    bool getRows(const std::string table, const StrVec rows, ResVec &rowResult);
    bool getRowsWithColumns(const std::string table, const StrVec rows, const StrVec columns, ResVec &rowResult);
    bool delRow(const std::string table, const std::string row);
    bool delRowWithColumn(const std::string table, const std::string row, const std::string column);
    bool delRowWithColumns(const std::string table, const std::string row, const StrVec columns);
    bool scan(const std::string table, const std::string startRow, StrVec columns, ResVec &values);
    bool scanWithStop(const std::string table, const std::string startRow, const std::string stopRow, StrVec columns, ResVec &values);

    // HBase Util Functions 
    void printRow(const ResVec &rowResult);

  private:
    boost::shared_ptr<TTransport> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
    HbaseClient client;
    bool _is_connected;
    
};
#endif  // HBCLI_H_
