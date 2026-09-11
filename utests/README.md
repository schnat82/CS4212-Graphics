CS4212 VEC 3 LAB 1 NATHAN SCHMUCKER

This project is to set up a basic vec 3 class with the core vector math functions. It also has catch2 tests to verify the different vector operations work correctly.

THINGS I ALREADY HAD DOWNLOADED AND SET UP
    CMake
    VS Code
    vcpkg
    git bash
    github repo

BUILDING PROJECT:
    STEP 1: open terminal in root folder
    (i used the command: cd C:\Users\schmu\Documents\CS4212\starterCode)

    STEP 2: use cmake commands to configure project to test and build
    configure:
    cmake --preset dev

    build:
    cmake --build buildDev

    STEP 3: run the tests in the utest_vec3.cpp file. This file includes both the startercode tests and the vec3 tests.
    ctest --test-dir buildDev --output-on-failure

    

WHAT THE TESTS CHECK:
 *  Accessing vector values
 *  Adding and Subtracting 
 *  Scalar Multiplication and Division
 *  Dot Product
 *  Cross Product
 *  Length and Length squared
 *  Unit vector calculation

 Each test uses the catch2 and float point tolerators 

 If changes are made to the project, rebuild and run tests again using the same commands from above:
 cmake --build builddev
 ctest --test-dir builddev --output-on-failure