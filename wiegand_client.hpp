#ifndef WIEGAND_CLIENT_HPP_
#define WIEGAND_CLIENT_HPP_
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


class wiegand_client {
  public:
    wiegand_client( boost::asio::io_service& _ios, const std::string& device, uint32_t baud )
    :m_ios(_ios), m_sport(_ios,device),m_cur_out_buf(0),m_writing(false),m_device(device) {
      if( !m_sport.is_open() ) {
        std::cerr<< "Unable to open device '"<<device<<"'\n";
        return;
      }
      std::cout<<"Listening on device '"<<m_device<<"' at "<<baud<<" baud\n";
      // set baud
      m_sport.set_option( boost::asio::serial_port_base::baud_rate(baud) );
      start_read();
    }


    void send( const std::string& msg ) {
      std::cerr<<"SEND: "<<msg;
      m_out_buf[m_cur_out_buf].insert( m_out_buf[m_cur_out_buf].end(), msg.begin(), msg.end() );
      if( !m_writing ) {
        m_writing = true;
        ++m_cur_out_buf;
        boost::asio::async_write( m_sport, boost::asio::buffer(&m_out_buf[!m_cur_out_buf].front(),
                                                                m_out_buf[!m_cur_out_buf].size() ),
                                   boost::bind( &wiegand_client::on_sent, this, _1, _2 ) );
      }
    }
    boost::function<void(const std::string&)> recv;

  private:
    void start_read() {
      boost::asio::async_read( m_sport, boost::asio::buffer( m_in_msg, sizeof(m_in_msg) ),
                               boost::bind( &wiegand_client::on_recv, this, _1, _2 ) );
    }
    void on_sent( const boost::system::error_code& ec, size_t bytes_transferred ) {
      if( ec ) {
        std::cerr<<"Error reading from '"<<m_device<<"': "<<boost::system::system_error(ec).what()<<std::endl;
      }
      ++m_cur_out_buf;
      m_out_buf[m_cur_out_buf].clear();
      if( m_out_buf[!m_cur_out_buf].size() ) {
        boost::asio::async_write( m_sport, boost::asio::buffer(&m_out_buf[!m_cur_out_buf].front(),
                                                                m_out_buf[!m_cur_out_buf].size() ),
                                   boost::bind( &wiegand_client::on_sent, this, _1, _2 ) );
      }else {
        m_writing = false;
      }
    }

    void on_recv( const boost::system::error_code& ec, size_t bytes_transferred ) {
      if( !ec ) {
        // verify that the message is of the proper form.
        if( m_in_msg[0] != '*' || m_in_msg[3] != '#' || m_in_msg[15] != 0x0D ) {
          // TODO: validate wiegand data size is less than or equal to 42
          // TODO: validate that weigand data is all hex numbers
          // TODO: validate that parody bit is 0,1,2 or 3
          
          std::string m(m_in_msg,sizeof(m_in_msg) );
          std::cerr<<"Invalid message: "<<m;
          uint32_t start_byte = m.find( '*' );
          uint32_t valid_bytes = sizeof(m_in_msg) - start_byte;
          if( start_byte < sizeof(m_in_msg) ) {
            memmove(m_in_msg, m_in_msg+start_byte, valid_bytes );
             boost::asio::async_read( m_sport, boost::asio::buffer( m_in_msg+valid_bytes, start_byte ),
                                      boost::bind( &wiegand_client::on_recv, this, _1, _2 ) );
          }
          return;
        } else {
          std::cerr<<"RECV: "<<std::string(m_in_msg,sizeof(m_in_msg) );  
          if( recv ) 
            recv( std::string(m_in_msg,sizeof(m_in_msg) ) );  
        }
      } else {
        std::cerr<<"Error reading from '"<<m_device<<"': "<<boost::system::system_error(ec).what()<<std::endl;
      }
      start_read();
    }

  private:
    boost::asio::io_service& m_ios;
    boost::asio::serial_port m_sport;
    char                     m_in_msg[16];

    std::string              m_device;
    bool                     m_writing;
    int                      m_cur_out_buf:1;
    std::vector<char>        m_out_buf[2];

};


#endif // WIEGAND_CLIENT_HPP_
