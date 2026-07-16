#!/usr/bin/env bash

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
module="$(basename "$root")"
version="$(tr -d '\r\n' < "$root/VERSION")"
deb_version="${version/-beta./~beta}"
package="appeal-module-$(printf '%s' "$module" | tr '[:upper:]' '[:lower:]')"
arch="${ARCH:-amd64}"
stage="${TMPDIR:-/tmp}/${package}_${deb_version}_${arch}"
output="$root/Dist/${package}_${deb_version}_${arch}.deb"

make -C "$root" clean all
rm -rf "$stage"
mkdir -p "$stage/DEBIAN" "$stage/usr/share/appeal/Resources/Modules/$module" "$stage/usr/share/doc/$package"
cp "$root/$module.so" "$stage/usr/share/appeal/Resources/Modules/$module/"
cp "$root/LICENSE" "$stage/usr/share/doc/$package/copyright"

cat > "$stage/DEBIAN/control" <<CONTROL
Package: $package
Version: $deb_version
Section: games
Priority: optional
Architecture: $arch
Maintainer: Red Whistle
Depends: appeal
Description: $module shared-library module for Appeal
 Installs the $module module into Appeal's shared resource tree.
CONTROL

find "$stage" -type d -exec chmod 755 {} +
find "$stage" -type f -exec chmod 644 {} +
mkdir -p "$root/Dist"
dpkg-deb --build --root-owner-group "$stage" "$output"
echo "Created $output"
