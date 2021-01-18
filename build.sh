echo -e

# Need to tell fucking cmake where openal is
if [[ -z "${OPENALDIR}" ]]; then
    echo "OPENALDIR env variable already set."
else
    echo "SURE HOPE YOU'RE USING WINDOWS FOR THIS LOL"
    export OPENALDIR="/c/Program\ Files\ \(x86\)/OpenAL\ 1.1\ SDK/"
fi

(
    if [[ ! -d build ]]; then
        echo "No build directory found, creating..."
        mkdir build
    fi
    cd build

    echo "Running cmake..."
    cmake -DOPENALDIR=$OPENALDIE .. -B ../bin
)
