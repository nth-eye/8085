#include "highlighter.h"
#include "settings.h"

static const QString instruction_patterns[] = {
    "nop", "hlt", "di", "ei", "rim", "sim", "out", "in",
    "xthl", "sphl", "pop", "push", "pchl", "rst",
    "jmp", "jz", "jnz", "jp", "jm", "jc", "jnc", "jpe", "jpo",
    "call", "cz", "cnz", "cp", "cm", "cc", "cnc", "cpe", "cpo",
    "ret", "rz", "rnz", "rp", "rm", "rc", "rnc", "rpe", "rpo",
    "xchg", "ldax", "lhld", "lda", "stax", "shld", "sta", "lxi",
    "mvi", "mov", "inx", "dcx", "inr", "dcr", "cma", "stc", "cmc",
    "adi", "aci", "sui", "sbi", "add", "adc", "sub", "sbb",
    "rlc", "rrc", "ral", "rar", "daa", "dad",
    "ani", "xri", "ori", "cpi", "ana", "xra", "ora", "cmp"
};

static const QString directive_patterns[] = {
    "db", "ds", "dw", "org"
};

static const QString register_patterns[] = {
    "b", "c", "d", "e", "h", "l", "a", "m"
};

std::pair<QTextCharFormat, QVector<QRegularExpression>> Highlighter::rules[] = {
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
    {QTextCharFormat(), {}},
};

void Highlighter::init()
{
    update();

    for (const QString &pattern : instruction_patterns)
        rules[2].second.append(QRegularExpression("\\b" + pattern + "\\b", QRegularExpression::CaseInsensitiveOption));

    for (const QString &pattern : register_patterns)
        rules[3].second.append(QRegularExpression("\\b" + pattern + "\\b", QRegularExpression::CaseInsensitiveOption));

    for (const QString &pattern : directive_patterns)
        rules[4].second.append(QRegularExpression("\\b" + pattern + "\\b", QRegularExpression::CaseInsensitiveOption));

    rules[5].second.append(QRegularExpression(";[^\n]*"));

    rules[6].second.append(QRegularExpression("\\b([a-zA-Z@?]\\w{1,5}):\\B"));
}

QRegularExpression Highlighter::num_regex = QRegularExpression("\\b([\\da-f]+)([bqh]?)\\b", QRegularExpression::CaseInsensitiveOption);

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{}

//Highlighter::~Highlighter()
//{

//}

void Highlighter::highlightBlock(const QString &text)
{
    auto match_iter = num_regex.globalMatch(text);
    while (match_iter.hasNext()) {
        auto match = match_iter.next();
        setFormat(match.capturedStart(1), match.capturedLength(1), rules[0].first);
        setFormat(match.capturedStart(2), match.capturedLength(2), rules[1].first);
    }

//    QVector<QRegularExpression> label_rules;

//    match_iter = label_regex.globalMatch(text);
//    while (match_iter.hasNext()) {
//        auto match = match_iter.next();
//        label_rules.append(QRegularExpression(match.captured(1), QRegularExpression::MultilineOption));
//        setFormat(match.capturedStart(1), match.capturedLength(1), label_format);
//    }

//    for (auto &pattern : label_rules) {
//        auto match_iter = pattern.globalMatch(text);
//        while (match_iter.hasNext()) {
//            auto match = match_iter.next();
//            setFormat(match.capturedStart(), match.capturedLength(), label_ref_format);
//        }
//    }

    for (const auto &[format, patterns] : rules) {
        for (const auto &pattern : patterns) {
            auto match_iter = pattern.globalMatch(text);
            while (match_iter.hasNext()) {
                auto match = match_iter.next();
                setFormat(match.capturedStart(), match.capturedLength(), format);
            }
        }
    }
}

void Highlighter::update()
{
    Settings settings;

    for (int i = 0; i < Settings::settings_list.size(); ++i)
        rules[i].first.setForeground(settings.color_value(Settings::settings_list[i]));
}
