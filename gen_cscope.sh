find $(pwd) -type f -name "*.[c(pp|c)hxsS]" -print -exec readlink -f {} \; >cscope.files
find /usr/include -type f -name "*.[c(pp|c)hxsS]" -print -exec readlink -f {} \; >>cscope.files
find /usr/src -type f -name "*.[c(pp|c)hxsS]" -print -exec readlink -f {} \; >>cscope.files
find /usr/local/src -type f -name "*.[c(pp|c)hxsS]" -print -exec readlink -f {} \; >>cscope.files
cscope -b
echo "To use vim with cscope export CSCOPE_DB env by running the following command:"
echo "export CSCOPE_DB=$(pwd)/cscope.out"


