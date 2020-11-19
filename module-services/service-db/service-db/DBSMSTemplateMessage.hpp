﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp" // for DBMessage, DBResponseMessage

#include <Interface/SMSTemplateRecord.hpp>
#include <MessageType.hpp>

#include <cstdint>
#include <memory>
#include <vector>

class DBSMSTemplateMessage : public DBMessage
{
  public:
    DBSMSTemplateMessage(MessageType messageType, const SMSTemplateRecord &rec = SMSTemplateRecord{});
    SMSTemplateRecord record;
};

class DBSMSTemplateGetCount : public DBSMSTemplateMessage
{
  public:
    DBSMSTemplateGetCount();
};

class DBSMSTemplateResponseMessage : public DBResponseMessage
{
  public:
    DBSMSTemplateResponseMessage(std::unique_ptr<std::vector<SMSTemplateRecord>> rec,
                                 uint32_t retCode,
                                 uint32_t limit     = 0,
                                 uint32_t offset    = 0,
                                 uint32_t count     = 0,
                                 MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<SMSTemplateRecord>> records;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};