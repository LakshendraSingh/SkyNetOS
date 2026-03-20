#!/bin/bash

echo "Environment setup starting"

function install_dependencies_if_not_installed(){
    if ! command -v $1 &> /dev/null
    then
    echo "$1 not installed, installing now"
    sudo dnf install -y $1
    else
    echo "$1 already installed"
    fi
}

install_dependencies_if_not_installed gcc
install_dependencies_if_not_installed qemu
install_dependencies_if_not_installed git
install_dependencies_if_not_installed vim

project_dir="SkynetOS"

if [ ! -d "$project_dir" ]; then
echo "creating project directory $project_dir"
mkdir -p "$project_dir/src" "$project_dir/bin" "$project_dir/docs"
else
echo "project directory $project_dir already exists"
fi

cat <<EOL > "$project_dir/src/hello.c"
#include <stdio.h>

int main(void){
printf("Hello, World");
return 0;
}
EOL

echo "Environment setup complete"