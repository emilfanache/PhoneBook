#ifndef EDITFORM_H
#define EDITFORM_H

#include <QDialog>
#include <memory>
#include <string>

#include "pb_operations.hpp"

namespace Ui {
class EditForm;
}

class EditForm : public QDialog {
    Q_OBJECT

 public:
    explicit EditForm(const std::string& number,
                      std::shared_ptr<PBOperations> pb_operations,
                      QWidget* parent = nullptr);
    ~EditForm();

 private slots:
    void on_closeButtonEdit_clicked();
    void on_okButtonEdit_clicked();

 signals:
    void TriggerTableUpdate();

 private:
    Ui::EditForm* ui;
    std::shared_ptr<PBOperations> pb_operations_;
    std::string number_;
};

#endif  // EDITFORM_H
