#!/bin/sh

generate_test_file() {
    # We're going to create a test file that contains
    # test.xml 1000 times.

    # Opening root element tag
    sed -n '1p' < test.xml > test_big.xml

    # Create a temp file that consists of the original
    # xml without the root element start and end tags
    sed '1d; $d' < test.xml > test_piece.xml

    for i in `seq 1 1000`; do
	cat test_piece.xml >> test_big.xml
    done
    rm test_piece.xml

    # Closing tag
    sed -n '$p' < test.xml >> test_big.xml
}

[ -e test_big.xml ] || generate_test_file

qmake
make
./build/release/large-file-test test_big.xml
