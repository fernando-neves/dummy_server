#ifndef TYPEDEFS_H
#define TYPEDEFS_H

/* THIS PROJECT INCLUDES */
#include "includes.h"

typedef std::string buffer_t;
typedef boost::shared_ptr<buffer_t> buffer_ptr;

typedef boost::asio::ip::tcp::endpoint tcp_endpoint;
typedef boost::shared_ptr<tcp_endpoint> tcp_endpoint_ptr;

typedef boost::asio::ip::tcp::socket tcp_socket;
typedef boost::shared_ptr<tcp_socket> tcp_socket_ptr;

typedef boost::system::error_code boost_error;
#endif // !TYPEDEFS_H