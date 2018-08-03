/* THIS PROJECT INCLUDES */
//#include <protocol/tcp/tcp_local_server.h>
#include <helper/settings.h>

/* C++ INCLUDES */
#include <thread>

/* JSON LIB INCLUDES */
#include <json/json.h>

int main() {
    helper::settings::get()->load_settings("server_manager.conf");

    scheduler::get();
    scheduler::get()->init();

    protocol::tcp::tcp_local_server::get();
    protocol::tcp::tcp_local_server::get()->initialize();

    std::this_thread::sleep_for(std::chrono::milliseconds(UINT32_MAX));

    return 0;
}
