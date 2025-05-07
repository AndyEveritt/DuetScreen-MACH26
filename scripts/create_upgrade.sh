rm -rf out/update
if [ -f out/update.tar.gz ]; then
    rm "out/update.tar.gz"
fi

if [ -f out/DuetScreen.tar.gz ]; then
    rm "out/DuetScreen.tar.gz"
fi

# Copy binary
mkdir -p out/update/usr/bin
cp ../buildroot-duetscreen/output/target/usr/bin/DuetScreen out/update/usr/bin

# Copy assets
mkdir -p out/update/etc/assets
cp -r assets out/update/etc

# Copy libraries
# mkdir -p out/update/usr/lib
# cp ../buildroot-duetscreen/output/target/usr/lib/liblvgl* out/update/usr/lib/

echo "rm -rf /etc/assets" > out/update/pre-update
chmod +x out/update/pre-update

echo "/etc/init.d/S20DuetScreen restart" > out/update/post-update
chmod +x out/update/post-update

cd out/update
tar -czf ../update.tar.gz *
cp ../update.tar.gz ../DuetScreen.tar.gz
cd -