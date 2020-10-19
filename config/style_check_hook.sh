#!/bin/bash 
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved. 
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# taken from: https://raw.githubusercontent.com/andrewseidl/githook-clang-format/master/clang-format.hook
# - with just added check for cpp file & check for proper version of clang-format
# - with use of clang-format diff.py - for changed chunk changes only
# might be worth to consider: https://github.com/Sarcasm/run-clang-format


L_GIT_DIR=$(git rev-parse --show-toplevel)
source $L_GIT_DIR/config/format-config.sh

RED='\e[38;2;255;13;0m'
YELLOW="\e[38;2;255;232;0m"
GREEN="\e[38;2;0;222;27m"
ORANGE="\e[38;2;255;100;0m"

OK="${GREEN}OK!\e[0m"
ERROR="${RED}Error!\e[0m"
FIXED="${YELLOW}Fixed!\e[0m"


# if autoformatting was disabled by user - then ignore this commit hook
if [[ $DISABLE_AUTO_FORMATTING ]]; then
    [[ $VERBOSE ]] && echo "auto formatting is disabled"
    exit 0
fi

# check if clang-format in path is in proper version, version is 3rd column in `clang-format --version`
CVER=$( [[ $(which clang-format) ]] && echo $(clang-format --version | cut -d ' ' -f 3 | cut -d '.' -f 1) || echo "0")
# check for either clang-format or clang-format-9
if [[ $CVER -lt 10 && ! $(which clang-format-10) ]]; then
    cat << EOF >&1
Either install:
    clang-format in at least version 10 and set as default"
    or
    clang-format-10

    Your clang format version in path used:
        $(clang-format --version):
        $(clang-format-10 --version)
    git commit aborted"
EOF
    exit 1
fi

get_clang_format_script() {
    local declare searchpaths=(
        $(which "clang-format-diff.py")                                 # clang-format-diff in path
        "/usr/share/clang/clang-format-10/clang-format-diff.py"          # clang-format-diff location for clang format 9 Ubuntu/Debian 18.04
        "/usr/share/clang/clang-format-${CVER}/clang-format-diff.py"    # clang-format-diff location on Ubuntu/Debian
        "/usr/share/clang/clang-format-diff.py"                         # clang-format_diff location on Arch last resort
    )
    local tool=""
    for el in ${searchpaths[@]}; do
        if [[ -f ${el} ]]; then
            tool=${el}
            break
        fi
    done
    if [[ ${tool} == "" ]]; then
        echo clang-format-diff not found in path and: ${sarchpaths[@]} >2
    fi
    echo "${tool}"
}

L_CLANG_DIFF_TOOL=$(get_clang_format_script)
if ! [[ $L_CLANG_DIFF_TOOL ]] || [[ $L_CLANG_DIFF_TOOL == "" ]]; then
    exit 1
fi

check_file() {
    file="${1}"
    last_commit="${2}"
    if [ -f $file ]; then
        #[[ $VERBOSE ]] && echo -en "Checking: \e[33m${file}\e[0m :\t"
        results["${file}"]="${OK}"
        case ${last_commit} in
        "True")
            if [[ ${FIX,,} == "true" ]]; then
                git diff -U0 --no-color remotes/origin/master...HEAD ${file} | ${L_CLANG_DIFF_TOOL} -style file -p1 -i
                STATUS=$(git status --short -- ${file})
                if [ -n "${STATUS}" ]; then
                    git add "${file}"
                    results["${file}"]="${FIXED}";
                fi
            else
                OUT=$(git diff -U0 --no-color remotes/origin/master...HEAD ${file} | ${L_CLANG_DIFF_TOOL} -style file -p1 )
                if [ -n "${OUT}" ]; then
                    results["${file}"]="${ERROR}"
                    return 1
                fi
            fi
            ;;
        "Stage")
            if [[ ${FIX,,} == "true" ]]; then
                git diff -U0 --no-color --cached ${file} | ${L_CLANG_DIFF_TOOL} -style file -p1 -i
                STATUS=$(git status --short -- ${file})
                if [ -n "${STATUS}" ]; then
                    git add "${file}"
                    results["${file}"]="${FIXED}";
                fi
            else
                OUT=$(git diff -U0 --no-color --cached ${file} | ${L_CLANG_DIFF_TOOL} -style file -p1)
                if [ -n "${OUT}" ]; then
                    results["${file}"]="${ERROR}"
                    return 1
                fi
            fi
            ;;
        *)
            OUT=$(git diff -U0 --no-color --cached ${file} | ${L_CLANG_DIFF_TOOL} -style file -p1 )
            if [ -n ${OUT} ]; then
                results["${file}"]="${ERROR}"
                return 1
            fi
            ;;
        esac
    fi
    return 0
}

# bash function using above config function
shouldnt_ignore() {
    # change full name path to path relative to root git dir
    local fname=${1/"$L_GIT_DIR"/"./"}
    for el in ${ignore_paths[@]}; do
        if [[ ./${fname}  =~ ^${el}.* ]]; then
            [[ $VERBOSE ]] && echo "Ignore: ${fname} formatting due to: $el match!"
            return 1
        fi
    done
    return 0
}

function help() {
        echo "Runs clang-format on source files"
        cat <<- EOM
		ussge:
		   $0 [option]
		        --about             does nothing
		        --last              checks current branch against origin/master - doesn't fix
		        --branch-fix        checks current branch against origin/master and fixes errors - you have to 'git add' and 'git commit' them
		        --fix               fix style in currently staged files (ignores user.fixinstage variale), this is usefull for manual style applying
		
		If you would like to automatially fix style before commit
		add to your git config "user.fixinstage" variable with value "True"
		by calling:
		    git config user.fixinstage True
		EOM
}


case "${1}" in
    --about|--help )
        help
        FILES=''
        LAST=""
        exit 0
        ;;
    --last)
        FILES=$(git diff --name-only remotes/origin/master...HEAD)
        LAST="True"
        FIX="false"
        ;;
    --branch-fix)
        FILES=$(git diff --name-only remotes/origin/master...HEAD)
        LAST="True"
        FIX="true"
        ;;
    --fix)
        FILES=$(git diff-index --cached --name-only HEAD)
        LAST="Stage" 
        FIX=true
        ;;
    *)
        FILES=$(git diff-index --cached --name-only HEAD)
        LAST="Stage" 
        FIX=$(git config user.fixinstage)
        FIX=${FIX:-false}
        ;;
esac

declare -A results

EXIT_CODE=0
for file in ${FILES}; do
    if [[ ${file} =~ ^.*\.(cpp|hpp|c|h|cxx|gcc|cc)$ ]] && shouldnt_ignore ${file}; then
        check_file "${file}" ${LAST}
        RESULT=$?
        if [[ ${RESULT} -eq 1 ]]; then
            EXIT_CODE=1
        fi
    fi
done

for FILE in "${!results[@]}"
do
    echo -e "  ${FILE} : ${results[${FILE}]}"
done

echo "exit: ${EXIT_CODE}"
exit ${EXIT_CODE}
