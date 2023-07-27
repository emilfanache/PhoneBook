#include <string>
#include <utility>

#include <QComboBox>
#include <QDebug>
#include <QObject>
#include <QPushButton>

#include "editform.h"
#include "insertform.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "contact.hpp"
#include "contact_builder.hpp"
#include "pb_operations.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    try {
        SetPBOperations(std::make_shared<PBOperations>());
    } catch (const std::runtime_error& e) {
        PrintErrorLabel(ui->errorLabel, e.what());
        return;
    }
    ListContacts();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::SetPBOperations(std::shared_ptr<PBOperations> oper) {
    pb_operations_ = std::move(oper);
}

void MainWindow::PrintErrorLabel(QLabel* label, const char* err_str) {
    label->setText(QString::fromUtf8(err_str));
    label->setStyleSheet("QLabel { color: red; font: bold; }");
}

void MainWindow::ListContacts() {
    std::vector<std::shared_ptr<Contact>> all_contacts;
    QTableWidget* tbl = ui->PhonebookViewTable;
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    try {
        pb_operations_->GetAllContacts(&all_contacts);
        tbl->setRowCount(all_contacts.size());
    } catch (...) {
        PrintErrorLabel(ui->errorLabel,
                        "Error fetching contacts from the database!");
        ui->updateButton->setEnabled(false);
        ui->refreshButton->setEnabled(false);
        tbl->setColumnHidden(MainWindow::ColumnID::ID, true);
        return;
    }

    if (all_contacts.size() == 0) {
        ui->updateButton->setEnabled(false);
        ui->refreshButton->setEnabled(false);
    } else {
        ui->updateButton->setEnabled(true);
        ui->refreshButton->setEnabled(true);
    }

    int row_id = 0;
    for (const auto& contact : all_contacts) {
        tbl->setItem(row_id, MainWindow::ColumnID::FirstName,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetFirstName())));
        tbl->setItem(row_id, MainWindow::ColumnID::LastName,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetLastName())));
        tbl->setItem(
            row_id, MainWindow::ColumnID::Number,
            new QTableWidgetItem(QString::fromStdString(contact->GetNumber())));
        // make number column read only
        QTableWidgetItem* item =
            tbl->item(row_id, MainWindow::ColumnID::Number);

        // type
        QComboBox* typeBox = new QComboBox();
        for (int pt = Contact::PhoneType::Home; pt <= Contact::PhoneType::Other;
             pt++) {
            typeBox->addItem(
                Contact::GetType(static_cast<Contact::PhoneType>(pt)).c_str());
        }
        typeBox->setCurrentIndex(contact->GetType());
        tbl->setCellWidget(row_id, MainWindow::ColumnID::Type, typeBox);

        tbl->setItem(row_id, MainWindow::ColumnID::Nickname,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetNickname())));
        tbl->setItem(row_id, MainWindow::ColumnID::Address,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetAddress())));

        QWidget* pWidget = new QWidget();
        QPushButton* delete_row = new QPushButton("Delete", this);
        QPushButton* edit_row = new QPushButton("Edit", this);
        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(delete_row);
        pLayout->addWidget(edit_row);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        tbl->setCellWidget(row_id, MainWindow::ColumnID::Actions, pWidget);
        connect(delete_row, SIGNAL(clicked(bool)), this, SLOT(deleteThisRow()));
        connect(edit_row, SIGNAL(clicked(bool)), this, SLOT(editThisRow()));

        tbl->setItem(
            row_id, MainWindow::ColumnID::ID,
            new QTableWidgetItem(QString::number(contact->GetUserId())));
        tbl->setColumnHidden(MainWindow::ColumnID::ID, true);
        row_id++;
    }
}

void MainWindow::on_updateButton_clicked() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    int row_count = ui->PhonebookViewTable->rowCount();
    for (int row = 0; row < row_count; row++) {
        std::string f_name = tbl->item(row, MainWindow::ColumnID::FirstName)
                                 ->text()
                                 .toStdString();
        std::string l_name = tbl->item(row, MainWindow::ColumnID::LastName)
                                 ->text()
                                 .toStdString();
        std::string num =
            tbl->item(row, MainWindow::ColumnID::Number)->text().toStdString();

        // get type
        QWidget* wdg = tbl->cellWidget(row, MainWindow::ColumnID::Type);
        QComboBox* comboType = qobject_cast<QComboBox*>(wdg);
        Contact::PhoneType type =
            Contact::GetType(comboType->currentText().toStdString());

        std::string nick = tbl->item(row, MainWindow::ColumnID::Nickname)
                               ->text()
                               .toStdString();
        std::string addr =
            tbl->item(row, MainWindow::ColumnID::Address)->text().toStdString();
        int user_id = tbl->item(row, MainWindow::ColumnID::ID)->text().toInt();

        try {
            Contact contact = Contact::Build(f_name, num)
                                  .HasLastName(l_name)
                                  .HasPhoneType(type)
                                  .HasNickname(nick)
                                  .HasAddress(addr)
                                  .HasUserId(user_id);
            pb_operations_->UpdateContact(contact);
        } catch (const std::runtime_error& e) {
            PrintErrorLabel(ui->errorLabel, e.what());
            return;
        } catch (const std::invalid_argument& e) {
            PrintErrorLabel(ui->errorLabel, e.what());
            return;
        } catch (...) {
            PrintErrorLabel(ui->errorLabel, "General error!");
            return;
        }
    }
}

void MainWindow::deleteThisRow() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    QWidget* w = qobject_cast<QWidget*>(sender()->parent());
    if (w) {
        int row = tbl->indexAt(w->pos()).row();
        bool ok;
        int user_id = tbl->item(row, MainWindow::ColumnID::ID)->text().toInt();
        pb_operations_->DeleteContact(user_id);
        tbl->removeRow(row);
        tbl->setCurrentCell(0, 0);
        if (!tbl->rowCount()) {
            ui->updateButton->setEnabled(false);
            ui->refreshButton->setEnabled(false);
        }
    }
}

void MainWindow::editThisRow() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    QWidget* w = qobject_cast<QWidget*>(sender()->parent());
    if (w) {
        int row = tbl->indexAt(w->pos()).row();
        int user_id = tbl->item(row, MainWindow::ColumnID::ID)->text().toInt();
        EditForm* editForm = new EditForm(user_id, pb_operations_, this);
        connect(editForm, SIGNAL(TriggerTableUpdate()), this,
                SLOT(ReceiveTableUpdateTrigger()));
        editForm->show();
    }
}

void MainWindow::ReceiveTableUpdateTrigger() {
    ui->PhonebookViewTable->setRowCount(0);
    ListContacts();
}

void MainWindow::on_addButton_clicked() {
    InsertForm* insForm = new InsertForm(pb_operations_, this);
    connect(insForm, SIGNAL(TriggerTableUpdate()), this,
            SLOT(ReceiveTableUpdateTrigger()));
    insForm->show();
}

void MainWindow::on_closeMainButton_clicked() {
    this->close();
}

void MainWindow::on_refreshButton_clicked() {
    ui->errorLabel->clear();
    ui->PhonebookViewTable->setRowCount(0);
    ListContacts();
}
