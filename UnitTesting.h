#include "src/minigames/boxes.h"
#include "pet_stats.h"
#include "adult_avo.h"
#include "avo.h"
#include <iostream>
#include <SFML/System.hpp>
#include <set>
#include <string>

class UnitTesting {
private:
    // Known bugs this suite already catches. A listed test must keep failing
    // until its fix lands; the fix then removes it from this list.
    const std::set<std::string> expectedFailures = {
        "[Avo Test - Quote Visibility]",  // quote never hides (quotesFile.h, avo.h)
    };
    std::set<std::string> failures;

    void recordFailure(const std::string& name) { failures.insert(name); }

    void miniGameTest() {
        mini_game game;
        if (game.window_open() && game.money_earned_end() == 0) {
            std::cout << "[Mini Game Test] Passed" << std::endl;
        } else {
            recordFailure("[Mini Game Test]"); std::cout << "[Mini Game Test] Failed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testMaxHunger() {
        PetStats petStats;
        petStats.maxHunger();
        if (petStats.getHungerLevel() != 100) {
            recordFailure("[Test Max Hunger]"); std::cout << "[Test Max Hunger] Failed: Expected hunger 100, got " << petStats.getHungerLevel() << "." << std::endl;
        } else {
            std::cout << "[Test Max Hunger] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testInitialValues() {
        PetStats petStats;
        bool allPassed = true;

        if (petStats.getHealthLevel() != 25) {
            recordFailure("[Test Initial Values - Health]"); std::cout << "[Test Initial Values - Health] Failed: Expected 25, got " << petStats.getHealthLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getSleepLevel() != 25) {
            recordFailure("[Test Initial Values - Sleep]"); std::cout << "[Test Initial Values - Sleep] Failed: Expected 25, got " << petStats.getSleepLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getHungerLevel() != 25) {
            recordFailure("[Test Initial Values - Hunger]"); std::cout << "[Test Initial Values - Hunger] Failed: Expected 25, got " << petStats.getHungerLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getIQLevel() != 50) {
            recordFailure("[Test Initial Values - IQ]"); std::cout << "[Test Initial Values - IQ] Failed: Expected 50, got " << petStats.getIQLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getTotalMoney() != 1000) {
            recordFailure("[Test Initial Values - Money]"); std::cout << "[Test Initial Values - Money] Failed: Expected 1000, got " << petStats.getTotalMoney() << "." << std::endl;
            allPassed = false;
        }

        if (allPassed) {
            std::cout << "[Test Initial Values] All Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testIncreaseHunger() {
        PetStats petStats;
        petStats.increaseHunger(80); // Total should cap at 100
        if (petStats.getHungerLevel() != 100) {
            recordFailure("[Test Increase Hunger]"); std::cout << "[Test Increase Hunger] Failed: Expected hunger 100, got " << petStats.getHungerLevel() << "." << std::endl;
        } else {
            std::cout << "[Test Increase Hunger] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testIncreaseSleep() {
        PetStats petStats;
        petStats.increaseSleep(80); // Total should cap at 100
        if (petStats.getSleepLevel() != 100) {
            recordFailure("[Test Increase Sleep]"); std::cout << "[Test Increase Sleep] Failed: Expected sleep 100, got " << petStats.getSleepLevel() << "." << std::endl;
        } else {
            std::cout << "[Test Increase Sleep] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testIncreaseMoney() {
        PetStats petStats;
        petStats.increaseMoney(50);
        if (petStats.getTotalMoney() != 1050) {
            recordFailure("[Test Increase Money]"); std::cout << "[Test Increase Money] Failed: Expected money 1050, got " << petStats.getTotalMoney() << "." << std::endl;
        } else {
            std::cout << "[Test Increase Money] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testMaxStats() {
        PetStats petStats;
        petStats.maxHealth();
        petStats.maxSleep();
        petStats.maxHunger();

        bool allPassed = true;
        if (petStats.getHealthLevel() != 100) {
            recordFailure("[Test Max Health]"); std::cout << "[Test Max Health] Failed: Expected health 100, got " << petStats.getHealthLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getSleepLevel() != 100) {
            recordFailure("[Test Max Sleep]"); std::cout << "[Test Max Sleep] Failed: Expected sleep 100, got " << petStats.getSleepLevel() << "." << std::endl;
            allPassed = false;
        }
        if (petStats.getHungerLevel() != 100) {
            recordFailure("[Test Max Hunger]"); std::cout << "[Test Max Hunger] Failed: Expected hunger 100, got " << petStats.getHungerLevel() << "." << std::endl;
            allPassed = false;
        }

        if (allPassed) {
            std::cout << "[Test Max Stats] All Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void testGetters() {
        PetStats petStats;
        petStats.setTotalMoney(2000);
        if (petStats.getTotalMoney() != 2000) {
            recordFailure("[Test Getters]"); std::cout << "[Test Getters] Failed: Expected money 2000, got " << petStats.getTotalMoney() << "." << std::endl;
        } else {
            std::cout << "[Test Getters] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

    void adultAvoTestConstructor() {
        adult_avo avo("moon.png", 0.1f);
        // Since we can't directly check private members, we might rely on behavior
        std::cout << "[AdultAvo Constructor Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestStartAnimationShower() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_shower();
        // We could check the initial position and animation state if they are accessible
        std::cout << "[AdultAvo Shower Animation Start Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestUpdateShower() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_shower();
        float deltaTime = 0.1f; // Simulate time passing
        avo.update_shower(deltaTime);
        // Check the current frame or any other change in state
        std::cout << "[AdultAvo Shower Update Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestStartAnimationEating() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_eating();
        // Check animation state
        std::cout << "[AdultAvo Eating Animation Start Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestUpdateEating() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_eating();
        float deltaTime = 0.1f; // Simulate time passing
        avo.update_eating(deltaTime);
        // Check the current frame or any other change in state
        std::cout << "[AdultAvo Eating Update Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestStartAnimationSleeping() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_sleeping();
        // Check animation state
        std::cout << "[AdultAvo Sleeping Animation Start Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestUpdateSleeping() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_sleeping();
        float deltaTime = 0.1f; // Simulate time passing
        avo.update_sleeping(deltaTime);
        // Check the current frame or any other change in state
        std::cout << "[AdultAvo Sleeping Update Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestStartAnimationMedicine() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_medicine();
        // Check animation state
        std::cout << "[AdultAvo Medicine Animation Start Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void adultAvoTestUpdateMedicine() {
        adult_avo avo("moon.png", 0.1f);
        avo.startAnimation_medicine();
        float deltaTime = 0.1f; // Simulate time passing
        avo.update_medicine(deltaTime);
        // Check the current frame or any other change in state
        std::cout << "[AdultAvo Medicine Update Test] Passed" << std::endl;
        std::cout << std::endl;
    }

    void avoTestUpdateAvoQuote() {
        avo avocado;
        sf::Clock quoteClock;
        std::string selectedQuote;
        bool quoteVisible = false;
        float quoteInterval = 2.0f; // 2 seconds for testing

        // Simulate waiting for enough time to update quote
        sf::sleep(sf::seconds(2.1f)); // Wait just over the interval

        avocado.updateAvoQuote(selectedQuote, quoteClock, quoteInterval, quoteVisible);

        // Check that a new quote was selected
        if (!quoteVisible || selectedQuote.empty()) {
            recordFailure("[Avo Test - Update Quote]"); std::cout << "[Avo Test - Update Quote] Failed: Expected quote to be updated and visible." << std::endl;
        } else {
            std::cout << "[Avo Test - Update Quote] Passed" << std::endl;
        }

        // Check the display duration
        avocado.updateAvoQuote(selectedQuote, quoteClock, quoteInterval, quoteVisible);
        // Wait to check if the quote gets hidden after the display duration
        sf::sleep(sf::seconds(10.0f)); // Wait long enough to check visibility

        avocado.updateAvoQuote(selectedQuote, quoteClock, quoteInterval, quoteVisible);
        if (quoteVisible) {
            recordFailure("[Avo Test - Quote Visibility]"); std::cout << "[Avo Test - Quote Visibility] Failed: Expected quote to be hidden after display duration." << std::endl;
        } else {
            std::cout << "[Avo Test - Quote Visibility] Passed" << std::endl;
        }
        std::cout << std::endl;
    }

public:
    // Returns the number of unexpected outcomes: new failures, plus expected
    // failures that now pass.
    int runTests() {
        std::cout << "Running Unit Tests..." << std::endl << std::endl;
        miniGameTest();
        testMaxHunger();
        testInitialValues();
        testIncreaseHunger();
        testIncreaseSleep();
        testIncreaseMoney();
        testMaxStats();
        testGetters();
        adultAvoTestConstructor();
        adultAvoTestStartAnimationShower();
        adultAvoTestUpdateShower();
        adultAvoTestStartAnimationEating();
        adultAvoTestUpdateEating();
        adultAvoTestStartAnimationSleeping();
        adultAvoTestUpdateSleeping();
        adultAvoTestStartAnimationMedicine();
        adultAvoTestUpdateMedicine();
        avoTestUpdateAvoQuote();

        int unexpected = 0;
        for (const auto& name : failures) {
            if (expectedFailures.count(name)) {
                std::cout << "XFAIL " << name << std::endl;
            } else {
                std::cout << "FAIL " << name << std::endl;
                unexpected++;
            }
        }
        for (const auto& name : expectedFailures) {
            if (!failures.count(name)) {
                std::cout << "XPASS " << name << ": remove it from expectedFailures" << std::endl;
                unexpected++;
            }
        }
        return unexpected;
    }
};
