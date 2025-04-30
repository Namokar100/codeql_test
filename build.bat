@echo off
echo Building C++ Cryptography Project...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Navigate to build directory
cd build

REM Generate build files with CMake
echo Generating build files with CMake...
cmake .. -G "MinGW Makefiles"

REM Build the project
echo Building the project...
cmake --build .

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo Build completed successfully.
    echo Executable can be found in the build directory.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)

REM Return to the original directory
cd ..

echo Build process completed. 