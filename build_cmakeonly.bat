if "%2"=="clean" (
    rd /s /q Build-Scratch
)

mkdir Build-Scratch
cd Build-Scratch
cmake .. -G %1
cd ../
