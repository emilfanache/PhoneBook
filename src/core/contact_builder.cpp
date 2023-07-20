#include "contact_builder.hpp"

ContactBuilder& ContactBuilder::HasLastName(const std::string& last_name) {
    contact_.last_name_ = last_name;
    return *this;
}

ContactBuilder& ContactBuilder::HasNumber(const std::string& number) {
    contact_.number_ = number;
    return *this;
}

ContactBuilder& ContactBuilder::HasPhoneType(Contact::PhoneType type) {
    contact_.type_ = type;
    return *this;
}

ContactBuilder& ContactBuilder::HasNickname(const std::string& nickname) {
    contact_.nickname_ = nickname;
    return *this;
}

ContactBuilder& ContactBuilder::HasAddress(const std::string& address) {
    contact_.address_ = address;
    return *this;
}
