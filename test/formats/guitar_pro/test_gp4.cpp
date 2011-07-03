#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <formats/guitar_pro/guitarproimporter.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/position.h>

const std::string FILE_TEST1 = "data/test1.gp4";

struct Gp4Fixture
{
    Gp4Fixture()
    {
        BOOST_CHECK_NO_THROW(importer.load(FILE_TEST1));
        doc = importer.load(FILE_TEST1);
        score = doc->GetGuitarScore();
    }

    GuitarProImporter importer;
    std::shared_ptr<PowerTabDocument> doc;
    Score* score;
};

BOOST_FIXTURE_TEST_SUITE(GuitarPro4Import, Gp4Fixture)

    /// Verify that the header and song information is imported correctly
    BOOST_AUTO_TEST_CASE(HeaderConversion)
    {
        const PowerTabFileHeader& header = doc->GetHeader();

        BOOST_CHECK_EQUAL(header.GetSongTitle(), "FileName");
        BOOST_CHECK_EQUAL(header.GetSongArtist(), "Artist");
        BOOST_CHECK_EQUAL(header.GetSongAudioReleaseTitle(), "Album");
        BOOST_CHECK_EQUAL(header.GetSongComposer(), "Author");
        BOOST_CHECK_EQUAL(header.GetSongAudioReleaseYear(), 2010);
        BOOST_CHECK_EQUAL(header.GetSongCopyright(), "Copyright 2011");
        BOOST_CHECK_EQUAL(header.GetSongGuitarScoreTranscriber(), "Tab Creator");
        BOOST_CHECK_EQUAL(header.GetSongGuitarScoreNotes(), "Some Comments");
    }

    /// Verify that guitars are imported correctly
    BOOST_AUTO_TEST_CASE(TrackImport)
    {
        BOOST_CHECK_EQUAL(score->GetGuitarCount(), 2u);

        Score::GuitarConstPtr guitar1 = score->GetGuitar(0);

        BOOST_CHECK_EQUAL(guitar1->GetDescription(), "First Track");
        BOOST_CHECK_EQUAL(guitar1->GetNumber(), 0u);
        BOOST_CHECK_EQUAL(guitar1->GetCapo(), 1u);

        using namespace midi;
        // 7-string tuning
        BOOST_CHECK(guitar1->GetTuning() == Tuning("", 0, false,
                                                    {MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
                                                    MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2, MIDI_NOTE_B1}));
    }

    BOOST_AUTO_TEST_SUITE(ReadBarline)

        BOOST_AUTO_TEST_CASE(BarlineProperties)
        {
            System::BarlineConstPtr barline1 = score->GetSystem(0)->GetStartBar();
            BOOST_CHECK(barline1->IsRepeatStart());
        }

        BOOST_AUTO_TEST_CASE(ReadTimeSignature)
        {
            const TimeSignature& timeSig1 = score->GetSystem(0)->GetStartBar()->GetTimeSignature();

            BOOST_CHECK(timeSig1.IsSameMeter(TimeSignature(5, 4)));
            BOOST_CHECK(timeSig1.IsShown());
        }

        BOOST_AUTO_TEST_CASE(ReadKeySignature)
        {
            //System::BarlineConstPtr barline1 = score->GetSystem(0)->GetStartBar();

            // TODO - generate appropriate test case for key signatures (TuxGuitar export seems to be broken??)
            /*BOOST_CHECK(barline1->GetKeySignature().IsSameKey(KeySignature(KeySignature::majorKey,
                                                                                   KeySignature::twoSharps)));*/
        }

        BOOST_AUTO_TEST_CASE(ReadRehearsalSigns)
        {
            // Check that rehearsal sign letters are set in sequential order (A, B, C, etc),
            // and that data is read correctly
            const RehearsalSign& sign1 = score->GetSystem(0)->GetStartBar()->GetRehearsalSign();

            BOOST_CHECK_EQUAL(sign1.GetDescription(), "Section 1");
            BOOST_CHECK(sign1.IsSet());
            BOOST_CHECK_EQUAL(sign1.GetLetter(), 'A');

            const RehearsalSign& sign2 = score->GetSystem(0)->GetBarline(0)->GetRehearsalSign();

            BOOST_CHECK_EQUAL(sign2.GetDescription(), "Section 2");
            BOOST_CHECK(sign2.IsSet());
            BOOST_CHECK_EQUAL(sign2.GetLetter(), 'B');
        }

        BOOST_AUTO_TEST_CASE(ReadAlternateEndings)
        {
            // TODO - create test cases (TuxGuitar cannot export alternate endings to gp4)
        }

        BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(TempoMarkers)
    {
        BOOST_CHECK_EQUAL(score->GetTempoMarkerCount(), 1u);
    }

    BOOST_AUTO_TEST_SUITE(NoteProperties)

        BOOST_AUTO_TEST_CASE(PositionEffects)
        {
            /// Check that the first position is tremolo picked and is tapped
            const Position* pos = score->GetSystem(0)->GetStaff(0)->GetPosition(0, 0);
            BOOST_CHECK(pos->HasTremoloPicking());
            BOOST_CHECK(pos->HasTap());

            BOOST_CHECK(pos->HasArpeggioUp());

            // TODO - test tremolo bar
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()