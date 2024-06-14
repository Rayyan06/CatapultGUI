#include "SerialWrapper.h"

SerialWrapper::SerialWrapper()
    :io(), serial(io)
{
}

SerialWrapper::SerialWrapper(std::string port, unsigned int baud_rate, Callback readCallback)
    : io(), serial(io, port), readCallback(readCallback)//, timeout(io)
{
    m_port = port;
    m_baudrate = baud_rate;
    open();
}

void SerialWrapper::writeString(std::string s)
{
    boost::asio::write(serial, boost::asio::buffer(s.c_str(), s.size()));
}

void SerialWrapper::asyncReadLine()
{
    boost::asio::async_read_until(
        serial, 
        buf, 
        "\n", 
        [this](const boost::system::error_code& ec, std::size_t bytesReceived) 
        { 
            this->readHandler(ec, bytesReceived);
        });
    
}

void SerialWrapper::readHandler(const boost::system::error_code& ec, std::size_t size)
{
    if (ec) return;

    std::istream str(&buf);
    std::string line;
    std::getline(str, line);

    if (this->readCallback) {
       readCallback(line);
    }
}

bool SerialWrapper::isOpen() const
{
    return serial.is_open();
}

void SerialWrapper::open()
{
    if (!serial.is_open()) {
        serial.open(m_port);

        serial.set_option(boost::asio::serial_port_base::baud_rate(m_baudrate));
    }


}



void SerialWrapper::close()
{
    serial.cancel();
    serial.close();
}

SerialWrapper::~SerialWrapper() {
    if (serial.is_open()) {
        serial.close();
    }

}