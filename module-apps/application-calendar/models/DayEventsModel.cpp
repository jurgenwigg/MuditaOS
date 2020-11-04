// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayEventsModel.hpp"
#include "application-calendar/widgets/DayEventsItem.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include <ListView.hpp>
#include <queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/queries/RecordQuery.hpp>

DayEventsModel::DayEventsModel(app::Application *app) : DatabaseModel(app), application(app)
{}

unsigned int DayEventsModel::requestRecordsCount()
{
    return recordsCount;
}

unsigned int DayEventsModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::dayEvents::height;
}

void DayEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    auto query = std::make_unique<db::query::events::GetFiltered>(filterFrom, filterTill, offset, limit);
    query->setQueryListener(
        db::QueryCallback::fromFunction([this](auto response) { return handleQueryResponse(response); }));
    DBServiceAPI::GetQuery(application, db::Interface::Name::Events, std::move(query));
}

gui::ListItem *DayEventsModel::getItem(gui::Order order)
{
    auto record = getRecord(order);
    if (record == nullptr) {
        LOG_DEBUG("Empty record in DayEventsModel::GetItem");
        return nullptr;
    }

    auto *item = new gui::DayEventsItem();
    item->setEvent(record);
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to event details window");
        auto rec  = std::make_unique<EventsRecord>(*record);
        auto data = std::make_unique<EventRecordData>(std::move(rec));
        application->switchWindow(style::window::calendar::name::details_window, std::move(data));
        return true;
    };

    return item;
}

bool DayEventsModel::updateRecords(std::vector<EventsRecord> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

auto DayEventsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    if (auto response = dynamic_cast<db::query::events::GetFilteredResult *>(queryResult); response != nullptr) {
        if (recordsCount != (response->getCountResult())) {
            recordsCount = response->getCountResult();
            list->rebuildList(style::listview::RebuildType::Full, 0, true);
            return false;
        }

        auto records = *response->getResult();
        if (auto app = dynamic_cast<app::ApplicationCalendar *>(application); app != nullptr) {
            if (response->getCountResult() == 0) {
                LOG_DEBUG("Empty records");
                if (app->getEquivalentToEmptyWindow() == EquivalentWindow::DayEventsWindow) {
                    app->switchToNoEventsWindow(dayMonthTitle, filterFrom);
                    return true;
                }
            }
            auto eventShift = app->getEventShift();
            if (eventShift) {
                for (auto &record : records) {
                    record.date_from += hours(eventShift);
                    record.date_till += hours(eventShift);
                }
            }
            return updateRecords(std::move(records));
        }
        LOG_ERROR("App is not a calendar");
        return false;
    }
    LOG_ERROR("GetFilteredResult response is nullptr");
    return false;
}

void DayEventsModel::setFilters(TimePoint from, TimePoint till, const std::string &dayMonth)
{
    this->filterFrom    = from;
    this->filterTill    = till;
    this->dayMonthTitle = dayMonth;
}