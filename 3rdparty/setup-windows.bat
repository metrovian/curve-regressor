git submodule foreach --recursive "git reset --hard"
git submodule foreach --recursive "git clean -fd"
git submodule update --init --recursive

cd sqlite
nmake /f makefile.msc sqlite3.c

cd ..