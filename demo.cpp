/**
 *
 * This is a function test class for HbCli class.
 *
 * Author:
 *   jiuling.ypf<jiuling.ypf@taobao.com>
 *
 * Date:
 *   2012-08-23
 *
 */
#include <stdio.h>
#include <iostream>

#include "hbcli.h"

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Invalid arguments!\n" << "Usage: demo host port" << std::endl;
    return -1;
  }
  HbCli myhbcli(argv[1], argv[2]);
 
  myhbcli.connect();
   
  std::string table("test_table");
  ColVec columns;
  columns.push_back(ColumnDescriptor());
  columns.back().name = "entry:";
  columns.back().maxVersions = 1;
  columns.back().compression = "LZO";
  columns.back().inMemory = true;
  columns.back().blockCacheEnabled = true;
  columns.back().bloomFilterType = "ROW";
  columns.back().timeToLive = 3 * 24 * 3600;
  if (!myhbcli.tableExists(table))
    myhbcli.createTable(table, columns);
  
  myhbcli.putRow(table, "row1", "entry:url", "11111111");
  myhbcli.putRow(table, "row1", "entry:ref", "22222222");
  
  RowMap rowMap;
  StrMap columnMap;
  columnMap.insert(std::pair<std::string, std::string>("entry:0", "00000000"));
  columnMap.insert(std::pair<std::string, std::string>("entry:1", "11111111"));
  rowMap.insert(std::pair<std::string, StrMap>("row1", columnMap));
  rowMap.insert(std::pair<std::string, StrMap>("row2", columnMap));
  myhbcli.putRows(table, rowMap);

  std::vector<TRowResult> rowResult;
  myhbcli.getRow(table, "row1", rowResult);
  myhbcli.printRow(rowResult);
  
  columnMap.clear();
  columnMap.insert(std::pair<std::string, std::string>("entry:a", "33333333"));
  columnMap.insert(std::pair<std::string, std::string>("entry:b", "44444444"));
  myhbcli.putRowWithColumns(table, "row2", columnMap);
  
  myhbcli.getRow(table, "row2", rowResult);
  myhbcli.printRow(rowResult);
  
  StrVec columnVec;
  columnVec.push_back("entry:b");
  myhbcli.getRowWithColumns(table, "row2", columnVec, rowResult);
  myhbcli.printRow(rowResult);

  columnVec.clear();
  rowResult.clear();
  columnVec.push_back("entry:"); 
  myhbcli.scan(table, "", columnVec, rowResult);
  myhbcli.printRow(rowResult);

  columnVec.clear();
  rowResult.clear();
  columnVec.push_back("entry:url"); 
  myhbcli.scanWithStop(table, "row1", "row2", columnVec, rowResult);
  myhbcli.printRow(rowResult);

  columnVec.clear();
  columnVec.push_back("entry:url");
  columnVec.push_back("entry:ref");
  myhbcli.delRowWithColumns(table, "row1", columnVec);
  myhbcli.delRow(table, "row1");
  myhbcli.delRowWithColumn(table, "row2", "entry:a");
  myhbcli.delRow(table, "row2");
  
  myhbcli.deleteTable(table);
  
  myhbcli.disconnect();
}
