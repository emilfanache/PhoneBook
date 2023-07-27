#include <string>

#include <QObject>

#include "contact.hpp"
#include "contact_builder.hpp"
#include "insertform.h"
#include "mainwindow.h"
#include "ui_insertform.h"

InsertForm::InsertForm(std::shared_ptr<PBOperations> pb_operations,
                       QWidget* parent)
    : pb_operations_(pb_operations), QDialog(parent), ui(new Ui::InsertForm) {
    ui->setupUi(this);

    for (int pt = Contact::PhoneType::Home; pt <= Contact::PhoneType::Other;
         pt++) {
        ui->typeBox->addItem(
            Contact::GetType(static_cast<Contact::PhoneType>(pt)).c_str());
    }
}

InsertForm::~InsertForm() {
    delete ui;
}

void InsertForm::on_closeButton_clicked() {
    this->close();
}

void InsertForm::on_okButton_clicked() {
    std::string fname = ui->fnameInput->text().toStdString();
    std::string lname = ui->lnameInput->text().toStdString();
    std::string num = ui->numberInput->text().toStdString();
    std::string type_str = ui->typeBox->currentText().toStdString();
    Contact::PhoneType type = Contact::GetType(type_str);
    std::string nickname = ui->nicknameInput->text().toStdString();
    std::string addr = ui->addrInput->toPlainText().toStdString();

    try {
        Contact local_contact = Contact::Build(fname, num)
                                    .HasLastName(lname)
                                    .HasPhoneType(type)
                                    .HasNickname(nickname)
                                    .HasAddress(addr);

        // std::cout << local_contact << std::endl;
        pb_operations_->AddContact(local_contact);
    } catch (const std::runtime_error& e) {
        MainWindow::PrintErrorLabel(ui->errorLabel, e.what());
        return;
    } catch (const std::invalid_argument& e) {
        MainWindow::PrintErrorLabel(ui->errorLabel, e.what());
        return;
    } catch (...) {
        MainWindow::PrintErrorLabel(ui->errorLabel, "General error!");
        return;
    }
    emit TriggerTableUpdate();
    this->close();
}