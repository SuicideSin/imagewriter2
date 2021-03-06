#ifndef REMOVABLEDEVICESMODEL_H
#define REMOVABLEDEVICESMODEL_H

#include <QAbstractListModel>

/* This abstract class represents a list of all removable
 * devices available on this system which are suitable for
 * writing images to. If possible, implementations of this
 * should avoid to include devices which are currently used. */
class RemovableDevicesModel : public QAbstractListModel
{
public:
    virtual ~RemovableDevicesModel() {}

    enum DeviceType {
        Unknown,
        DVD,
        USB,
        HDD
    };

    Q_ENUM(DeviceType)

    enum Roles {
        // NameRole = Qt::DisplayRole
        PathRole = Qt::UserRole,
        SizeRole,
        TypeRole
    };

    Q_ENUM(Roles)

    virtual Q_INVOKABLE QVariant openDeviceHandle(unsigned int index) = 0;
};

#endif // REMOVABLEDEVICESMODEL_H
