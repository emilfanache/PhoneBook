#ifndef INSERTFORM_H
#define INSERTFORM_H

#include <QDialog>
#include <memory>

#include "pb_operations.hpp"

namespace Ui {
class InsertForm;
}

class InsertForm : public QDialog {
    Q_OBJECT

 public:
    InsertForm(std::shared_ptr<PBOperations> pb_operations,
               QWidget* parent = nullptr);
    ~InsertForm();

 private slots:
    void on_closeButton_clicked();
    void on_okButton_clicked();

 signals:
    void TriggerTableUpdate();

 private:
    Ui::InsertForm* ui;
    std::shared_ptr<PBOperations> pb_operations_;
};

#endif  // INSERTFORM_H
