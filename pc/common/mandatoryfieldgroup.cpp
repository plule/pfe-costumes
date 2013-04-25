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
            if (((QLineEdit*)widget->qt_metacast("QLineEdit"))->text().isEmpty()) {
                enable = false;
                break;
            }
            else
                continue;
        }
    }
    okButton->setEnabled(enable);
}


void MandatoryFieldGroup::clear()
{
    qDebug() << "clear";
    widgets.clear();
}
