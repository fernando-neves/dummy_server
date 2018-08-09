#ifndef INCLUDES_H
#define INCLUDES_H

/* BOOST 1.58.0 INCLUDES */
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

/* C++ INCLUDES */
#include <thread>
#include <string>
#include <stdint.h>
#include <iostream>

#define DEFAULT_BUFFER_SIZE		100 * 1000

#define DEFAULT_PORT_TCP		9090
#define DEFAULT_PORT_UDP		9191

#endif // !INCLUDES_H