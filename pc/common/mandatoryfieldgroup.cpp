/**
  * Adapted from : http://doc.qt.digia.com/qq/qq11-mandatoryfields.html
  */

#include "mandatoryfieldgroup.h"

void MandatoryFieldGroup::add(QWidget *widget)
{
    if (!widgets.contains(widget)) {
        if (widget->inherits("QComboBox"))
            connect((QComboBox*)widget->qt_metacast("QComboBox"), SIGNAL(highlighted(int)),
                    this, SLOT(changed()));
        else if (widget->inherits("QLineEdit"))
            connect((QLineEdit*)widget->qt_metacast("QLineEdit"), SIGNAL(textChanged(const QString&)),
                    this, SLOT(changed()));
        else {
            qWarning("MandatoryFieldGroup: unsupported class %s", widget->metaObject()->className());
            return;
        }
        widgets.append(widget);
        changed();
    }
}

void MandatoryFieldGroup::remove(QWidget *widget)
{
    widgets.removeOne(widget);
    changed();
}


void MandatoryFieldGroup::setOkButton(QPushButton *button)
{
    if (okButton && okButton != button)
        okButton->setEnabled(true);
    okButton = button;
    changed();
}


void MandatoryFieldGroup::changed()
{
    if (!okButton)
        return;
    bool enable = true;
    foreach (QWidget *widget, widgets) {
        if (widget->inherits("QComboBox")) {
            if (((QComboBox*)widget->qt_metacast("QComboBox"))->currentText().isEmpty()) {
                enable = false;
                break;
            }
            else
                continue;
        }
        if (widget->inherits("QLineEdit")) {
            QLineEdit *lineEdit = ((QLineEdit*)widget->qt_metacast("QLineEdit"));
            if (lineEdit->text().isEmpty()) {
                lineEdit->setStyleSheet("QLineEdit{background: yellow}");
                enable = false;
                break;
            } else {
                lineEdit->setStyleSheet("QLineEdit{background: white}");
                continue;
            }
        }
    }
    okButton->setEnabled(enable);
}


void MandatoryFieldGroup::clear()
{
    widgets.clear();
}
