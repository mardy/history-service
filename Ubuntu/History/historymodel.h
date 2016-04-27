/*
 * Copyright (C) 2013-2014 Canonical, Ltd.
 *
 * Authors:
 *  Gustavo Pichorim Boiko <gustavo.boiko@canonical.com>
 *
 * This file is part of history-service.
 *
 * history-service is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * history-service is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include "types.h"
#include "historyqmlfilter.h"
#include "historyqmlsort.h"
#include <QAbstractListModel>
#include <QStringList>
#include <QQmlParserStatus>

class HistoryModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(HistoryQmlFilter *filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(HistoryQmlSort *sort READ sort WRITE setSort NOTIFY sortChanged)
    Q_PROPERTY(EventType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool matchContacts READ matchContacts WRITE setMatchContacts NOTIFY matchContactsChanged)
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY canFetchMoreChanged)
    Q_ENUMS(EventType)
    Q_ENUMS(MessageType)
    Q_ENUMS(MatchFlag)
    Q_ENUMS(MessageStatus)
    Q_ENUMS(AttachmentFlag)
    Q_ENUMS(Role)

public:
    enum EventType {
        EventTypeText = History::EventTypeText,
        EventTypeVoice = History::EventTypeVoice
    };

    enum MessageType {
        MessageTypeText = History::MessageTypeText,
        MessageTypeMultiPart = History::MessageTypeMultiPart,
        MessageTypeInformation = History::MessageTypeInformation
    };

    enum MatchFlag {
        MatchCaseSensitive = History::MatchCaseSensitive,
        MatchCaseInsensitive = History::MatchCaseInsensitive,
        MatchContains = History::MatchContains,
        MatchPhoneNumber = History::MatchPhoneNumber
    };

    enum MessageStatus
    {
        MessageStatusUnknown = History::MessageStatusUnknown,
        MessageStatusDelivered = History::MessageStatusDelivered,
        MessageStatusTemporarilyFailed = History::MessageStatusTemporarilyFailed,
        MessageStatusPermanentlyFailed = History::MessageStatusPermanentlyFailed,
        MessageStatusAccepted = History::MessageStatusAccepted,
        MessageStatusRead = History::MessageStatusRead,
        MessageStatusDeleted = History::MessageStatusDeleted,
        MessageStatusPending = History::MessageStatusPending // pending attachment download
    };

    enum AttachmentFlag
    {
        AttachmentDownloaded = History::AttachmentDownloaded,
        AttachmentPending = History::AttachmentPending,
        AttachmentError = History::AttachmentError
    };
 
    enum Role {
        AccountIdRole = Qt::UserRole,
        ThreadIdRole,
        ParticipantsRole,
        ParticipantIdsRole,
        TypeRole,
        PropertiesRole,
        LastRole
    };

    explicit HistoryModel(QObject *parent = 0);

    Q_INVOKABLE virtual bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual void fetchMore(const QModelIndex &parent = QModelIndex());
    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    HistoryQmlFilter *filter() const;
    void setFilter(HistoryQmlFilter *value);

    HistoryQmlSort *sort() const;
    void setSort(HistoryQmlSort *value);

    EventType type() const;
    void setType(EventType value);

    bool matchContacts() const;
    void setMatchContacts(bool value);

    Q_INVOKABLE QVariantMap threadForProperties(const QString &accountId,
                                                int eventType,
                                                const QVariantMap &properties,
                                                int matchFlags = (int)History::MatchCaseSensitive,
                                                bool create = false);

    Q_INVOKABLE QString threadIdForProperties(const QString &accountId,
                                                int eventType,
                                                const QVariantMap &properties,
                                                int matchFlags = (int)History::MatchCaseSensitive,
                                                bool create = false);

    Q_INVOKABLE QVariantMap threadForParticipants(const QString &accountId,
                                                  int eventType,
                                                  const QStringList &participants,
                                                  int matchFlags = (int)History::MatchCaseSensitive,
                                                  bool create = false);
    Q_INVOKABLE QString threadIdForParticipants(const QString &accountId,
                                                int eventType,
                                                const QStringList &participants,
                                                int matchFlags = (int)History::MatchCaseSensitive,
                                                bool create = false);
    Q_INVOKABLE bool writeTextInformationEvent(const QString &accountId,
                                   const QString &threadId,
                                   const QStringList &participants,
                                   const QString &message);

    Q_INVOKABLE virtual QVariant get(int row) const;

    // QML parser status things
    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void countChanged();
    void filterChanged();
    void sortChanged();
    void typeChanged();
    void matchContactsChanged();
    void canFetchMoreChanged();

protected Q_SLOTS:
    void triggerQueryUpdate();
    virtual void updateQuery() = 0;
    void onContactInfoChanged(const QString &accountId, const QString &identifier, const QVariantMap &contactInfo);
    void watchContactInfo(const QString &accountId, const QString &identifier, const QVariantMap &currentInfo);

protected:
    virtual void timerEvent(QTimerEvent *event);
    bool lessThan(const QVariantMap &left, const QVariantMap &right) const;
    int positionForItem(const QVariantMap &item) const;
    bool isAscending() const;

    HistoryQmlFilter *mFilter;
    HistoryQmlSort *mSort;
    EventType mType;
    bool mMatchContacts;

private:
    QHash<int, QByteArray> mRoles;
    int mUpdateTimer;
    bool mWaitingForQml;
};

#endif // HISTORYMODEL_H
