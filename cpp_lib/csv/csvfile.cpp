
#ifndef CSV_FILE_H_
#define CSV_FILE_H_

#include <csvfile.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <array>

// file doc: http://www.cplusplus.com/doc/tutorial/files/
// stream_readtream constructor: http://www.cplusplus.com/forum/general/17771/#msg89650

using namespace std;  

CSVFile::CSVFile(const string& file_name) {
    m_filename = file_name;
    open_ = false;
}

CSVFile::~CSVFile()
{
    close();
}

void CSVFile::set_filepath(string file)
{
    m_filename = file;
}



int CSVFile::open(bool write)
{    
    contents.clear();
    stream_read.open(m_filename, ios::binary);
    if(!stream_read.is_open()) {
        if(write) {
            stream_write.open(m_filename, ios::binary);
            if(stream_write.is_open()) {
                open_ = true;
                return 0;
            }
            return -1;
        } else {
            std::cout << "read only but file not found" << std::endl;
            return -1;
        }

    } else {
        if(write) {
            string out_file = remove_extension(m_filename) + "_OUT.csv";
            stream_write.open(out_file, ios::binary);
            stream_write << stream_read.rdbuf(); // Copy read file into output file
        }
    }

    open_ = true;

    bool line_is_header = true;         // First line assumed to be the header

    stream_read.seekg(0);               // "Rewind" infile stream

    while (stream_read) {
        // Grab whole row
        string s;
        if (!getline(stream_read, s))
            break;

        istringstream ss (s);
        vector<string> record;

        while(ss) {
            // Split string by comma delimiter
            string s;
            if (!getline(ss, s, ','))
                break;
            record.push_back(s);
        }

        if (line_is_header)
            m_header = record;
        else
            contents.push_back(record);

        line_is_header = false; // First iteration clears through the header
    }

    return 0;
}

void CSVFile::close()
{
    if(stream_read.is_open())
        stream_read.close();
    if(stream_write.is_open())
        stream_write.close();

    clear_all();
    open_ = false;
}

void CSVFile::write_row(const map<string,string>& row_map) {
    vector<string> row_vec(m_header.size(), "NULL");
    map<int,string> index_map = index_from_string(row_map);
    for(auto it = index_map.begin(); it != index_map.end(); ++it) {
        row_vec[it->first] = it->second;
    }
    // for(auto&& i : row_vec) { cout << i << endl; }

    string v_string = merge_row_vector(row_vec);
    stream_write << endl << v_string;
}

void CSVFile::write_row(const std::vector<string> &row_vec)
{
    string v_string = merge_row_vector(row_vec);
    stream_write << v_string << endl;
}

map<int,string> CSVFile::index_from_string(const map<string,string>& row_map) {
    map<int,string> index_map;
    for(auto it = row_map.begin(); it != row_map.end(); ++it) {
        index_map[index_from_string(it->first)] = it->second;
    }
    return index_map;
}

int CSVFile::index_from_string(const string& s) {
    // Given column string, returns index of column
    int pos = find(m_header.begin(), m_header.end(), s) - m_header.begin();
    if (pos == (int)m_header.size()) {
        throw out_of_range("\nColumn: " + s + " not found in header");
    }
    return pos;
}

// http://stackoverflow.com/questions/1430757/c-vector-to-string
string CSVFile::merge_row_vector(const vector<string>& row_vec) {
    stringstream sstream;
    for(vector<string>::size_type ctr = 0; ctr < row_vec.size(); ++ctr) {
        string current = row_vec[ctr];
        // cout << "called : " << current << endl;
        if (ctr == 0)
            sstream << current << ",";
        else if (current == "NULL")
            sstream << ",";
        else if (ctr == row_vec.size() - 1)
            sstream << current;
        else
            sstream << current << ",";
    }
    return sstream.str();

//    stringstream sstream;
//    for(vector<string>::size_type ctr = 0; ctr < row_vec.size(); ++ctr) {
//        string current = row_vec[ctr];
//        // cout << "called : " << current << endl;
//        if (ctr == 0)
//            sstream << current << "\t";
//        else if (current == "NULL")
//            sstream << "\t";
//        else if (ctr == row_vec.size() - 1)
//            sstream << current;
//        else
//            sstream << current << "\t";
//    }
//    return sstream.str();

}

vector<string> CSVFile::column_info(const string& s) {
    // Returns all data in the passed column(s)
    // Need to overload this function later
    vector<string> data;
    int index = index_from_string(s);

    for (auto&& i : contents) {
        data.push_back(i[index]);
    }

    return data;
}

CSVFile::CsvVector CSVFile::get_column(const int& index) {
    CSVFile::CsvVector column_data;
    vector<string> buffer;

    for(unsigned i = 0; i < contents.size(); ++i) {
        buffer.push_back(contents[i][index]);
    }
    column_data.push_back(buffer);
    return column_data;
}

CSVFile::CsvVector CSVFile::get_column(const vector<int>& indices) {
    CSVFile::CsvVector column_data;
    for(auto&& index : indices)
    {
        vector<string> buffer;
        for(unsigned i = 0; i < contents.size(); ++i)
        {
            buffer.push_back(contents[i][index]);
        }
        column_data.push_back(buffer);
    }
    return column_data;
}

CSVFile::CsvVector CSVFile::get_column(const string& s) {
    CSVFile::CsvVector column_data;
    column_data.push_back(column_info(s));
    return column_data;
}

CSVFile::CsvVector CSVFile::get_column(const vector<string>& col_vec) {
    CSVFile::CsvVector column_data;
    for (auto&& column : col_vec) {
        column_data.push_back(column_info(column));
    }
    return column_data;
}

vector<string> CSVFile::get_row(const int& i) {
    return contents[i];
}

CSVFile::CsvVector CSVFile::get_row(const int& start, const int& end) {
    CsvVector row_range;
    for(unsigned i = start; i != (unsigned)end; ++i) {
        row_range.push_back(contents[i]);
    }
    return row_range;
}

ostream& print(ostream& os, const CSVFile& csv) {
    string csv_string;
    for (auto&& i : csv.contents) {
        stringstream s;
        for (auto&& j : i) {
            s << j << ' ';
        }
        s << "\n";
        csv_string += s.str();
    }
    os << csv_string;
    return os;
}



ostream& print(ostream&, const CSVFile&);

void CSVFile::headers() {
    for (auto&& i : m_header)
        cout << i << ", ";
}

vector<string>::size_type CSVFile::get_size() {
    return contents.size();
}

uint32_t CSVFile::get_count()
{
    return contents.size();
}

string CSVFile::remove_extension(const string& s) {
    string ret = s;
    ret.erase(ret.find_last_of("."), string::npos);
    return ret;
}

vector<string> CSVFile::get_header() {
    return m_header;
}


//CSVFile::CsvVector CSVFile::rows() {
//    return contents;
//}

void CSVFile::display(const vector<string> &row)
{
    if(!row.size())
      return;
    std::vector<std::string>::const_iterator i=row.begin();

    for(;i != row.end();++i)
      std::cout<< "\t" <<*i << "\t";
}

void CSVFile::display(const CSVFile::CsvVector &rows)
{
    if(!rows.size())
      return;
    CsvVector::const_iterator i=rows.begin();
    for(; i != rows.end(); ++i){
      display(*i);
      std::cout<<std::endl;
    }
}

void CSVFile::display_all()
{
    display(contents);
}

void CSVFile::clear_all() {
    for(auto &var : contents)
        var.clear();
    contents.clear();
}

#endif
