Research/CMakeDemo
=====================
Build from commandline
------------------
"path\to\cmake.exe" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=path\to\Emscripten.cmake -G "CodeBlocks - NMake Makefiles" path\to\CMakeDemo
"path\to\cmake.exe" --build "path\to\CMakeDemo" --target CMakeDemo

*Example commands for my local system:*

"C:\Program Files\JetBrains\CLion 2020.2\bin\cmake\win\bin\cmake.exe" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=E:\bin\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake -G "CodeBlocks - NMake Makefiles" E:\dev\SkiaViki\SkiaViki\Research\CMakeDemo-master
"C:\Program Files\JetBrains\CLion 2020.2\bin\cmake\win\bin\cmake.exe" --build E:\dev\SkiaViki\SkiaViki\Research\CMakeDemo-master --target CMakeDemo
