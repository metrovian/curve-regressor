git submodule foreach --recursive "git reset --hard"
git submodule foreach --recursive "git clean -fd"
git submodule update --init --recursive

cd sqlite
make -f Makefile.msc sqlite3.c

cd ..