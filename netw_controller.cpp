
#include "netw_controller.h"


using namespace std;

/////////////////// DEBUG FUNCTIONS ///////////////////////////////
void netw_controller::d_print_stock_list()
// just prints data in stock list vector of strings
{
    for (string s : stock_list) cout << s << '\n';
}

////////////// FILE SPECIFIC FUNCTIONS ////////////////////////

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
bool file_exists(const string& s){
    if (FILE * file = fopen(s.c_str(), "r")){
        fclose(file);
        return true;
    } else {
        return false;
    }
}

//////////////// MEMBER FUNCTION /////////////////////

void netw_controller::initialize_setting(){
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
        ofile << "=The program will read the line that starts with '::' as the api key\n";
        ofile << "=Put the api key below:\n";
        ofile << "::\n";
        ofile << "================================================\n";
        ofile.close();
    }
}

void netw_controller::parse_setting_file(){
    netw c("def", api_key);
    string line;
    ifstream ifile ("Setting.txt");
    if (ifile.is_open()){
        while( getline(ifile, line) ){
            if(line[0] == '='){
                continue;
            } else if(line[0] == ':'){
                string api = line.substr(1, line.size());
                this->api_key = api;
            } else if (line.size() > 12 || line.size() < 1) {
                cerr << "Invalid Stock Symbol is Setting.txt download list\n";
                exit(1);
            } else {
                stock_list.push_back(trim_copy(line));
            }      
        }
        ifile.close();
    } else {
        cerr << "Can't open Setting.txt error\n";
    }
}

void netw_controller::stock_list_send_req(int outputsize){
    // set initial string stock as TEMP so that the format will be csv
    netw c("TEMP", api_key);
    for (size_t i = 0; i < stock_list.size(); ++i){
        if (file_exists(stock_list[i] + ".txt")){
            cout << "\n============" << stock_list[i] << " file exists, no update made===========\n";
        } else {
            c.change_url(stock_list[i], outputsize, "1day");
            cout << stock_list[i] << '\n';
            c.print_data();
            c.run_curl();
        }
    }
}

void netw_controller::main_loop(int outputsize){
    initialize_setting();
    parse_setting_file();
    stock_list_send_req(outputsize);
}

//////////////// CONSTRUCTOR ///////////////////////
netw_controller::netw_controller()
// Nothing planned during construction yet for algo_main class
{

}
