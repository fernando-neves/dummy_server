#ifndef SOCKET_CONTROL_H
#define SOCKET_CONTROL_H

/* THIS PROJECT INCLUDES */
#include "scheduler.h"
#include "logger.h"

namespace protocol {
	namespace udp {
		class socket_control;
		typedef boost::shared_ptr<socket_control> socket_control_ptr;

		class socket_control : public boost::enable_shared_from_this<socket_control> {
		public:
			socket_control(
				udp_socket_ptr _downstream_socket
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

			bool get_terminated(
			) {
				return this->terminated;
			}

		private:
			void set_recv(
			) {
				if (!current_socket)
					terminate();

				if (terminated)
					return;

				if (is_recving)
					return;

				is_recving = true;

				auto boost_buffer =
					boost::asio::buffer(
						buffer_recv,
						sizeof(buffer_recv)
					);

				auto bounded_function = boost::bind(
					&socket_control::on_recv_packet,
					this,
					this->shared_from_this(),
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
					&socket_control::on_send_packet,
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
				socket_control_ptr this_holder,
				const boost_error& error_code,
				const size_t& bytes_transferred
			) {
				is_recving = false;

				if (error_code) {
					this->terminate();
					return;
				}

				buffer_ptr data_buffer =
					boost::make_shared<buffer_t>(
						buffer_recv,
						bytes_transferred
						);

				this->add_data_buffer(data_buffer);

				this->check_send();
				this->set_recv();
			}

			void on_send_packet(
				socket_control_ptr this_holder,
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
			char buffer_recv[DEFAULT_BUFFER_SIZE];

			bool terminated;

			bool is_recving;
			bool is_sending;

			udp_socket_ptr current_socket;
			std::vector<buffer_ptr> to_downstream_buffer;
		};
	}

}
#endif // !SOCKET_CONTROL_H