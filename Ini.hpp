#pragma once

#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace cmf {

    class Value {
    public:
        explicit Value() = default;

        virtual ~Value() noexcept = default;

        explicit Value(const bool value) {
            *this = value;
        }

        explicit Value(const int value) {
            *this = value;
        }

        explicit Value(const double value) {
            *this = value;
        }

        explicit Value(const char *value) : _value(value) {}

        explicit Value(std::string value) : _value(std::move(value)) {}

        Value &operator=(const bool value) {
            _value = value ? "true" : "false";
            return *this;
        }

        Value &operator=(const int value) {
            _value = std::to_string(value);
            return *this;
        }

        Value &operator=(const double value) {
            _value = std::to_string(value);
            return *this;
        }

        Value &operator=(const char *value) {
            _value = value;
            return *this;
        }

        Value &operator=(const std::string &value) {
            _value = value;
            return *this;
        }

        explicit operator bool() {
            return _value == "true";
        }

        explicit operator int() {
            return std::stoi(_value);
        }

        explicit operator double() {
            return std::stof(_value);
        }

        explicit operator std::string &() {
            return _value;
        }

        bool operator==(const Value &rhs) const {
            return _value == rhs._value;
        }

        bool operator!=(const Value &rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream &operator<<(std::ostream &os, const Value &value) {
            os << value._value;
            return os;
        }

    private:
        std::string _value;
    };

    class Ini {
    public:
        using Section = std::map<std::string, Value>;

    public:
        explicit Ini() = default;

        virtual ~Ini() noexcept = default;

        explicit Ini(std::string fileName) : _fileName(std::move(fileName)) {}

        static std::string trim(std::string str, const std::string &whitespace = " \r\n\t\v\f") {
            if (str.empty()) {
                std::cerr << "loading file failed: key or value is not found." << std::endl;
                return str;
            }
            str.erase(0, str.find_first_not_of(whitespace));
            str.erase(str.find_last_not_of(whitespace) + 1);
            return str;
        }


        bool load(const std::string &fileName) {
            _sections.clear();
            _fileName = fileName;

            std::ifstream fin(_fileName);
            if (!fin.is_open()) {
                std::cerr << "loading file failed: " << fileName << "is not found." << std::endl;
                return false;
            }
            std::string line;
            std::string section;
            while (std::getline(fin, line)) {
                if ("\r" == line || '#' == line[0]) {  //如果是注释就跳过
                    continue;
                } else if ('[' == line[0]) {    //如果是section就解析出标题，给这个标题创建一个空内容
                    section = line.substr(1, line.find_first_of(']') - 1);
                    _sections[section] = Section();
                } else {    //最后就是内容,把=两边解析出来去除空格存到map中
                    int pos = line.find_first_of('=');
                    if (pos < 0) {
                        std::cerr << "loading file failed: = is not found." << std::endl;
                        return false;
                    }
                    _sections[section][trim(line.substr(0, pos))] = trim(line.substr(pos + 1, line.length()));
                }
            }
            fin.close();
            return true;
        }

        [[nodiscard]] bool save(const std::string &fileName) const {
            std::ofstream ofs(fileName.c_str());
            if (ofs.fail()) {
                std::cerr << "open file failed: " << fileName << "is not found." << std::endl;
                return false;
            }
            ofs << getString();
            ofs.close();
            return true;
        }

        [[nodiscard]] std::string getString() const {
            std::stringstream ss;
            for (const auto &_section: _sections) {
                ss << "[" << _section.first << "]" << std::endl;
                for (const auto &iter: _section.second) {
                    ss << iter.first << " = " << iter.second << std::endl;
                }
                ss << std::endl;
            }
            return ss.str();
        }

        void clear() {
            _sections.clear();
        }


        Value &get(const std::string &section, const std::string &key) {
            if (has(section, key)) {
                return _sections[section][key];
            }
        }

        void set(const std::string &section, const std::string &key, const Value &value) {
            _sections[section][key] = value;
        }

        void remove(const std::string &section) {
            _sections.erase(section);
        }

        void remove(const std::string &section, const std::string &key) {
            auto it = _sections.find(section);
            if (it != _sections.end()) {
                it->second.erase(key);
            }
        }

        bool has(const std::string &section) {
            return _sections.find(section) != _sections.end();
        }

        bool has(const std::string &section, const std::string &key) {
            auto it = _sections.find(section);
            if (it != _sections.end()) {
                return it->second.find(key) != it->second.end();
            }
            return false;
        }

        Section &operator[](const std::string &section) {
            //问题：如果没有选型则会添加！！！
            return _sections[section];
        }

        friend std::ostream &operator<<(std::ostream &os, const Ini &ini) {
            for (const auto &_section: ini._sections) {
                os << "[" << _section.first << "]" << std::endl;
                for (const auto &iter: _section.second) {
                    os << iter.first << " = " << iter.second << std::endl;
                }
            }
            return os;
        }

    private:
        std::string _fileName;
        std::map<std::string, Section> _sections;
    };
}