echo -e

(
    if [[ ! -d build ]]; then
        echo "No build directory found, creating..."
        mkdir build
    fi
    cd build

    echo "Running cmake..."
    cmake ..
)