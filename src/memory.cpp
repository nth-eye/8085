#include "memory.h"
#include <QDebug>

StackModel::StackModel(Memory &mem_, QObject *parent)
    : QAbstractListModel(parent), mem(mem_)
{}

int StackModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return 0x10000;
}

QVariant StackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto idx = index.row();
    const auto val = mem[idx];

    switch (role) {
        case AddressRole: return QStringLiteral("%1").arg(idx, 4, 16, QLatin1Char('0')).toUpper();
        case ByteRole: return QStringLiteral("%1").arg(val, 2, 16, QLatin1Char('0')).toUpper();
        default: return QVariant();
    }
}

bool StackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags StackModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> StackModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[AddressRole] = "address";
    names[ByteRole] = "byte";

    return names;
}

MemoryModel::MemoryModel(Memory &mem_, QObject *parent)
    : QAbstractTableModel(parent), mem(mem_)
{}

int MemoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4096;
}

int MemoryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 16;
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto idx = index.row() * 16 + index.column();
    const auto val = mem[idx];

    switch (role) {
        case OffsetRole: return QStringLiteral("%1").arg(idx, 4, 16, QLatin1Char('0')).toUpper();
        case ByteRole: return QStringLiteral("%1").arg(val, 2, 16, QLatin1Char('0')).toUpper();
        case CharRole: return QChar::isPrint(val) ? QString(QChar(val)) : QString('.');
        default: return QVariant();
    }
}

bool MemoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> MemoryModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[OffsetRole] = "offset";
    names[ByteRole] = "byte";
    names[CharRole] = "char";

    return names;
}
