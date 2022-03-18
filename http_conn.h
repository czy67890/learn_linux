#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include<sys/uio.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "locker.h"
class http_conn{
public:
  static const int FILENAME_LEN = 20;
  static const int READ_BUF_SIZE = 2048;
  static const int WRITE_BUF_SIZE = 1024;
  enum METHOD{
    GET = 0,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNEXT,PATCH
  };
  enum CHECK_STATE{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_CONTENT
  };
  enum HTTP_CODE{
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
  };
  enum LINE_STATUS{
    LINE_OK = 0,LINE_BAD,LINE_OPEN
  };
http_conn();
~http_conn();
  void init(int sockfd,const sockaddr_in &addr);
  void close_conn(bool real_close = true);
  void process();
  bool read();
  bool write();
private:
  void init();
  HTTP_CODE parse_header(char *text);
  HTTP_CODE process_read();
  bool process_write(HTTP_CODE ret);
  HTTP_CODE parse_request_line(char *text);
  HTTP_CODE parse_content(char *text);
  HTTP_CODE do_request();
  char *get_line(){
    return m_read_buf+m_start_line;
  }
  LINE_STATUS parse_line();
  void unmap();
  bool add_content_length(int content_len);
  bool add_response(const char * format,...);
  bool add_content(const char * content);
  bool add_status_line(int status,const char * title);
  bool add_headers(int content_length);
  bool add_linger();
  bool add_blank_line();
private:
  static int m_epollfd;
  static int m_user_count;
private:
  int m_content_length;
  int m_sockfd;
  sockaddr_in m_addr;
  char m_read_buf[READ_BUF_SIZE];
  int m_read_idx;
  int m_check_idx;
  int m_start_line;
  char m_write_buf[WRITE_BUF_SIZE];
  int m_write_idx;
  CHECK_STATE m_check_state;
  METHOD m_method;
  char m_real_file[FILENAME_LEN];
  char *m_url;
  char *m_version;
  char *m_host;
  bool m_linger;
  char *m_file_addr;
  struct stat m_file_stat;
  struct iovec m_iv[2];
  int m_iv_count;
};
#endif
