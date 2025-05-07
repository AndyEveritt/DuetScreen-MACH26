PREVIOUS_TAG=$(git describe --tags --abbrev=0 HEAD^ 2>/dev/null || echo "")
if [ -n "$PREVIOUS_TAG" ]; then
echo "## CHANGELOG"
echo "Changes since ${PREVIOUS_TAG}:"
echo ""
git log --pretty=format:"* %s" ${PREVIOUS_TAG}..HEAD
echo ""
else
echo "CHANGELOG"
echo "Initial Release"
echo ""
git log --pretty=format:"* %s"
fi