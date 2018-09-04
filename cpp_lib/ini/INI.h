#ifndef __LIB_CPP_INI__
#define __LIB_CPP_INI__
/**
 * @file INI.h
 * @author Various
 @code
    INI file format:
        [section1]
            key11 = value11
            key12 = value12
        [section2]
            key21 = value21
        [section2/section21]    ; this is a inline comment
            key221 = value221
        ; this is a comment
 @endcode
 */

#include <string>
#include <functional>
#include <memory>
#include <list>
#include <string>

namespace cppframework{namespace ini{
    ///< Dữ liệu của một section trong cấu hình
    class Section {
    public:
        Section();
        virtual ~Section();
        ///< Tên của section
        const std::string& Name() const;
        /**
        * @brief Trả lại giá trị của trường 'key' trong section. Trả lại string có độ dài bằng 0 nếu không tìm thấy key tương ứng
        */
        const std::string Value(const std::string& key) const ;
        /**
        * @brief Trả lại Section con có tên 'name' trong section hiện tại
        * @retval nullptr Không tìm thấy section tương ứng
        */

        int SetValue(const std::string &nameOfValue, const std::string &val) ;

        const std::shared_ptr<Section> FindSection(const std::string& name) const ;
        ///< Trả lại số lượng key-value trong section
        int ValueCount() const;
        ///< Trả lại số lượng section con trong section hiện tại
        int SectionCount() const;
        /**
        * @brief Gọi 'callback' qua tất cả các trường key-value của section. Nếu callback trả về khác 0, dừng vòng lặp
        * @return int Mã lỗi từ callback
        */
        int IterateValue  (const std::function<int(const std::string& key, const std::string& value)>& callback) const;
        /**
        * @brief Gọi 'callback' qua tất cả các section con của section hiện tại. Nếu callback trả về khác 0, dừng vòng lặp
        * @return int Mã lỗi từ callback
        */
        int IterateSection(const std::function<int(const std::shared_ptr<Section>& section)>& callback) const;
        ///< Xóa tất cả dữ liệu của section
        void Clear();
        /**
        * @brief Thiết lập tên của section
        * @return int Mã lỗi
        */
        int  SetName(const std::string &name);
        /**
        * @brief Thêm trường key-value vào section
        * @return int Mã lỗi
        */
        int  Insert(const std::string &key, const std::string& value);
        /**
        * @brief Thêm section con vào section hiện tại
        * @return int Mã lỗi
        */
        int  Insert(const std::string &name, const std::shared_ptr<Section>& section);
    private:
        struct KEYVALUE{
            std::string key;
            std::string value;
        };
        std::list<struct KEYVALUE> m_values;
        std::list<std::shared_ptr<Section>> m_sections;
        std::string m_name;
    };
    class Reader{
    public:
        virtual ~Reader(){}
        /**
         * @brief Đọc dữ liệu từ tập tin 'filePath'
         * @return std::shared_ptr<Section> Section root chứa tất cả các section đã đọc được
         */
        static std::shared_ptr<Section> Load(const std::string& filePath);
    private:
        static void _trim_comment(std::string& s);
    };
    class Writer{
    public:
        virtual ~Writer(){}
        /**
        * @brief Lưu tất cả dữ liệu trong section ra tập tin filePath
        * @return Mã lỗi
        */
        static int Save(const std::string &filePath, const std::shared_ptr<Section>& section, bool append = false);
    private:
        static int  _save(const std::string &root, std::ofstream& file, const std::shared_ptr<Section>& section, int deepLevel);
    };
}}

#endif

