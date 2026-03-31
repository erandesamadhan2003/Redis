#include "./resp_parser.h"
using namespace std;

RESPParser::RESPParser() {}

vector<string> RESPParser::parse(const string& input) {
    vector<string> result;
    size_t pos = 0;

    if (input.empty()) return result;

    if (input[0] != '*') {
        cerr << "Invalid RESP format: Expected '*' at the beginning\n";
        return result;
    }

    pos++;
    size_t crlf_pos = input.find("\r\n", pos);
    if (crlf_pos == string::npos) return result;

    int result_size = stoi(input.substr(pos, crlf_pos - pos));
    if (result_size < 0) return result;

    result.reserve(result_size);
    pos = crlf_pos + 2;

    int parsed_count = 0;
    while (pos < input.size() && parsed_count < result_size) {
        if (input[pos] != '$') return result;
        pos++;

        crlf_pos = input.find("\r\n", pos);
        if (crlf_pos == string::npos) return result;

        int bulk_length = stoi(input.substr(pos, crlf_pos - pos));
        pos = crlf_pos + 2;

        if (bulk_length == -1) {
            result.push_back("");
            parsed_count++;
            continue;
        }

        if (bulk_length < 0 || pos + bulk_length > input.size()) return result;

        result.push_back(input.substr(pos, bulk_length));
        pos += bulk_length;

        if (pos + 1 >= input.size() || input[pos] != '\r' || input[pos+1] != '\n') return result;
        pos += 2;

        parsed_count++;
    }

    return result;
}

string RESPParser::serialize(const vector<string>& data) {
    if (data.size() == 0) return "$-1\r\n";
    if (data.size() == 1) return "$" + to_string(data[0].size()) + "\r\n" + data[0] + "\r\n";
    string result = "*" + to_string(data.size()) + "\r\n";
    for (size_t i = 0; i < data.size(); ++i) {
        result += "$" + to_string(data[i].size()) + "\r\n" + data[i] + "\r\n";
    }
    return result;
}