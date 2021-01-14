#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QUrl>
#include <QQuickTextDocument>

#include "highlighter.h"


class DocHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ get_document WRITE set_document NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ get_cursor_pos WRITE set_cursor_pos NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ get_selection_start WRITE set_selection_start NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ get_selection_end WRITE set_selection_end NOTIFY selectionEndChanged)

//    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

//    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
//    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ get_file_url NOTIFY fileUrlChanged)

//    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)

public:
    explicit DocHandler(QObject *parent = nullptr);

    QQuickTextDocument *get_document() const;
    void set_document(QQuickTextDocument *document);

    int get_cursor_pos() const;
    void set_cursor_pos(int pos);

    int get_selection_start() const;
    void set_selection_start(int pos);

    int get_selection_end() const;
    void set_selection_end(int pos);

//    int fontSize() const;
//    void setFontSize(int size);

//    QString fileName() const;
//    QString fileType() const;
    QUrl get_file_url() const;

//    bool modified() const;
//    void setModified(bool m);

public Q_SLOTS:
    void load(const QUrl &file_url_);
    void save(const QUrl &file_url_);
    void update();

Q_SIGNALS:
    void documentChanged();
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();

//    void fontSizeChanged();

//    void textChanged();
    void fileUrlChanged();

    void loaded(const QString &text);
    void error(const QString &message);

    void modifiedChanged();

private:
    void reset();
//    QTextCursor text_cursor() const;
    QTextDocument *text_document() const;
//    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    QQuickTextDocument *document;

    int cursor_pos;
    int selection_start;
    int selection_end;

//    QFont font;
    QUrl file_url;
    Highlighter *highlighter;
};

#endif // EDITOR_H
