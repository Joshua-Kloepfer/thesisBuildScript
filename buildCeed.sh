SOURCE_DIR=$1

cd $SOURCE_DIR
make configure ROCM_DIR=$ROCM_PATH
Make -j
