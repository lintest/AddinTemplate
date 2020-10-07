mkdir build32Win
cd build32Win
cmake .. -A Win32 -DMySuffix2=32
cmake --build . --config Release
cd ..

mkdir build64Win
cd build64Win
cmake .. -A x64 -DMySuffix2=64
cmake --build . --config Release
cd ..
