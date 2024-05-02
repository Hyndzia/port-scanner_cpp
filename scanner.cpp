//code provided by github.com/Hyndzia
//this will be later added into "IP-Probes" for mutli-threaded implementation

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <unistd.h>
#include "scanner.h"

bool Scanner::isPortOpen(uint16_t port) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::deadline_timer timer(io_context);
    bool flag = false;

    try {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipv4), port);
        socket.async_connect(endpoint, [&](const boost::system::error_code& ec) {
            if (!ec) {
                flag = true;
            }
        });

        timer.expires_from_now(boost::posix_time::milliseconds(100));
        timer.async_wait([&](const boost::system::error_code& ec) {
            if (ec == boost::asio::error::operation_aborted) {
            } else if (!flag) {
                
            }
        });
        io_context.run();
        socket.close();

    } catch(const boost::system::system_error& e) {
        throw std::runtime_error(std::string("") + e.what());
    }

    return flag;
}

void Scanner::scanPorts(){
    std::cout<<"\n  Scanning ports on "<<host<<"("<<ipv4<<")"<<"...."<<std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (uint16_t port = start-1; port <= end; port++) {
        if(isPortOpen(port)){
            std::cout<<"\n  Port "<<port<<" open!"<<std::endl;
            found++;
        }
        //else std::cout<<"Port "<<port<<" closed!"<<std::endl;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
    auto seconds = duration.count()/1000;
    std::cout<<"\n  Scanning completed in "<<seconds<<"s!"<<std::endl;
    std::cout<<"  A total of "<<found<<" ports found!\n\n"<<std::endl;
}

std::string Scanner::resolveDNS() {
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, "");

        for (const auto& endpoint : endpoints) {
            if (endpoint.endpoint().address().is_v4()) {
                return endpoint.endpoint().address().to_string();
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("") + e.what());
    }
    return "";
}

void Scanner::scanUI() {
    try {
        int tempStart, tempEnd;
        system("clear");//comment out if compiling on windows
        //system("cls"); //comment out if compiling on linux
        std::cout << R"(
                       _
   _ __    ___   _ __ | |_   ___   ___  __ _  _ __   _ __    ___  _ __    ___   _      _
  | '_ \  / _ \ | '__|| __| / __| / __|/ _` || '_ \ | '_ \  / _ \| '__|  / __|_| |_  _| |_
  | |_) || (_) || |   | |_  \__ \| (__| (_| || | | || | | ||  __/| |    | (__|_   _||_   _|
  | .__/  \___/ |_|    \__| |___/ \___|\__,_||_| |_||_| |_| \___||_|     \___| |_|    |_|
  |_|
    )" << std::endl;
        std::cout << "\n\n  Enter hostname: "; //only DNS or ipv4
        //scanf("%s", &host); //cin doesnt work when entering host
	std::cin>>host;
        ipv4 = resolveDNS();
        std::cout << "  Enter starting port: ";
        std::cin >> tempStart;
        std::cout << "  Enter ending port: ";
        std::cin >> tempEnd;
        if (tempStart <= 0 || tempStart > 65535 || tempEnd <= 0 || tempEnd > 65535) {
            throw std::invalid_argument("Invalid port number. Port numbers must be in the range of 1-65535.");
        } else{
            start = static_cast<uint16_t>(tempStart);
            end = static_cast<uint16_t>(tempEnd);
        }
        scanPorts();
    } catch (const std::exception& e){
        std::cerr <<"\n  "<< e.what()<<"\n"<<std::endl;
    }

}
