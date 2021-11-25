#include "editsnippets.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QStandardPaths>

EditSnippets::EditSnippets(QWidget* parent) : QDialog(parent) {
    setupUi(this);
    m_snippetsPath =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) +
        snippetsRelativePath + '/';

    loadSnippetBookList();
}

void
EditSnippets::loadSnippetBookList() {
    QDir snippetsDir(m_snippetsPath);
    foreach (const QFileInfo snippetFileInfo,
             snippetsDir.entryInfoList({ "*.json" }, QDir::Files)) {
        QListWidgetItem* item =
            new QListWidgetItem(snippetFileInfo.fileName(), ui_lwSnippetFiles);
        item->setData(Qt::UserRole + 1, snippetFileInfo.absoluteFilePath());
        ui_lwSnippetFiles->addItem(item);
    }

    ui_lwSnippetFiles->setCurrentRow(0);
}

void
EditSnippets::addSnippetBook() {
    bool ok;
    QString fileName = QInputDialog::getText(
        this, tr("Snippet Book"), tr("Filename"), QLineEdit::Normal, "", &ok);
    if (ok && fileName.length() > 0) {
        if (QFileInfo(fileName).completeSuffix() != "json") fileName += ".json";

        QListWidgetItem* item =
            new QListWidgetItem(fileName, ui_lwSnippetFiles);
        item->setData(Qt::UserRole + 1, m_snippetsPath + fileName);
    }
}

void
EditSnippets::deleteSnippetBook() {
    QListWidgetItem* item =
        ui_lwSnippetFiles->takeItem(ui_lwSnippetFiles->currentRow());
    qDebug() << "deleteSnippetBook" << item->data(Qt::UserRole + 1).toString();
    QFile::remove(item->data(Qt::UserRole + 1).toString());

    delete item;
}

void
EditSnippets::loadSnippetBook(QListWidgetItem* item) {
    if (m_currentSnippetBook.modified) save();

    ui_twSnippets->setRowCount(0);
    m_currentSnippetBook.fileName = item->data(Qt::UserRole + 1).toString();

    QFile snippetFile(m_currentSnippetBook.fileName);
    if (snippetFile.open(QIODevice::ReadOnly)) {
        QJsonDocument snippetsDefine =
            QJsonDocument::fromJson(snippetFile.readAll());
        ui_leBookLabel->setText(snippetsDefine["label"].toString());

        QJsonArray snippetArray = snippetsDefine["snippets"].toArray();

        QString iconsPath = m_snippetsPath + "/icons/";
        foreach (const QJsonValue& value, snippetArray) {
            QJsonObject obj = value.toObject();

            int newRow = ui_twSnippets->rowCount();
            ui_twSnippets->insertRow(newRow);
            ui_twSnippets->setItem(
                newRow, 0, new QTableWidgetItem(obj["label"].toString()));

            QString icon = obj["icon"].toString();
            QString iconPath = icon;
            if (icon.length() > 0 && QFileInfo(icon).isRelative())
                iconPath = iconsPath + icon;

            ui_twSnippets->setItem(newRow, 1,
                                   new QTableWidgetItem(QIcon(iconPath), icon));
            QString tex = obj["tex"].toString();
            ui_twSnippets->setItem(newRow, 2, new QTableWidgetItem(tex));
            int pos = obj["pos"].toInt(tex.length());
            ui_twSnippets->setItem(newRow, 3,
                                   new QTableWidgetItem(QString::number(pos)));
        }
    } else
        ui_leBookLabel->clear();

    ui_twSnippets->setCurrentCell(0, 0);
    m_currentSnippetBook.modified = false;
}

void
EditSnippets::loadSnippetItem(int row) {
    if (row != -1) {
        ui_frSnippet->setEnabled(true);
        ui_leSnippetLabel->setText(ui_twSnippets->item(row, 0)->text());
        ui_leSnippetIcon->setText(ui_twSnippets->item(row, 1)->text());
        ui_tbSnippetIcon->setIcon(ui_twSnippets->item(row, 1)->icon());

        QObject::disconnect(ui_pteSnippetTex, nullptr, this, nullptr);
        ui_pteSnippetTex->setPlainText(ui_twSnippets->item(row, 2)->text());
        ui_pteSnippetTex->document()->setModified(false);
        QObject::connect(ui_pteSnippetTex, &QPlainTextEdit::textChanged, this,
                         &EditSnippets::updateSnippetTex);

        QObject::disconnect(ui_spbSnippetPos, nullptr, this, nullptr);
        ui_spbSnippetPos->setValue(ui_twSnippets->item(row, 3)->text().toInt());
        QObject::connect(ui_spbSnippetPos,
                         QOverload<int>::of(&QSpinBox::valueChanged), this,
                         &EditSnippets::updateSnippetPos);
    } else {
        ui_frSnippet->setEnabled(false);
        ui_leSnippetLabel->clear();
        ui_leSnippetIcon->clear();
        ui_tbSnippetIcon->setIcon(QIcon());
        ui_pteSnippetTex->clear();
        ui_spbSnippetPos->setValue(-1);
    }
}

