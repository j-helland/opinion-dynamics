echo -e

# thank you John Luke Lusty
echo "Detected OS $OSTYPE"
if [[ "$OSTYPE" == "msys" ]]; then
    setx OPENALDIR          "C:\Program Files (x86)\OpenAL 1.1 SDK"
    setx LIBSND_INCLUDE_DIR "C:\Program Files\Mega-Nerd\libsndfile\include"
    setx LIBSND_LIBRARY     "C:\Program Files\Mega-Nerd\libsndfile\lib\libsndfile-1.lib"
else
    echo "Go fuck yourself"
fi

echo "GO RELOAD YOUR ENV, FUCK-ASS"

echo "ENV:"
echo "  OPENALDIR=$OPENALDIR"
echo "  LIBSND_INCLUDE_DIR=$LIBSND_INCLUDE_DIR"
echo "  LIBSND_LIBRARY=$LIBSND_LIBRARY"

echo "Running cmake..."
cmake . -B bin
