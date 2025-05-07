VERSION_INFO=$(cat version_compatibility.json)
MIN_VERSION=$(echo $VERSION_INFO | jq -r .minCompatibleVersion)
MAX_VERSION=$(echo $VERSION_INFO | jq -r .maxCompatibleVersion)

echo "## Version Compatibility:"
echo "- Minimum Compatible RRF Version: $MIN_VERSION"
if [ "$MAX_VERSION" != "null" ]; then
echo "- Maximum Compatible RRF Version: $MAX_VERSION"
fi