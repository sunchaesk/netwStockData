#ifndef NETW_CONTROLLER_H
#define NETW_CONTROLLER_H

#include <bits/stdc++.h>
#include "netw.h"

using namespace std;

class netw_controller{
    public:
        netw_controller();

        void main_loop(int outputsize);
        
        void parse_setting_file();
        void stock_list_send_req(int outputsize);
        void initialize_setting();

        // debug use functions
        void d_print_stock_list();
    private:
        vector<string> stock_list;
        string api_key;
};

#endif
