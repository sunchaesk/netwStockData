
#include "netw.h"

string netw::sym = "";

using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

/*
  https://api.twelvedata.com/time_series?symbol=CNY/JPY&interval=1day&apikey=your_apikey */

/* TODO
 * - make parse_data and callback function more efficient
 */

inline bool file_exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}


void initialize_folder()
// check if the 'data' dir is created, if not create
{
    if(fs::is_directory("data")){
        return;
    }
    fs::create_directory("data");
}

void netw::initialize_setting(){
    if(!file_exists("Setting.txt")){
        ofstream ofile;
        ofile.open("Setting.txt", ios::out);
        ofile << "================================================\n";
        ofile << "=This is the setting text file for the program\n";
        ofile << "=Make a list of stock data you want below without the equal sign as the first character\n";
        ofile << "=If a 'v' is at the end of the line, then the program will not download full data (for first time downloads)";
        ofile << "=15: aapl v\n";
        ofile << "================================================\n";
        ofile << "=You also have to initialize the api key for TwelveData api\n";
        ofile << "=The program will read the line that starts with ':' as the api key\n";
        ofile << "=Put the api key below:\n";
        ofile << ":\n";
        ofile << "================================================\n";
        ofile.close();
    }
}

/////////////////// MEMBER FUNCTIONS //////////////////////////
void netw::print_data()
// function for debugging, just prints obj data
{
    cout << "\n\n=====================\n" << this->data_req
         << "\n\n" << this->outputsize << "\n\n" << this->startdate << "\n\n"
         << this->intv << "\n\n" << sym << "\n=============";
    initialize_setting();
}

vector<char> temp_buffer = {};
size_t parse_received_data(char *buffer, size_t itemsize, size_t nitems, void *ignorethis){
    initialize_folder();
    size_t bytes = itemsize * nitems;
    for (size_t i = 0; i < bytes; ++i){
        temp_buffer.push_back(buffer[i]);
    }
    return bytes;
}


void netw::run_curl()
// send login req to kibot
// but only if the response is that u r already logined
{
    curl = curl_easy_init();
    if(!curl){
        cerr << "curl init failure\n";
        exit(1);
    }

    curl_easy_setopt(curl, CURLOPT_URL, data_req);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA)
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_received_data);

    CURLcode result = curl_easy_perform(curl);
    cout << result << '\n';
    // checking if the returned data is json which means I am out of reqs
    string f_loc = "data/" + netw::sym + ".txt";
    ofstream ofile;
    ofile.open(f_loc, ios::app);
    if (temp_buffer[0] == '{'){
        cerr << "Req failed: Only 8 req allowed per minute\n";
        cerr << "Waiting and retrying...\n";
//
        usleep(60000000);
        result = curl_easy_perform(curl);
        if (temp_buffer[0] == '{'){
            cerr << "Out of reqs for the month\n";
            exit(1);
        }
    } else {
        for (char ch : temp_buffer) ofile << ch;
        temp_buffer.clear();
    }
    curl_easy_cleanup(curl);
}

void netw::change_url(const string& sym, int outputsize, const string& intv){
    string suffix = "";

    if (sym.size() < 1 || sym.size() > 12){
        cerr << "Symbol Error during URL change\n";
    }
    suffix += ("&symbol=" + sym);
    this->sym = sym;

    if (outputsize < 0 || outputsize > 5000){
        cerr << "Outputsize Error during URL change\n";
    }
    suffix += ("&outputsize=" + to_string(outputsize));
    this->outputsize = outputsize;

    suffix += ("&interval=" + intv);
    this->intv = intv;

    suffix += ("&apikey=" + string(api_key));

    suffix += "&format=csv";

    strncpy(data_req, "https://api.twelvedata.com/time_series?", 150);
    strncat(data_req, suffix.c_str(), 150);
}

