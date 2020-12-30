#ifndef MEMORY_H
#define MEMORY_H

#include <QAbstractTableModel>
#include <QAbstractListModel>

#include "types.h"

using Memory = std::array<Byte, 0x10000>;

class StackModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit StackModel(Memory &mem_, QObject *parent = nullptr);

    enum {
        AddressRole = Qt::UserRole,
        ByteRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
private:
    Memory &mem;
};


class MemoryModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit MemoryModel(Memory &mem_, QObject *parent = nullptr);

    enum {
        OffsetRole = Qt::UserRole,
        ByteRole,
        CharRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
private:
    Memory &mem;
};

//class MnemonicModel : public QAbstractListModel {
//    Q_OBJECT
//public:
//    explicit MnemonicModel(Memory &mem_, QObject *parent = nullptr);

//    enum {
//        AddressRole = Qt::UserRole,
//        MnemonicRole
//    };

//    // Basic functionality:
//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
//    Qt::ItemFlags flags(const QModelIndex &index) const override;
//    virtual QHash<int, QByteArray> roleNames() const override;
//private:
//    Memory &mem;
//};

#endif // MEMORY_H
