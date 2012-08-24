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

#include "hbcli.h"

HbCli::HbCli(const char *server, const char *port):
			socket(new TSocket(server, boost::lexical_cast<int>(port))),
			transport(new TBufferedTransport(socket)), 
			//transport(new TFramedTransport(socket)), 
			protocol(new TBinaryProtocol(transport)), 
			client(protocol) {
  _is_connected = false;
}

HbCli::~HbCli() {
  
}

bool HbCli::connect() {
  _is_connected = false;
  try {
    transport->open();
    _is_connected = true;
  } catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
  }
  return _is_connected;
}

bool HbCli::disconnect() {
  try {
    transport->close();
    _is_connected = false;
    return true;
  } catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
    return false;
  }
}

bool HbCli::reconnect() {
  bool flag = disconnect();
  flag = flag & connect();
  return flag;
}

inline bool HbCli::isconnect() {
  return _is_connected;
} 

bool HbCli::createTable(const std::string table, const ColVec &columns) {
  if (!isconnect()) {
    return false;
  }
  try {
    std::cout << "creating table: " << table << std::endl;
    client.createTable(table, columns);
    return true;
  } catch (const AlreadyExists &ae) {
    std::cerr << "WARN: " << ae.message << std::endl;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
  }
  return false;
}

bool HbCli::deleteTable(const std::string table) {
  if (!isconnect()) {
    return false;
  }
  bool exist = tableExists(table);    
  if (exist) {
    try {
      if (client.isTableEnabled(table)) {
        std::cout << "disabling table: " << table << std::endl;
        client.disableTable(table);
      }
      std::cout << "deleting table: " << table << std::endl;
      client.deleteTable(table);
      return true;
    } catch (const TTransportException &tte) {
      std::cerr << "ERROR: " << tte.what() << std::endl;
      _is_connected = false;
      return false;
    }
  } else {
    return false;
  }
}

bool HbCli::tableExists(const std::string table) {
  if (!isconnect()) {
    return false;
  }
  StrVec tables;
  try {
    client.getTableNames(tables);
    for (StrVec::const_iterator it = tables.begin(); it != tables.end(); ++it) {
      if (table == *it)
        return true;
    }
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
  }
  return false;
}

