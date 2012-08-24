/**
 *
 * This is a performance test class for HbCli class.
 *
 * Author:
 *   jiuling.ypf<jiuling.ypf@taobao.com>
 *
 * Date:
 *   2012-08-23
 *
 */
#include <iostream>
#include<ctime>

#include "hbcli.h"

const int COUNT = 100000;

struct timeval tvpre, tvafter;

static inline long gen_ms(timeval tvpre, timeval tvafter) {
  return (tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000;
}

static char *rand_str(char *str, const int len) {
  int i;
  for(i=0; i<len; ++i)
    str[i]='A' + rand() % 26;
  str[++i]='\0';
  return str;
}

static void pre_put_rows(HbCli myhbcli, std::string table, int klen, int vlen, int lnum) {
  char key[klen+1];
  char value[vlen+1];
  RowMap rowMap;
  for (int i=0; i<COUNT; i++) {
    sprintf(key, "row%6d", i);
    rand_str(value, vlen);
    StrMap columnMap;
    columnMap.insert(std::pair<std::string, std::string>("entry:t", value));
    rowMap.insert(std::pair<std::string, StrMap>(key, columnMap));
    if (rowMap.size() == lnum) {
      myhbcli.putRows(table, rowMap);
      rowMap.clear();
    }
  }
}

long test_row_get(HbCli myhbcli, std::string table) {
  long ms = 0;
  char key[20];
  std::vector<TRowResult> rowResult;
  for (int i=0; i<COUNT; i++) {
    int val = rand() % COUNT;
    sprintf(key, "row%6d", val);
    gettimeofday(&tvpre, NULL);
    myhbcli.getRow(table, key, rowResult);
    gettimeofday(&tvafter, NULL);
    ms += gen_ms(tvpre, tvafter);
  }
  return ms;
}

long test_row_get_list(HbCli myhbcli, std::string table, int lnum) {
  long ms = 0;
  char key[20];
  std::vector<TRowResult> rowResult;
  StrVec rowVec;
  for (int i=0; i<COUNT; i++) {
    int val = rand() % COUNT;
    sprintf(key, "row%6d", val);
    rowVec.push_back(key);
    if (rowVec.size() == lnum) {
      gettimeofday(&tvpre, NULL);
      myhbcli.getRows(table, rowVec, rowResult);
      rowVec.clear();
      gettimeofday(&tvafter, NULL);
      ms += gen_ms(tvpre, tvafter);
    }
  }
  return ms;
}

int main(int argc, char** argv) {
  if (argc < 7) {
    std::cerr << "Invalid arguments!\n" << "Usage: testget host port key_len val_len list_num block_cache_flag" << std::endl;
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
  if (strcmp(argv[6], "true") == 0) {
    columns.back().inMemory = true;
    columns.back().blockCacheEnabled = true;
  } else {
    columns.back().inMemory = false;
    columns.back().blockCacheEnabled = false;
  }
  columns.back().bloomFilterType = "ROW";
  columns.back().timeToLive = 3 * 24 * 3600;
  if (!myhbcli.tableExists(table))
    myhbcli.createTable(table, columns);
 
  int klen = atoi(argv[3]);
  int vlen = atoi(argv[4]);
  int lnum = atoi(argv[5]);
  
  pre_put_rows(myhbcli, table, klen, vlen, lnum);
  
  long actual_ms = 0;
  struct timeval tvstart, tvend;
  gettimeofday(&tvstart, NULL);
  if (lnum == 1)
    actual_ms = test_row_get(myhbcli, table);
  else
    actual_ms = test_row_get_list(myhbcli, table, lnum);
  gettimeofday(&tvend, NULL);
  long total_ms = gen_ms(tvstart, tvend);
  std::cout << "total time in ms: " << total_ms << std::endl;
  std::cout << "actual time in ms: " << actual_ms << std::endl;
  std::cout << "qps in total time: " << (long)(COUNT*1000)/total_ms << std::endl;
  std::cout << "qps in actual time: " << (long)(COUNT*1000)/actual_ms << std::endl;
  
  myhbcli.deleteTable(table);
  
  myhbcli.disconnect();
}
