// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/BasePresenter.hpp>

#include <module-apps/application-notes/model/NotesRepository.hpp>

namespace app::notes
{
    class NoteEditWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<NoteEditWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual void save(const NotesRecord &note) = 0;
        };
    };

    class NoteEditWindowPresenter : public NoteEditWindowContract::Presenter
    {
      public:
        explicit NoteEditWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository);

        void save(const NotesRecord &note) override;

      private:
        std::unique_ptr<AbstractNotesRepository> notesRepository;
    };
} // namespace app::notes