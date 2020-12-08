// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotePreviewWindowPresenter.hpp"

namespace app::notes
{
    NotePreviewWindowPresenter::NotePreviewWindowPresenter(std::unique_ptr<AbstractNotesRepository> notesRepository)
        : notesRepository{std::move(notesRepository)}
    {}

    AbstractNotesRepository &NotePreviewWindowPresenter::getRepository()
    {
        return *notesRepository;
    }
} // namespace app::notes