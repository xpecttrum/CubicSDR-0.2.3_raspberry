#!/bin/sh
# found at https://github.com/paralect/robomongo/blob/master/install/linux/fixup_deb.sh.in
set -e
mkdir fix_up_deb
dpkg-deb -x .deb fix_up_deb
dpkg-deb --control .deb fix_up_deb/DEBIAN
rm .deb
chmod 0644 fix_up_deb/DEBIAN/md5sums
find -type d -print0 | xargs -0 chmod 755
fakeroot dpkg -b fix_up_deb .deb
rm -rf fix_up_deb
