#pragma once

#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

class Value {
public:
    explicit Value() = default;

    virtual ~Value() noexcept = default;

    Value(const bool value) {
        *this = value;
    }

    Value(const int value) {
        *this = value;
    }

    Value(const double value) {
        *this = value;
    }

    Value(const char *value) : _value(value) {}

    Value(const std::string &value) : _value(value) {}

    const Value &operator=(const bool value) {
        _value = value ? "true" : "false";
        return *this;
    }

    const Value &operator=(const int value) {
        _value = std::to_string(value);
        return *this;
    }

    const Value &operator=(const double value) {
        _value = std::to_string(value);
        return *this;
    }

    const Value &operator=(const char *value) {
        _value = value;
        return *this;
    }

    const Value &operator=(const std::string &value) {
        _value = value;
        return *this;
    }

    operator bool() {
        return _value == "true";
    }

    operator int() {
        return std::stoi(_value);
    }

    operator double() {
        return std::stof(_value);
    }

    operator std::string &() {
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

    Ini(const std::string fileName) : _fileName(fileName) {}

    const std::string Trim(std::string str, const std::string &whitespace = " \r\n\t\v\f") {
        if (str.empty()) {
            std::cerr << "loading file failed: key or value is not found." << std::endl;
            return str;
        }
        str.erase(0, str.find_first_not_of(whitespace));
        str.erase(str.find_last_not_of(whitespace) + 1);
        return str;
    }


    bool Load(const std::string &fileName) {
        _sections.clear();
        _fileName = fileName;

        std::ifstream fin(_fileName);
        if (false == fin.is_open()) {
            std::cerr << "loading file failed: " << fileName << "is not found." << std::endl;
            return false;
        }
        std::string line = "";
        std::string section = "";
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
                _sections[section][Trim(line.substr(0, pos))] = Trim(line.substr(pos + 1, line.length()));
            }
        }
        fin.close();
        return true;
    }

    bool Save(const std::string &fileName) {
        std::ofstream ofs(fileName.c_str());
        if (ofs.fail()) {
            std::cerr << "open file failed: " << fileName << "is not found." << std::endl;
            return false;
        }
        ofs << GetString();
        ofs.close();
        return true;
    }

    std::string GetString() const {
        std::stringstream ss;
        for (auto it = _sections.begin(); it != _sections.end(); ++it) {
            ss << "[" << it->first << "]" << std::endl;
            for (auto iter = it->second.begin(); iter != it->second.end(); ++iter) {
                ss << iter->first << " = " << iter->second << std::endl;
            }
            ss << std::endl;
        }
        return ss.str();
    }

    void Clear() {
        _sections.clear();
    }


    Value &Get(const std::string &section, const std::string &key) {
        if (Has(section, key)) {
            return _sections[section][key];
        }
    }

    void Set(const std::string &section, const std::string &key, const Value &value) {
        _sections[section][key] = value;
    }

    void Remove(const std::string &section) {
        _sections.erase(section);
    }

    void Remove(const std::string &section, const std::string &key) {
        auto it = _sections.find(section);
        if (it != _sections.end()) {
            it->second.erase(key);
        }
    }

    bool Has(const std::string &section) {
        return _sections.find(section) != _sections.end();
    }

    bool Has(const std::string &section, const std::string &key) {
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
        for (auto it = ini._sections.begin(); it != ini._sections.end(); ++it) {
            os << "[" << it->first << "]" << std::endl;
            for (auto iter = it->second.begin(); iter != it->second.end(); ++iter) {
                os << iter->first << " = " << iter->second << std::endl;
            }
        }
        return os;
    }

private:
    std::string _fileName;
    std::map<std::string, Section> _sections;
};