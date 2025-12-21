/*
Original MMO Client/Server Framework using ASIO
by David Barr (javidx9) – OLC-3 License

This file was implemented alongside the tutorial for learning purposes.
Code was written in parallel with the tutorial, and debugging, understanding,
and integration into the Lan Chart App project were done by Hamdan, 2025.

See LICENSE file for full OLC-3 license details.
*/
#pragma once
#include "net_common.h"

namespace olc
{
	namespace net
	{
		
		template <typename T>

		struct message_header
		{

			T id{};

			uint32_t size = 0;

		};

		template <typename T>
		struct message
		{
			message_header<T> header{};

			std::vector<uint8_t> body;

			
			size_t size() const
			{
				return body.size();
			}

			friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
			{

				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;

				return os;
			}

			
			template<typename DataType>

			friend message<T>& operator << (message<T>& msg, const DataType& data)
			{

				
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");
				
				size_t i = msg.body.size();
				
				msg.body.resize(msg.body.size() + sizeof(DataType));
				
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				msg.header.size = msg.size();

				return msg;
			}

			template<typename DataType>
			friend message<T>& operator >> (message<T>& msg, DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

				size_t i = msg.body.size() - sizeof(DataType);

				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				msg.body.resize(i);

				msg.header.size = msg.size();

				return msg;
			}



			friend message<T>& operator<<(message<T>& msg, const std::string& s) {

				const uint32_t len = static_cast<uint32_t>(s.size());


				size_t i = msg.body.size();
				msg.body.resize(i + len);
				if (len > 0)
					std::memcpy(msg.body.data() + i, s.data(), len);


				msg << len;

				msg.header.size = msg.size();
				return msg;
			}

			friend message<T>& operator>>(message<T>& msg, std::string& s) {
				uint32_t len = 0;
				msg >> len;

				// Safety: check that the message actually contains len bytes
				if (len > msg.body.size()) {
					s.clear();
					return msg;
				}


				size_t start = msg.body.size() - static_cast<size_t>(len);


                s.resize(len);
				if (len > 0)
					std::memcpy(&s[0], msg.body.data() + start, len);


				msg.body.resize(start);
				msg.header.size = msg.size();

				return msg;
			}

		};


		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};

	}
}
