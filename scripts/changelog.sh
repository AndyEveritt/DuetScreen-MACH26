PREVIOUS_TAG=$(git describe --tags --abbrev=0 HEAD^ 2>/dev/null || echo "")
if [ -n "$PREVIOUS_TAG" ]; then
echo "## CHANGELOG"
echo "Changes since ${PREVIOUS_TAG}:"
echo ""
COMMITS=$(git log --pretty=format:"%s" ${PREVIOUS_TAG}..HEAD)
else
echo "## CHANGELOG"
echo "Initial Release"
echo ""
COMMITS=$(git log --pretty=format:"%s")
fi

# Initialize arrays for each category
declare -a MERGES=()
declare -a FIXES=()
declare -a FEATURES=()
declare -a CHORES=()
declare -a REFACTORS=()
declare -a OTHERS=()

# Function to process a commit part
process_commit_part() {
    local part="$1"
    if [[ "$part" == "Merge "* ]]; then
        MERGES+=("$part")
        return 0
    elif [[ "$part" == *"fix("* ]]; then
        FIXES+=("${part#*fix(}")
        return 0
    elif [[ "$part" == *"feat("* ]]; then
        FEATURES+=("${part#*feat(}")
        return 0
    elif [[ "$part" == *"chore("* ]]; then
        CHORES+=("${part#*chore(}")
        return 0
    elif [[ "$part" == *"refactor("* ]]; then
        REFACTORS+=("${part#*refactor(}")
        return 0
    fi
    return 1
}

# Process each commit and categorize it
while IFS= read -r line; do
    categorized=false
    if [[ "$line" == "" ]]; then
        continue
    fi

    # echo "Processing: $line"
    if [[ "$line" == "Merge "* ]]; then
        MERGES+=("$line")
        categorized=true
        continue
    fi
    
    # Split the commit message into parts based on conventional commit keywords
    parts=$(echo "$line" | sed -E 's/(fix\(|feat\(|chore\(|refactor\()/\n\1/g')
    
    while IFS= read -r part; do
        # Trim leading and trailing whitespace without xargs
        part=$(echo "$part" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
        if [[ -n "$part" ]]; then
            if process_commit_part "$part"; then
                categorized=true
            fi
        fi
    done <<< "$parts"
    
    if [ "$categorized" = false ]; then
        OTHERS+=("$line")
    fi
done <<< "$COMMITS"

# Print each category only if it has entries
if [ ${#MERGES[@]} -gt 0 ]; then
    echo -e "\n### 🔄 Merges"
    printf "* %s\n" "${MERGES[@]}"
fi

if [ ${#FIXES[@]} -gt 0 ]; then
    echo -e "\n### � Fixes"
    printf "* %s\n" "${FIXES[@]}"
fi

if [ ${#FEATURES[@]} -gt 0 ]; then
    echo -e "\n### ✨ Features"
    printf "* %s\n" "${FEATURES[@]}"
fi

if [ ${#CHORES[@]} -gt 0 ]; then
    echo -e "\n### 🔧 Chores"
    printf "* %s\n" "${CHORES[@]}"
fi

if [ ${#REFACTORS[@]} -gt 0 ]; then
    echo -e "\n### ♻️ Refactoring"
    printf "* %s\n" "${REFACTORS[@]}"
fi

if [ ${#OTHERS[@]} -gt 0 ]; then
    echo -e "\n### Other Changes"
    printf "* %s\n" "${OTHERS[@]}"
fi
echo ""
