#ifndef CPP_LIB_CSVFILE_H
#define CPP_LIB_CSVFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdint.h>



     
class CSVFile {

public:
typedef std::vector<std::vector<std::string> > CsvVector;
friend std::ostream& print(std::ostream&, const CSVFile&);

public:
    CSVFile();
    CSVFile(const std::string&);
    ~CSVFile();

    void set_filepath(std::string file);
    int open(bool write = false);
    void close();

    inline std::vector<std::string>::size_type get_size();
    uint32_t get_count();
    inline void headers();
    std::vector<std::string> get_header();

    CsvVector get_column(const int&);
    CsvVector get_column(const std::vector<int>&);
    CsvVector get_column(const std::string&);
    CsvVector get_column(const std::vector<std::string>&);

    std::vector<std::string> get_row(const int&);
    CsvVector get_row(const int&, const int&); // get some rows in a range

    void write_row(const std::map<std::string,std::string>&);
    void write_row(const std::vector<std::string>&row_vec);

    inline CsvVector rows() {
        return contents;
    }

    void display(const std::vector<std::string> &row);
    void display(const CsvVector &rows);
    void display_all();
    void clear_all();
    bool is_open() { return open_;}

private:
    std::string          m_filename;
    std::vector<std::string>  m_header;
    CsvVector            contents;
    std::ifstream        stream_read;
    std::ofstream        stream_write;
    bool                 open_;

    inline std::string remove_extension(const std::string&);
    std::vector<std::string> column_info(const std::string&);
    std::string merge_row_vector(const std::vector<std::string>&);
    int index_from_string(const std::string&);
    std::map<int,std::string> index_from_string(const std::map<std::string,std::string>& row_map);
};


#endif /* CPP_LIB_CSVFILE_H */
