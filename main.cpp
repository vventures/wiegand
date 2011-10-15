#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

#include "wiegand_client.hpp"


int main( int argc, char** argv ) {
  try {
      std::string device;
      std::string config;
      uint32_t    baud = 9600;

      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
          ("device,d", po::value<std::string>(&device), "Serial port to read from." )
          ("baud,b",   po::value<uint32_t>(&baud)->default_value(baud), "Baudrate for serial port." )
          ("config,c", po::value<std::string>(&config), "Configuration file to use." )
          ("help,h", "print this help message." )
      ;
      po::variables_map vm;
      po::store( po::parse_command_line(argc,argv,desc), vm );
      po::notify(vm);

      if( vm.count("help") ) { std::cout << desc << std::endl; return -1; }

     std::cout<<"Attempting to connect to device: '"<<device<<"' at "<<baud<<" baud\n";

      boost::asio::io_service main_ios;
      wiegand_client client( main_ios, device, baud );

      main_ios.run();
      return 0;
   } catch ( const boost::exception& e ) {
      std::cerr<<boost::diagnostic_information(e)<<std::endl;
   }
   return -1;
}
