#!/usr/bin/env bash
# Run this script in your own desktop session, outside the agent sandbox.
set -euo pipefail
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
binary="$repo_dir/build_XX/executables/SimulIDE_2.0.0-/simulide"
install_dir="$HOME/.local/opt/simulide-edu-si"
launcher="$HOME/.local/bin/simulide-edu-si"
applications_dir="${XDG_DATA_HOME:-$HOME/.local/share}/applications"

if [[ ! -x "$binary" ]]; then
    echo "Najprej prevedi program: cd \"$repo_dir/build_XX\" && qmake && make -j4" >&2
    exit 1
fi

mkdir -p "$install_dir/data" "$install_dir/fonts" "$install_dir/docs/images" \
         "$(dirname "$launcher")" "$applications_dir"
# Replace through a temporary file, also allowing updates while an old copy runs.
install -m755 "$binary" "$install_dir/simulide.new"
mv -f "$install_dir/simulide.new" "$install_dir/simulide"
cp -R "$repo_dir/resources/data/." "$install_dir/data/"
cp -R "$repo_dir/resources/fonts/." "$install_dir/fonts/"
cp "$repo_dir/resources/icons/simulide.png" "$install_dir/simulide.png"
cp "$repo_dir/README.md" "$repo_dir/COPYING" "$repo_dir/copyright.txt" "$install_dir/"
cp "$repo_dir/docs/images/simulide-edu-primer-sheme.png" "$install_dir/docs/images/"
ln -sfn "$install_dir/simulide" "$launcher"

cat > "$applications_dir/simulide-edu-si.desktop" <<DESKTOP
[Desktop Entry]
Version=1.0
Type=Application
Name=SimulIDE-Edu-Si
GenericName=Electronic Circuit Simulator
Comment=Pedagoška prilagoditev simulatorja elektronskih vezij
Exec="$install_dir/simulide" %F
Icon=$install_dir/simulide.png
Terminal=false
Categories=Education;Electronics;
MimeType=application/x-simulide-circuit;
StartupNotify=false
DESKTOP

if command -v desktop-file-validate >/dev/null; then
    desktop-file-validate "$applications_dir/simulide-edu-si.desktop"
fi
if command -v update-desktop-database >/dev/null; then
    update-desktop-database "$applications_dir"
fi
echo "Nameščeno v: $install_dir"
echo "Zagon: $launcher"
echo 'V meniju aplikacij poišči SimulIDE-Edu-Si.'
