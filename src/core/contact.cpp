#include <algorithm>

#include "contact.hpp"
#include "contact_builder.hpp"

std::map<Contact::PhoneType, std::string> Contact::type_str_;

Contact::Contact(const std::string& first_name, const std::string& number) {
    if (number.empty()) {
        throw(std::invalid_argument("Phone Number can not be empty!"));
    }

    if (!std::all_of(number.begin(), number.end(), ::isdigit)) {
        throw(std::invalid_argument("Phone Number can only contain numbers!"));
    }

    if (first_name.empty()) {
        throw(std::invalid_argument("First Name can not be empty!"));
    }

    first_name_ = first_name;
    number_ = number;
    SetTypeMap();
}

ContactBuilder Contact::Build(const std::string& name,
                              const std::string& number) {
    return ContactBuilder{name, number};
}

std::ostream& operator<<(std::ostream& out, const Contact& rhs) {
    out << "User ID:  " << rhs.user_id_ << std::endl
        << "First name:  " << rhs.first_name_ << std::endl
        << "Last name:   " << rhs.last_name_ << std::endl
        << "Phone Number: " << rhs.number_ << std::endl
        << "Type:         " << rhs.type_ << std::endl
        << "Nickname:    " << rhs.nickname_ << std::endl
        << "Address:     " << rhs.address_ << std::endl;

    return out;
}

std::string Contact::GetFirstName() const {
    return first_name_;
}

std::string Contact::GetLastName() const {
    return last_name_;
}

std::string Contact::GetNumber() const {
    return number_;
}

Contact::PhoneType Contact::GetType() const {
    return type_;
}

std::string Contact::GetType(Contact::PhoneType type) {
    auto itr = type_str_.find(type);
    if (itr != type_str_.end()) {
        return itr->second;
    }

    return std::string("Other");
}

Contact::PhoneType Contact::GetType(const std::string& type) {
    for (const auto& kv : type_str_) {
        if (kv.second == type) {
            return kv.first;
        }
    }

    return Contact::PhoneType::Other;
}

void Contact::SetTypeMap() {
    type_str_[Contact::PhoneType::Home] = "Home";
    type_str_[Contact::PhoneType::Mobile] = "Mobile";
    type_str_[Contact::PhoneType::Other] = "Other";
}

std::string Contact::GetNickname() const {
    return nickname_;
}

std::string Contact::GetAddress() const {
    return address_;
}

int Contact::GetUserId() const {
    return user_id_;
}