void
EditSnippets::addSnippet() {
    int newRow = ui_twSnippets->rowCount();
    ui_twSnippets->insertRow(newRow);
    ui_twSnippets->setItem(newRow, 0, new QTableWidgetItem(""));
    ui_twSnippets->setItem(newRow, 1, new QTableWidgetItem(""));
    ui_twSnippets->setItem(newRow, 2, new QTableWidgetItem(""));
    ui_twSnippets->setItem(newRow, 3, new QTableWidgetItem("-1"));
}

void
EditSnippets::deleteSnippet() {
    ui_twSnippets->removeRow(ui_twSnippets->currentRow());
    m_currentSnippetBook.modified = true;
}

void
EditSnippets::updateBookLabel(const QString&) {
    m_currentSnippetBook.modified = true;
}

void
EditSnippets::updateSnippetLabel(const QString& label) {
    int row = ui_twSnippets->currentRow();
    if (row != -1) {
        m_currentSnippetBook.modified = true;
        ui_twSnippets->item(row, 0)->setText(label);
    }
}

void
EditSnippets::updateSnippetIcon(const QString& iconPath) {
    int row = ui_twSnippets->currentRow();
    if (row != -1) {
        m_currentSnippetBook.modified = true;

        QIcon icon;
        if (QFileInfo(iconPath).isRelative())
            icon.addFile(m_snippetsPath + "/icons/" + iconPath);
        else
            icon.addFile(iconPath);

        ui_twSnippets->item(row, 1)->setIcon(icon);
        ui_twSnippets->item(row, 1)->setText(iconPath);
    }
}

void
EditSnippets::updateSnippetTex() {
    int row = ui_twSnippets->currentRow();
    if (row != -1) {
        m_currentSnippetBook.modified = true;

        QString tex = ui_pteSnippetTex->toPlainText();
        ui_twSnippets->item(row, 2)->setText(tex);
        if (ui_spbSnippetPos->value() == -1)
            ui_twSnippets->item(row, 3)->setText(QString::number(tex.length()));
    }
}

void
EditSnippets::updateSnippetPos(int pos) {
    // int pos = ui_spbSnippetPos->value();
    int row = ui_twSnippets->currentRow();
    if (row != -1) {
        m_currentSnippetBook.modified = true;

        if (pos == -1) pos = ui_pteSnippetTex->toPlainText().length();

        ui_twSnippets->item(row, 3)->setText(QString::number(pos));
    }
}

void
EditSnippets::selectIcon() {
    QString iconPath = m_snippetsPath + "/icons/";
    QString iconFileName = QFileDialog::getOpenFileName(
        this, tr("Icon"), iconPath, "*.png *.jpg *.jpeg *.gif *.ico");
    QString iconRelPath = QDir(iconPath).relativeFilePath(iconFileName);

    if (iconRelPath.contains(QRegularExpression("^\\.\\.")))
        ui_leSnippetIcon->setText(iconFileName);
    else
        ui_leSnippetIcon->setText(iconRelPath);

    ui_leSnippetIcon->textEdited(ui_leSnippetIcon->text());
}

void
EditSnippets::save() {
    QJsonDocument json;

    QJsonObject mainObj;
    QString bookLabel = ui_leBookLabel->text();
    if (bookLabel.length() > 0)
        mainObj["label"] = bookLabel;
    else
        mainObj["label"] = QFileInfo(m_currentSnippetBook.fileName).baseName();

    QJsonArray snippetArray;
    for (int i = 0; i < ui_twSnippets->rowCount(); i++) {
        QJsonObject sObj;
        sObj["label"] = ui_twSnippets->item(i, 0)->text();
        sObj["icon"] = ui_twSnippets->item(i, 1)->text();
        sObj["tex"] = ui_twSnippets->item(i, 2)->text();
        sObj["pos"] = ui_twSnippets->item(i, 3)->text().toInt();
        snippetArray.append(sObj);
    }

    mainObj["snippets"] = snippetArray;

    json.setObject(mainObj);
    qDebug() << m_currentSnippetBook.fileName << json.toJson();

    QFile bookFile(m_currentSnippetBook.fileName);
    if (bookFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        bookFile.write(json.toJson());
    }
}

void
EditSnippets::accept() {
    qDebug() << "accept";
    if (m_currentSnippetBook.modified) save();

    QDialog::accept();
}
