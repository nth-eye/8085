#include "doc_handler.h"

#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QQmlFile>
#include <QQmlFileSelector>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QDebug>

DocHandler::DocHandler(QObject *parent)
    : QObject(parent)
    , document(nullptr)
    , cursor_pos(-1)
    , selection_start(0)
    , selection_end(0)
{
    highlighter = new Highlighter();
}

QQuickTextDocument *DocHandler::get_document() const
{
    return document;
}

void DocHandler::set_document(QQuickTextDocument *doc)
{
    if (doc == document || !doc)
        return;

    if (document)
        document->textDocument()->disconnect(this);
    document = doc;
    connect(document->textDocument(), &QTextDocument::modificationChanged, this, &DocHandler::modifiedChanged);
    highlighter->setDocument(document->textDocument());

    emit documentChanged();
}

int DocHandler::get_cursor_pos() const
{
    return cursor_pos;
}

void DocHandler::set_cursor_pos(int pos)
{
    if (pos == cursor_pos)
        return;

    cursor_pos = pos;
    reset();
    emit cursorPositionChanged();
}

int DocHandler::get_selection_start() const
{
    return selection_start;
}

void DocHandler::set_selection_start(int pos)
{
    if (pos == selection_start)
        return;

    selection_start = pos;
    emit selectionStartChanged();
}

int DocHandler::get_selection_end() const
{
    return selection_end;
}

void DocHandler::set_selection_end(int pos)
{
    if (pos == selection_end)
        return;

    selection_end = pos;
    emit selectionEndChanged();
}

//int DocumentHandler::fontSize() const
//{
//    QTextCursor cursor = textCursor();
//    if (cursor.isNull())
//        return 0;
//    QTextCharFormat format = cursor.charFormat();
//    return format.font().pointSize();
//}

//void DocumentHandler::setFontSize(int size)
//{
//    if (size <= 0)
//        return;

//    QTextCursor cursor = textCursor();
//    if (cursor.isNull())
//        return;

//    if (!cursor.hasSelection())
//        cursor.select(QTextCursor::WordUnderCursor);

//    if (cursor.charFormat().property(QTextFormat::FontPointSize).toInt() == size)
//        return;

//    QTextCharFormat format;
//    format.setFontPointSize(size);
//    mergeFormatOnWordOrSelection(format);
//    emit fontSizeChanged();
//}

//QString DocumentHandler::fileName() const
//{
//    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
//    const QString fileName = QFileInfo(filePath).fileName();
//    if (fileName.isEmpty())
//        return QStringLiteral("untitled.txt");
//    return fileName;
//}

//QString DocumentHandler::fileType() const
//{
//    return QFileInfo(fileName()).suffix();
//}

QUrl DocHandler::get_file_url() const
{
    return file_url;
}

void DocHandler::load(const QUrl &url)
{
    if (url == file_url)
        return;

    QQmlEngine *engine = qmlEngine(this);
    if (!engine) {
        qWarning() << "load() called before DocHandler has QQmlEngine";
        return;
    }

    const QUrl path = QQmlFileSelector::get(engine)->selector()->select(url);
    const QString fileName = QQmlFile::urlToLocalFileOrQrc(path);

    if (QFile::exists(fileName)) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            // QTextCodec *codec = QTextCodec::codecForUtfText(data);
            if (QTextDocument *doc = text_document())
                doc->setModified(false);

            emit loaded(/*codec->toUnicode(*/data/*)*/);
            reset();
        }
    }

    file_url = url;
    emit fileUrlChanged();
}

void DocHandler::save(const QUrl &file_url_)
{
//    QTextDocument *doc = textDocument();
//    if (!doc)
//        return;

//    const QString file_path = file_url_.toLocalFile();
//    const bool is_html = QFileInfo(file_path).suffix().contains(QLatin1String("htm"));
//    QFile file(file_path);
//    if (!file.open(QFile::WriteOnly | QFile::Truncate | (is_html ? QFile::NotOpen : QFile::Text))) {
//        emit error(tr("Cannot save: ") + file.errorString());
//        return;
//    }
//    file.write((is_html ? doc->toHtml() : doc->toPlainText()).toUtf8());
//    file.close();

//    if (file_url_ == file_url)
//        return;

//    file_url = file_url_;
//    emit fileUrlChanged();
}

void DocHandler::reset()
{
//    emit fontFamilyChanged();
//    emit alignmentChanged();
//    emit boldChanged();
//    emit italicChanged();
//    emit underlineChanged();
//    emit fontSizeChanged();
//    emit textColorChanged();
}

void DocHandler::update()
{
    highlighter->setDocument(document->textDocument());
    // highlighter->rehighlight();
}

//QTextCursor DocumentHandler::text_cursor() const
//{
//    QTextDocument *doc = textDocument();
//    if (!doc)
//        return QTextCursor();

//    QTextCursor cursor = QTextCursor(doc);
//    if (m_selectionStart != m_selectionEnd) {
//        cursor.setPosition(m_selectionStart);
//        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
//    } else {
//        cursor.setPosition(m_cursorPosition);
//    }
//    return cursor;
//}

QTextDocument *DocHandler::text_document() const
{
    if (!document)
        return nullptr;

    return document->textDocument();
}

//bool DocumentHandler::modified() const
//{
//    return m_document && m_document->textDocument()->isModified();
//}

//void DocumentHandler::setModified(bool m)
//{
//    if (m_document)
//        m_document->textDocument()->setModified(m);
//}
