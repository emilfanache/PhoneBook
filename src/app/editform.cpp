#include "contact.hpp"
#include "contact_builder.hpp"
#include "editform.h"
#include "pb_operations.hpp"
#include "ui_editform.h"

EditForm::EditForm(const std::string& number,
                   std::shared_ptr<PBOperations> pb_operations, QWidget* parent)
    : number_(number),
      pb_operations_(pb_operations),
      QDialog(parent),
      ui(new Ui::EditForm) {
    ui->setupUi(this);

    for (int pt = Contact::PhoneType::Home; pt <= Contact::PhoneType::Other;
         pt++) {
        ui->typeBoxEdit->addItem(
            Contact::GetType(static_cast<Contact::PhoneType>(pt)).c_str());
    }
    std::shared_ptr<Contact> contact = pb_operations_->GetContact(number_);
    ui->fnameInputEdit->setText(
        QString::fromStdString(contact->GetFirstName()));
    ui->lnameInputEdit->setText(QString::fromStdString(contact->GetLastName()));
    ui->numROLabel->setText(QString::fromStdString(contact->GetNumber()));
    Contact::PhoneType type = contact->GetType();
    ui->typeBoxEdit->setCurrentIndex(type);
    ui->nicknameInputEdit->setText(
        QString::fromStdString(contact->GetNickname()));
    ui->addrInputEdit->setText(QString::fromStdString(contact->GetAddress()));
}

EditForm::~EditForm() {
    delete ui;
}

void EditForm::on_closeButtonEdit_clicked() {
    this->close();
}

void EditForm::on_okButtonEdit_clicked() {
    std::string fname = ui->fnameInputEdit->text().toStdString();
    std::string lname = ui->lnameInputEdit->text().toStdString();
    std::string num = ui->numROLabel->text().toStdString();
    std::string type_str = ui->typeBoxEdit->currentText().toStdString();
    Contact::PhoneType type = Contact::GetType(type_str);
    std::string nickname = ui->nicknameInputEdit->text().toStdString();
    std::string addr = ui->addrInputEdit->toPlainText().toStdString();

    Contact local_contact = Contact::Build(fname, num)
                                .HasLastName(lname)
                                .HasPhoneType(type)
                                .HasNickname(nickname)
                                .HasAddress(addr);

    std::cout << local_contact << std::endl;

    pb_operations_->UpdateContact(local_contact);
    emit TriggerTableUpdate();
    this->close();
}