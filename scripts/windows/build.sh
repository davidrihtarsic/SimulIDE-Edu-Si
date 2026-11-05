#!/usr/bin/env bash
# Run in an MSYS2 MINGW64 shell with the packages from windows.yml installed.
set -euo pipefail
cd "$(dirname "$0")/../.."
repo_dir="$PWD"

# Qt loads ANGLE dynamically, so it is optional in the MSYS2 package graph
# but windeployqt expects these runtime libraries when creating the package.
for library in libGLESv2.dll libEGL.dll; do
    if [[ ! -f "/mingw64/bin/$library" ]]; then
        echo "Missing $library: install mingw-w64-x86_64-angleproject." >&2
        exit 1
    fi
done

mkdir -p resources/qm
for translation in resources/translations/*.ts resources/translations/qt/*.ts; do
    lrelease-qt5 "$translation" -qm "resources/qm/$(basename "${translation%.ts}").qm"
done

# Use a fresh directory, independent of any Linux build files in build_XX.
# Translations have already been generated above; the original recipe uses
# shell separators and move commands that are not portable between shells.
mkdir -p build_windows
cat > build_windows/SimulIDE_Windows.pro <<'PROJECT'
BUILD_DIR = $$PWD
include(../SimulIDE.pri)
PRE_TARGETDEPS -= runLrelease
PROJECT
cd build_windows
qmake-qt5 -spec win32-g++ SimulIDE_Windows.pro 'CONFIG+=release' 'CONFIG-=debug_and_release'
mingw32-make -j2
cd "$repo_dir"

package_dir="$repo_dir/dist/SimulIDE-Edu-Si-Windows-x64"
mkdir -p "$package_dir"
mapfile -t executables < <(find build_windows/executables -name simulide.exe -type f)
if [[ ${#executables[@]} != 1 ]]; then
    echo 'Expected exactly one compiled simulide.exe.' >&2
    exit 1
fi
cp "${executables[0]}" "$package_dir/simulide.exe"

# MSYS2 names qmake qmake-qt5; windeployqt also searches for qmake.exe.
if [[ ! -f /mingw64/bin/qmake.exe ]]; then
    cp /mingw64/bin/qmake-qt5.exe /mingw64/bin/qmake.exe
fi
windeployqt-qt5 --release --dir "$(cygpath -w "$package_dir")" "$(cygpath -w "$package_dir/simulide.exe")"

# windeployqt gathers Qt plugins, but MSYS2 libraries can have additional
# dependencies (zlib, GCC runtime, image/audio libraries). Copy their full
# import closure from the same toolchain, avoiding unrelated DLL downloads.
python scripts/windows/copy_dependencies.py "$package_dir" /mingw64/bin

cp -R resources/data "$package_dir/data"
cp -R resources/fonts "$package_dir/fonts"
cp README.md COPYING copyright.txt "$package_dir/"
mkdir -p "$package_dir/docs/images"
cp docs/images/simulide-edu-primer-sheme.png "$package_dir/docs/images/"
mkdir -p "$package_dir/licenses"
cp -R /mingw64/share/licenses/. "$package_dir/licenses/"
pacman -Q > "$package_dir/BUILD-PACKAGES.txt"
git rev-parse HEAD > "$package_dir/SOURCE-COMMIT.txt"
cp scripts/windows/START-WINDOWS.txt "$package_dir/"
