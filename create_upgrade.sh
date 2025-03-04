rm -rf out/update
if [ -f out/update.tar.gz ]; then
    rm "out/update.tar.gz"
fi
mkdir -p out/update/usr/bin
cp ../buildroot-duetscreen/output/target/usr/bin/DuetScreen out/update/usr/bin
echo "/etc/init.d/S20DuetScreen restart" > out/update/post-update
chmod +x out/update/post-update

cd out/update
tar -czf ../update.tar.gz *
cd -