#include "signal_opt_service.h"

int main(int argc, char const *argv[])
{
    signal_opt_service::signal_opt_service so_service;
    if(so_service.initialize())
    {
        so_service.start();
    }
    return 0;
}
