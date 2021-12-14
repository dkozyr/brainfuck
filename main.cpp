#include "core/Compiler.h"

#include <boost/program_options.hpp>

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;

    std::string script;
    po::options_description options("Allowed options");
    options.add_options()
        ("help", "produce help message")
        ("path", po::value<std::string>(&script), "Brainfuck script path")
        ("optimization", po::value<bool>()->default_value(true), "Use optimizations")
        ("array", po::value<size_t>()->default_value(1000000), "Data array size")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);    

    if(vm.count("help")) {
        std::cout << options;
        return 1;
    }

    try {
        std::cout << "Script: " << script << std::endl;
        const auto array_size = vm["array"].as<size_t>();

        Compiler bf(script);
        if(vm["optimization"].as<bool>()) {
            bf.ExecuteOptimized(array_size);
        } else {
            bf.Execute(array_size);
        }
    } catch(const std::exception& e) {
        std::cerr << "Failed! Exception: " << e.what() << std::endl;
    }
    return 0;
}
