#!/bin/bash
# Test server handling of a very large HTTP header key via curl

HOST="127.0.0.1"
PORT=8082
LARGE_HEADER_KEY="X-$(printf 'A%.0s' {1..8192})"

echo "Sending request with header key size: ${#LARGE_HEADER_KEY} bytes"

curl -v \
  --max-time 5 \
  --header "TEST : close" \
  --header "${LARGE_HEADER_KEY}: test-value" \
  --header "Connection: close" \
  "http://${HOST}:${PORT}/index.html"

EXIT_CODE=$?

echo ""
case $EXIT_CODE in
  0)  echo "SUCCESS: Request completed." ;;
  28) echo "ERROR: Connection timed out." ;;
  7)  echo "ERROR: Connection refused – is a server listening on ${HOST}:${PORT}?" ;;
  *)  echo "ERROR: curl exited with code $EXIT_CODE" ;;
esac