#include "SerialWrapper.h"

#include <functional>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <string_view>


SerialWrapper::SerialWrapper(boost::asio::io_service& io) : m_io(io), m_serial(io)
{
}

SerialWrapper::SerialWrapper(boost::asio::io_service& io, const std::string port, unsigned int baud_rate)
    : m_io(io), m_serial(io)//, timeout(io)
{
    m_serial.open(port);    
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

}

void SerialWrapper::writeString(std::string s)
{
    boost::asio::write(m_serial, boost::asio::buffer(s.c_str(), s.size()));
}

void SerialWrapper::asyncReadLine()
{
    m_serial.async_read_some(
        boost::asio::buffer(m_buf),
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            this->readHandler(ec, bytes_transferred);
        });
    //m_io.run();
    
}

void SerialWrapper::readHandler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (!ec) {

        for (std::size_t i = 0; i < bytes_transferred; ++i) {
            if (m_buf[i] == 0x03) {
                // EOT character found, clear the buffer
                line_buffer.clear();
                std::cout << "EOT found. Buffer cleared." << std::endl;
            }
            else {
                line_buffer += m_buf[i];
            }

            // Process the complete line if we have reached 13 characters
            if (m_buf[i] == '\n') {
                std::string line = line_buffer;
                line_buffer.clear();
                readCallback(line);
            }
        }

        asyncReadLine();
    }
    else {
        std::cerr << "Error: " << ec.message() << std::endl;
    }
}

bool SerialWrapper::isOpen() const
{
    return m_serial.is_open();
}

void SerialWrapper::open(std::string_view port, unsigned int baudrate)
{
    if (!m_serial.is_open()) {
        m_serial.open(static_cast<std::string>(port));

    }
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(baudrate));


}



void SerialWrapper::close()
{
    m_serial.cancel();
    m_serial.close();
}

void SerialWrapper::setCallback(Callback callback)
{
    this->readCallback = callback;
}

SerialWrapper::~SerialWrapper() {
    if (m_serial.is_open()) {
        m_serial.close();
    }

}