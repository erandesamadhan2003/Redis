#include "./resp_parser.h"
using namespace std;

RESPParser::RESPParser() {}

vector<string> RESPParser::parse(const string& input) {
    vector<string> result;
    size_t pos = 0;

    if (input.empty()) {
        return result;
    }

    if (input[0] != '*') {
        cerr << "Invalid RESP format: Expected '*' at the beginning\n";
        return result;
    }

    pos++; 

    size_t crlf_pos = input.find("\r\n", pos);
    if (crlf_pos == string::npos) {
        cerr << "Invalid RESP format: Missing CRLF after array size\n";
        return result;
    }

    string size_str = input.substr(pos, crlf_pos - pos);
    
    if (size_str.empty() || (size_str[0] == '-' && size_str.length() == 1)) {
        cerr << "Invalid RESP format: Invalid array size\n";
        return result;
    }
    
    int result_size = stoi(size_str);
    
    if (result_size < 0) {
        cerr << "Invalid RESP format: Array size cannot be negative\n";
        return result;
    }
    
    result.reserve(result_size);
    pos = crlf_pos + 2; 

    int parsed_count = 0;
    while (pos < input.size() && parsed_count < result_size) {
        // Check for bulk string type
        if (input[pos] != '$') {
            cerr << "Invalid RESP format: Expected '$' at the beginning of bulk string\n";
            return result;
        }

        pos++; 
        
        crlf_pos = input.find("\r\n", pos);
        if (crlf_pos == string::npos) {
            cerr << "Invalid RESP format: Missing CRLF after bulk string length\n";
            return result;
        }

        string len_str = input.substr(pos, crlf_pos - pos);
        
        if (len_str.empty()) {
            cerr << "Invalid RESP format: Empty bulk string length\n";
            return result;
        }
        
        int bulk_length = stoi(len_str);
        pos = crlf_pos + 2; 
        
        if (bulk_length == -1) {
            result.push_back("");  
            parsed_count++;
            continue;
        }
        
        if (bulk_length < 0) {
            cerr << "Invalid RESP format: Invalid bulk string length (only -1 allowed for null)\n";
            return result;
        }
        
        if (pos + bulk_length > input.size()) {
            cerr << "Invalid RESP format: Bulk string length exceeds input size\n";
            return result;
        }

        result.push_back(input.substr(pos, bulk_length));
        pos += bulk_length;
        
        if (pos + 1 >= input.size() || input[pos] != '\r' || input[pos + 1] != '\n') {
            cerr << "Invalid RESP format: Missing CRLF after bulk string data\n";
            return result;
        }
        pos += 2; 
        
        parsed_count++;
    }
    
    if (parsed_count != result_size) {
        cerr << "Warning: Expected " << result_size << " elements but parsed " << parsed_count << "\n";
    }
    
    return result;
}

string RESPParser::serialize(const vector<string>& data) {
    string result = "*" + to_string(data.size()) + "\r\n";
    for (const auto& item : data) {
        result += "$" + to_string(item.size()) + "\r\n" + item + "\r\n";
    }
    return result;
}