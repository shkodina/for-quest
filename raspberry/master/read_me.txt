alias l="ls -l"

java_lib_dir= /usr/lib/jvm/jdk-8-oracle-arm-vfp-hflt/jre/lib/ext

sed -e "s/4444/11/g" map.xml > tmp;mv tmp map.xml
