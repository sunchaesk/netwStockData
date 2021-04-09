#ifndef NETW_H
#define NETW_H

#include <bits/stdc++.h>
#include <curl/curl.h>


using namespace std;

class curl_network{
    public:
        curl_network(const string& sym, int outputsize, const string& startdate, const string& intv);
        curl_network(const string& sym);
        curl_network(const string& sym, int outputsize, const string& intv);
        curl_network() {};
        // size_t parse_received_data(char *buffer, size_t itemsize, size_t nitmes, void *ignorethis);
        void initialize_folder();
        void run_curl();
        void change_url(const string& sym, int outputsize, const string& intv);
        static string sym;

        // functions for debug (just testin')
        void print_data();
    private:
        CURL *curl;
        char api_key[70];
        char data_req[150];

        int outputsize;
        string startdate;
        string intv;

};

/*
https://api.twelvedata.com/time_series?symbol=CNY/JPY&interval=1day&apikey=your_apikey */

#endif
