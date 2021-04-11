#include <cli/cliasyncsession.h>
#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace cli;
using namespace std;
// Sends a WebSocket message and prints the response
int main()
{
    try
    {

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        std::vector<std::string> strs;
        auto rootMenu = make_unique< Menu >( "Client" );
        rootMenu -> Insert(
            "connect",
            [&](std::ostream& out, std::string str){ out << "connect to " << str << "\n";
                
                boost::split(strs,str,boost::is_any_of(":"));
                auto const results = resolver.resolve(strs[0],strs[1]);
                // Make the connection on the IP address we get from a lookup
                net::connect(ws.next_layer(), results.begin(), results.end());
                // Set a decorator to change the User-Agent of the handshake
                ws.set_option(websocket::stream_base::decorator(
                    [](websocket::request_type& req)
                    {
                        req.set(http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                                " websocket-client-coro");
                    }));

                // Perform the websocket handshake
                ws.handshake(strs[0], "/");
                        
                    },
            "Connect to host" );
        
        rootMenu -> Insert(
            "send",
            [&](std::ostream& out, std::string str){ out << "send message: " << str << "\n";
                ws.write(net::buffer(std::string(str)));
                ws.close(websocket::close_code::normal);
             },
            "Send message" ); 
        
        boost::asio::io_context ios;
        Cli cli( std::move(rootMenu) );
    // global exit action
        cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );

        CliFileSession input(cli);
        input.Start();
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}