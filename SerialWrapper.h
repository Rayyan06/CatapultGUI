#pragma once

#include <boost/asio.hpp>
#include <string>

class SerialWrapper
{
public:
    using Callback = std::function<void(const std::string&)>;
    /*
    * Default constructor
    * Initializes the IO service and sets serial to nullptr.
    */
    SerialWrapper();
    /*
    * Constructor
    * \param port device name, example "/dev/ttyUSB0" or "COM6"
    * \param baud_rate communication speed, example 9600 or 115200
    * \throws boost::system::system_error if it cannot open the serial device
    *
    */
    SerialWrapper(std::string port, unsigned int baud_rate, Callback readCallback);

    /*
    * Write a string to the serial device.
    * \param s string to write
    * \throws boost::system::system_error on failure
    *
    */
    void writeString(std::string s);

    /*
    * Asynchronously reads a line is received from the serial device.
    * Eventual '\n' or '\r\n' characters at the end of the string are removed.
    * \return a string containing the received line
    * \throws a boost::system::system_error on failure
    */
    void asyncReadLine();

    /*
    * Reads the line which has been recieved.
    * \throws a boost::system::system_error on failue 
    */
    void readHandler(const boost::system::error_code& ec, std::size_t size);

    /*
    * Returns true if the serial port is currently open
    * \return boolean 
    */
    bool isOpen() const;

    /* opens the Serial port */
    void open();
    /*
    * Closes the serial port 
    */
    void close();

    ~SerialWrapper();

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    boost::asio::streambuf buf;

    std::string m_port;
    unsigned int m_baudrate;

    /* Our Read Callback function which handles the logging */
    Callback readCallback;
    //boost::asio::deadline_timer timeout;
};

