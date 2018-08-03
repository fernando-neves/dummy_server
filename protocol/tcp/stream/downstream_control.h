#ifndef DOWNSTREAM_CONTROL_H
#define DOWNSTREAM_CONTROL_H

/* THIS PROJECT INCLUDES */
#include <helper/scheduler.h>
#include <helper/logger.h>

namespace protocol {
    namespace tcp {
        class downstream_control;
        typedef boost::shared_ptr<downstream_control> downstream_control_ptr;

        class downstream_control : public boost::enable_shared_from_this<downstream_control> {
        public:
            downstream_control(
                    tcp_socket_ptr _downstream_socket
            ) {
                current_socket = _downstream_socket;

                terminated = false;
                is_recving = false;
                is_sending = false;
            }

            void initialize(
            ) {
                this->set_recv();
            }

            void set_recv(
            ) {
                if (!current_socket)
                    terminate();

                if (terminated)
                    return;

                if (is_recving)
                    return;

                is_recving = true;

                buffer_ptr data_buffer =
                        boost::make_shared<buffer_t>();

                /* TODO - MAYBE BUG, WHY buffer is not large ENOUGH */
                data_buffer->resize(UINT16_MAX);

                auto boost_buffer =
                        boost::asio::buffer(
                                (char*)data_buffer->data(),
                                data_buffer->size()
                        );

                auto bounded_function = boost::bind(
                        &downstream_control::on_recv_packet,
                        this,
                        this->shared_from_this(),
                        data_buffer,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                );

                current_socket->async_receive(
                        boost_buffer,
                        bounded_function
                );
            }

            void check_send(
            ) {
                if (!current_socket)
                    terminate();

                if (terminated)
                    return;

                if (is_sending)
                    return;

                buffer_ptr data_buffer =
                        this->get_data_buffer();

                if (!data_buffer)
                    return;

                is_sending = true;

                auto boost_buffer =
                        boost::asio::buffer(
                                (char*)data_buffer->data(),
                                data_buffer->size()
                        );

                auto bounded_function = boost::bind(
                        &downstream_control::on_send_packet,
                        this,
                        this->shared_from_this(),
                        data_buffer,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                );

                current_socket->async_send(
                        boost_buffer,
                        bounded_function
                );
            }

            void on_recv_packet(
                    downstream_control_ptr this_holder,
                    buffer_ptr data_buffer,
                    const boost_error& error_code,
                    const size_t& bytes_transferred
            ) {
                is_recving = false;

                if (error_code) {
                    this->terminate();
                    return;
                }

                data_buffer->resize(bytes_transferred);
                this->add_data_buffer(data_buffer);

                helper::logger::get() << "\n " << __FUNCTION__ << ": " << __LINE__ << " - RECV_BYTES: " << *data_buffer;

                this->check_send();
                this->set_recv();
            }

            void on_send_packet(
                    downstream_control_ptr this_holder,
                    buffer_ptr data_buffer,
                    const boost_error& error_code,
                    const size_t& bytes_transferred
            ) {
                is_sending = false;

                if (error_code) {
                    this->terminate();
                    return;
                }

                this->check_send();
                this->set_recv();
            }

            void add_data_buffer(
                    buffer_ptr data_buffer
            ) {
                this->to_downstream_buffer.push_back(data_buffer);
                this->check_send();
            }

            buffer_ptr get_data_buffer(
            ) {
                if (terminated)
                    return nullptr;

                if (!to_downstream_buffer.size())
                    return nullptr;

                auto data_buffer = *to_downstream_buffer.begin();
                to_downstream_buffer.erase(to_downstream_buffer.begin());

                return data_buffer;
            }

            void terminate() { terminated = true; }
        private:
            char buffer_recv[100 * 1000];

            bool terminated;
            bool is_recving;
            bool is_sending;

            tcp_socket_ptr current_socket;
            std::vector<buffer_ptr> to_downstream_buffer;
        };
    }
}
#endif // !DOWNSTREAM_CONTROL_H