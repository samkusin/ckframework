#include "catch.hpp"

#include "cinek/cstringstack.hpp"

using namespace cinek;

static const char* kTinyString = "Test";
static const char* kSmallString = "The rain in spain falls mainly on the plain.";
static const char* kMediumString = "The Babylon project was our last best hope for " \
    "peace. It failed. But in the year of the shadow war, it became something " \
    "greater, our last best hope for victory. The year is 2260, the place Babylon 5.";
static const char* kLongString = "We the people of the United States, in order to " \
    " form a more perfect union, establish justice, insure domestic tranquility, " \
    " provide for the common defense, promote the general welfare, and secure the " \
    " blessings of liberty to ourselves and our posterity, do ordain and establish " \
    " this Constitution for the United States of America.";
const char* kBigString = "Four score and seven years ago our fathers brought forth " \
    "on this continent a new nation, conceived in liberty, and dedicated to the " \
    "proposition that all men are created equal.\n\n" \
    "Now we are engaged in a great civil war, testing whether that nation, or any " \
    "nation so conceived and so dedicated, can long endure. We are met on a great " \
    "battlefield of that war. We have come to dedicate a portion of that field, " \
    "as a final resting place for those who here gave their lives that that nation " \
    "might live. It is altogether fitting and proper that we should do this.\n\n" \
    "But, in a larger sense, we can not dedicate, we can not consecrate, we can not "\
    "hallow this ground. The brave men, living and dead, who struggled here, have " \
    "consecrated it, far above our poor power to add or detract. The world will little " \
    "note, nor long remember what we say here, but it can never forget what they did " \
    "here. It is for us the living, rather, to be dedicated here to the unfinished " \
    "work which they who fought here have thus far so nobly advanced. It is rather " \
    "for us to be here dedicated to the great task remaining before us—that from " \
    "these honored dead we take increased devotion to that cause for which they gave " \
    "the last full measure of devotion—that we here highly resolve that these dead " \
    "shall not have died in vain—that this nation, under God, shall have a new birth " \
    "of freedom—and that government of the people, by the people, for the people, " \
    "shall not perish from the earth.";

static int safeCStrCmp(const char* str1, const char* str2)
{
    return strncmp(str1, str2, 32768);
}

TEST_CASE("baseline small string stack with growth", "[cstringstack]")
{
    const size_t kInitialCapacity = 64;


    CStringStack cstrstack(kInitialCapacity);

    SECTION("validating initial state is empty")
    {
        REQUIRE(cstrstack.count() == 0);
        REQUIRE(cstrstack.size() == 0);
        REQUIRE(cstrstack.capacity() == kInitialCapacity);
    }

    SECTION("adding one string with no change in capacity")
    {
        cstrstack.reset();

        const char* str = cstrstack.create(kSmallString);

        REQUIRE(!safeCStrCmp(str, kSmallString));
        REQUIRE(cstrstack.count() == 1);
        REQUIRE(cstrstack.size() == strlen(kSmallString)+1);
        REQUIRE(cstrstack.capacity() == kInitialCapacity);
    }

    SECTION("adding one string with no change in capacity and reset")
    {
        const char* str = cstrstack.create(kTinyString);

        REQUIRE(cstrstack.count() == 1);
        REQUIRE(cstrstack.size() == strlen(kTinyString)+1);
        REQUIRE(cstrstack.capacity() == kInitialCapacity);

        cstrstack.reset();

        REQUIRE(cstrstack.count() == 0);
        REQUIRE(cstrstack.size() == 0);
    }

    SECTION("adding three strings with change in capacity")
    {
        cstrstack.reset();

        const char* strings[3];

        size_t expectedSize = 0;
        size_t expectedCount = 0;

        strings[expectedCount] = cstrstack.create(kTinyString);
        REQUIRE(!safeCStrCmp(strings[expectedCount], kTinyString));
        expectedSize += strlen(kTinyString);
        ++expectedCount;

        strings[expectedCount] = cstrstack.create(kSmallString);
        REQUIRE(!safeCStrCmp(strings[expectedCount], kSmallString));
        expectedSize += strlen(kSmallString);
        ++expectedCount;

        REQUIRE(cstrstack.count() == expectedCount);
        REQUIRE(cstrstack.size() == expectedSize + expectedCount);
        REQUIRE(cstrstack.capacity() == kInitialCapacity);

        //  the next add should grow the memory container.  capacity will
        //  change and must at least fit the next string
        strings[expectedCount] = cstrstack.create(kMediumString);
        REQUIRE(!safeCStrCmp(strings[expectedCount], kMediumString));
        expectedSize += strlen(kMediumString);
        ++expectedCount;

        REQUIRE(!safeCStrCmp(strings[0], kTinyString));
        REQUIRE(!safeCStrCmp(strings[1], kSmallString));
        REQUIRE(!safeCStrCmp(strings[2], kMediumString));
        REQUIRE(cstrstack.size() == expectedSize + expectedCount);
        REQUIRE(cstrstack.capacity() >= kInitialCapacity);
    }
}

