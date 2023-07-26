#ifndef CONTACT_BUILDER_HPP_
#define CONTACT_BUILDER_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "contact.hpp"

class ContactBuilder {
 public:
    explicit ContactBuilder(const std::string& name, const std::string& number)
        : contact_(name, number) {}

    ContactBuilder& HasLastName(const std::string& last_name);
    ContactBuilder& HasNumber(const std::string& home_number);
    ContactBuilder& HasPhoneType(Contact::PhoneType type);
    ContactBuilder& HasNickname(const std::string& nickname);
    ContactBuilder& HasAddress(const std::string& address);
    ContactBuilder& HasUserId(int user_id);

    operator Contact() const { return std::move(contact_); }

 private:
    Contact contact_;
};

#endif  // CONTACT_BUILDER_HPP_