////// CONSTRUCTORS /////////////////////////////////////////
netw::netw(const string& sym, const string& api)
// this constructor is for when downloading all data
{
    strncpy(api_key, api.c_str(), 70);

    string suffix = "";

    if (sym.size() < 1 || sym.size() > 12){
        cerr << "Symbol Error\n";
        exit(1);
    }
    suffix += ("&symbol=" + sym);
    this->sym = sym;

    this->outputsize = 5000;
    this->startdate = "NOT SPECIFIED";
    this->intv = "1day";

    suffix += "&outputsize=5000";
    suffix += "&interval=1day";
    suffix += ("&apikey=" + string(api_key));
    suffix += "&format=csv";

    // copy the neccessary and CONCAT the other flags
    strncpy(data_req, "https://api.twelvedata.com/time_series?", 150);
    strncat(data_req, suffix.c_str(), 150);

    initialize_setting();
}

netw::netw(const string& sym, int outputsize, const string& startdate, const string& intv, const string& api){
    // login url username: guest, psswd: guest
    // you get 20 min access
    strncpy(api_key, api.c_str(), 70);

    //data_req = "";
    //data_req += "http://api.kibot.com/?action=history";

    string suffix = "";

    if (sym.size() < 1 || sym.size() > 12){
        cerr << "Symbol Error\n";
        exit(1);
    }
    suffix += ("&symbol=" + sym);
    this->sym = sym;

    if (outputsize < 0 || outputsize > 5000){
        cerr << "Invalid Period Has to be between 1 and 5000\n";
        exit(1);
    }
    suffix += ("&outputsize=" + to_string(outputsize));
    this->outputsize = outputsize;

    if (startdate.size() > 12){
        cerr << "Invalid startdate (too long)\n";
        exit(1);
    }
    suffix += ("&startdate=" + startdate);
    this->startdate = startdate;

    suffix += ("&interval=" + intv);
    this->intv = intv;

    suffix += ("&apikey=" + string(api_key));

    suffix += "&format=csv";

    // init_curl();
    strncpy(data_req, "https://api.twelvedata.com/time_series?", 150);
    strncat(data_req, suffix.c_str(), 150);

    initialize_setting();
}

netw::netw(const string& sym, int outputsize, const string& intv, const string& api){

    strncpy(api_key, api.c_str(), 70);

    string suffix = "";

    if (sym.size() < 1 || sym.size() > 12){
        cerr << "Symbol Error\n";
        exit(1);
    }
    suffix += ("&symbol=" + sym);
    this->sym = sym;

    if (outputsize < 0 || outputsize > 5000){
        cerr << "Invalid Period Has to be between 1 and 5000\n";
        exit(1);
    }
    suffix += ("&outputsize=" + to_string(outputsize));
    this->outputsize = outputsize;

    suffix += ("&interval=" + intv);
    this->intv = intv;

    suffix += ("&apikey=" + string(api_key));

    suffix += "&format=csv";

    // init_curl();
    strncpy(data_req, "https://api.twelvedata.com/time_series?", 150);
    strncat(data_req, suffix.c_str(), 150);

    initialize_setting();
}


// void parse_data(char *buffer, size_t byte, string f_loc) noexcept
//     // used for curl option CURLOPT_WRITEFUNCTION
//     // not the actual, the data is (the pointer buffer) is sent from the actual WRITEFUNCTION
// {
//     ofstream ofile;
//     ofile.open(f_loc, ios::out | ios::app);
//     string temp = "";
//     for (long unsigned int i = 0; i < byte; ++i){
//         char curr = buffer[i];
//         if (curr != '\n'){
//             temp += curr;
//         } else {
//             ofile << temp << '\n';
//             temp = "";
//         }
//     }
// }
// 
// // void file_not_exist(char *buffer, size_t byte){
// //     initialize_folder();
// // }
// 
// size_t parse_received_data(char *buffer, size_t itemsize, size_t nitems, void *ignorethis) noexcept
//     // create file with sym name in data folder
//     // send the buffer point to parse_data func
// {
//     string fname = curl_network::sym + ".txt";
//     initialize_folder();
//     size_t bytes = itemsize * nitems;
//     ifstream ifile;
//     ifile.open("data" + '/' + fname);
//     if (!ifile){
//         fstream f;
//         f.open("data/" + fname, ios::out);
//         f.close();
//     }
//     parse_data(buffer, bytes, ("data/" + fname));
//     return bytes;
// }
