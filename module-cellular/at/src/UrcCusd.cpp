// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcCusd.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

using namespace at::urc;

auto Cusd::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

auto Cusd::isActionNeeded() const noexcept -> bool
{
    if (isValid()) {
        auto status = getStatus();
        if (status) {
            return *status == StatusType::FurtherUserActionRequired;
        }
    }
    return false;
}

auto Cusd::getMessage() const noexcept -> std::optional<std::string>
{
    if (!isValid()) {
        return std::nullopt;
    }

    auto message = tokens[Tokens::Response];
    utils::findAndReplaceAll(message, "\"", "");

    return utils::trim(message);
}

auto Cusd::getStatus() const noexcept -> std::optional<StatusType>
{
    if (isValid()) {
        int statusInt;
        try {
            statusInt = std::stoi(tokens[Tokens::Status]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        auto status = magic_enum::enum_cast<StatusType>(statusInt);
        if (status.has_value()) {
            return status.value();
        }
    }

    return std::nullopt;
}

auto Cusd::getDCS() const noexcept -> std::optional<int>
{
    if (!isValid()) {
        return std::nullopt;
    }
    int dcs;
    try {
        dcs = std::stoi(tokens[Tokens::DCS]);
    }
    catch (const std::exception &e) {
        return std::nullopt;
    }

    return dcs;
}