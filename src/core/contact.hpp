
#ifndef CONTACT_HPP_
#define CONTACT_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <string>

class ContactBuilder;

class Contact {
 public:
    enum PhoneType { Home, Mobile, Other };

    /* defined a public constructor that sets everything empty
     * in order to get the data from the db
     */
    friend class ContactBuilder;
    friend std::ostream& operator<<(std::ostream& out, const Contact& rhs);
    static ContactBuilder Build(const std::string& name,
                                const std::string& number);

    std::string GetFirstName() const;
    std::string GetLastName() const;
    std::string GetNumber() const;
    Contact::PhoneType GetType() const;
    static std::string GetType(Contact::PhoneType type);
    static Contact::PhoneType GetType(const std::string& type);
    std::string GetNickname() const;
    std::string GetAddress() const;
    int GetUserId() const;

 private:
    /* Make constructor private to enforce the setting by using
     * the builder methods
     */
    explicit Contact(const std::string& first_name, const std::string& number);
    static void SetTypeMap();

    std::string first_name_;
    std::string last_name_;
    std::string number_;
    Contact::PhoneType type_;
    std::string nickname_;
    std::string address_;
    int user_id_;
    static std::map<Contact::PhoneType, std::string> type_str_;
};

std::ostream& operator<<(std::ostream& out, const Contact& rhs);

#endif  // CONTACT_HPP_
