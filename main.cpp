#include "Server.hpp"

int main(int ac, char **av)
{
    try
    {
        if(ac != 2)
            exit(EXIT_FAILURE);
        Server server(std::atoi(av[1]));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}