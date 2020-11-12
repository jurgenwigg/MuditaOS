// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcQind.hpp>
#include <log/debug.hpp>

using namespace at::urc;

auto Qind::isCsq() const noexcept -> bool
{
    if (tokens.size() == magic_enum::enum_count<enum CSQ>()) {
        return tokens[CSQ].find(type_csq) != std::string::npos;
    }
    return false;
}

auto Qind::isFota() const noexcept -> bool
{
    return tokens.size() > 0 && tokens[Fota] == type_fota;
}

auto Qind::isFotaValid() const noexcept -> bool
{
    return isFota() && tokens.size() >= fotaMinTokenSize;
}

auto Qind::getFotaStage() const noexcept -> std::optional<FotaStage>
{
    if (isFotaValid()) {
        for (auto &stage : magic_enum::enum_values<FotaStage>()) {
            if (tokens[Stage] == magic_enum::enum_name(stage)) {
                return stage;
            }
        }
    }
    return std::nullopt;
}

auto Qind::getFotaParameter() const noexcept -> std::string
{
    if (isFotaValid() && tokens.size() >= fotaMinTokenSize) {
        return tokens[Param];
    }
    return std::string();
}

auto Qind::validate(enum CSQ check) const noexcept -> bool
{
    try {
        if (isCsq()) {
            int rssi = std::stoi(tokens[RSSI]);
            int ber  = std::stoi(tokens[BER]);
            LOG_DEBUG("> %d %d", rssi, ber);
            switch (check) {
            case RSSI:
                return rssi != invalid_rssi_low && rssi != invalid_rssi_high;
            case BER:
                return ber != invalid_ber;
            default:
                return false;
            }
        }
    }
    catch (const std::exception &ex) {
        LOG_FATAL("exception: %s", ex.what());
    }
    return false;
}

auto Qind::getRSSI() const noexcept -> std::optional<int>
{
    if (isCsq()) {
        int rssi;
        try {
            rssi = std::stoi(tokens[RSSI]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        if (rssi != invalid_rssi_low && rssi != invalid_rssi_high) {
            return rssi;
        }
    }

    return std::nullopt;
}

auto Qind::getBER() const noexcept -> std::optional<int>
{
    if (isCsq()) {
        int ber;
        try {
            ber = std::stoi(tokens[BER]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        if (ber != invalid_ber) {
            return ber;
        }
    }

    return std::nullopt;
}