bool HbCli::putRow(const std::string table, const std::string row, const std::string column, const std::string value) {
  if (!isconnect()) {
    return false;
  }
  std::vector<Mutation> mutations;
  mutations.clear();
  mutations.push_back(Mutation());
  mutations.back().column = column;
  mutations.back().value = value;
  try {
    client.mutateRow(table, row, mutations);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::putRowWithColumns(const std::string table, const std::string row, const StrMap columns) {
  if (!isconnect()) {
    return false;
  }
  std::vector<Mutation> mutations;
  mutations.clear();
  for (StrMap::const_iterator it = columns.begin(); it != columns.end(); ++it) {
    mutations.push_back(Mutation());
    mutations.back().column = it->first;
    mutations.back().value = it->second;
  }
  try {
    client.mutateRow(table, row, mutations);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::putRows(const std::string table, const RowMap rows) {
  if (!isconnect()) {
    return false;
  }
  std::vector<BatchMutation>  rowBatches;
  for (RowMap::const_iterator it = rows.begin(); it != rows.end(); ++it) {
    rowBatches.push_back(BatchMutation());
    std::string row = it->first;
    StrMap columns = it->second;
    std::vector<Mutation> mutations;
    mutations.clear();
    for (StrMap::const_iterator iter = columns.begin(); iter != columns.end(); ++iter) {
      mutations.push_back(Mutation());
      mutations.back().column = iter->first;
      mutations.back().value = iter->second;
    }
    rowBatches.back().row = row;
    rowBatches.back().mutations = mutations;
  }
  try {
    client.mutateRows(table, rowBatches);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::getRow(const std::string table, const std::string row, ResVec &rowResult) {
  if (!isconnect()) {
    return false;
  }
  try {
    client.getRow(rowResult, table, row);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::getRowWithColumns(const std::string table, const std::string row, const StrVec columns, ResVec &rowResult) {
  if (!isconnect()) {
    return false;
  }
  try {
    client.getRowWithColumns(rowResult, table, row, columns);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::getRows(const std::string table, const StrVec rows, ResVec &rowResult) {
  if (!isconnect()) {
    return false;
  }
  try {
    client.getRows(rowResult, table, rows);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::getRowsWithColumns(const std::string table, const StrVec rows, const StrVec columns, ResVec &rowResult) {
  if (!isconnect()) {
    return false;
  }
  try {
    client.getRowsWithColumns(rowResult, table, rows, columns);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::delRow(const std::string table, const std::string row) {
  if (!isconnect()) {
    return false;
  }
  try {
    client.deleteAllRow(table, row);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::delRowWithColumn(const std::string table, const std::string row, const std::string column) {
  if (!isconnect()) {
    return false;
  }
  std::vector<Mutation> mutations;
  mutations.clear();
  mutations.push_back(Mutation());
  mutations.back().column = column;
  mutations.back().isDelete = true;
  try {
    client.mutateRow(table, row, mutations);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::delRowWithColumns(const std::string table, const std::string row, const StrVec columns) {
  if (!isconnect()) {
    return false;
  }
  std::vector<Mutation> mutations;
  mutations.clear();
  for (StrVec::const_iterator it = columns.begin(); it != columns.end(); ++it) {
    mutations.push_back(Mutation());
    mutations.back().column = *it;
    mutations.back().isDelete = true;
  }
  try {
    client.mutateRow(table, row, mutations);
    return true;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    return false;
  }
}

bool HbCli::scan(const std::string table, const std::string startRow, StrVec columns, ResVec &values) {
  if (!isconnect()) {
    return false;
  }
  const int32_t nRows = 50;
  int scanner = client.scannerOpen(table, startRow, columns);
  try {
    while (true) {
      std::vector<TRowResult> value;
      client.scannerGetList(value, scanner, nRows);
      if (value.size() == 0)
        break;
      for (ResVec::const_iterator it = value.begin(); it != value.end(); ++it)
        values.push_back(*it);
    }
    client.scannerClose(scanner);
  } catch (const IOError &ioe) {
    std::cerr << "FATAL: Scanner raised IOError" << std::endl;
    client.scannerClose(scanner);
    return false;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    client.scannerClose(scanner);
    return false;
  }
  return true;
}

bool HbCli::scanWithStop(const std::string table, const std::string startRow, const std::string stopRow, StrVec columns, ResVec &values) {
  if (!isconnect()) {
    return false;
  }
  const int32_t nRows = 50;
  int scanner = client.scannerOpenWithStop(table, startRow, stopRow, columns);
  try {
    while (true) {
      std::vector<TRowResult> value;
      client.scannerGetList(value, scanner, nRows);
      if (value.size() == 0)
        break;
      for (ResVec::const_iterator it = value.begin(); it != value.end(); ++it)
        values.push_back(*it);
    }
    client.scannerClose(scanner);
  } catch (const IOError &ioe) {
    std::cerr << "FATAL: Scanner raised IOError" << std::endl;
    client.scannerClose(scanner);
    return false;
  } catch (const TTransportException &tte) {
    std::cerr << "ERROR: " << tte.what() << std::endl;
    _is_connected = false;
    client.scannerClose(scanner);
    return false;
  }
}

void HbCli::printRow(const ResVec &rowResult) {
  for (size_t i = 0; i < rowResult.size(); i++) {
    std::cout << "row: " << rowResult[i].row << ", cols: ";
    for (CellMap::const_iterator it = rowResult[i].columns.begin();
         it != rowResult[i].columns.end(); ++it) {
      std::cout << it->first << " => " << it->second.value << "; ";
    }
    std::cout << std::endl;
  }
}
