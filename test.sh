#!/bin/bash

echo "Running tests..."

if [ ! -e parser ]; then
  echo "But first, running make..."
  make
  echo
fi

failed=0
touch .myout

for INPUT_FILE in $(ls samples/*.java); do
  echo -n "testing $INPUT_FILE..."
  ./parser < $INPUT_FILE &> .myout
  diff=$(diff .myout ${INPUT_FILE/java/out})
  if [ "$diff" != "" ]; then
    echo "failed!"
    echo "$thediff"
    echo
    failed=$(( $failed + 1 ))
  else
    echo "passed!"
  fi
done

rm .myout
echo
if (( $failed )); then
  echo "===== failed $failed tests"
else
  echo "===== passed all tests! :)"
fi