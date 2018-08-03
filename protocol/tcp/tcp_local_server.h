#ifndef TCP_LOCAL_SERVER_H
#define TCP_LOCAL_SERVER_H

/* THIS PROJECT INCLUDES */
#include <protocol/tcp/stream/downstream_control.h>

namespace protocol {
    namespace tcp {
        class tcp_local_server {
        public:
            void set_local_endpoint(
                    std::string _local_address,
                    uint16_t _local_port
            ) {
                local_address = _local_address;
                local_port = _local_port;
            }

            void initialize(
            ) {
                try {
                    tcp_endpoint local_endpoint =
                            tcp_endpoint(
                                    boost::asio::ip::address_v4::from_string(local_address),
                                    local_port
                            );

                    current_acceptor =
                            boost::make_shared<boost::asio::ip::tcp::acceptor>(
                                    scheduler::get()->get_svc(),
                                    local_endpoint
                            );
                }
                catch (const std::exception&) {


                }
                this->set_accept();
            }

            static tcp_local_server* get(
            ) {
                static tcp_local_server* m = nullptr;
                if (!m)
                    m = new tcp_local_server();
                return m;
            }
        private:
            void set_accept(
            ) {
                tcp_socket_ptr downstream_socket =
                        boost::make_shared<tcp_socket>(scheduler::get()->get_svc());

                auto bounded_function = boost::bind(
                        &tcp_local_server::on_accept_downstream,
                        this,
                        downstream_socket,
                        boost::asio::placeholders::error
                );

                current_acceptor->async_accept(
                        *downstream_socket,
                        bounded_function
                );
            }

            void on_accept_downstream(
                    tcp_socket_ptr downstream_socket,
                    const boost_error& error_code
            ) {
                this->set_accept();

                if (error_code)
                    return; /* TODO - NANDO - AFTER CREATE CLOSE CLOSE(DOWNSTREAM_SOCKET) */

                std::string remote_address = downstream_socket->remote_endpoint().address().to_v4().to_string();
                uint16_t remote_port = downstream_socket->remote_endpoint().port();

                std::cout << "\n ON ACCEPT SUCCESS -  REMOTE ENDPOINT - " << remote_address << ": " << remote_port;

                downstream_socket->set_option(boost::asio::ip::tcp::no_delay(true));

                downstream_control_ptr _downstream_control =
                        create_downstream_control(downstream_socket);

                _downstream_control->initialize();
            }

            downstream_control_ptr create_downstream_control(
                    tcp_socket_ptr downstream_socket
            ) {
                downstream_control_ptr _downstream_control =
                        boost::make_shared<downstream_control>(downstream_socket);

                downstream_socket_list.push_back(_downstream_control);
                return _downstream_control;
            }
        private:
            std::string local_address;
            uint16_t local_port;

            std::vector<downstream_control_ptr> downstream_socket_list;

            boost::shared_ptr<boost::asio::ip::tcp::acceptor> current_acceptor;
        };
    }
}

















#endif // !TCP_LOCAL_SERVER_H